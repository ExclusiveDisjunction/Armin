#pragma once

#include "Container.h"
#include "Controls.h"
#include "..\..\Common.h"
#include "..\..\Controls\ArminControls.h"

namespace Armin
{
	namespace Files
	{
		class Task;
	}

	namespace Editors::Task
	{
		class ViewTask
		{
		private:
			HWND _Base;
			bool _Loaded, _EditMode, _DummySelect;

			Vector<ControlBase*> MiscControls;
			Button* CompleteTask, * Close, * EditThis;
			Label* Title, * Instructions, * DueBy, * RequiresAssurance;
			ScrollViewer* AssignedScroll;
			Grid* AssignedView;
			Vector<ComponentViewer*> AssignedObjects;

			static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);

			void LoadControls();
			void FillAssigned();

			LRESULT Paint();
			LRESULT Destroy();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT Size();

			Files::Task* Source;
		public:
			ViewTask(Files::Task* Target, bool EditMode = true);
			~ViewTask();
		};
	}
}


