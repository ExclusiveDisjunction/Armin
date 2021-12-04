#include "Components.h"

#include "ComponentRefrence.h"

#include <iostream>
using namespace std;

namespace Armin::Files
{
	TimecardEntry::TimecardEntry(ProjectBase* File, TimecardEntryList* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;
	}
	TimecardEntry::TimecardEntry(ProjectBase* File, TimecardEntryList* ParentList, TimecardEntry* ToClone) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			TimeIn = ToClone->TimeIn;
			TimeOut = ToClone->TimeOut;
			Date = ToClone->Date;
			Description = ToClone->Description;
		}
	}
	TimecardEntry::TimecardEntry(ProjectBase* File, TimecardEntryList* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	TimecardEntry::~TimecardEntry()
	{
		_ParentList->Pop(this);
	}

	void TimecardEntry::Fill(ifstream& InFile)
	{
		AString Header;
		getline(InFile, Header);

		AStringList Parts = Header.Split('~');
		if (Parts.Size == 0)
			return;
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

		AString First = Parts[0], & Identifyer = Parts[1];
		First.RemoveAllOf(L'\t');
		if (First != "begin" || Identifyer != "TimecardEntry")
			return;

		uint Size = Multiline ? Parts.Size : Parts.Size - 1;
		for (uint i = 2; i < Size; i++)
		{
			AStringList ArgPart = Parts[i].Split(':');
			if (ArgPart.Size < 2)
				continue;

			AString Name = ArgPart[0], Value = ArgPart[1];
			for (uint j = 2; j < ArgPart.Size; j++)
				Value += ":" + ArgPart[j];

			if (Name == "ForUser")
			{
				unsigned long long ID = Value.ToULong();
				Component* Temp = _ParentFile->GetFromID(ID, CT_User);
				if (!Temp || !dynamic_cast<User*>(Temp))
					continue;
			}
			else if (Name == "TimeIn")
				TimeIn = DateTime::FromBackStringA(Value);
			else if (Name == "TimeOut")
				TimeOut = DateTime::FromBackStringA(Value);
			else if (Name == "Description")
				Description = (String)Value.ReplaceChar('`', '\n');
			else if (Name == "Date")
				Date = DateTime::FromBackStringA(Value);
			else if (Name == "ID")
				_ID = Value.ToULong();
		}
	}
	void TimecardEntry::Push(ofstream& OutFile, uint TabIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < TabIndex; i++)
			TabIndexValue += L'\t';

		OutFile << TabIndexValue << "begin~TimecardEntry";
		OutFile << "~TimeIn:" << TimeIn.ToBackString() << "~TimeOut:" << TimeOut.ToBackString() << "~Description:" << Description.ReplaceChar('\n', '`') << "~Date:" << Date.ToBackString() << "~ID:" << ID << "~end" << endl;
	}
}