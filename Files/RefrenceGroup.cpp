#include "Components.h"

#include "ComponentRefrence.h"

using namespace std;

namespace Armin::Files
{
	RefrenceGroup::RefrenceGroup(ProjectBase* File, RefrenceGroupList* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	RefrenceGroup::RefrenceGroup(ProjectBase* File, RefrenceGroupList* ParentList, RefrenceGroup* ToClone) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			Title(ToClone->Title());
			Notes = ToClone->Notes;
			Targets = ToClone->Targets;
		}
	}
	RefrenceGroup::RefrenceGroup(ProjectBase* File, RefrenceGroupList* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	RefrenceGroup::~RefrenceGroup()
	{
		for (ComponentReference* Ref : Targets)
			delete Ref;
		Targets.Clear();

		_ParentList->Pop(this);
	}

	void RefrenceGroup::Fill(ifstream& InFile)
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
				Temp.RemoveAllOf('\t');
				if (Temp == "")
					continue;

				AStringList ThisParts = Temp.Split('~');
				if (ThisLine.TabIndex() == TabIndex && ThisParts[1] == "RefrenceGroup" && ThisParts[0] == "end")
					break;
			}
		}

		uint Size = Multiline ? Parts.Size : Parts.Size - 1;
		for (uint i = 2; i < Size; i++)
		{
			AStringList ArgPart = Parts[i].Split(':');
			if (ArgPart.Size < 2)
				continue;

			AString Name = ArgPart[0], Value = ArgPart[1];
			for (uint j = 2; j < ArgPart.Size; j++)
				Value += L':' + ArgPart[j];

			if (Name == "Title") Title((String)Value);
			else if (Name == "Notes") Notes = (String)Value.ReplaceChar('\n', '`');
			else if (Name == "Targets") Targets = ReferenceList(Value, _ParentFile);
			else if (Name == "ID") _ID = Value.ToLong();
		}
	}
	void RefrenceGroup::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += '\t';

		AString Header = TabIndexValue + "begin~InventoryItem";
		OutFile << Header;
		OutFile << "Title:" << Title() << "~ID:" << ID << "~Notes" << Notes.ReplaceChar('\n', '`') << "~Targets:" << Targets.ToString() << "~end" << endl;
	}
}