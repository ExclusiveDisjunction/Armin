#include "PHS.h"

#include "..\Arithmetic.h"

using namespace std;

_PHSNodeList::_PHSNodeList()
{
}
_PHSNodeList::~_PHSNodeList()
{
	Clear();
}

_PHSNode* _PHSNodeList::GetAtIndex(uint Index) const
{
	if (Index >= _Size)
		return nullptr;

	if (Index == 0 && _Size >= 1)
		return First;
	else if (Index == (_Size - 1) && _Size >= 1)
		return Last;

	Index++;
	_PHSNode* Current = First;
	for (uint i = 1; i < Index; i++)
		Current = Current->NextObj;

	return Current;
}
_PHSNode* _PHSNodeList::Add(_PHSNode* Parent, _PHSDoc* Doc)
{
	using seg = _PHSNode*;

	seg last = this->Last;
	seg New = new _PHSNode(Parent, Doc);

	New->LastObj = Last;
	New->NextObj = nullptr;

	if (_Size == 0)
		First = New;
	else
		last->NextObj = New;

	_Size++;
	this->Last = New;
	return New;
}

void _PHSNodeList::OutputAll(wostream& Out, uint TabIndex)
{
	if (_Size == 0)
		return;

	using seg = _PHSNode*;

	seg Current = this->First;
	for (uint i = 0; i < _Size; i++)
	{
		Current->Push(Out, TabIndex);
		Current = Current->NextObj;
	}
}

_PHSNode* _PHSNodeList::operator[](uint Index) const
{
	if (Index >= _Size || Index < 0)
		return nullptr;
	
	return GetAtIndex(Index);
}
_PHSNode* _PHSNodeList::operator[](String Name) const
{
	_PHSNode* Current = First;
	for (uint i = 0; i < _Size; i++)
	{
		if (Current->_Name == Name)
			return Current;
		Current = Current->NextObj;
	}

	return nullptr;
}

bool _PHSNodeList::Remove(uint Index)
{
	using seg = _PHSNode*;

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
		seg SecondLast = Last->LastObj;

		delete Last;
		Last = nullptr;
		Last = SecondLast;

		if (SecondLast)
			SecondLast->NextObj = 0;

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
bool _PHSNodeList::Remove(_PHSNode* Node)
{
	if (!Contains(Node))
		return false;

	uint Index = IndexOf(Node);
	return Remove(Index);
}

bool _PHSNodeList::Contains(_PHSNode* Node) const
{
	for (uint i = 0; i < _Size; i++)
	{
		if (operator[](i) == Node)
			return true;
	}

	return false;
}
bool _PHSNodeList::Contains(String Name) const 
{
	for (uint i = 0; i < _Size; i++)
	{
		if (operator[](i)->Name() == Name)
			return true;
	}

	return false;
}
uint _PHSNodeList::IndexOf(_PHSNode* Node) const
{
	for (uint i = 0; i < _Size; i++)
	{
		if (operator[](i) == Node)
			return i;
	}

	return uintmax;
}

void _PHSNodeList::Clear()
{
	using seg = _PHSNode*;

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
