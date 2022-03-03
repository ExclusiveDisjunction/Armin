#pragma once

#include "UI\Controls.h"
#include "..\Common.h"

namespace Armin::UI
{
	class Welcome
	{
	private:
		HWND _Base;
		Grid* Viewer;
		ScrollViewer* ViewerScroll; //ID: None
		Vector<Button*> Recents; //ID: 1
		Button* OpenCmd, * NewCmd, * OpenLast, * OpenWithout, * Quit; //ID:2, 3, 4, 5, 6, 7
		ControlList MiscControls;

		StringList Paths;

		static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);

		void LoadControls();
		void MoveControls();
		void RefreshList();

		LRESULT Paint();
		LRESULT Command(WPARAM wp, LPARAM lp);
		LRESULT Destroy();
		LRESULT Size();

	public:
		Welcome(HINSTANCE ins);

		void Open();
		void Close();

		static bool DoQuit;
	};
}
