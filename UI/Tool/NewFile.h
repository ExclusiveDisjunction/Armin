#pragma once

#include "UI\Button.h"
#include "UI\CheckableButton.h"
#include "UI\TextBox.h"
#include "..\..\UICommon.h"

namespace Armin::UI
{
	class NewFile
	{
	private:
		HWND _Base;
		bool _Loaded;

		String _Return;

		void CreateBase(HINSTANCE ins);

		LRESULT Paint();
		LRESULT Size();
		LRESULT KeyDown(WPARAM Key);
		LRESULT Command(WPARAM wp, LPARAM lp);

		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
	public:
		NewFile();

		static LRESULT RunMessageLoop(NewFile* Object, HINSTANCE ins, bool* Running);
	
		String const& Return = _Return;
		static String Execute(HINSTANCE ins);
	};
}