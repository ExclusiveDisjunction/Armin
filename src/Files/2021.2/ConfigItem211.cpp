#include "Components.h"

using namespace std;

namespace Armin::Files::V2021
{
	ConfigItem::ConfigItem(ArminSessionBase* File, ConfigItemSet* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	ConfigItem::ConfigItem(ArminSessionBase* File, ConfigItemSet* ParentList, ConfigItem* ToClone) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			Title(ToClone->Title());
			Value = ToClone->Value;
		}
	}
	ConfigItem::ConfigItem(ArminSessionBase* File, ConfigItemSet* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	ConfigItem::~ConfigItem()
	{
		_ParentList->Pop(this);
	}

	void ConfigItem::Fill(ifstream& InFile)
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
			else if (Name == "Value") this->Value = (String)Value;
			else if (Name == "ID") _ID = Value.ToULong();
		}
	}
	void ConfigItem::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		OutFile << TabIndexValue << "begin~ConfigItem";
		OutFile << "~Title:" << _Title << "~Value:" << Value << "~ID:" << _ID << "~end" << endl;
	}
}