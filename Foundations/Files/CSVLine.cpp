#include "CSV.h"

_CSVLine::_CSVLine(String Raw)
{
	StringList Parts = Raw.Split(L',');
	_Elem = new _CSVElementList(Parts);
}
_CSVLine::~_CSVLine()
{
	if (_Elem)
		delete _Elem;

	_Elem = nullptr;
}

String _CSVLine::Raw() const
{
	if (!_Elem)
		return String();

	String Return = (_Elem->Count() == 0 ? L"" : _Elem->Item(0u)->Value());
	for (uint i = 1; i < _Elem->Count(); i++)
		Return += L',' + _Elem->Item(i)->Value();

	return Return;
}

_CSVLineList::_CSVLineList(StringList Raw)
{
	for (String Item : Raw)
		_Data.Add(new _CSVLine(Item));
}
_CSVLineList::~_CSVLineList()
{
	for (CSVLine Item : _Data)
		delete Item;

	_Data.Clear();
}

_CSVLine* _CSVLineList::Add()
{
	_CSVLine* Return = new _CSVLine(L"");
	_Data.Add(Return);

	return Return;
}
bool _CSVLineList::Remove(_CSVLine*& Obj)
{
	if (!Contains(Obj))
		return false;

	uint Index = IndexOf(Obj);
	return RemoveAt(Index);
}
bool _CSVLineList::RemoveAt(uint Index)
{
	if (Index >= Count())
		return false;

	_Data.RemoveAt(Index);

	return true;
}

bool _CSVLineList::Contains(_CSVLine*& Obj) const
{
	return _Data.Contains(Obj);
}
uint _CSVLineList::IndexOf(_CSVLine*& Obj) const
{
	return _Data.IndexOf(Obj);
}