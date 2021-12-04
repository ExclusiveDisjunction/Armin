#include "GroupSelector.h"

#include "..\..\Files\ArminSessions.h"

namespace Armin::UI::Tool
{
	using namespace Files;

	GroupSelector::GroupSelector(HINSTANCE ins, Files::InventorySystem* System, GroupSelectorSource Source, bool Multiselect)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Multi = Multiselect;
		if (!System)
			System = dynamic_cast<InventorySystem*>(LoadedProject);

		_Base = CreateWindowExW(NULL, MAKEINTATOM(_ThisAtom), L"Group Selector", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 400, 500, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_SHOW);
		UpdateWindow(_Base);

		LoadControls(Source, System, Multiselect);
	}

	StringList GroupSelector::Execute(HINSTANCE ins, InventorySystem* System, GroupSelectorSource Source, bool Multiselect)
	{
		GroupSelector* Selector = new GroupSelector(ins, System, Source, Multiselect);

		MSG msg;
		int Result;
		while ((Result = GetMessageW(&msg, Selector->_Base, 0, 0)) != 0)
		{
			if (Result < 0)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		StringList Return = Selector->Return;
		delete Selector;

		return Return;
	}

	LRESULT __stdcall GroupSelector::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		GroupSelector* Item = reinterpret_cast<GroupSelector*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_SIZE:
			return Item->Size();
		case WM_PAINT:
			return Item->Paint();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_DESTROY:
			return Item->Destroy();
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}
	ATOM GroupSelector::_ThisAtom = ATOM();
	void GroupSelector::InitBase(HINSTANCE ins)
	{
		WNDCLASS wn = { 0 };
		wn.lpfnWndProc = GroupSelector::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"GroupSelectorClass";
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(GroupSelector*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}

	void GroupSelector::LoadControls(GroupSelectorSource Source, InventorySystem* System, bool Multi)
	{
		InventorySystem* File = dynamic_cast<InventorySystem*>(LoadedProject);
		if (!File)
			return;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 120;
			int Height = 30;

			StyleSheet Style;
			Style.Background = Background2;
			Style.BaseBackground = Background2;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;

			XCoord = (WndRect.right / 2) - (Width / 2);

			Submit = new Button(XCoord, YCoord, Width, Height, L"Submit", _Base, (HMENU)1, ins, Style, TextStyle);
			YCoord += 5 + Height;

			Style.Background = Grey3;

			XCoord = 10;
			Width = WndRect.right - (XCoord * 2);

			Text = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle);

			YCoord += 5 + Height;
			Height = WndRect.bottom - (10 + YCoord);

			Scroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			View = new Grid(0, 0, Width, Height, Scroll, ins, Style);
			Scroll->SetViewer(View);
		}

		{
			RECT ThisRect;
			GetClientRect(View, &ThisRect);

			int XCoord = 5;
			int YCoord = 5;
			int Width = 900;
			int Height = 27;

			StyleSheet Style;
			Style.BorderThickness = 0;
			Style.Radius = 20;
			Style.Background = Style.BaseBackground = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 0;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;
			TextStyle.FontSize = 13;

			StringList Groups;

			InventoryItemGroup* Inventory = File->InventoryItems;
			OperationInventoryItemGroup* OperationInventory = File->OperationInventoryItems;

			if (Source & GSS_Inventory && Inventory)
			{
				for (uint i = 0; i < Inventory->Count; i++)
				{
					String Current = Inventory->Item(i)->Group;
					if (Current == L"")
						Current = L"[NONE]";

					if (!Groups.Contains(Current))
						Groups.Add(Current);
				}
			}
			else if (Source & GSS_OperationInventory && OperationInventory)
			{
				for (uint i = 0; i < OperationInventory->Count; i++)
				{
					String Current = OperationInventory->Item(i)->Group;

					if (Current == L"")
						Current = L"[NONE]";

					if (!Groups.Contains(Current))
						Groups.Add(Current);
				}
			}

			View->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Groups.Size + 1)));
			Scroll->Reset();

			for (uint i = 0; i < Groups.Size; i++, YCoord += 5 + Height)
				this->Groups.Add(new ToggleButton(XCoord, YCoord, Width, Height, View, ins, (HMENU)2, Groups[i], Multi, Style, TextStyle));
		}

		_Loaded = true;
	}

	LRESULT GroupSelector::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		HBRUSH Bk = CreateSolidBrush(static_cast<COLORREF>(Background2));

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT GroupSelector::Command(WPARAM wp, LPARAM lp)
	{
		if (wp == 1)
		{
			_Calculated = true;

			String Raw = Text->GetText();
			if (_Multi)
			{
				StringList Splits = Raw.Split(L',');
				for (String& Item : Splits)
				{
					Item = Item.RemoveFromFront(L' ');
					Item = Item.RemoveFromEnd(L' ');
					if (Item == L"[NONE]")
						Item = String();
				}

				Return = Splits;
			}
			else
			{
				if (Raw == L"" || Raw == L"[NONE]")
					Return = L"";
				else
					Return = Raw;
			}

			DestroyWindow(_Base);
		}
		else if (wp == 2)
		{		
			ToggleButton* Target = reinterpret_cast<ToggleButton*>(lp);
			if (!Target)
				return 0;

			bool Checked = Target->CheckState();

			if (Checked)
			{
				String Header = Target->Text();
				if (_Multi)
				{
					String OldText = Text->GetText();
					if (OldText == String())
						Text->SetText(Header);
					else
						Text->SetText(OldText + L", " + Header);
				}
				else
					Text->SetText(Header);
			}			
		}

		return 0;
	}
	LRESULT GroupSelector::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 120;
			int Height = 30;

			XCoord = (WndRect.right / 2) - (Width / 2);

			Submit->Move(XCoord, YCoord, Width, Height);

			XCoord = 10;
			Width = WndRect.right - (XCoord * 2);

			Text->Move(XCoord, YCoord, Width, Height);

			YCoord += 5 + Height;
			Height = WndRect.bottom - (10 + YCoord);

			Scroll->Move(XCoord, YCoord, Width, Height);
			View->Move(0, 0, Width, Height);
		}

		{
			int XCoord = 5;
			int YCoord = 5;
			int Width = 900;
			int Height = 30;

			View->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Groups.Size + 1)));

			for (uint i = 0; i < Groups.Size; i++)
			{
				Groups[i]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;
			}
		}

		return 0;
	}
	LRESULT GroupSelector::Destroy()
	{
		if (!_Calculated)
			Return = L"";

		return 0;
	}
}