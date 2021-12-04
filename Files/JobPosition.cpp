#include "Components.h"

using namespace std;

namespace Armin::Files
{
	JobPosition::JobPosition(ArminSessionBase* File, JobPositionList* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	JobPosition::JobPosition(ArminSessionBase* File, JobPositionList* ParentList, JobPosition* ToClone) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			Description = ToClone->Description;
			Title(ToClone->Title());
		}
	}
	JobPosition::JobPosition(ArminSessionBase* File, JobPositionList* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	JobPosition::~JobPosition()
	{
		_ParentList->Pop(this);
	}

	void JobPosition::Fill(ifstream& InFile)
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
			AStringList ArgPart = Parts[i].Split(':');
			if (ArgPart.Size < 2)
				continue;

			AString Name = ArgPart[0], Value = ArgPart[1];
			for (uint j = 2; j < ArgPart.Size; j++)
				Value += ':' + ArgPart[j];

			if (Name == "Title") _Title = (String)Value;
			else if (Name == "Description") Description = (String)Value.ReplaceChar('`', '\n');
			else if (Name == "ID") _ID = Value.ToLong();
		}
	}
	void JobPosition::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		OutFile << TabIndexValue << "begin~JobPosition";
		OutFile << "~Title:" << _Title << "~Description:" << Description.ReplaceChar('\n', '`') << "~ID:" << _ID;

		OutFile << "~end" << endl;
	}
}