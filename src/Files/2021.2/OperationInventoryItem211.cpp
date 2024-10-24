#include "Components.h"

#include "ArminSessions.h"

using namespace std;

namespace Armin::Files::V2021
{
	OperationInventoryItem::OperationInventoryItem(ArminSessionBase* File, OperationInventoryItemGroup* ParentList) : InventoryBase(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	OperationInventoryItem::OperationInventoryItem(ArminSessionBase* File, OperationInventoryItemGroup* ParentList, OperationInventoryItem* ToClone) : InventoryBase(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			SerialNumber = ToClone->SerialNumber;
			Group = ToClone->Group;
			Description = ToClone->Description;
			CurrentState = ToClone->CurrentState;
		}
	}
	OperationInventoryItem::OperationInventoryItem(ArminSessionBase* File, OperationInventoryItemGroup* ParentList, std::ifstream& InFile) : InventoryBase(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	OperationInventoryItem::~OperationInventoryItem()
	{
		_ParentList->Pop(this);
	}

	String OperationInventoryItem::ISToString(OperationInventoryItem::ItemState State)
	{
		switch (State)
		{
		case IS_WorkingOrder:
			return L"Working Order";
		case IS_PartialWorkingOrder:
			return L"Partial Working Order";
		case IS_Broken:
			return L"Broken";
		case IS_UnderRepair:
			return L"Under Repair";
		case IS_NotInPossession:
			return L"Not In Possession";
		}

		return L"(Undefined State)";
	}
	AString OperationInventoryItem::ISToAString(OperationInventoryItem::ItemState State)
	{
		switch (State)
		{
		case IS_WorkingOrder:
			return "Working Order";
		case IS_PartialWorkingOrder:
			return "Partial Working Order";
		case IS_Broken:
			return "Broken";
		case IS_UnderRepair:
			return "Under Repair";
		case IS_NotInPossession:
			return "Not In Possession";
		}

		return "(Undefined State)";
	}

	void OperationInventoryItem::Fill(ifstream& InFile)
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
			AStringList ArgPart = Parts[i].Split(':');
			if (ArgPart.Size < 2)
				continue;

			AString Name = ArgPart[0], Value = ArgPart[1];
			for (uint j = 2; j < ArgPart.Size; j++)
				Value += L':' + ArgPart[j];

			if (Name == "SerialNumber") SerialNumber = (String)Value;
			else if (Name == "Description") Description = (String)Value.ReplaceChar('`', '\n');
			else if (Name == "Group") Group = (String)Value;
			else if (Name == "CurrentState") CurrentState = (ItemState)Value.ToInt();
			else if (Name == "TargetImage")
			{
				unsigned long long ID = Value.ToULong();
				Component* Target = _ParentFile->GetFromID(ID, CT_Image);
				TargetImage = new ComponentReference(Target);
			}
			else if (Name == "ID") _ID = Value.ToLong();
		}
	}
	void OperationInventoryItem::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		OutFile << TabIndexValue << "begin~OperationInventoryItem";
		OutFile << "~SerialNumber:" << SerialNumber << "~Description:" << Description.ReplaceChar('\n', '`') << "~Group:" << Group << "~CurrentState:" << (int)CurrentState << "~TargetImage:" << (!TargetImage || !TargetImage->Target() ? 0 : TargetImage->Target()->ID) << "~ID:" << _ID;

		OutFile << "~end" << endl;
	}
}