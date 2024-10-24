#pragma once

#include "..\Str.h"
#include "..\Container.h"

using std::wofstream;

class _CSVDoc;
class _CSVLine;
class _CSVLineList;
class _CSVElement;
class _CSVElementList;

_CSVDoc* LoadCSVDoc(String Path);
_CSVDoc* CreateCSVDoc(String Path);

template<typename T>
class CSVList
{
protected:
	Vector<T> _Data;

public:

	virtual T Add() = 0;
	virtual bool Remove(T& Obj) = 0;
	virtual bool RemoveAt(uint Index) = 0;

	virtual bool Contains(T& Obj) const = 0;
	virtual uint IndexOf(T& Obj) const = 0;

	virtual T& operator[](uint Index) const = 0;
	virtual T& Item(uint Index) const
	{
		return operator[](Index);
	}
	virtual uint Count() const
	{
		return _Data.Size;
	}
};

typedef class _CSVDoc
{
private:
	_CSVLineList* _Lines;
	String _Path;

	bool Loaded;

	_CSVDoc(String Path);
public:
	_CSVDoc()
	{
		Loaded = false;
	}
	~_CSVDoc();

	_CSVLineList* Lines()
	{
		return _Lines;
	}
	const String Path() const
	{
		return _Path;
	}

	void Save();

	friend _CSVDoc* LoadCSVDoc(String);
	friend _CSVDoc* CreateCSVDoc(String);
} *CSVDoc, *CSVDocument;

typedef class _CSVLine
{
private:
	_CSVElementList* _Elem;

	_CSVLine(String Raw);

public:
	_CSVLine()
	{
		_Elem = nullptr;
	}
	~_CSVLine();
	friend _CSVLineList;

	String Raw() const;
	_CSVElementList* Elements()
	{
		return _Elem;
	}
} *CSVLine;
typedef class _CSVLineList : public CSVList<_CSVLine*>
{
public:
	_CSVLineList()
	{

	}
	_CSVLineList(String Raw) : _CSVLineList(Raw.Split(L'\n'))
	{
	}
	_CSVLineList(StringList Raw);
	~_CSVLineList();

	_CSVLine*& operator[](uint Index) const override
	{
		return _Data[Index];
	}

	_CSVLine* Add() override;
	bool Remove(_CSVLine*& Obj) override;
	bool RemoveAt(uint Obj) override;

	bool Contains(_CSVLine*& Obj) const override;
	uint IndexOf(_CSVLine*& Obj) const override;
} *CSVLineList;

typedef class _CSVElement
{
private:
	String _Value;

	_CSVElement(String Val)
	{
		_Value = Val;
	}
public:
	_CSVElement()
	{
		_Value = L"";
	}
	friend _CSVElementList;

	String Value() const
	{
		return _Value;
	}
	operator String() const
	{
		return _Value;
	}
} *CSVElement, *CSVElem;
typedef class _CSVElementList : public CSVList<_CSVElement*>
{
public:
	_CSVElementList()
	{

	}
	_CSVElementList(String Raw) : _CSVElementList(Raw.Split(L','))
	{
	}
	_CSVElementList(StringList Raw);
	~_CSVElementList();

	_CSVElement*& operator[](uint Index) const override
	{
		return _Data[Index];
	}

	_CSVElement* Add() override;
	bool Remove(_CSVElement*& Obj) override;
	bool RemoveAt(uint Index) override;

	bool Contains(_CSVElement*& Obj) const override;
	uint IndexOf(_CSVElement*& Obj) const override;
} *CSVElementList, *CSVElemList;