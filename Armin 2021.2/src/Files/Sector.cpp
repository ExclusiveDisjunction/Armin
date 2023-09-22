#include "Components.h"

using namespace std;

namespace Armin::Files
{
	Sector::Sector(ArminSessionBase* File, SectorList* ParentList) : Sector(File, ParentList, nullptr) {}
	Sector::Sector(ArminSessionBase* File, SectorList* ParentList, ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	Sector::Sector(ArminSessionBase* File, SectorList* ParentList, Sector* ToClone) : Component(File, true)
	{
		Last = Next = nullptr;

		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		if (ToClone)
		{
			BeginPos = ToClone->BeginPos;
			EndPos = ToClone->EndPos;
		}
	}
	Sector::~Sector()
	{
		_ParentList->Pop(this);
	}

	void Sector::Fill(ifstream& InFile)
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
				if (Temp == AString())
					continue;

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

			if (Name == "BeginPos") BeginPos = (streampos)Value.ToULong();
			else if (Name == "EndPos") EndPos = (streampos)Value.ToULong();
			else if (Name == "ID") _ID = Value.ToULong();
		}
	}
	void Sector::Push(ofstream& OutFile, uint TabIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < TabIndex; i++)
			TabIndexValue += '\t';

		OutFile << TabIndexValue << "begin~Sector";
		OutFile << "~BeginPos:" << (streamoff)BeginPos << "~EndPos:" << (streamoff)EndPos << "~ID:" << ID << "~end" << endl;
	}
}