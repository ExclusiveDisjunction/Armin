#pragma once

#include "BaseTypes.h"
#include "Container.h"

namespace Armin::Files
{
	class ProjectBase;
	class Component;
	class ReferenceList;
	class ReferenceIterator;

	class ComponentReference
	{
	private:
		bool Active;
		Component* Ref;

		ComponentReference* Next, * Last;
	public:
		ComponentReference() = delete;
		ComponentReference(Component* Obj);
		ComponentReference(ComponentReference* ToClone) : ComponentReference(!ToClone ? nullptr : ToClone->Target()) {}
		ComponentReference(const ComponentReference& Obj) = delete;
		ComponentReference(const ComponentReference&& Obj) = delete;
		~ComponentReference();

		friend Component;
		friend ReferenceList;
		friend ReferenceIterator;

		ComponentReference& operator=(const ComponentReference& Obj) = delete;
		ComponentReference& operator=(const ComponentReference&& Obj) = delete;
		ComponentReference& operator=(Component* Obj);

		friend bool operator==(const ComponentReference& One, const ComponentReference& Two);
		friend bool operator!=(const ComponentReference& One, const ComponentReference& Two);

		template<typename T>
		static Vector<ComponentReference*> Generate(const Vector<T*>& Objs)
		{
			Vector<ComponentReference*> Return;
			for (T* Obj : Objs)
			{
				Component* Conv = static_cast<Component*>(Obj);
				if (Conv)
					Return.Add(new ComponentReference(Conv));
			}

			return Return;
		}
		template<typename T>
		static Vector<T*> Convert(const Vector<ComponentReference*>& Objs)
		{
			Vector<T*> Return;
			for (ComponentReference* Obj : Objs)
			{
				Component* Current = Obj->operator Armin::Files::Component * ();
				if (!Current)
					continue;

				T* Conv = dynamic_cast<T*>(Current);
				if (Conv)
					Return.Add(Conv);
			}

			return Return;
		}

		Component* Target() const
		{
			return operator Component * ();
		}
		void SetTarget(Component* New);

		operator Component* () const;
		Component* operator*() const;
		Component* operator->() const;
	};

	class ReferenceIterator
	{
	private:
		ComponentReference* Current;
	public:
		using iterator_category = std::bidirectional_iterator_tag;
		using diffrence_type = std::ptrdiff_t;
		using value_type = ComponentReference*;
		using pointer = ComponentReference**;
		using reference = ComponentReference*&;

		ReferenceIterator(ComponentReference* Obj) : Current(Obj) {}

		reference operator*()
		{
			reference Obj = Current;
			return Obj;
		}
		pointer operator->() { return &Current; }

		ReferenceIterator& operator++() { Current = Current->Next; return *this; }
		ReferenceIterator& operator--() { Current = Current->Last; return *this; }

		friend bool operator==(const ReferenceIterator& a, const ReferenceIterator& b) { return a.Current == b.Current;	}
		friend bool operator!=(const ReferenceIterator& a, const ReferenceIterator& b) { return !(a == b); }
	};

	class ReferenceList
	{
	private:
		using Type = ComponentReference*;

		Type First = nullptr, Last = nullptr;
		uint _Size = 0;

	public:
		ReferenceList();
		ReferenceList(const String& Param, ProjectBase* File);
		ReferenceList(const AString& Param, ProjectBase* File);
		ReferenceList(Type Obj);
		ReferenceList(const Vector<Type>& Obj);
		ReferenceList(const Vector<Component*>& Obj);
		ReferenceList(const ReferenceList& Obj);
		ReferenceList(const ReferenceList&& Obj) = delete;
		~ReferenceList()
		{
			Clear();
		}

		Type operator[](unsigned int Index) const;
		Type Item(unsigned int Index) const;

		uint const& Size = _Size;

		void Clear();
		void Append(ComponentReference* Obj);
		void Add(Component* Obj);
		void Remove(ComponentReference* Obj);

		bool Contains(ComponentReference* Obj) const;
		bool Contains(Component* Obj) const;
		Type Find(Component* Obj) const;
		uint IndexOf(ComponentReference* Obj) const;

		ReferenceIterator begin() { return !First ? end() : ReferenceIterator(First); }
		ReferenceIterator end() { return ReferenceIterator(!Last ? nullptr : Last->Next); }
		const ReferenceIterator begin() const { return !First ? end() : ReferenceIterator(First); }
		const ReferenceIterator end() const { return ReferenceIterator(!Last ? nullptr : Last->Next); }

		ReferenceList& operator=(const ReferenceList& Other);
		ReferenceList& operator=(Type Obj);
		bool operator==(const ReferenceList& Obj) const;
		bool operator!=(const ReferenceList& Obj) const;

		String ToString() const;
		AString ToAString() const;

		operator Vector<Type>() const;
		operator Vector<Component*>() const;
	};
}