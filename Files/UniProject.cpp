#include "ArminSessions.h"

#include <iostream>
#include <fstream>
using namespace std;

namespace Armin::Files
{
	UniProject::UniProject() 
	{

	}
	UniProject::UniProject(String Path)
	{

	}

	void UniProject::ConfigureMemory()
	{
		delete InventoryItems;
		delete OperationInventoryItems;
		delete Users;
		delete Positions;
		delete Tasks;
		delete CompletedTasks;
		delete Images;

		if (Config & UPC_Users)
		{
			Positions = new JobPositionList(this, this);
			Users = new UserSet(this, this);
		}
		if (Config & UPC_Tasks)
		{
			Tasks = new TaskList(this, this);
			CompletedTasks = new CompletedTaskList(this, this);
		}
		if (Config & UPC_Inventory)
		{
			InventoryItems = new InventoryItemGroup(this, this);
			OperationInventoryItems = new OperationInventoryItemGroup(this, this);
		}
		if (Config & UPC_Resource)
			Images = new ImageList(this, this);
	}

	void UniProject::Save()
	{
		ofstream OutFile((AString)Path, ios::trunc | ios::beg);

		OutFile << "begin~UniProject" << "~ID:" << CurrentID << "~Config:" << Config << endl;

		if (Images)
			Images->Push(OutFile, 1);
		if (ConfigItems)
			ConfigItems->Push(OutFile, 1);
		if (Positions)
			Positions->Push(OutFile, 1);
		if (Users)
			Users->Push(OutFile, 1);
		if (Tasks)
			Tasks->Push(OutFile, 1);
		if (CompletedTasks)
			CompletedTasks->Push(OutFile, 1);
		if (InventoryItems)
			InventoryItems->Push(OutFile, 1);
		if (OperationInventoryItems)
			OperationInventoryItems->Push(OutFile, 1);
		if (RefrenceGroups)
			RefrenceGroups->Push(OutFile, 1);

		OutFile << "end~UniProject";
	}
	void UniProject::Load()
	{
		ifstream InFile((AString)Path, ios::beg);

		AString Header;
		getline(InFile, Header);

		AStringList Parts = Header.Split('~');
		bool Multiline = Parts[Parts.Size - 1] != "end";

		uint Size = Multiline ? Parts.Size : Parts.Size - 1;
		for (uint i = 2; i < Size; i++)
		{
			
		}

		SetID(Parts[2].ToLong());
		Config = Parts[3].ToLong();
		ConfigureMemory();

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
					if (ThisParts[0] != "begin")
						continue;

					InFile.seekg(PrePos);
					if (ThisParts[1] == Positions->Name && (Config & UPC_Users))
						Positions->Fill(InFile);
					else if (ThisParts[1] == Users->Name && (Config & UPC_Users))
						Users->Fill(InFile);
					else if (ThisParts[1] == InventoryItems->Name && (Config & UPC_Inventory))
						InventoryItems->Fill(InFile);
					else if (ThisParts[1] == OperationInventoryItems->Name && (Config & UPC_Inventory))
						OperationInventoryItems->Fill(InFile);
					else if (ThisParts[1] == Tasks->Name && (Config & UPC_Tasks))
						Tasks->Fill(InFile);
					else if (ThisParts[1] == CompletedTasks->Name && (Config & UPC_Tasks))
						CompletedTasks->Fill(InFile);
					else if (ThisParts[1] == RefrenceGroups->Name)
						RefrenceGroups->Fill(InFile);
					else if (ThisParts[1] == ConfigItems->Name)
						ConfigItems->Fill(InFile);
					else if (ThisParts[1] == Images->Name && (Config & UPC_Resource))
						Images->Fill(InFile);
					else
						InFile.seekg(ThisPos);
				}
			}
		}
	}

	Component* UniProject::GetFromID(unsigned long long ID, int Filter) const
	{
		Component* Return = nullptr;
		if (ID == 0)
			return nullptr;

		if (ConfigItems && (Filter & CT_ConfigItem))
		{
			for (uint i = 0; i < ConfigItems->Count; i++)
			{
				ConfigItem* Current = ConfigItems->Item(i);
				if (Current->ID == ID)
					Return = Current;
			}
		}

		if (Users && ((Filter & CT_User) || (Filter & CT_TimecardEntry)))
		{
			bool User = Filter & CT_User;
			bool Timecard = Filter & CT_TimecardEntry;

			for (uint i = 0; i < Users->Count; i++)
			{
				class User* Current = Users->Item(i);
				if (Current->ID == ID && Users)
					Return = Current;

				if (Timecard)
				{
					for (uint i = 0; i < Current->TimecardEntries->Count; i++)
					{
						TimecardEntry* Time = Current->TimecardEntries->Item(i);
						if (Time->ID == ID)
							Return = Current;
					}
				}
			}
		}

		if (Positions && (Filter & CT_JobPosition))
		{
			for (uint i = 0; i < Positions->Count; i++)
			{
				JobPosition* Current = Positions->Item(i);
				if (Current->ID == ID)
					Return = Current;
			}
		}

		InventoryItemGroup* Inventory = InventoryItems;
		if (Inventory && (Filter & CT_InventoryItem))
		{
			for (uint i = 0; i < Inventory->Count; i++)
			{
				InventoryItem* Current = Inventory->Item(i);
				if (Current->ID == ID)
					Return = Current;
			}
		}

		OperationInventoryItemGroup* OperationInventory = OperationInventoryItems;
		if (OperationInventory && (Filter & CT_OperationInventoryItem))
		{
			for (uint i = 0; i < OperationInventory->Count; i++)
			{
				OperationInventoryItem* Current = OperationInventory->Item(i);
				if (Current->ID == ID)
					Return = Current;
			}
		}

		if (Tasks && (Filter & CT_Task))
		{
			for (uint i = 0; i < Tasks->Count; i++)
			{
				Task* Current = Tasks->Item(i);
				if (Current->ID == ID)
					Return = Current;
			}
		}

		if (RefrenceGroups && (Filter & CT_RefrenceGroup))
		{
			for (uint i = 0; i < RefrenceGroups->Count; i++)
			{
				RefrenceGroup* Group = RefrenceGroups->Item(i);
				if (Group->ID == ID)
					Return = Group;
			}
		}

		if (Images && (Filter & CT_Image))
		{
			for (uint i = 0; i < Images->Count; i++)
			{
				Image* This = Images->Item(i);
				if (This->ID == ID)
					Return = This;
			}
		}

		if (CompletedTasks && ((Filter & CT_CompletedTask) || (Filter & CT_Task) || (Filter & CT_OperationInventoryItem)))
		{
			bool Comp = Filter & CT_CompletedTask;
			bool Tasks = Filter & CT_Task;
			bool Oper = Filter & CT_OperationInventoryItem;

			for (uint i = 0; i < CompletedTasks->Count; i++)
			{
				CompletedTask* Item = CompletedTasks->Item(i);
				if (Item->ID == ID && Comp) //This is done here because Tasks must be checked within each Completed Task
					Return = Item;

				if (Item->Tasks && Tasks)
				{
					TaskList* TaskL = Item->Tasks;
					for (uint i = 0; i < TaskL->Count; i++)
					{
						Task* Current = TaskL->Item(i);
						if (Current->ID == ID)
							Return = Current;
					}
				}

				if (Item->OperationInventoryItems && Oper)
				{
					OperationInventoryItemGroup* OperItems = Item->OperationInventoryItems;
					for (uint i = 0; i < OperItems->Count; i++)
					{
						OperationInventoryItem* Current = OperItems->Item(i);
						if (Current->ID == ID)
							Return = Current;
					}
				}
			}
		}

		return Return;
	}
}