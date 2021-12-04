#include "Components.h"

#include "ArminSessions.h"

using namespace std;

namespace Armin::Files
{
	InventoryItem::InventoryItem(ArminSessionBase* File, InventoryItemGroup* ParentList) : InventoryBase(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	InventoryItem::InventoryItem(ArminSessionBase* File, InventoryItemGroup* ParentList, InventoryItem* ToClone) : InventoryBase(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			SerialNumber = ToClone->SerialNumber;
			Description = ToClone->Description;
			Group = ToClone->Group;
			IsInPossession = ToClone->IsInPossession;
		}
	}
	InventoryItem::InventoryItem(ArminSessionBase* File, InventoryItemGroup* ParentList, std::ifstream& InFile) : InventoryBase(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	InventoryItem::~InventoryItem()
	{
		_ParentList->Pop(this);
	}

	void InventoryItem::Fill(ifstream& InFile)
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

				AStringList ThisParts = Temp.Split(L'~');
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
				Value += ':' + ArgPart[j];

			if (Name == "SerialNumber") SerialNumber = (String)Value;
			else if (Name == "Description") Description = (String)Value.ReplaceChar(L'\n', L'`');
			else if (Name == "Group") Group = (String)Value;
			else if (Name == "IsInPossession") IsInPossession = Value == "True" ? true : false;
			else if (Name == "TargetImage")
			{
				unsigned long long ID = Value.ToULong();
				Component* Target = _ParentFile->GetFromID(ID, CT_Image);
				TargetImage = new ComponentReference(Target);
			}
			else if (Name == "ID") _ID = Value.ToLong();
		}
	}
	void InventoryItem::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		OutFile << TabIndexValue << "begin~InventoryItem";
		OutFile << "~SerialNumber:" << SerialNumber << "~Description:" << Description.ReplaceChar('\n', '`') << "~Group:" << Group << "~IsInPossession:" << (IsInPossession ? "True" : "False") << "~TargetImage:" << (!TargetImage || !TargetImage->Target() ? 0 : TargetImage->Target()->ID) << "~ID:" << _ID;

		OutFile << "~end" << endl;
	}
}