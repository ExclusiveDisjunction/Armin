#include "Components.h"

#include "ComponentRefrence.h"

using namespace std;

namespace Armin::Files::V2021
{
	User::User(ArminSessionBase* File, UserSet* ParentList) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		IsAdmin = IsAssurance = false;
		Username = FirstName = MiddleName = LastName = Password = String();
	}
	User::User(ArminSessionBase* File, UserSet* ParentList, User* ToClone) : Component(File, true)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			Username = ToClone->Username;
			Password = ToClone->Password;
			FirstName = ToClone->FirstName;
			MiddleName = ToClone->MiddleName;
			LastName = ToClone->LastName;
			IsAdmin = ToClone->IsAdmin;
			IsAssurance = ToClone->IsAssurance;
			TargetImage = new ComponentReference(ToClone->TargetImage);
			Positions = ToClone->Positions;
		}
	}
	User::User(ArminSessionBase* File, UserSet* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	User::~User()
	{		
		if (TargetImage)
			delete TargetImage;

		_ParentList->Pop(this);
	}

	void User::Fill(ifstream& InFile)
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
				Value += L':' + ArgPart[j];

			if (Name == "Username") Username = (String)Value;
			else if (Name == "Password") Password = (String)Value.ShiftEachChar(15);
			else if (Name == "FirstName") FirstName = (String)Value;
			else if (Name == "MiddleName") MiddleName = (String)Value;
			else if (Name == "LastName") LastName = (String)Value;
			else if (Name == "IsAdmin") IsAdmin = (Value == "True");
			else if (Name == "IsAsurance") IsAssurance = (Value == "True");
			else if (Name == "ID") _ID = Value.ToULong();
			else if (Name == "TargetImage")
			{
				unsigned long long ID = Value.ToULong();
				Component* Object = _ParentFile->GetFromID(ID, CT_Image);
				TargetImage = new ComponentReference(Object);
			}
			else if (Name == "Positions") Positions = ReferenceList(Value, _ParentFile);
		}
	}
	void User::Push(ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += "\t";

		AString Header = TabIndexValue + "begin~User";
		OutFile << Header;
		OutFile << "~Username:" << Username << "~FirstName:" << FirstName << "~MiddleName:" << MiddleName << "~LastName:" << LastName << "~IsAdmin:" << (IsAdmin ? "True" : "False") << "~IsAssurance:" << (IsAssurance ? "True" : "False") << "~Password:" << Password.ShiftEachChar(-15) << "~TargetImage:" << (!TargetImage || !TargetImage->Target() ? 0 : TargetImage->Target()->ID) << "~ID:" << ID << "~Positions:" << Positions.ToString() << "~end" << endl;
	}
}