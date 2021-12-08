#include "ArminSessions.h"

using namespace std;

namespace Armin::Files
{
	void InventoryProjectRc::Save()
	{
		ofstream OutFile((AString)Path);

		OutFile << "begin~InventoryProjectRc~ID:" << CurrentID << endl;
		if (Images)
			Images->Push(OutFile, 1);
		if (InventoryItems)
			InventoryItems->Push(OutFile, 1);
		if (OperationInventoryItems)
			OperationInventoryItems->Push(OutFile, 1);
		if (RefrenceGroups)
			RefrenceGroups->Push(OutFile, 1);
		if (ConfigItems)
			ConfigItems->Push(OutFile, 1);
		OutFile << "end~InventoryProjectRc" << endl;

		OutFile.close();
	}
	void InventoryProjectRc::Load()
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
					else if (ThisParts[1] == Images->Name)
						Images->Fill(InFile);
					else
						InFile.seekg(ThisPos);
				}
			}
		}
	}

	Component* InventoryProjectRc::GetFromID(unsigned long long ID, int Filter) const
	{
		if (ID == 0)
			return nullptr;

		Component* Return = nullptr;

		if (InventoryItems && (Filter & CT_InventoryItem))
		{
			for (uint i = 0; i < InventoryItems->Count; i++)
			{
				InventoryItem* This = InventoryItems->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}
		if (OperationInventoryItems && (Filter & CT_OperationInventoryItem))
		{
			for (uint i = 0; i < OperationInventoryItems->Count; i++)
			{
				OperationInventoryItem* This = OperationInventoryItems->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}

		if (Images && (Filter & CT_Image))
		{
			bool Imgs = Filter & CT_Image;
			for (uint i = 0; i < Images->Count; i++)
			{
				Image* This = Images->Item(i);
				if (Imgs && This->ID == ID)
					Return = This;				
			}
		}

		if (ConfigItems && (Filter & CT_ConfigItem))
		{
			for (uint i = 0; i < ConfigItems->Count; i++)
			{
				ConfigItem* This = ConfigItems->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}
		if (RefrenceGroups && (Filter & CT_RefrenceGroup))
		{
			for (uint i = 0; i < RefrenceGroups->Count; i++)
			{
				RefrenceGroup* This = RefrenceGroups->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}

		if (Return && !(Filter & Return->ObjectType()))
			return nullptr;

		return Return;
	}
}