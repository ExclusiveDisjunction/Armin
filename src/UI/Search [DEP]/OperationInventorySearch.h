#pragma once

#include "Container.h"
#include "Controls.h"
#include "..\..\Common.h"

namespace Armin::Windows::Search
{
	class OperationInventorySearch
	{
	private:
		HWND _Base;
		bool _Loaded;

		Vector<ControlBase*> MiscControls;
		Button* Run;
		TextBox* SerialNumber, * Group, *DescriptionContains;
		CheckableButton* WorkingOrder, * PartialWorkingOrder, * Broken, * UnderRepair, * NotInPossession;

		void LoadControls();
		void RunSearch();

		LRESULT Size();
		LRESULT Paint();
		LRESULT Command(WPARAM wp, LPARAM lp);
		LRESULT Destroy() 
		{
			_Loaded = false;
			return 0;
		}

		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
	public:
		OperationInventorySearch(HINSTANCE ins);
		~OperationInventorySearch()
		{
			SetWindowLongPtrW(_Base, GWLP_USERDATA, 0l);
			DestroyWindow(_Base);

			Destroy();
		}
	};
}