#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\Common.h"

namespace Armin::UI
{
	class About
	{
	private:
		HWND _Base;

		Vector<ControlBase*> MiscControls;

		LRESULT Paint();
		void LoadControls();

		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
	public:
		About(HINSTANCE ins);
		~About();
	};
}