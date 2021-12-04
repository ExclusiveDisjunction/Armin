#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\..\Common.h"

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
			GroupSelector(HINSTANCE ins, Files::InventorySystem* System, GroupSelectorSource Source, bool Multiselect = false);

			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);

			HWND _Base;
			Files::InventorySystem* _System;
			bool _Multi;

			StringList Return;
			bool _Loaded, _Calculated;

			TextBox* Text;
			ScrollViewer* Scroll;
			Grid* View;
			Button* Submit;

			Vector<ControlBase*> MiscControls;
			Vector<ToggleButton*> Groups;

			void LoadControls(GroupSelectorSource Source, Files::InventorySystem* System, bool Multi);

			LRESULT Paint();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT Size();
			LRESULT Destroy();
		public:
			static StringList Execute(HINSTANCE ins, Files::InventorySystem* System, GroupSelectorSource Source, bool Multiselect = false);
		};
	}
}