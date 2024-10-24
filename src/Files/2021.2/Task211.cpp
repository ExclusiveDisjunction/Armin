#include "Components.h"

#include "ComponentRefrence.h"

using namespace std;

namespace Armin::Files::V2021
{
	Task::Task(ArminSessionBase* File, TaskList* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	Task::Task(ArminSessionBase* File, TaskList* ParentList, Task* ToClone) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			Title(ToClone->Title());
			Instructions = ToClone->Instructions;
			RequiresAssurance = ToClone->RequiresAssurance;
			DueBy = ToClone->DueBy;
			AssignedTo = ToClone->AssignedTo;
		}
	}
	Task::Task(ArminSessionBase* File, TaskList* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	Task::~Task()
	{
		_ParentList->Pop(this);
	}

	void Task::Fill(ifstream& InFile)
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

			if (Name == "Title") Title((String)Value);
			else if (Name == "Instructions") Instructions = (String)Value.ReplaceChar('`', '\n');
			else if (Name == "RequiresAssurance") RequiresAssurance = (Value == "True");
			else if (Name == "DueBy") DueBy = DateTime::FromBackStringA(Value);
			else if (Name == "ID") _ID = Value.ToLong();
			else if (Name == "AssignedTo") AssignedTo = ReferenceList(Value, _ParentFile);
		}
	}
	void Task::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		AString Header = TabIndexValue + "begin~Task";
		OutFile << Header;
		OutFile << "~Title:" << Title() << "~Instructions:" << Instructions.ReplaceChar('\n', '`') << "~RequiresAssurance:" << (RequiresAssurance ? "True" : "False") << "~DueBy:" << DueBy.ToBackString() << "~ID:" << ID << "~AssignedTo:" << AssignedTo.ToString() << "~end" << endl;
	}
}