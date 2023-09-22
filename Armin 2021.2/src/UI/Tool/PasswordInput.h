#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\..\Common.h"

namespace Armin::UI::Tool
{
	class PasswordInput
	{
	private:
		PasswordInput(HINSTANCE ins);

		HWND _Base;

		Vector<Label*> MiscControls;
		TextBox* Password;
		Button* Submit, * Close;

		bool GotReturn = false;
		String Return = L"";

		void LoadControls();

		LRESULT Paint();
		LRESULT KeyDown(WPARAM wp);
		LRESULT Command(WPARAM wp, LPARAM lp);

		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
	public:
		static String Execute(HINSTANCE ins);
	};
}