#pragma once

#ifndef Phsophor_SortedSet
#define Phopshor_SortedSet

#include <istream>
#include <vector>

template<typename T>
class SortedSet
{
private:
	std::vector<T> _List;
public:
	SortedSet()
	{
	}
	SortedSet(T* List, int Len) : SortedSet()
	{
		for (int i = 0; i < Len; i++)
			_List.push_back(List[i]);
	}
	SortedSet(std::vector<T> Values)
	{
		_List = Values;
	}

	T& operator[](int Index)
	{
		if (Index >= Count())
			throw new std::exception("The index supplied was greater than or equal to the count.");
		return _List[Index];
	}
	int Count()
	{
		return _List.size();
	}

	void Add(T Object)
	{
		_List.push_back(Object);
	}
	void Remove(T Object)
	{
		std::vector<T> Other = _List;
		_List.clear();
		for (int i = 0; i < Count(); i++)
		{
			if (Object == Other[i])
				continue;
			else
				_List.push_back(Other[i]);
		}
	}
	void Reset()
	{
		_List.clear();
	}

	bool Contains(T Obj)
	{
		return _List.Contains(Obj);
	}
	int IndexOf(T Obj)
	{
		return _List.Contains(Obj);
	}
};

#endif