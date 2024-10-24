#pragma once

template<typename T>
class ReadOnlySet
{
protected:
	unsigned int _Count;
public:
	virtual T operator[](unsigned int Index) const = 0;
	virtual T Item(unsigned int Index) const//
	{
		return operator[](Index);
	}
	const unsigned int& Count = _Count;

	virtual bool Contains(T& Obj) const = 0;
	virtual unsigned int IndexOf(T& Obj) const = 0;
};

template<typename T>
class NoReplaceSet : public ReadOnlySet<T>
{
public:
	virtual void Remove(T& Obj) = 0;
};

template<typename T>
class ModifySet : public NoReplaceSet<T>
{
	virtual void Replace(T NewObj, T OldObj) = 0;
};