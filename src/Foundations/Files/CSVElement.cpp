#include "CSV.h"

_CSVElementList::_CSVElementList(StringList Raw)
{
	for (String Obj : Raw)
		_Data.Add(new _CSVElement(Obj));
}
_CSVElementList::~_CSVElementList()
{
	for (CSVElement Item : _Data)
		delete Item;

	_Data.Clear();
}

_CSVElement* _CSVElementList::Add()
{
	_CSVElement* Return = new _CSVElement(String());
	_Data.Add(Return);

	return Return;
}
bool _CSVElementList::Remove(_CSVElement*& Obj)
{
	if (!Contains(Obj))
		return false;

	uint Index = IndexOf(Obj);
	return RemoveAt(Index);
}
bool _CSVElementList::RemoveAt(uint Index)
{
	if (Index >= Count())
		return false;

	_Data.RemoveAt(Index);
	return true;
}

bool _CSVElementList::Contains(_CSVElement*& Obj) const
{
	return _Data.Contains(Obj);
}
uint _CSVElementList::IndexOf(_CSVElement*& Obj) const
{
	return _Data.IndexOf(Obj);
}