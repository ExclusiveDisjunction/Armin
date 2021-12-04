#include "ObjectSelector.h"

#include "Sort.h"
#include "Components.h"
#include "..\..\Search\SearchByName.h"

namespace Armin::Windows::Tool
{
	using namespace Files;
	using namespace Windows::Search;

	ObjectSelector::ObjectSelector(class SearchCriteria& Criteria, HINSTANCE ins) : _Criteria(Criteria)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Object Selector", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 880, 450, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadControls();
	}

	ATOM ObjectSelector::_ThisAtom = ATOM();
	void ObjectSelector::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = ObjectSelector::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"ObjectSelectorClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(ObjectSelector*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall ObjectSelector::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ObjectSelector* Item = reinterpret_cast<ObjectSelector*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			Item->Paint();
			return 0;
		case WM_CREATE:
			return 0;
		case WM_SIZE:
			Item->MoveControls();
			return 0;
		case WM_DESTROY:
			Item->Destroy();
			return 0;
		case WM_KEYDOWN:
			return Item->KeyDown(wp);
		case WM_COMMAND:
			return Item->Command(wp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	Vector<Component*> ObjectSelector::Execute(Files::SearchCriteria & Criteria, HINSTANCE ins)
	{
		ObjectSelector* Object = new ObjectSelector(Criteria, ins);

		MSG msg;
		int Result;
		while ((Result = GetMessageW(&msg, Object->_Base, NULL, NULL)) != 0)
		{
			if (Result < 0)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		Vector<Component*> Return = Object->Return;
		delete Object;

		return Return;
	}

	void ObjectSelector::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		{
			int XCoord = 0;
			int YCoord = 0;
			int Height = WndRect.bottom;
			int Width = 180;

			StyleSheet Style;
			Style.Background = Grey1;

			G1 = new Grid(XCoord, YCoord, Width, Height, _Base, ins, Style);

			XCoord += Width + 10;
			YCoord = 10;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);
			Style.Background = Background1;

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			Refresh();
		}

		{
			RECT Rect;
			GetClientRect(*G1, &Rect);

			int XCoord = 10;
			int YCoord = 24;
			const int Width = Rect.right - (10 + XCoord);
			const int Height = 30;

			StyleSheet Style;
			Style.Background = Grey1;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 25;

			TextStyleSheet Text;
			Text.FontFamily = StandardFontName;
			Text.FontSize = 13;
			Text.Alignment = TA_CenterAlignment;
			Text.Foreground = FontColor;

			SearchByName = new Button(XCoord, YCoord, Width, Height, L"Search By Name", G1, (HMENU)3, ins, Style, Text);
			YCoord += 20 + Height;

			DeselectAll = new Button(XCoord, YCoord, Width, Height, L"Clear Selection", G1, (HMENU)4, ins, Style, Text);
			YCoord += 7 + Height;
			SelectAll = new Button(XCoord, YCoord, Width, Height, L"Select All", G1, (HMENU)5, ins, Style, Text);
			EnableWindow(*SelectAll, _Criteria.Multiselect);
			YCoord += 20 + Height;

			Submit = new Button(XCoord, YCoord, Width, Height, L"Submit", G1, (HMENU)6, ins, Style, Text);
			YCoord += 7 + Height;
			Cancel = new Button(XCoord, YCoord, Width, Height, L"Cancel", G1, (HMENU)7, ins, Style, Text);
		}
	}
	void ObjectSelector::MoveControls()
	{
		if (!_Loaded)
			return;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{
			int XCoord = 0;
			int YCoord = 0;
			int Height = WndRect.bottom;
			int Width = 180;

			G1->Move(XCoord, YCoord, Width, Height);

			XCoord += Width + 10;
			YCoord = 10;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}
	}
	void ObjectSelector::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		HBRUSH Bk = CreateSolidBrush(static_cast<COLORREF>(Background2));

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
	}
	void ObjectSelector::Destroy()
	{
		if (ProperReturn)
			GetReturn();
		else
			Return.Clear();
	}

	void ObjectSelector::Refresh()
	{
		CloseControls(Objects);

		ArminSessionBase* Loaded = LoadedSession;
		Vector<Component*> Objects = _Criteria.GetComponents(Loaded);

		String Sort = _Criteria.SortBy;

		if (Sort == L"Object ID")
			QuickSort(Objects, [](Component*& One, Component*& Two) { return One->ID < Two->ID; });
		else if (Sort == L"Object Type")
			QuickSort(Objects, [](Component*& One, Component*& Two) { return One->ObjectType() < Two->ObjectType(); });

		if (!_Criteria.Ascending)
			Objects.Reverse();

		this->Objects = ComponentViewer::GenerateList(Objects, ObjectView, NULL, _Criteria.Multiselect, true, ObjectScroll);

		if (_FirstRefresh)
		{
			Vector<Component*>& PreSelected = _Criteria.PreSelected;

			if (!_Criteria.Multiselect)
				PreSelected.Resize(1);

			for (uint i = 0; i < this->Objects.Size(); i++)
			{
				ComponentViewer* This = this->Objects[i];
				if (PreSelected.Contains(This->Target()))
				{
					PreSelected.Remove(This->Target());
					This->CheckState(true);
				}
			}

			_FirstRefresh = false;
		}
	}
	void ObjectSelector::GetReturn()
	{
		Return = ComponentViewer::RetriveFromList(Objects);
	}

	LRESULT ObjectSelector::KeyDown(WPARAM key)
	{
		if (key == VK_ESCAPE)
		{
			ProperReturn = false;
			DestroyWindow(_Base);
		}

		if (key == VK_RETURN)
		{
			ProperReturn = true;
			DestroyWindow(_Base);
		}

		return 0;
	}
	LRESULT ObjectSelector::Command(WPARAM id)
	{
		switch (id)
		{
		case 3:
			Return = SearchByName::Execute(_Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			this->ProperReturn = true;
			DestroyWindow(_Base);
			return 0;
		case 4:
		{
			for (uint i = 0; i < Objects.Size(); i++)
				Objects[i]->CheckState(false);
			return 0;
		}
		case 5:
		{
			if (_Criteria.Multiselect)
			{
				for (uint i = 0; i < Objects.Size(); i++)
					Objects[i]->CheckState(true);
			}
			return 0;
		}
		case 6:
			ProperReturn = true;
		case 7:
			DestroyWindow(_Base);
			return 0;
		default:
			return 0;
		}
	}
}