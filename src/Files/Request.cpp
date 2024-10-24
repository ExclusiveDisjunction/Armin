#include "Components.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace Armin::Files
{
	Request::Request(ProjectBase* File, RequestList* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_Parent = ParentList->Parent;
		_ParentList->Append(this);
	}
	Request::Request(ProjectBase* File, RequestList* ParentList, Request* Obj) : Component(File, true)
	{
		_ParentList = ParentList;
		_Parent = ParentList->Parent;
		_ParentList->Append(this);

		if (Obj)
		{
			Title(Obj->Title());
			Creator = new ComponentReference(Obj->Creator);
			ForUsers = Obj->ForUsers;
			RequestMsg = Obj->RequestMsg;
			Completed = Obj->Completed;
			CompleteMsg = Obj->CompleteMsg;
		}
	}
	Request::Request(ProjectBase* File, RequestList* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_Parent = ParentList->Parent;
		_ParentList->Append(this);

		Fill(InFile);
	}
	Request::~Request()
	{
		_ParentList->Pop(this);

		delete Creator;
		Creator = nullptr;
	}

	void Request::Fill(ifstream& InFile)
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
				Temp.RemoveAllOf(L'\t');
				if (Temp == "")
					continue;

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
				Value += L':' + ArgPart[j];

			if (Name == "Title") _Title = (String)Value;
			else if (Name == "ID") _ID = Value.ToULong();
			else if (Name == "ForUsers") ForUsers = ReferenceList(Value, ParentFile());
			else if (Name == "RequestMsg") RequestMsg = (String)(Value.ReplaceChar('`', '\n'));
			else if (Name == "Completed") Completed = Value == "True";
			else if (Name == "CompleteMsg") CompleteMsg = (String)(Value.ReplaceChar('`', '\n'));
			else if (Name == "Creator") 
			{
				if (Creator)
					delete Creator;
				
				unsigned long long ThisID = Value.ToULong();
				Component* Obj = _ParentFile->GetFromID(ThisID, CT_User);
				Creator = new ComponentReference(Obj);
			}
		}
	}
	void Request::Push(ofstream& OutFile, uint TabIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < TabIndex; i++)
			TabIndexValue += '\t';

		OutFile << TabIndexValue << "begin~Request" << "~Title:" << Title() << "~ID:" << ID << "~ForUsers:" << ForUsers.ToAString() << "~RequestMsg:" << RequestMsg.ReplaceChar(L'\n', L'`') << "~Completed:" << (Completed ? "True" : "False") << "~CompleteMsg:" << CompleteMsg.ReplaceChar(L'\n', L'`') << "~Creator:" << (!Creator ? "0" : Creator->ToString()) << "~end" << endl;
	}
}