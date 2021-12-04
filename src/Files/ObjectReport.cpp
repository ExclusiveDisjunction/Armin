#include "Components.h"

using namespace std;

namespace Armin::Files
{
	ObjectReport::ObjectReport(ArminSessionBase* File, ObjectReportSet* ParentList) : Component(File, true), OperationInventoryItemParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	ObjectReport::ObjectReport(ArminSessionBase* File, ObjectReportSet* ParentList, ObjectReport* ToClone) : Component(File, true), OperationInventoryItemParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			IsFault = ToClone->IsFault;
			Description = ToClone->Description;

			OperationInventoryItemGroup* Inv = ToClone->OperationInventoryItems;
			if (Inv)
			{
				for (uint i = 0; i < Inv->Count; i++)
					new OperationInventoryItem(File, OperationInventoryItems, Inv->Item(i));
			}
		}
	}
	ObjectReport::ObjectReport(ArminSessionBase* File, ObjectReportSet* ParentList, std::ifstream& OutFile) : Component(File, false), OperationInventoryItemParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(OutFile);
	}
	ObjectReport::~ObjectReport()
	{
		_ParentList->Pop(this);
	}

	void ObjectReport::Fill(ifstream& InFile)
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
				if (ThisLine.TabIndex() <= TabIndex)
					break;

				else if (ThisLine.TabIndex() - 1 == TabIndex)
				{
					if (ThisParts[0] != "begin")
						continue;

					streampos Pos = InFile.tellg();
					InFile.seekg(PrePos);
					if (ThisParts[1] == OperationInventoryItems->Name)
						OperationInventoryItems->Fill(InFile);
					else
						InFile.seekg(Pos);
				}
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
				Value += ':' + ArgPart[j];

			if (Name == "IsFault") IsFault = Value == "True";
			else if (Name == "Description") Description = (String)Value.ReplaceChar('`', '\n');
			else if (Value == "ID") _ID = Value.ToLong();
		}
	}
	void ObjectReport::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		OutFile << TabIndexValue << "begin~ObjectReport";
		OutFile << "~IsFault:" << String(IsFault) << "~Description:" << Description.ReplaceChar('\n', '`') << "~ID:" << ID;
		if (OperationInventoryItems->Count > 0)
		{
			OutFile << endl;
			OperationInventoryItems->Push(OutFile, PreIndex + 1);
			OutFile << TabIndexValue << "end~ObjectReport" << endl;
		}
		else
			OutFile << "~end" << endl;
	}

	String ObjectReport::ToString() const
	{
		return L"Object Report | Targets: {Collection: Size = " + String(OperationInventoryItems ? OperationInventoryItems->Count : 0) + L"}" + L" | Is Fault: " + (IsFault ? L"True" : L"False");
	}
}