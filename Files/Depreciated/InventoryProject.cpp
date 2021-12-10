#include "ArminSessions.h"

using namespace std;

namespace Armin::Files
{
	void InventoryProject::Save()
	{
		ofstream OutFile(Path);

		OutFile << "begin~InventoryProject~ID:" << CurrentID << endl;
		if (InventoryItems)
			InventoryItems->Push(OutFile, 1);
		if (OperationInventoryItems)
			OperationInventoryItems->Push(OutFile, 1);
		if (RefrenceGroups)
			RefrenceGroups->Push(OutFile, 1);
		if (ConfigItems)
			ConfigItems->Push(OutFile, 1);
		OutFile << "end~InventoryProject" << endl;

		OutFile.close();
	}
	void InventoryProject::Load()
	{
		ifstream InFile(Path);

		AString Header;
		getline(InFile, Header);

		AStringList Parts = Header.Split('~');
		bool Multiline = Parts[Parts.Size - 1] != "end";

		SetID(Parts[2].ToLong());

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
				if (ThisLine.TabIndex() <= 0)
					break;

				else if (ThisLine.TabIndex() - 1 == 0)
				{
					streampos ThisPos = InFile.tellg();
					if (Parts[0] != "begin")
						continue;

					InFile.seekg(PrePos);
					if (ThisParts[1] == InventoryItems->Name)
						InventoryItems->Fill(InFile);
					else if (ThisParts[1] == OperationInventoryItems->Name)
						OperationInventoryItems->Fill(InFile);
					else if (ThisParts[1] == RefrenceGroups->Name)
						RefrenceGroups->Fill(InFile);
					else if (ThisParts[1] == ConfigItems->Name)
						ConfigItems->Fill(InFile);
					else
						InFile.seekg(ThisPos);
				}
			}
		}
	}

	Component* InventoryProject::GetFromID(unsigned long long ID, int Filter) const
	{
		if (ID == 0)
			return nullptr;

		Component* Return = nullptr;

		if (InventoryItems && InventoryItems->Count != 0 && (Filter & CT_InventoryItem))
		{
			for (uint i = 0; i < InventoryItems->Count; i++)
			{
				InventoryItem* This = InventoryItems->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}

		if (OperationInventoryItems && OperationInventoryItems->Count != 0 && (Filter & CT_OperationInventoryItem))
		{
			for (uint i = 0; i < OperationInventoryItems->Count; i++)
			{
				OperationInventoryItem* This = OperationInventoryItems->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}

		if (RefrenceGroups && RefrenceGroups->Count != 0 && (Filter & CT_RefrenceGroup))
		{
			for (uint i = 0; i < RefrenceGroups->Count; i++)
			{
				RefrenceGroup* This = RefrenceGroups->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}

		if (ConfigItems && ConfigItems->Count != 0 && (Filter & CT_ConfigItem))
		{
			for (uint i = 0; i < ConfigItems->Count; i++)
			{
				ConfigItem* This = ConfigItems->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}
		return Return;
	}
}