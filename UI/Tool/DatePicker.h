#pragma once

#include "Container.h"
#include "DateTime.h"
#include "UI\Controls.h"
#include "..\..\Common.h"

namespace Armin::UI::Tool
{
	class DatePicker
	{
	private:
		DatePicker(const DateTime& Prev, HINSTANCE ins, bool HasDate = false);

		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

		DateTime Return;
		bool ProperReturn = false, _Loaded = false, _HasDate = false;

		HWND _Base;
		TextBox* MM, * DD, * YYYY, * HH, * MiMi;
		CheckableButton* HasDate, * IsAM, * IsPM;
		Button* Submit, * Cancel, * Now;
		Label* Title;
		Vector<ControlBase*> MiscControls;

		bool Process();
		void MessageLoop();

		void LoadControls();
		LRESULT Command(WPARAM wp, LPARAM lp);
		LRESULT KeyDown(WPARAM key);
		LRESULT Paint();
		LRESULT Destroy();
	public:
		static DateTime Execute(const DateTime& Prev, HINSTANCE ins, bool HasDate = false);
	};
}