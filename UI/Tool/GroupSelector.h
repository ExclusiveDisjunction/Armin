#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\..\UICommon.h"

namespace Armin
{
	namespace Files
	{
		class InventorySystem;
	}

	namespace UI::Tool
	{
		enum GroupSelectorSource
		{
			GSS_Inventory = 1,
			GSS_OperationInventory = 2
		};

		class GroupSelector
		{
		private:
			GroupSelector(Files::InventorySystem* System, GroupSelectorSource Source, bool Multiselect = false);

			void Construct(HINSTANCE ins);

			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);

			HWND _Base;
			Files::InventorySystem* _System;
			GroupSelectorSource Source;
			bool _Multi;

			StringList Return;
			bool _Loaded, _Calculated;

			TextBox* Text;
			ScrollViewer* Scroll;
			Grid* View;
			Button* Submit;

			Vector<ControlBase*> MiscControls;
			Vector<ToggleButton*> Groups;

			void LoadControls();

			LRESULT Paint();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT Size();
			LRESULT Destroy();
		public:
			GroupSelector() = delete;
			GroupSelector(const GroupSelector& Obj) = delete;
			GroupSelector(const GroupSelector&& Obj) = delete;
			~GroupSelector()
			{
				SetWindowLongPtr(_Base, GWLP_USERDATA, 0);
				DestroyWindow(_Base);
			}

			GroupSelector& operator=(const GroupSelector& Obj) = delete;
			GroupSelector& operator=(const GroupSelector&& Obj) = delete;

			static StringList Execute(HINSTANCE ins, Files::InventorySystem* System, GroupSelectorSource Source, bool Multiselect = false);
			static LRESULT RunMessageLoop(GroupSelector* Object, HINSTANCE ins, WindowState* Running);
		};
	}
}