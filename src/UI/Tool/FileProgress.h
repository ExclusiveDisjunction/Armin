#pragma once

#include <Windows.h>
#include "UI\Controls.h"

namespace Armin::UI::Tool
{
	class FileProgress
	{
	private:
		HWND _Base;
		bool Mode;

		static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);

	public:
		FileProgress(bool Mode, HINSTANCE ins); //If Mode is true, then it is saving, and if it is false it is loading.
		~FileProgress()
		{
			SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);
			DestroyWindow(_Base);
		}
	};
}