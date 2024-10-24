#pragma once

#include <iostream>
#include <fstream>

#include "AString.h"
#include "Arithmetic.h"
#include "ComponentRefrence.h"
#include "ComponentTypes.h"
#include "Container.h"
#include "Str.h"

namespace Armin::Files
{
	class ProjectBase;
	class ComponentReference;

	class Component
	{
	protected:
		String _Title;
		unsigned long long _ID;
		ProjectBase* _ParentFile;

		Component(ProjectBase* File, bool GetID);
	public:
		virtual ~Component();

		Vector<ComponentReference*> Refrences;

		virtual String Title() const { return _Title; }
		virtual void Title(String New) { _Title = New; }
		const unsigned long long& ID = _ID;

		ProjectBase* ParentFile() const { return _ParentFile; }

		virtual ComponentTypes ObjectType() const = 0;
		virtual String ToString() const { return L"No Data"; }
		virtual void Fill(std::ifstream& InFile) = 0;
		virtual void Push(std::ofstream& OutFile, uint PreIndex = 0u) const = 0;
	};

	template<typename T, typename ParentT>
	class ComponentList
	{
	protected:
		ProjectBase* _File = nullptr;
		T* First, * Last;
		ParentT* _Parent;
		unsigned int _Count;

	public:
		ComponentList() = delete;
		ComponentList(ProjectBase* File, ParentT* Parent) : _File(File), First(nullptr), Last(nullptr), _Parent(Parent), _Count(0) {}
		ComponentList(const ComponentList<T, ParentT>& Obj) = delete;
		ComponentList(const ComponentList<T, ParentT>&& Obj) = delete;
		~ComponentList()
		{
			Clear();
		}

		ComponentList<T, ParentT>& operator=(const ComponentList<T, ParentT>& Obj) = delete;
		ComponentList<T, ParentT>& operator=(const ComponentList<T, ParentT>&& Obj) = delete;

		T* operator[](unsigned int Index) const
		{
			if (Index >= Count)
				return nullptr;

			if (Index == 0 && Count >= 1)
				return First;
			else if (Index == (Count - 1) && Count >= 1)
				return Last;

			T* Current = First;
			for (uint i = 0; i < Index; i++)
				Current = Current->Next;

			return Current;
		}
		T* Item(unsigned int Index) const
		{
			return operator[](Index);
		}

		const unsigned int& Count = _Count;
		ProjectBase* ParentFile() const { return _File; }
		ParentT* const& Parent = _Parent;

		String Name = T::ThisName + L"List";

		void Clear() //ONLY removes the data stored in the class.
		{
			if (_Count == 0)
				return;
			if (_Count == 1)
				delete First;
			else
			{
				T* Current = First;
				for (uint i = 0; i < Count; i++)
				{
					T* Temp = Current->Next;
					delete Current;
					Current = Temp;
				}
			}

			_Count = 0;
			First = nullptr;
			Last = nullptr;
		}
		void Append(T* Item) //Adds an item by adding it to the chain. 
		{
			if ((!First && !Last) || _Count == 0)
			{
				First = Last = Item;
				_Count++;

				return;
			}

			T* Last = this->Last;
			if (Last)
				Last->Next = Item;
			Item->Last = Last;

			this->Last = Item;
			_Count++;
		}
		void Pop(T* Obj) //Removes the object from the chain. Does not delete the object.
		{
			if (_Count == 1)
			{
				this->First = this->Last = nullptr;
				_Count = 0;

				return;
			}

			T* Last = Obj->Last;
			T* Next = Obj->Next;

			if (Last)
				Last->Next = Next;
			if (Next)
				Next->Last = Last;

			if (Obj == this->Last)
				this->Last = Last;
			else if (Obj == this->First)
				this->First = Next;

			_Count--;
		}

		void Fill(std::ifstream& InFile)
		{
			Clear();

			AString Header;
			getline(InFile, Header);

			AStringList Parts = Header.Split('~');
			bool Mutliline = Parts[Parts.Size - 1] != "end";

			if (Mutliline)
			{
				int TabIndex = Header.TabIndex();

				while (!InFile.eof())
				{
					AString ThisLine;
					std::streampos PrePos = InFile.tellg();
					getline(InFile, ThisLine);

					AString Temp = ThisLine;
					Temp.RemoveAllOf('\t');
					if (Temp == AString())
						continue;

					AStringList ThisParts = Temp.Split('~');
					if (ThisLine.TabIndex() == TabIndex)
						break;

					else if (ThisLine.TabIndex() - 1 == TabIndex)
					{
						if (ThisParts[1] == AString(T::ThisName) && ThisParts[0] == "begin")
						{
							InFile.seekg(PrePos);
							new T(_File, this, InFile);
						}
					}
				}
			}
		}
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const
		{
			AString TabIndexValue;
			for (uint i = 0; i < PreIndex; i++)
				TabIndexValue += '\t';

			OutFile << TabIndexValue << "begin~" << Name;

			if (Count != 0)
			{
				OutFile << std::endl;

				for (T* Obj = First; Obj; Obj = Obj->Next)
					Obj->Push(OutFile, PreIndex + 1);

				OutFile << TabIndexValue << "end~" << Name << std::endl;
			}
			else
				OutFile << "~end" << std::endl;
		}

		bool Contains(T*& Obj) const
		{
			T* Current = First;
			for (uint i = 0; i < _Count; i++, Current = Current->Next)
			{
				if (Current == Obj)
					return true;
			}

			return false;
		}
		bool Contains(const String& Obj) const
		{
			for (T* Current = First; Current; Current = Current->Next)
			{
				if (Current->Title() == Obj)
					return true;
			}

			return false;
		}
		uint IndexOf(T*& Obj) const
		{
			T* Current = First;
			for (uint i = 0; i < _Count; i++, Current = Current->Next)
			{
				if (Current == Obj)
					return i;
			}

			return 4294967295U;
		}

		operator Vector<T*>() const
		{
			Vector<T*> Data;
			T* Current = First;
			for (uint i = 0; i < Count; i++)
			{
				Data.Add(Current);
				Current = Current->Next;
			}

			return Data;
		}
	};
	class ComponentParent
	{
	public:
		virtual ProjectBase* ParentFile() = 0;
	};
}