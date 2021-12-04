#include "Components.h"

#include "ComponentRefrence.h"

using namespace std;

namespace Armin::Files
{
	CompletedTask::CompletedTask(ArminSessionBase* File, CompletedTaskList* ParentList) : Component(File, true), OperationInventoryItemParent(File), TaskParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	CompletedTask::CompletedTask(ArminSessionBase* File, CompletedTaskList* ParentList, CompletedTask* ToClone) : Component(File, true), TaskParent(File), OperationInventoryItemParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			DateCompleted = ToClone->DateCompleted;

			if (AssuredBy)
				delete AssuredBy;

			AssuredBy = new ComponentReference(ToClone->AssuredBy->Target());
			CompletedBy = ToClone->CompletedBy;
			ActionsDone = ToClone->ActionsDone;

			TaskList* Tasks = ToClone->Tasks;
			if (Tasks)
			{
				for (uint i = 0; i < Tasks->Count; i++)
					new Task(File, Tasks, Tasks->Item(i));
			}

			OperationInventoryItemGroup* Items = ToClone->OperationInventoryItems;
			if (Items)
			{
				for (uint i = 0; i < Items->Count; i++)
					new class OperationInventoryItem(File, OperationInventoryItems, Items->Item(i));
			}
		}
	}
	CompletedTask::CompletedTask(ArminSessionBase* File, CompletedTaskList* ParentList, std::ifstream& InFile) : Component(File, false), OperationInventoryItemParent(File), TaskParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	CompletedTask::~CompletedTask()
	{
		_ParentList->Pop(this);
	}

	String CompletedTask::Title() const
	{
		if (!Tasks || Tasks->Count == 0)
			return L",<>.";

		return Tasks->Item(0)->Title();
	}

	void CompletedTask::Fill(ifstream& InFile)
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

				else if (ThisLine.TabIndex() - 1 == TabIndex)
				{
					if (ThisParts[0] != "begin")
						continue;

					streampos Pos = InFile.tellg();
					InFile.seekg(PrePos);
					if (ThisParts[1] == Tasks->Name)
						Tasks->Fill(InFile);
					else if (ThisParts[1] == OperationInventoryItems->Name)
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

			if (Name == "DateCompleted") DateCompleted = DateTime::FromBackStringA(Value);
			else if (Name == "AssuredBy") AssuredBy = new ComponentReference(dynamic_cast<User*>(_ParentFile->GetFromID(Value.ToLong(), CT_User)));
			else if (Name == "ID") _ID = Value.ToLong();
			else if (Name == "CompletedBy") CompletedBy = ReferenceList(Value, _ParentFile);
			else if (Name == "ActionsDone") ActionsDone = String(Value);
		}
	}
	void CompletedTask::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		OutFile << TabIndexValue << "begin~CompletedTask";
		OutFile << "~DateCompleted:" << DateCompleted.ToBackStringA() << "~AssuredBy:" << (AssuredBy && AssuredBy->Target() != nullptr ? AssuredBy->Target()->ID : 0) << "~ID:" << _ID << "~CompletedBy:" << CompletedBy.ToString() << "~ActionsDone:" << ActionsDone;

		if (OperationInventoryItems || Tasks)
		{
			OutFile << endl;
			if (OperationInventoryItems)
				OperationInventoryItems->Push(OutFile, PreIndex + 1);
			if (Tasks)
				Tasks->Push(OutFile, PreIndex + 1);
			OutFile << TabIndexValue << "end~CompletedTask" << endl;
		}
		else
			OutFile << "~end" << endl;
	}
}