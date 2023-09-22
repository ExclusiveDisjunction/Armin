#include "Settings Parsing.h"

#include "..\Arithmetic.h"

#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

const wchar_t* SettingsParser::DisregardFileFilter = L"****";

SettingsParser::SettingsParser(String Path)
{
	_Path = Path;
	Fill();
}

SettingsParser* SettingsParser::Create(String Path)
{
	wofstream OutFile(Path);
	OutFile << L"begin~SettingsFile" << endl << L"\tbegin~SettingsItemList~end" << endl << L"end~SettingsFile";
	OutFile.close();

	return new SettingsParser(Path);
}
SettingsParser* SettingsParser::Load(String Path)
{
	if (!FileExists(Path))
		return nullptr;
	return new SettingsParser(Path);
}

SettingsItem* SettingsParser::FindObject(SettingsItemTypes Type, String Name, String FileTypeFilter)
{
	SettingsItemList* List = _Items;
	SettingsItem* Return = nullptr;
	for (uint i = 0; i < List->Count; i++)
	{
		SettingsItem* Item = List->Item(i);
		if (Item->Type == Type && Item->Name == Name && (FileTypeFilter == L"****" ? true : Item->FileTypeFilter == FileTypeFilter))
			Return = Item;
	}
	return Return;
}
Vector<SettingsItem*> SettingsParser::FindMultipleObjects(SettingsItemTypes Type, String Name, String FileTypeFilter)
{
	SettingsItemList* List = _Items;
	Vector<SettingsItem*> Return;
	for (uint i = 0; i < List->Count; i++)
	{
		SettingsItem* Item = List->Item(i);
		if (Item->Type == Type && Item->Name == Name && (FileTypeFilter == L"****" ? true : Item->FileTypeFilter == FileTypeFilter))
			Return.Add(Item);

	}
	return Return;
}

void SettingsParser::Save()
{
	wofstream OutFile(_Path);

	OutFile << L"begin~SettingsParser" << endl;

	if (_Items)
		_Items->Push(OutFile, 1);

	OutFile << L"end~SettingsParser";
	OutFile.close();
}
void SettingsParser::Fill()
{
	wifstream InFile(_Path);

	String Header;
	getline(InFile, Header);

	StringList Parts = Header.Split(L'~');
	bool Multiline = Parts[Parts.Size - 1] != L"end";

	_Items = new SettingsItemList(this);

	if (Multiline)
	{
		while (!InFile.eof())
		{
			String ThisLine;
			streampos PrePos = InFile.tellg();
			getline(InFile, ThisLine);

			String Temp = ThisLine;
			Temp.RemoveAllOf(L'\t');
			if (Temp == L"")
				continue;

			StringList ThisParts = Temp.Split(L'~');
			if (ThisLine.TabIndex() == 0 && ThisParts[1] == L"SettingsParser" && ThisParts[0] == L"end")
				break;

			else if (ThisLine.TabIndex() - 1 == 0)
			{
				if (ThisParts[1] != L"SettingsItemList" || ThisParts[0] != L"begin")
					continue;

				InFile.seekg(PrePos);
				_Items->Fill(InFile);

			}
		}
	}
}

StringList SettingsParser::GetAllValues() const
{
	SettingsItemList* Temp = _Items;
	StringList Return;

	for (uint i = 0; i < Temp->Count; i++)
		Return.Add(Temp->Item(i)->Value);

	return Return;
}

//SettingsItem

void SettingsItem::Fill(wifstream& InFile)
{
	String Header;
	getline(InFile, Header);

	StringList Parts = Header.Split(L'~');
	bool Multiline = Parts[Parts.Size - 1] != L"end";

	if (Multiline)
		return;

	for (uint i = 2; i < Parts.Size - 1; i++)
	{
		StringList ArgPart = Parts[i].Split(L':');
		if (ArgPart.Size < 2)
			continue;

		String Name = ArgPart[0], Value = ArgPart[1];
		for (uint j = 2; j < ArgPart.Size; j++)
			Value += L':' + ArgPart[j];

		if (Name == L"Name") this->Name = Value;
		else if (Name == L"Value") this->Value = Value;
		else if (Name == L"FileTypeFilter") FileTypeFilter = Value;
		else if (Name == L"Type") Type = (SettingsItemTypes)Value.ToInt();
	}
}
void SettingsItem::Push(wofstream& OutFile, uint PreIndex) const
{
	String TabIndexValue;
	for (uint i = 0; i < PreIndex; i++)
		TabIndexValue += L"\t";

	String Header = TabIndexValue + L"begin~SettingsItem";
	OutFile << Header;
	OutFile << L"~Name:" << Name << L"~Value:" << Value << L"~Type:" << (int)Type << L"~FileTypeFilter:" << FileTypeFilter << L"~end" << endl;
}

//SettingsItemList

SettingsItemList::SettingsItemList(SettingsParser* File)
{
	_File = File;
}

SettingsItem* SettingsItemList::operator[](uint Index) const
{
	if (Index >= _Size)
		return nullptr;

	if (_Size == 0)
		return nullptr;
	else if ((_Size == 1 && Index == 0) || (_Size == 2 && Index == 0))
		return First;
	else if (_Size == 2 && Index == 1)
		return Last;

	SettingsItem* Current = First;
	Index++;

	for (uint i = 1; i < Index; i++)
		Current = Current->Next;

	return Current;
}

SettingsItem* SettingsItemList::Add()
{
	return Add(SettingsItemTypes::Setting, L"");
}
SettingsItem* SettingsItemList::Add(SettingsItemTypes Type, String Name, String Value, String FileTypeFilter)
{
	SettingsItem* New = new SettingsItem(_File);

	New->Type = Type;
	New->Name = Name;
	New->Value = Value;
	New->FileTypeFilter = FileTypeFilter;

	InternalAdd(New);

	return New;
}
void SettingsItemList::Remove(SettingsItem*& Obj)
{
	Pop(Obj);

	delete Obj;
	Obj = nullptr;
}
void SettingsItemList::Remove(String Name, SettingsItemTypes Type, String Value, String FileTypeFilter)
{
	SettingsItem* Current = First;
	SettingsItem* Target = nullptr;
	for (uint i = 0; i < _Size; i++)
	{
		if (Current->Name == Name && Current->Type == Type && Current->Value == Value && Current->FileTypeFilter == FileTypeFilter)
			Target = Current;

		Current = Current->Next;
	}

	if (Target)
	{
		Pop(Target);

		delete Target;
	}
}
void SettingsItemList::Clear()
{
	SettingsItem* Current = First;
	for (uint i = 0; i < _Size; i++)
	{
		SettingsItem* Temp = Current->Next;
		delete Current;
		Current = Temp;
	}
}

bool SettingsItemList::Contains(SettingsItem*& Obj) const
{
	SettingsItem* Current = First;
	for (uint i = 0; i < Count; i++)
	{
		if (Current == Obj)
			return true;

		Current = Current->Next;
	}

	return false;
}
uint SettingsItemList::IndexOf(SettingsItem*& Obj) const
{
	SettingsItem* Current = First;
	for (uint i = 0; i < Count; i++)
	{
		if (Current == Obj)
			return i;

		Current = Current->Next;
	}

	return uintmax;
}

void SettingsItemList::Fill(wifstream& InFile)
{
	String Header;
	getline(InFile, Header);

	StringList Parts = Header.Split(L'~');
	bool Multiline = Parts[Parts.Size - 1] != L"end";

	if (Multiline)
	{
		int TabIndex = Header.TabIndex();

		while (!InFile.eof())
		{
			String ThisLine;
			streampos PrePos = InFile.tellg();
			getline(InFile, ThisLine);

			String Temp = ThisLine;
			Temp.RemoveAllOf(L'\t');
			if (Temp == L"")
				continue;

			StringList ThisParts = Temp.Split(L'~');
			if (ThisLine.TabIndex() == TabIndex && ThisParts[1] == L"SettingsItemList" && ThisParts[0] == L"end")
				break;

			else if (ThisLine.TabIndex() - 1 == TabIndex)
			{
				InFile.seekg(PrePos);

				SettingsItem* Item = new SettingsItem(_File);
				Item->Fill(InFile);
				InternalAdd(Item);
			}
		}
	}
}
void SettingsItemList::Push(wofstream& OutFile, uint PreIndex) const
{
	String TabIndexValue;
	for (uint i = 0; i < PreIndex; i++)
		TabIndexValue += L"\t";

	String Header = TabIndexValue + L"begin~SettingsItemList";
	OutFile << Header;

	if (_Size != 0)
	{
		OutFile << endl;

		SettingsItem* Current = First;
		for (uint i = 0; i < _Size; i++)
		{
			Current->Push(OutFile, PreIndex + 1);
			Current = Current->Next;
		}

		OutFile << TabIndexValue << L"end~SettingsItemList" << endl;
	}
	else
		OutFile << L"~end" << endl;
}

void SettingsItemList::InternalAdd(SettingsItem* New)
{
	if (_Size == 0)
	{
		First = Last = New;
		_Size = 1;
	}

	else if (_Size == 1)
	{
		Last = New;

		First->Next = Last;
		Last->Last = First;

		_Size = 2;
	}

	else
	{
		SettingsItem* Prev = Last;
		Last = New;

		if (Prev)
			Prev->Next = Last;
		Last->Last = Prev;

		_Size++;
	}
}
void SettingsItemList::Pop(SettingsItem*& Obj)
{
	if (_Size == 1)
	{
		this->First = this->Last = nullptr;
		_Size = 0;

		return;
	}

	SettingsItem* Last = Obj->Last;
	SettingsItem* Next = Obj->Next;

	if (Last)
		Last->Next = Next;
	if (Next)
		Next->Last = Last;

	if (Obj == this->Last)
		this->Last = Last;
	else if (Obj == this->First)
		this->First = Next;

	_Size--;
}