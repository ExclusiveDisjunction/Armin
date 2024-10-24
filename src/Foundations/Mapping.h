#pragma once

#include <iostream>

#include "Arithmetic.h"
#include "Misc.h"

template<typename TKey, typename TValue> class Mapping;

template<typename TKey, typename TValue>
class MappingPair
{
public:
	TKey Key;
	TValue Value;

	MappingPair(const TKey& _Key, const TValue& _Value) : Key(const_cast<TKey&>(_Key)), Value(const_cast<TValue&>(_Value)) {}
	MappingPair() 
	{
		Key = TKey(); 
		Value = TValue();
	}
};

template<typename TKey, typename TValue>
class MappingSegment
{
private:
	MappingSegment* Next, * Last;

	MappingPair<TKey, TValue> Root;
	TKey& Key = Root.Key;
	TValue& Value = Root.Value;

	MappingSegment(const TKey& _Key, const TValue& _Value) : Root(_Key, _Value) {}
	MappingSegment() : Root() {}

	operator MappingPair<TKey, TValue> () const
	{
		return Root;
	}
public:
	friend Mapping<TKey, TValue>;
};

template<typename TKey, typename TValue>
class Mapping
{
private:
	using seg = MappingSegment<TKey, TValue>*;
	using pair = MappingPair<TKey, TValue>;
	using uint = unsigned int;

	seg GetAtIndex(uint Index) const
	{
		seg Return;

		if (_Size == 0 || Index >= _Size)
			Return = nullptr;
		else if (Index == 0)
			Return = First;
		else if (Index == 1 && _Size != 2)
			Return = First->Next;
		else if (Index == _Size - 1)
			Return = Last;
		else if (Index == _Size - 2)
			Return = Last->Last;
		else
		{
			Return = First;
			for (uint i = 0; i < Index; i++)
				Return = Return->Next;
		}

		return Return;
	}
	seg AddNewSegment(const TKey& Key, const TValue& Value)
	{
		seg Return = new MappingSegment<TKey, TValue>(Key, Value);

		if (_Size == 0)
		{
			First = Last = Return;
			_Size = 1;
		}
		else if (_Size == 1)
		{
			Last = Return;
			First->Next = Last;

			_Size++;
		}
		else
		{
			Last->Next = Return;
			Return->Last = Last;
			Last = Return;

			_Size++;
		}

		return Return;
	}

	seg First, Last;
	uint _Size;

public:
	Mapping() : _Size(0), First(nullptr), Last(nullptr) {}
	Mapping(const Mapping<TKey, TValue>& Obj)
	{
		_Size = Obj._Size;
		for (uint i = 0; i < Obj.Size; i++)
			Add(Obj[i]);
	}
	Mapping(const Mapping<TKey, TValue>&& Obj)
	{
		_Size = Obj._Size;
		First = Obj.First;
		Last = Obj.Last;

		Obj._Size = 0;
		Obj.First = Obj.Last = nullptr;
	}
	Mapping(const std::initializer_list<pair>& Obj)
	{
		for (pair& Item : Obj)
			Add(Item);
	}

	const uint& Size = _Size;
	pair& operator[](uint Index) const
	{
		seg Return = GetAtIndex(Index);
		if (!Return)
			throw std::exception("The index provided was out of range.");

		return Return->Root;
	}
	TValue& operator[](const TKey& Key) const
	{
		seg Current = First;
		for (uint i = 0; i < _Size; i++, Current = Current->Next)
		{
			if (Current->Key == Key)
				return Current->Value;
		}

		throw std::exception("The key provided is not in this mapping");
	}
	pair& Item(uint Index) const { return operator[](Index); }

	void Add(const TKey& Key, const TValue& Value)
	{
		if (Contains(Key))
			return;

		AddNewSegment(Key, Value);
	}
	void Add(const pair& Pair)
	{
		if (Contains(Pair.Key))
			return;

		AddNewSegment(Pair.Key, Pair.Value);
	}
	bool Remove(const TKey& Key)
	{
		uint Index = IndexOf(Key);
		if (Index == 0)
		{
			if (_Size == 1)
			{
				delete First;
				First = Last = nullptr;
				_Size = 0;
			}
			else
			{
				seg NewFirst = First->Next;
				delete First;
				First = NewFirst;
				_Size--;
			}
		}
		else if (Index == 1 && _Size == 2)
		{
			delete Last;
			Last = First;
			_Size--;
		}

		seg Current = First;
		for (uint i = 0; i < _Size; i++, Current = Current->Next)
		{
			if (Current->Key == Key)
			{
				seg LastItem = Current->Last;
				seg NextItem = Current->Next;

				LastItem->Next = NextItem;
				NextItem->Last = LastItem;

				Current->Next = Current->Last = nullptr;
				delete Current;
				_Size--;

				return true;
			}
		}

		return false;
	}

	bool Contains(const TKey& Key)
	{
		seg Current = First;
		for (uint i = 0; i < _Size; i++, Current = Current->Next)
		{
			if (Current->Key == Key)
				return true;
		}

		return false;
	}
	uint IndexOf(const TKey& Key)
	{
		seg Current = First;
		for (uint i = 0; i < _Size; i++, Current = Current->Next)
		{
			if (Current->Key == Key)
				return i;
		}

		return uintmax;
	}
};