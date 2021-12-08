#include "Components.h"

#include "AString.h"
#include "ArminSessions.h"
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
			Begin = ToClone->Begin;
			End = ToClone->End;
			Title(ToClone->Title());
		}
	}
	Image::~Image()
	{
		_ParentList->Pop(this);
	}

	HGLOBAL Image::GetHandle()
	{
		ResourceSystem* System = dynamic_cast<ResourceSystem*>(_ParentFile);
		if (!System)
			return nullptr;

		AString Path = System->ResourcePath;
		ifstream InFile(Path, ios::binary | ios::beg);
		if (!InFile)
			return nullptr;
		
		streampos& Begin = this->Begin, & End = this->End;
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
	void Image::SetHandle(HGLOBAL ToCopy)
	{	
		unsigned long long Size = GlobalSize(ToCopy);
		void* Data = GlobalLock(ToCopy);

		SetHandle(Data, Size);
	}
	void Image::SetHandle(void* Data, unsigned long long Size)
	{	
		streampos& Begin = this->Begin, & End = this->End;
		unsigned long long ThisSize = (unsigned long long)(End - Begin);

		ResourceSystem* System = dynamic_cast<ResourceSystem*>(_ParentFile);
		if (!System)
			return;

		AString Path = System->ResourcePath;
		ofstream OutFile(Path, ios::binary | ios::app);
		if (!OutFile)
			return;

		if (ThisSize > Size)
		{
			OutFile.seekp(0, ios::end);
			Begin = OutFile.tellp();
		}

		OutFile.write((char*)Data, Size);
		End = OutFile.tellp();
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

			if (Name == "Begin") Begin = (streampos)Value.ToULong();
			else if (Name == "End") End = (streampos)Value.ToULong();
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
		OutFile << "~Begin:" << (unsigned long long)Begin << "~End:" << (unsigned long long)End << "~Title:" << (AString)Title() << "~ID:" << ID << "~end" << endl;
	}
}