#pragma once

#include "Container.h"
#include "Controls.h"
#include "..\..\Common.h"

namespace Armin::Windows::Search
{
	class InventorySearch
	{
	private:
		HWND _Base;

		Vector<ControlBase*> MiscControls;
		TextBox* SerialNumber, * Group, * DescriptionContains;
		Button* Run;
		CheckableButton* IsInPossession;

		void LoadControls();
		bool _Loaded;

		void RunSearch();

		LRESULT Size();
		LRESULT Paint();
		LRESULT Command(WPARAM wp, LPARAM lp);
		LRESULT Destroy();

		static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
	public:
		InventorySearch(HINSTANCE ins);
		~InventorySearch()
		{
			SetWindowLongPtrW(_Base, GWLP_USERDATA, 0l);
			DestroyWindow(_Base);

			Destroy();
		}
	};
}