#include "SearchByName.h"

#include "UI\Controls.h"
#include "..\..\Common.h"
#include "..\..\Files\Components.h"
#include "..\..\UI\ArminControls.h"

namespace Armin::UI::Search
{
	using namespace Files;

	SearchByName::SearchByName(Files::SearchCriteria& Criteria, HINSTANCE ins, ProjectBase* File)
	{
		this->Criteria = Criteria;
		this->File = !File ? LoadedProject : File;

		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Search", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 880, 450, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadControls();
	}

	Vector<Component*> SearchByName::Execute(Files::SearchCriteria& Criteria, HINSTANCE ins, ProjectBase* File)
	{
		SearchByName* Item = new SearchByName(Criteria, ins, File);

		MSG msg;
		int Result;
		while ((Result = GetMessageW(&msg, Item->_Base, NULL, NULL)) != 0)
		{
			if (Result < 0)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		Vector<Component*> Return = Item->Return;
		delete Item;

		return Return;
	}

	void SearchByName::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseXCoord = 0;
		int BaseYCoord = 0;

		{
			StyleSheet Style;
			Style.Background = Accent2;
			Style.BorderBrush = 0x00FFFFFF;
			Style.BorderThickness = 0;

			int XCoord = 0, YCoord = 0, Width = 180, Height = WndRect.bottom;
			
			ButtonHost = new Grid(XCoord, YCoord, Width, Height, _Base, ins, Style);
			
			BaseXCoord = XCoord + Width + 10;
		}
		
		{
			RECT Client;
			GetClientRect(*ButtonHost, &Client);

			int XCoord = 5;
			int YCoord = 5;
			int Width = Client.right - (XCoord * 2);
			int Height = 27;

			StyleSheet Style;
			Style.Background = Style.BaseBackground = Accent2;
			Style.BorderBrush = 0xFFFFFFFF;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;

			Submit = new Button(XCoord, YCoord, Width, Height, L"Submit", ButtonHost, (HMENU)1, ins, Style, TextStyle);
			YCoord += 10 + Height;

			Cancel = new Button(XCoord, YCoord, Width, Height, L"Cancel", ButtonHost, (HMENU)2, ins, Style, TextStyle);
			YCoord += 25 + Height;

			SelectAll = new Button(XCoord, YCoord, Width, Height, L"Select All", ButtonHost, (HMENU)3, ins, Style, TextStyle);
			EnableWindow(*SelectAll, Criteria.Multiselect);
			YCoord += 10 + Height;

			ClearSelection = new Button(XCoord, YCoord, Width, Height, L"Clear Selection", ButtonHost, (HMENU)4, ins, Style, TextStyle);
		}

		{
			int ButtonSize = 80;

			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord = 10;
			int Width = WndRect.right - (10 + XCoord + ButtonSize + 5);
			int Height = 27;

			StyleSheet Style;
			Style.Background = Style.BaseBackground = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.Foreground = FontColor;
			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;

			NameToSearch = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			XCoord += 5 + Width;

			_RunSearch = new Button(XCoord, YCoord, ButtonSize, Height, L"Search", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 10 + Height;

			XCoord = BaseXCoord;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);
		}

		Refresh();
	}
	void SearchByName::GetReturn()
	{
		Return = ComponentViewer::RetriveFromList(Objects);
	}
	void SearchByName::Refresh()
	{
		Criteria.Arguments = NameToSearch->GetText();
		Vector<Component*> Selected = Criteria.GetComponents(File);
		
		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(Selected, ObjectView, NULL, Criteria.Multiselect, true, ObjectScroll);

		if (_FirstRefresh)
		{
			Vector<Component*>& PreSelected = Criteria.PreSelected;

			if (!Criteria.Multiselect)
				PreSelected.Resize(1);

			for (uint i = 0; i < this->Objects.Size; i++)
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

	ATOM SearchByName::_ThisAtom = ATOM();
	void SearchByName::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = SearchByName::WndProc;
		wn.hInstance = ins;
		wn.hCursor = ArminCursor;
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"SearchByNameClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(SearchByName*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall SearchByName::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		SearchByName* Item = reinterpret_cast<SearchByName*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_SIZE:
			return Item->Size();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_KEYDOWN:
			return Item->KeyDown(wp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	LRESULT SearchByName::Paint()
	{
		PAINTSTRUCT ps = { 0 };
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background2);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT SearchByName::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseXCoord = 0;
		int BaseYCoord = 0;

		{
			int XCoord = 0, YCoord = 0, Width = 180, Height = WndRect.bottom;

			ButtonHost->Move(XCoord, YCoord, Width, Height);

			BaseXCoord = XCoord + Width + 10;
		}

		{
			RECT Client;
			GetClientRect(*ButtonHost, &Client);

			int XCoord = 5;
			int YCoord = 5;
			int Width = Client.right - (XCoord * 2);
			int Height = 27;

			Submit->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Cancel->Move(XCoord, YCoord, Width, Height);
			YCoord += 25 + Height;

			SelectAll->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			ClearSelection->Move(XCoord, YCoord, Width, Height);
		}

		{
			int ButtonSize = 80;

			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord = 10;
			int Width = WndRect.right - (10 + XCoord + ButtonSize + 5);
			int Height = 27;

			NameToSearch->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			_RunSearch->Move(XCoord, YCoord, ButtonSize, Height);
			YCoord += 10 + Height;

			XCoord = BaseXCoord;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}

		return 0;
	}
	LRESULT SearchByName::KeyDown(WPARAM wp)
	{
		switch (wp)
		{
		case VK_RETURN:
			if (GetKeyState(VK_CONTROL) & 0x8000)
				Command(1, 0);
			else
				Refresh();
			break;
		case VK_ESCAPE:
			Return.Clear();
			DestroyWindow(_Base);
			break;
		}
		return 0;
	}
	LRESULT SearchByName::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1: //Submit
			GetReturn();
			DestroyWindow(_Base);
			break;
		case 2: //Close
			DestroyWindow(_Base);
			break;
		case 3: //Select All
			if (Objects.Size != 0 && !Criteria.Multiselect)
			{
				EnableWindow(*SelectAll, false);
				break;
			}

			for (uint i = 0; i < Objects.Size; i++)
				Objects[i]->CheckState(true);
			break;
		case 4: //Clear Selection
			for (uint i = 0; i < Objects.Size; i++)
				Objects[i]->CheckState(false);
			break;
		}
		return 0;
	}
}