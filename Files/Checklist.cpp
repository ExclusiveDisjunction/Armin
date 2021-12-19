#include "Components.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace Armin::Files
{
	Checklist::Checklist(ProjectBase* File, ChecklistGroup* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = ParentList->Parent;
	}
	Checklist::Checklist(ProjectBase* File, ChecklistGroup* ParentList, const Checklist*& Obj) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = ParentList->Parent;

		if (Obj)
		{
			Title(Obj->Title());
			Creator = new ComponentReference(Obj->Creator);
			Items = Obj->Items;
		}
	}
	Checklist::Checklist(ProjectBase* File, ChecklistGroup* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = ParentList->Parent;

		Fill(InFile);
	}
	Checklist::~Checklist()
	{
		_ParentList->Pop(this);
	}

	void Checklist::Fill(std::ifstream& InFile)
	{
		AString Header;
		getline(InFile, Header);

		AStringList Parts = Header.Split('~');
		bool Multiline = Parts[Parts.Size - 1] != "end";
		int TabIndex = Header.TabIndex();

		if (Multiline)
		{
			while (!InFile.eof())
			{
				AString ThisLine;
				streampos PrePos = InFile.tellg();
				getline(InFile, ThisLine);

				AString Temp = ThisLine;
				Temp.RemoveAllOf(L'\t');
				if (Temp == "")
					continue;

				if (ThisLine.TabIndex() <= TabIndex)
					break;
			}
		}

		uint Size = Multiline ? Parts.Size : Parts.Size - 1;
		for (uint i = 2; i < Size; i++)
		{
			AStringList ArgPart = Parts[i].Split(L':');
			if (ArgPart.Size < 2)
				continue;

			AString Name = ArgPart[0], Value = ArgPart[1];
			for (uint j = 2; j < ArgPart.Size; j++)
				Value += L':' + ArgPart[j];

			if (Name == "Title") _Title = (String)Value;
			else if (Name == "ID") _ID = Value.ToULong();
			else if (Name == "Creator")
			{
				if (Creator)
					delete Creator;

				unsigned long long ThisId = Value.ToULong();
				Component* Temp = _ParentFile->GetFromID(ThisId, CT_User);
				User* Conv = dynamic_cast<User*>(Temp);

				Creator = new ComponentReference(Conv);
			}
			else if (Name == "Items")
			{
				Items.Clear();
				AStringList Parts = Value.Split('*');

				for (AString& Item : Parts)
				{
					AStringList Sec = Item.Split('`');
					if (Sec.Size < 2)
						continue;

					AString Status = Sec[0];
					AString Name = Sec[1];
					for (uint j = 2; j < Sec.Size; j++)
						Name += '`' + Sec[j];

					bool Stats = Status == "True";
					String Index = String(Name);

					Items.Add(Index, Stats);
				}
			}
		}
	}
	void Checklist::Push(std::ofstream& OutFile, uint TabIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < TabIndex; i++)
			TabIndexValue += '\t';

		AString ItemsString;
		if (Items.Size >= 1)
		{
			MappingPair<String, bool> Current = Items[0];
			ItemsString = AString(Current.Value ? "True" : "False") + '`' + Current.Key;
			for (uint i = 0; i < Items.Size; i++)
			{
				Current = Items[i];
				ItemsString += '*' + AString(Current.Value ? "True" : "False") + '`' + Current.Key;
			}
		}

		OutFile << TabIndexValue << "begin~Checklist" << "~Title:" << Title() << "~Creator:" << (!Creator ? "0" : Creator->ToString()) << "~Items:" << ItemsString << "~end" << endl;
	}
}