#include "ComponentRefrence.h"

#include "Components.h"

namespace Armin::Files::V2021
{
	ReferenceList::ReferenceList() : First(nullptr), Last(nullptr), _Size(0)
	{

	}
	ReferenceList::ReferenceList(const String& Param, ArminSessionBase* File) : ReferenceList()
	{
		StringList Parts = Param.Split(L'*');

		for (String Item : Parts)
		{
			unsigned long long ID = Item.ToULong();
			Component* Target = File->GetFromID(ID, CT_All);
			if (Target)
				Add(Target);
		}
	}
	ReferenceList::ReferenceList(const AString& Param, ArminSessionBase* File) : ReferenceList()
	{
		Clear();
		
		AStringList Parts = Param.Split('*');
		
		for (AString Item : Parts)
		{
			unsigned long long ID = Item.ToULong();
			Component* Target = File->GetFromID(ID, CT_All);
			if (Target)
				Add(Target);
		}
	}
	ReferenceList::ReferenceList(ReferenceList::Type Obj)
	{
		operator=(Obj);
	}
	ReferenceList::ReferenceList(const Vector<ComponentReference*>& Obj)
	{
		for (uint i = 0; i < Obj.Size; i++)
			Append(new ComponentReference(Obj[i]));
	}
	ReferenceList::ReferenceList(const Vector<Component*>& Obj)
	{
		for (uint i = 0; i < Obj.Size; i++)
			Add(Obj[i]);
	}
	ReferenceList::ReferenceList(const ReferenceList& Obj)
	{
		operator=(Obj);
	}

	ReferenceList::Type ReferenceList::operator[](unsigned int Index) const
	{
		if (Index >= Size)
			return nullptr;

		if (Index == 0 && Size >= 1)
			return First;
		else if (Index == (Size - 1) && Size >= 1)
			return Last;

		Type Current = First;
		for (uint i = 0; i < Index; i++)
			Current = Current->Next;

		return Current;
	}
	ReferenceList::Type ReferenceList::Item(unsigned int Index) const
	{
		return operator[](Index);
	}

	void ReferenceList::Clear()
	{
		if (Size == 0)
			return;
		if (Size == 1)
			delete First;
		else
		{
			Type Current = First;
			for (uint i = 0; i < Size; i++)
			{
				Type Temp = Current->Next;
				delete Current;
				Current = Temp;
			}
		}

		_Size = 0;
		First = Last = nullptr;
	}
	void ReferenceList::Append(ComponentReference* Obj)
	{
		if ((!First && !Last) || Size == 0)
		{
			First = Last = Obj;
			_Size++;

			return;
		}

		Type Last = this->Last;
		if (Last)
			Last->Next = Obj;
		Obj->Last = Last;

		this->Last = Obj;
		_Size++;
	}
	void ReferenceList::Add(Component* Obj)
	{
		Append(new ComponentReference(Obj));
	}
	void ReferenceList::Remove(ComponentReference* Obj)
	{
		if (Size == 1 && First == Obj)
		{
			delete First;
			First = Last = nullptr;
			_Size = 0;

			return;
		}

		Type Last = Obj->Last;
		Type Next = Obj->Next;

		if (Last)
			Last->Next = Next;
		if (Next)
			Next->Last = Last;

		if (Obj == this->Last)
			this->Last = Last;
		else if (Obj == this->First)
			this->First = Next;

		delete Obj;
		_Size--;
	}

	bool ReferenceList::Contains(ComponentReference* Obj) const
	{
		Type Current = First;
		for (uint i = 0; i < Size; i++, Current = Current->Next)
		{
			if (Current == Obj)
				return true;
		}

		return false;
	}
	bool ReferenceList::Contains(Component* Obj) const
	{
		Type Current = First;
		for (uint i = 0; i < Size; i++, Current = Current->Next)
		{
			if (Current->Target() == Obj)
				return true;
		}

		return false;
	}
	ReferenceList::Type ReferenceList::Find(Component* Obj) const
	{
		Type Current = First;
		for (uint i = 0; i < Size; i++, Current = Current->Next)
		{
			if (Current->Target() == Obj)
				return Current;
		}

		return nullptr;
	}
	uint ReferenceList::IndexOf(ComponentReference* Obj) const
	{
		Type Current = First;
		for (uint i = 0; i < Size; i++, Current = Current->Next)
		{
			if (Current == Obj)
				return true;
		}

		return false;
	}

	ReferenceList& ReferenceList::operator=(const ReferenceList& Other)
	{
		if (Size != 0)
			Clear();

		First = Last = nullptr;
		_Size = 0;
		
		if (Other.Size == 0)
			return *this;

		Type Current = Other.First;
		for (uint i = 0; i < Other.Size; i++, Current = Current->Next)
			Append(new ComponentReference(Current));

		return *this;
	}
	ReferenceList& ReferenceList::operator=(ReferenceList::Type Obj)
	{
		if (Size != 0)
			Clear();

		Append(new ComponentReference(Obj));
		return *this;
	}

	bool ReferenceList::operator==(const ReferenceList& Obj) const
	{
		const ReferenceList& One = *this;
		const ReferenceList& Two = Obj;

		if (One.Size == Two.Size)
		{
			bool Return = true;
			Type Onei = One.First, Twoi = Two.First;
			for (uint i = 0; i < One.Size; i++, Onei = Onei->Next, Twoi = Twoi->Next)
				Return &= (Onei->Target() == Twoi->Target());
		}
		
		return false;
	}
	bool ReferenceList::operator!=(const ReferenceList& Obj) const
	{
		return !(operator==(Obj));
	}

	String ReferenceList::ToString() const
	{
		if (Size == 0)
			return String();

		String Return = Size == 0 ? String() : (!First->Target() ? 0 : First->Target()->ID);
		Type Current = First->Next;
		for (uint i = 1; i < Size; i++, Current = Current->Next)
			Return += L"*" + (!Current->Target() ? 0 : Current->Target()->ID);

		return Return;
	}
	AString ReferenceList::ToAString() const
	{
		if (Size == 0)
			return AString();

		AString Return = Size == 0 ? AString() : (!First->Target() ? 0 : First->Target()->ID);
		Type Current = First->Next;
		for (uint i = 1; i < Size; i++, Current = Current->Next)
			Return += "*" + (!Current->Target() ? 0 : Current->Target()->ID);

		return Return;
	}

	ReferenceList::operator Vector<ReferenceList::Type>() const
	{
		Vector<Type> Return;
		Type Current = First;
		for (uint i = 0; i < Size; i++, Current = Current->Next)
			Return.Add(Current);

		return Return;
	}
}