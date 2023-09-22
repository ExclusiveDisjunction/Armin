#pragma once

#include "Container.h"

template<typename T, typename Condition>
void QuickSort(Vector<T>& Source, Condition Cond, int Begin, int End)
{
	auto Swap = [](T& One, T& Two)
	{
		T Temp = One;
		One = Two;
		Two = Temp;
	};
	auto Partition = [Cond, Swap](Vector<T>& Source, int Begin, int End) -> int
	{
		T Pivot = Source[End];
		int i = (Begin - 1);

		for (int j = Begin; j <= End - 1; j++)
		{
			if (Cond(Source[j], Pivot))
			{
				i++;

				Swap(Source[i], Source[j]);
			}
		}

		Swap(Source[i + 1], Source[End]);

		return (i + 1);
	};

	if (Begin < End)
	{
		int pi = Partition(Source, Begin, End);

		QuickSort<T, Condition>(Source, Cond, Begin, pi - 1);
		QuickSort<T, Condition>(Source, Cond, pi + 1, End);
	}
}

template<typename T, typename Condition>
void QuickSort(Vector<T>& Source, Condition Cond)
{
	QuickSort(Source, Cond, 0, Source.Size - 1);
}