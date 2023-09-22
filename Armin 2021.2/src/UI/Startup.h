#pragma once

#include "..\Common.h"

namespace Armin::UI
{
	class Startup
	{
	private:
		HWND _Base;

		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
		
		LRESULT Paint();
		LRESULT Destroy();
		LRESULT Timer(WPARAM wp);

	public:
		Startup(HINSTANCE ins);

		bool DidFail = false;
		void Open();
	};
}
