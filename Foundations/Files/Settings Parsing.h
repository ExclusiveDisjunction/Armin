#pragma once

#ifndef Phosphor_SettingsParsing
#define Phosphor_SettingsParsing

#include "PHS.h"
#include "Stream.h"
#include "..\Str.h"
#include "..\Container.h"

/*
 * SettingsParser.h && SettingsParser.cpp
 * These files are used to parse any PHS file used for settings. 
 * Contains these classes:
 *  - SettingsParser - The SessionBase class that enumerates to other objects.
 *  - SettingsItem - Represents a single data type for the Parser. This stores the acutal settings.
 *  - SettingsItemList - A ComponentList<SettingsItem> for the SettingsItem.
 *  - SettingsItemParent - A SettingsItem class for parening.
 *
 * NOTICE: Complies to the 3.1.0.0 Component Model Standards.
 *
 * Last Modified on: 7.16.2020
 * Last Lib Version Modified: 2.0.0.0
*/

enum SettingsItemTypes
{
	Setting,
	Recent,
	Config
};

class SettingsParser;
class SettingsItemList;
class SettingsItemParent;
class SettingsItem;

class SettingsItem
{
private:
	SettingsParser* _File;

	SettingsItem(SettingsParser* File)
	{
		_File = File;
	}

	void Fill(std::wifstream& InFile);
	void Push(std::wofstream& OutFile, uint PreIndex = 0) const;
public:
	friend SettingsItemList;

	SettingsItem* Next = nullptr, * Last = nullptr;

	String Name, Value, FileTypeFilter;
	SettingsItemTypes Type;
};
class SettingsItemList
{
private:
	SettingsItem* First, * Last;
	uint _Size;
	SettingsParser* _File;

	void InternalAdd(SettingsItem* New);
	void Pop(SettingsItem*& Obj);
public:
	SettingsItemList(SettingsParser* Parser);
	~SettingsItemList()
	{
		Clear();
	}

	friend SettingsItemParent;

	SettingsItem* operator[](uint Index) const;
	SettingsItem* Item(uint Index) const
	{
		return operator[](Index);
	}
	const uint& Count = _Size;

	SettingsItem* Add();
	SettingsItem* Add(SettingsItemTypes Type, String Name, String Value = String(), String FileTypeFilter = String());
	void Remove(SettingsItem*& Obj);
	void Remove(String Name, SettingsItemTypes Type, String Value, String FileTypeFilter);
	void Clear();

	bool Contains(SettingsItem*& Obj) const;
	uint IndexOf(SettingsItem*& Obj) const;

	void Fill(std::wifstream& InFile);
	void Push(std::wofstream& OutFile, uint PreIndex = 0) const;

	operator Vector<SettingsItem*>() const
	{
		Vector<SettingsItem*> Return;
		SettingsItem* Current = First;
		for (uint i = 0; i < _Size; i++)
			Return.Add(Current);

		return Return;
	}
};
class SettingsItemParent
{
protected:
	SettingsItemList* _Items;
public:
	~SettingsItemParent()
	{
		delete _Items;
	}

	virtual SettingsItemList* SettingsItems()
	{
		return _Items;
	}
};

class SettingsParser : public SettingsItemParent
{
private:
	String _Path;
	SettingsParser(String Path);

public:
	static const wchar_t* DisregardFileFilter;

	static SettingsParser* Create(String Path);
	static SettingsParser* Load(String Path);

	void Save();
	void Fill();

	SettingsItem* FindObject(SettingsItemTypes Type, String Name, String FileTypeFilter = DisregardFileFilter); // text '****' in a FileTypeFilter tells the proc to disregard that parameter.
	Vector<SettingsItem*> FindMultipleObjects(SettingsItemTypes Type, String Name, String FileTypeFilter = DisregardFileFilter);

	StringList GetAllValues() const;
};

#endif