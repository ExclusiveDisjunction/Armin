#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\..\UICommon.h"

namespace Armin::UI::Tool
{
	class PasswordInput
	{
	private:
		PasswordInput();
		void Construct(HINSTANCE ins);

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
		PasswordInput(const PasswordInput& Obj) = delete;
		PasswordInput(const PasswordInput&& Obj) = delete;
		~PasswordInput()
		{
			SetWindowLongPtr(_Base, GWLP_USERDATA, 0);
			DestroyWindow(_Base);
		}

		PasswordInput& operator=(const PasswordInput& Obj) = delete;
		PasswordInput& operator=(const PasswordInput&& Obj) = delete;

		static String Execute(HINSTANCE ins);
		static LRESULT RunMessageLoop(PasswordInput* Object, HINSTANCE ins, WindowState* Running);
	};
}