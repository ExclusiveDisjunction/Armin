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
		DatePicker(const DateTime& Prev);
		void Construct(HINSTANCE ins);

		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

		DateTime Return;
		bool ProperReturn = false, _Loaded = false;

		HWND _Base;
		TextBox* MM, * DD, * YYYY, * HH, * MiMi;
		CheckableButton* HasDate, * IsAM, * IsPM;
		Button* Submit, * Cancel, * Now;
		Label* Title;
		Vector<ControlBase*> MiscControls;

		bool Process();
		void LoadControls();

		LRESULT Command(WPARAM wp, LPARAM lp);
		LRESULT KeyDown(WPARAM key);
		LRESULT Paint();
	public:
		DatePicker() = delete;
		DatePicker(const DatePicker& Obj) = delete;
		DatePicker(const DatePicker&& Obj) = delete;
		~DatePicker()
		{
			SetWindowLongPtr(_Base, GWLP_USERDATA, 0);
			DestroyWindow(_Base);
		}

		static DateTime Execute(const DateTime& Prev, HINSTANCE ins);
		static LRESULT RunMessageLoop(DatePicker* Obj, HINSTANCE ins, bool* Running);
	};
}