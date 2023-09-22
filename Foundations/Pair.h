#pragma once

template<typename TypeOne, typename TypeTwo>
struct Pair
{
public:
	TypeOne OneValue;
	TypeTwo TwoValue;

	ValuePair()
	{
	}
	ValuePair(const TypeOne&& One, const TypeTwo&& Two)
	{
		OneValue = One;
		TwoValue = Two;
	}

	bool operator==(const ValuePair<TypeOne, TypeTwo>& Other) const
	{
		return OneValue == Other.OneValue &&
			TwoValue == Other.TwoValue;
	}
	bool operator!=(const ValuePair<TypeOne, TypeTwo>& Other) const
	{
		return !(*this == Other);
	}
};