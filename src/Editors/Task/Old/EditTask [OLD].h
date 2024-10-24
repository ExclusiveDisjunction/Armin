#pragma once

#include <Windows.h>

#include "Container.h"
#include "Controls.h"
#include "..\..\Common.h"
#include "..\..\Controls\ArminControls.h"

namespace Armin
{
	namespace Files
	{
		class Task;
		class ArminSessionBase;

		class SessionControl;
	}

	namespace Editors::Task
	{
		class EditTask
		{
		private:
			HWND _Base;

			Files::Task* Target;

			Button* ApplyBttn;
			Vector<ControlBase*> MiscControls;
			TextBox* Title, * Instructions;
			Label* DueBy;
			CheckableButton* RequiresAssurance;
			Button* DueBySelect, * ModifyAssigned, * ViewAssigned, * Multiselect;
			ScrollViewer* AssignedUsersScroll;
			Grid* AssignedUsersView;
			Vector<ComponentViewer*> AssignedUsers;

			bool _IsMultiselect = false;

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

			LRESULT Size();
			LRESULT Paint();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT Destroy();
			LRESULT KeyDown(WPARAM wp);
			LRESULT SpecialCommand(HMENU ID, uint Command, LPARAM Sender);

			bool _Loaded = false;
			void LoadControls();

			bool Apply(Files::ArminSessionBase* File);
		public:
			friend Files::SessionControl;

			EditTask(Files::Task* Target);
			~EditTask();
		};
	}
}