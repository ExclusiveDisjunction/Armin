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
				HasMouse = 8
			};

			Files::Component* _Source;
			char Flags;
			/* Flags Mapping (default value)
			0 - CanEdit (true)
			0 - Check (false)
			0 - DisableCommands (false)
			0 - HasMouse (false)

			0
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
			ComponentViewer(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE Ins, HMENU ID, Files::Component* Target, bool Multiselect, bool CanEdit = true, bool DisableCommands = false);
			ComponentViewer(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE Ins, HMENU ID, Files::Component* Target, bool Multiselect, bool CanEdit = true, bool DisableCommands = false) : ComponentViewer(XCoord, YCoord, Width, Height, !Parent ? NULL : *Parent, Ins, ID, Target, Multiselect, CanEdit, DisableCommands) {}

			/// <summary>Retrives the Render Text of a component, or the text that is displayed to users.</summary>
			/// <param name="Target">The component to display information for.</param>
			/// <returns>The UI string for this component.</returns>
			static String GetText(Files::Component* Target);

			/// <summary>Retrives all selected components from a list of ComponentViewers.</summary>
			/// <param name="Objects">The list of ComponentViewers to be evaluated. Only the selected objects will be returned.</param>
			/// <returns>A list of components that were selected in the Objects parameter.</returns>
			static Vector<Files::Component*> RetriveFromList(const Vector<ComponentViewer*>& Objects);
			/// <summary>Retrives all selected components from a list of ComponentViewers, and fills the Selected parameter with all the compoennt viewers that were selected.</summary>
			/// <param name="Objects">The list of ComponentViewers to be evaluated. Only the selected objects will be returned.</param>
			/// <param name="Selected">The list of ComponentViewers that are selected. After the function returns, the size of this parameter will equal the size of the Return value.</param>
			/// <returns>A list of components that were selected in the Objects parameter.</returns>
			static Vector<Files::Component*> RetriveFromList(const Vector<ComponentViewer*>& Objects, Vector<ComponentViewer*>& Selected);

			/// <summary>Resizes a list of Component Viewers to fit the parent grid.</summary>
			/// <param name="Objects">The list objects to re-size.</param>
			/// <param name="Parent">The parent grid of the objects. Not required.</param>
			/// <param name="Scroll">The parent scroll of the parent grid. Not required. If this and Parent are both non nullptr, the parent grid is re-sized.</param>
			/// <param name="XCoord">The XCoord of all the component viewers. This does not change.</param>
			/// <param name="YCoord">The YCoord of all the compoennt viewers. For each ComponentViewer in the list, this parameter increases by (Height + Spacing).</param>
			/// <param name="Width">The width of all component viewers. This paramter will be overriden if the ParentScroll's with is greater than 900.</param>
			/// <param name="Height">The height of all component viewers. This paramter does not change.</param>
			/// <param name="Spacing">The Y spacing between components.</param>
			static void ReSizeList(Vector<ComponentViewer*>& Objects, Grid* Parent = nullptr, ScrollViewer* Scroll = nullptr, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);

			static Vector<ComponentViewer*> GenerateListBase(const Vector<Files::Component*>& Objects, Grid* ParentGrid, HMENU ID, bool Multiselect, bool CanEdit, ScrollViewer* Scroll = nullptr, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);
			static Vector<ComponentViewer*> GenerateListRef(const Vector<Files::ComponentReference*>& Objects, Grid* ParentGrid, HMENU ID, bool Multiselect, bool CanEdit, ScrollViewer* Scroll = nullptr, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);
			static Vector<ComponentViewer*> GenerateListRef(const Files::ReferenceList& Objects, Grid* ParentGrid, HMENU ID, bool Multiselect, bool CanEdit, ScrollViewer* Scroll = nullptr, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5);
			template<typename T>
			static Vector<ComponentViewer*> GenerateList(const Vector<T*>& Objects, Grid* ParentGrid, HMENU ID, bool Multiselect, bool CanEdit, ScrollViewer* Scroll = nullptr, int XCoord = 5, int YCoord = 5, int Width = 900, int Height = 27, int Spacing = 5)
			{
				Vector<Files::Component*> New;
				for (uint i = 0; i < Objects.Size; i++)
					New.Add(static_cast<Files::Component*>(Objects[i]));

				return GenerateListBase(New, ParentGrid, ID, Multiselect, CanEdit, Scroll, XCoord, YCoord, Width, Height, Spacing);
			}
			/// <summary>Retrives all compoennts from a list of component viewers regardless of selection status.</summary>
			/// <param name="Objects">The objects to retrive.</param>
			/// <returns>A list of Armin::Files::Component retrived from the Objects parameter.</returns>
			static Vector<Files::Component*> GetAllComponents(const Vector<ComponentViewer*>& Objects);

			/// <summary>Opens the selected objects in their edit/view editors based on user status and edit mode.</summary>
			/// <param name="Objects">The objects to be opened. Only the selected ones will be opened</param>
			/// <param name="EditMode">When true, the edit editors will be opened for the objects, but when false the viewing editors will be opened.</param>
			static void OpenSelectedForEditView(const Vector<ComponentViewer*>& Objects, bool EditMode);
			/// <summary>Saves the objects in the list as a refrence group based on the mode.</summary>
			/// <param name="Objects">The component viewers (raw form) of the components</param>
			/// <param name="Mode">When true, only the selected objects will be saved, but when false all objects will be saved.</param>
			/// <returns>The refrence group created. If nullptr, the function failed.</returns>
			static Files::RefrenceGroup* SaveSelectedAsGroup(const Vector<ComponentViewer*>& Objects, bool Mode = false);
			/// <summary>Opens ComponentListRenderer for the objects provided, and sets the window's title to the Title parameter.</summary>
			/// <param name="Objects">The objects to open.</param>
			/// <param name="Title">The title of the window to pass.</param>
			/// <param name="ins">The HINSTANCE of the callee.</param>
			static void PopoutObjects(const Vector<ComponentViewer*>& Object, String Title, HINSTANCE ins);

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