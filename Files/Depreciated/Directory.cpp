#include "Components.h"

#include "Files\Stream.h"

using namespace std;

namespace Armin::Files
{
	Directory::Directory(ProjectBase* File, DirectoryList* ParentList) : Directory(File, ParentList, nullptr) { }
	Directory::Directory(ProjectBase* File, DirectoryList* ParentList, Directory* ToClone) : Component(File, true), SectorParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		//Directories cannot be copied/cloned, as the data stored within each instance is not consisitent.
	}
	Directory::Directory(ProjectBase* File, DirectoryList* ParentList, ifstream& InFile) : Component(File, false), SectorParent(File)
	{
		_ParentList = ParentList;
		_ParentList->Append(this);
		_Parent = _ParentList->Parent;

		Fill(InFile);
	}
	Directory::~Directory()
	{
		_ParentList->Pop(this);
	}

	HGLOBAL Directory::ReadSector(Sector* ToRead)
	{
		if (ToRead->Parent != this)
			return nullptr;

		ifstream InFile(TargetPath, ios::binary | ios::beg);
		if (!InFile)
			return nullptr;

		streampos& Begin = ToRead->BeginPos, & End = ToRead->EndPos;
		unsigned long long Size = End - Begin;

		InFile.seekg(Begin);
		if (!InFile)
			return nullptr;
		
		HGLOBAL Return = GlobalAlloc(GMEM_MOVEABLE, Size);
		char* Data = (char*)GlobalLock(Return);

		InFile.read(Data, Size);

		GlobalUnlock(Return);
		InFile.close();
		return Return;
	}
	void Directory::WriteToSector(Sector* ToWrite, HGLOBAL Data, bool ForceSize)
	{
		unsigned long long Size = GlobalSize(Data);
		LPVOID Void = GlobalLock(Data);

		WriteToSector(ToWrite, (char*)Void, Size, ForceSize);

		GlobalUnlock(Data);
	}
	void Directory::WriteToSector(Sector* ToWrite, char* Data, unsigned long long Size, bool ForceSize)
	{
		if (ToWrite->Parent != this)
			return;

		streampos& Begin = ToWrite->BeginPos, &End = ToWrite->EndPos;
		unsigned long long ThisSize = (unsigned long long)(End - Begin);

		ofstream OutFile(TargetPath, ios::binary | ios::app);
		if (!OutFile)
			return;

		if (ThisSize > Size)
		{
			if (ForceSize)
				return;

			OutFile.seekp(0, ios::end);
			Begin = OutFile.tellp();
		}
		
		OutFile.write(Data, Size);
		End = OutFile.tellp();
	}
	void Directory::CompressSectors()
	{
		return;

		if (CopyFileA((const char*)TargetPath, static_cast<const char*>((AString)FilePath((String)TargetPath) + "\\oldfile.arminrc"), false) != 0)
			return;

		AString OldPath = "oldfile.arminrc";

		ofstream OutFile(TargetPath, ios::trunc | ios::binary);
		if (!OutFile)
			return;

		ifstream InFile(OldPath, ios::binary | ios::beg);
		if (!InFile)
			return;

		for (uint i = 0; i < Sectors->Count; i++)
		{
			Sector* This = Sectors->Item(i);

			streampos& Begin = This->BeginPos, & End = This->EndPos;

			InFile.seekg(Begin);
			streamoff Size = End - Begin;
			char* Data = new char[Size];

			InFile.read(Data, Size);

			Begin = OutFile.tellp();
			OutFile.write(Data, Size);
			End = OutFile.tellp();

			delete[] Data;
		}

		OutFile.close();
		InFile.close();
		DeleteFileA(static_cast<LPCSTR>(OldPath));
	}
	Sector* Directory::CreateSector(HGLOBAL Data)
	{
		unsigned long long Size = GlobalSize(Data);
		LPVOID Void = GlobalLock(Data);

		Sector* Return = CreateSector((char*)Void, Size);
		GlobalUnlock(Data);
		return Return;
	}
	Sector* Directory::CreateSector(unsigned long long Size)
	{
		return CreateSector(nullptr, Size);
	}
	Sector* Directory::CreateSector(char* Data, unsigned long long Size)
	{
		if (Size == 0)
			return nullptr;
		
		ofstream OutFile(TargetPath, ios::ate | ios::app | ios::binary);
		if (!OutFile)
			return nullptr;

		Sector* Return = new Sector(_ParentFile, Sectors);

		Return->BeginPos = OutFile.tellp();
		if (Data)
			OutFile.write(Data, Size);
		else
		{
			for (unsigned long long i = 0; i < Size; i++)
				OutFile << '\0';
		}

		Return->EndPos = OutFile.tellp();
		OutFile.close();

		return Return;
	}

	void Directory::Fill(ifstream& InFile)
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
					if (ThisParts[1] == Sectors->Name)
						Sectors->Fill(InFile);
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

			if (Name == "TargetPath") TargetPath = Value;
			else if (Name == "ID") _ID = Value.ToULong();
		}
	}
	void Directory::Push(ofstream& OutFile, uint TabIndex) const
	{
		AString TabIndexValue;
		for (uint i = 0; i < TabIndex; i++)
			TabIndexValue += '\t';

		OutFile << TabIndexValue << "begin~Directory";
		OutFile << "~TargetPath:" << TargetPath << "~ID:" << ID;

		if (Sectors->Count != 0)
		{
			OutFile << endl;
			Sectors->Push(OutFile, TabIndex + 1);

			OutFile << TabIndexValue << "end~Directory" << endl;
		}
		else
			OutFile << "~end" << endl;

	}
}