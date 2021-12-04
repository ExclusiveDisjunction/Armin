#include "Components.h"

#include "AString.h"
#include "Files\Stream.h"
#include "..\Common.h"
#include "..\Config\Ins.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

namespace Armin::Files
{
	Image::Image(ProjectBase* File, ImageList* ParentList) : Image(File, ParentList, nullptr)
	{
	}
	Image::Image(ProjectBase* File, ImageList* ParentList, std::ifstream& InFile) : Component(File, false)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);

		Fill(InFile);
	}
	Image::Image(ProjectBase* File, ImageList* ParentList, Image* ToClone) : Component(File, !ToClone)
	{
		Next = Last = nullptr;

		_ParentList = ParentList;
		_ParentList->Append(this);

		if (ToClone)
		{
			TargetSector = new ComponentReference(ToClone->TargetSector);
			TargetDirectory = new ComponentReference(ToClone->TargetDirectory);
		}
	}
	Image::~Image()
	{
		_ParentList->Pop(this);
	}

	HGLOBAL Image::GetHandle()
	{
		if (!TargetDirectory || !TargetSector || !TargetDirectory->Target() || !TargetSector->Target())
			return nullptr;

		Directory* Dir = dynamic_cast<Directory*>(TargetDirectory->Target());
		Sector* Sector = dynamic_cast<class Sector*>(TargetSector->Target());

		return Dir->ReadSector(Sector);
	}
	void Image::SetHandle(HGLOBAL ToCopy)
	{	
		unsigned long long Size = GlobalSize(ToCopy);
		void* Data = GlobalLock(ToCopy);

		SetHandle(Data, Size);
	}
	void Image::SetHandle(void* Data, unsigned long long Size)
	{
		if (!TargetDirectory || !TargetSector || !TargetDirectory->Target() || !TargetSector->Target())
			return;

		Directory* Dir = dynamic_cast<Directory*>(TargetDirectory->Target());
		Sector* Sector = dynamic_cast<class Sector*>(TargetSector->Target());

		Dir->WriteToSector(Sector, Data, Size);
	}

	void Image::Fill(std::ifstream& InFile)
	{
		AString Header;
		streampos PrePos = InFile.tellg();
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

			if (Name == "TargetDirectory")
			{
				delete TargetDirectory;

				unsigned long long ID = Value.ToULong();
				Component* Target = _ParentFile->GetFromID(ID, CT_Directory);
				if (Target)
					TargetDirectory = new ComponentReference(Target);
				else
					TargetDirectory = nullptr;
			}
			else if (Name == "TargetSector")
			{
				delete TargetSector;

				unsigned long long ID = Value.ToULong();
				Component* Target = _ParentFile->GetFromID(ID, CT_Sector);
				if (Target)
					TargetSector = new ComponentReference(Target);
				else
					TargetSector = nullptr;
			}
			else if (Name == "Title") _Title = (String)Value;
			else if (Name == "ID") _ID = Value.ToULong();
		}
	}
	void Image::Push(std::ofstream& OutFile, uint PreIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < PreIndex; i++)
			TabIndexValue += L'\t';

		OutFile << TabIndexValue << "begin~Image";
		OutFile << "~TargetDirectory:" << (!TargetDirectory || !TargetDirectory->Target() ? 0 : TargetDirectory->Target()->ID) << "~TargetSector:" << (!TargetSector || !TargetSector->Target() ? 0 : TargetSector->Target()->ID) << "~Title:" << (AString)Title() << "~ID:" << ID << "~end" << endl;
	}
}