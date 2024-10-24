#pragma once

#include "Container.h"
#include "Controls.h"
#include "..\..\Common.h"
#include "..\..\Controls\ArminControls.h"

namespace Armin
{
	namespace Files
	{
		class CompletedTask;
	}

	namespace Editors::Task
	{
		class ViewCompletedTask
		{
		private:
			HWND _Base;
			bool _Loaded, _DummySelect;

			Files::CompletedTask* _Target;

			Vector<ControlBase*> MiscControls;
			Label* DateCompleted;
			ComponentViewer* AssuredBy, * Task;
			Button* Close;

			ScrollViewer* CompletedByScroll;
			Grid* CompletedByGrid;
			Vector<ComponentViewer*> CompletedBy;

			ScrollViewer* ReportsScroll;
			Grid* ReportsView;
			Vector<ComponentViewer*> Reports;

			void LoadControls();

			LRESULT Size();
			LRESULT Paint();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT SpecialCommand(HMENU ID, uint Command, LPARAM Sender);
			LRESULT Destroy();

			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);

		public:
			ViewCompletedTask(Files::CompletedTask* Target);
			~ViewCompletedTask();
		};
	}
}