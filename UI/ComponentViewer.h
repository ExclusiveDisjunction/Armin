#pragma once

#include <Windows.h>
#include "Container.h"
#include "UI\ControlBase.h"

#define ComponentViewer_Check WM_USER + 1
#define ComponentViewer_Uncheck WM_USER + 2

class Grid;
class ScrollViewer;

namespace Armin
{
	namespace Files
	{
		class Component;
		class ComponentReference;
		class RefrenceGroup;
		class ReferenceList;
	}

	namespace UI
	{
		class ComponentViewer;

		class ComponentViewerList
		{
		private:
			ComponentViewer* First = nullptr, * Last = nullptr;
			uint _Size;

			Grid* Viewer;
			ScrollViewer* Scroller;

			bool IsDeleting = false;

			void Append(ComponentViewer* New);
			void Pop(ComponentViewer* Obj);

			friend ComponentViewer;

		public:
			ComponentViewerList(Grid* Viewer, ScrollViewer* Scroller);
			ComponentViewerList(const ComponentViewerList& Obj) = delete;
			ComponentViewerList(const ComponentViewerList&& Obj) = delete;
			~ComponentViewerList();

			ComponentViewerList& operator=(const ComponentViewerList& Obj) = delete;
			ComponentViewerList& operator=(const ComponentViewerList&& Obj) = delete;

			ComponentViewer* operator[](uint Index) const;
			ComponentViewer* Item(uint Index) const { return operator[](Index); }
			uint const& Size = _Size;

			void Clear();

			Vector<Files::Component*> RetriveFromList() const; //Returns only those that are selected.
			Vector<Files::Component*> RetriveFromList(Vector<ComponentViewer*>& Selected) const; //Returns only those that are selected.
			Vector<Files::Component*> GetAllComponents() const;

			void ReSizeList(int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);

			void GenerateListBase(const Vector<Files::Component*>& Objects, HMENU ID, bool Multiselect, bool CanEdit, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);
			void GenerateListRef(const Vector<Files::ComponentReference*>& Objects, HMENU ID, bool Multiselect, bool CanEdit, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);
			template<typename T>
			void GenerateList(const Vector<T*>& Objects, HMENU ID, bool Multiselect, bool CanEdit, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5)
			{
				Vector<Files::Component*> New;
				for (uint i = 0; i < Objects.Size; i++)
					New.Add(static_cast<Files::Component*>(Objects[i]));

				return GenerateListBase(New, ID, Multiselect, CanEdit, XCoord, YCoord, Width, Height, Spacing);
			}

			void OpenSelectedForEditView(bool EditMode);
			Files::RefrenceGroup* SaveSelectedAsGroup(bool Mode = false);
			void PopoutObjects(String Title, HINSTANCE ins);
		};


		/// <summary>
		/// Represents a UI control that displays the data of a compoent, and allows for shortcuts for editing/viewing each component, and also has some basic functions for editor abstraction.
		/// </summary>
		class ComponentViewer : public ControlBase
		{
		private:
			enum FlagStates
			{
				CanEdit = 1,
				Checked = 2,
				DisableCommands = 4,
				HasMouse = 8,
				DontPopOnDelete = 16
			};
			
			ComponentViewerList* _ParentList = nullptr;
			Files::ComponentReference* _BSource = nullptr;
			Files::Component* _Source = nullptr;
			unsigned char Flags;
			/* Flags Mapping (default value)
			0 - CanEdit (true)
			0 - Check (false)
			0 - DisableCommands (false)
			0 - HasMouse (false)

			0 - DontPopOnDelete
			0
			0
			0

			*/
			bool _IsMultiselect;

			void Uncheck();
			void Check();

			int TextPos = 0; //Used to display the major text

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

		public:
			ComponentViewer(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE Ins, HMENU ID, Files::Component* Target, bool Multiselect, ComponentViewerList* ParentList, bool CanEdit = true, bool DisableCommands = false);
			ComponentViewer(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE Ins, HMENU ID, Files::Component* Target, bool Multiselect, ComponentViewerList* ParentList, bool CanEdit = true, bool DisableCommands = false) : ComponentViewer(XCoord, YCoord, Width, Height, !Parent ? NULL : *Parent, Ins, ID, Target, Multiselect, ParentList, CanEdit, DisableCommands) {}
			~ComponentViewer();

			ComponentViewer* Next = nullptr, * Last = nullptr;
			friend ComponentViewerList;

			bool CheckState() const;
			void CheckState(bool New);

			Files::Component* Target() const { return _Source; }
			void Target(Files::Component* New)
			{
				_Source = New;
				Redraw();
			}

			LRESULT Paint() override;
			LRESULT Click() override;
			LRESULT MouseLeave();
			LRESULT MouseMove();
			LRESULT RightClick() override;
			LRESULT MouseWheel(WPARAM wp);
			LRESULT KeyDown(WPARAM ID) override;
		};
	}
}