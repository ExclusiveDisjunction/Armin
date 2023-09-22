#include "PHS.h"

#include "..\Arithmetic.h"

using namespace std;

_PHSArgCollection::_PHSArgCollection()
{
}
_PHSArgCollection::~_PHSArgCollection()
{
	Clear();
}

_PHSArg* _PHSArgCollection::GetAtIndex(uint Index) const
{
	if (Index >= _Size)
		return nullptr;

	if (Index == 0 && _Size >= 1)
		return First;
	else if (Index == (_Size - 1) && _Size >= 1)
		return Last;

	Index++;
	PHSArg Current = First;
	for (uint i = 1; i < Index; i++)
		Current = Current->NextObj;

	return Current;
}
_PHSArg* _PHSArgCollection::Add(String Raw)
{
	using seg = _PHSArg*;

	seg Last = this->Last;
	seg New = new _PHSArg(Raw);

	New->LastObj = Last;
	New->NextObj = nullptr;

	if (_Size == 0)
		First = New;
	else
		Last->NextObj = New;

	_Size++;
	this->Last = New;
	return New;
}

void _PHSArgCollection::OutputAll(wostream& Out)
{
	if (_Size == 0)
		return;

	using seg = _PHSArg*;

	seg Current = this->First;
	for (uint i = 0; i < _Size; i++)
	{
		Out << L'~' << Current->_Text;
		Current = static_cast<_PHSArg*>(Current->NextObj);
	}
}

uint _PHSArgCollection::Count() const
{
	return _Size;
}
_PHSArg* _PHSArgCollection::operator[](uint Index) const
{
	if (Index >= Count() || Index < 0)
		return nullptr;

	return GetAtIndex(Index);
}
_PHSArg* _PHSArgCollection::operator[](String Name) const
{
	_PHSArg* Current = First;
	for (uint i = 0; i < _Size; i++)
	{
		if (Current->Name() == Name)
			return Current;
		Current = dynamic_cast<_PHSArg*>(Current->NextObj);
	}

	return nullptr;
}

bool _PHSArgCollection::Remove(uint Index)
{
	using seg = _PHSArg*;

	if (Index >= _Size)
		return false;

	if (Index == 0)
	{
		seg Second = First->NextObj;
		delete First;
		if (Second)
			Second->LastObj = 0;

		First = Second;
		_Size--;
		return true;
	}
	else if (Index == (_Size - 1))
	{
		seg SecondLast = Last->NextObj;
		delete Last;
		if (SecondLast)
			SecondLast->NextObj = 0;

		Last = SecondLast;
		_Size--;
		return true;
	}

	seg Target = GetAtIndex(Index);
	seg Next = Target->NextObj;
	seg Last = Target->LastObj;

	if (Last)
		Last->NextObj = Next;
	Next->LastObj = Last;

	delete Target;
	_Size--;

	return true;
}
bool _PHSArgCollection::Remove(_PHSArg* Obj)
{
	if (!Contains(Obj))
		return false;

	uint Index = IndexOf(Obj);
	return Remove(Index);
}

bool _PHSArgCollection::Contains(_PHSArg* Arg) const
{
	for (uint i = 0; i < _Size; i++)
	{
		if (operator[](i) == Arg)
			return true;
	}

	return false;
}
bool _PHSArgCollection::Contains(String Name) const
{
	for (uint i = 0; i < _Size; i++)
	{
		if (operator[](i)->Name() == Name)
			return true;
	}

	return false;
}
uint _PHSArgCollection::IndexOf(_PHSArg* Arg) const
{
	for (uint i = 0; i < _Size; i++)
	{
		if (operator[](i) == Arg)
			return i;
	}

	return uintmax;
}

void _PHSArgCollection::Clear()
{
	using seg = _PHSArg*;

	if (_Size == 0)
		return;

	seg Current = this->Last;
	for (uint i = 0; i < _Size; i++)
	{
		seg Temp = Current->LastObj;
		delete Current;
		Current = Temp;
	}

	_Size = 0;
	First = nullptr;
	Last = nullptr;
}