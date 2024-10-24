#include "ComboBox.h"

#include "ScrollViewer.h"
#include "Grid.h"
#include "Button.h"

#define ComboBox_SetNewText 0x401

struct ComboBoxWndInfo
{
	ScrollViewer* View;
	Grid* Grid;
	Vector<Button*> Objects;

	String Return;

	StyleSheet Style;
	TextStyleSheet TextStyle;
	HWND MsgTarget;
	bool SentMessage = false, Clicked = false;
};

ComboBox::ComboBox(int X, int Y, int Width, int Height, HWND Parent, HMENU ID, HINSTANCE ins, StringList Items, StyleSheet Style, TextStyleSheet TextStyle)
{
	if (!_ThisAtom)
		InitBase(ins);

	_Style = Style;
	_TextStyle = TextStyle;
	_Items = Items;
	
	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, X, Y, Width, Height, Parent, ID, ins, NULL);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

ATOM ComboBox::_ThisAtom = ATOM(), ComboBox::_WndAtom = ATOM();
void ComboBox::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = ComboBox::ThisProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hbrBackground = NULL;
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"ComboBoxClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(ComboBox*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);

	wn = { 0 };
	wn.lpfnWndProc = ComboBox::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hbrBackground = NULL;
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"ComboBoxPopoutClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(ComboBox*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_WndAtom = RegisterClassW(&wn);
}
LRESULT __stdcall ComboBox::ThisProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	ComboBox* Item = reinterpret_cast<ComboBox*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!Item)
		return DefWindowProcW(Window, Message, wp, lp);

	if (Message == WM_COMMAND)
		return Item->Command(wp, lp);
	else if (Message == WM_ERASEBKGND)
		return 1;
	else if (Message == WM_DESTROY)
		return Item->OnDestroy();
	else if (Message == ComboBox_SetNewText)
	{
		const wchar_t* Arr = reinterpret_cast<const wchar_t*>(wp);
		String This(Arr);

		Item->_Current = Item->_Items.IndexOf(This);
		Item->IsPopout = false;
		Item->Redraw();
		SendMessageW(GetParent(Window), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(Window)), reinterpret_cast<LPARAM>(Item));

		Item->UpdateDrawer();
		return 0;
	}

	return CommonWndProc(Window, Message, wp, lp);
}
LRESULT __stdcall ComboBox::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	ComboBoxWndInfo* Info = reinterpret_cast<ComboBoxWndInfo*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!Info)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_PAINT:
	{
		StyleSheet& Style = Info->Style;
		TextStyleSheet& TextStyle = Info->TextStyle;

		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(Window, &ps);

		RECT WndRect;
		GetClientRect(Window, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Style.BaseBackground);
		SelectObject(Dc, Bk);

		Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

		DeleteObject(Bk);
		EndPaint(Window, &ps);
		return 0;
	}
	case WM_COMMAND:
		Info->Return = reinterpret_cast<Button*>(lp)->Text();
		Info->Clicked = true;
	case WM_KILLFOCUS:
	{
		if (!Info->SentMessage && Info->Clicked)
		{
			SendMessageW(Info->MsgTarget, ComboBox_SetNewText, reinterpret_cast<WPARAM>(static_cast<const wchar_t*>(Info->Return)), 0);
			Info->SentMessage = true;
		}
		DestroyWindow(Window);
		SetFocus(GetAncestor(Info->MsgTarget, GA_ROOT));
		return 0;
	}
	case WM_DESTROY:
	{
		delete Info->View;
		Info->SentMessage = false;

		Info->View = nullptr;
		Info->Grid = nullptr;

		SetWindowLongPtrW(Window, GWLP_USERDATA, 0);
		delete Info;

		return 0;
	}
	}	

	return DefWindowProcW(Window, Message, wp, lp);
}

void ComboBox::Items(StringList New)
{
	_Items = New;
}

String ComboBox::Current()
{
	if (_Current >= _Items.Size)
		return String();

	return _Items[_Current];
}
bool ComboBox::Seek(String New)
{
	auto Condition = [New](String& Current) { return Current == New; };
	bool Return = _Items.Search(Condition);
	if (!Return)
		return false;

	_Current = _Items.IndexOf(New);
	Redraw();

	return true;
}
void ComboBox::UpdateDrawer()
{
	if (IsPopout)
	{
		ComboBoxWndInfo* Info = new ComboBoxWndInfo();
		Info->Style = _Style;
		Info->TextStyle = _TextStyle;
		Info->MsgTarget = _Base;
		Info->Return = (_Items.Size == 0 ? L"" : _Items[0]);

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		RECT WndRect = { 0 };
		GetClientRect(_Base, &WndRect);
		POINT XY = { 0, WndRect.bottom };
		MapWindowPoints(_Base, NULL, &XY, 1);

		int Height = (_Items.Size + 1) * 32;
		if (_Items.Size > 4u)
			Height = 5 * 32;

		_Wnd = CreateWindowExW(WS_EX_TOOLWINDOW, MAKEINTATOM(_WndAtom), L"", WS_VISIBLE | WS_POPUP, XY.x, XY.y, WndRect.right, Height, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Wnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Info));

		GetClientRect(_Wnd, &WndRect);

		StyleSheet Temp = _Style;
		Temp.BorderThickness = 0;
		Temp.Radius = 0;

		Info->View = new ScrollViewer(0, 0, WndRect.right, WndRect.bottom, _Wnd, ins, Temp);
		Info->Grid = new Grid(0, 0, WndRect.right + 40, static_cast<int>(32 * (_Items.Size + 7)), Info->View, ins, Temp);
		Info->View->SetViewer(Info->Grid);

		int XCoord = 3;
		int YCoord = 3;
		int Width = WndRect.right - XCoord * 2;
		Height = 27;

		for (uint i = 0; i < _Items.Size; i++)
		{
			Info->Objects.Add(new Button(XCoord, YCoord, Width, Height, _Items[i], Info->Grid, (HMENU)1, ins, Temp, _TextStyle, TA_LeftAlignment));
			YCoord += 5 + Height;
		}

		ShowWindow(_Wnd, SW_SHOW);
		UpdateWindow(_Wnd);
		SetFocus(_Wnd);
	}
	else
	{
		DestroyWindow(_Wnd);
		_Wnd = nullptr;
		IsPopout = false;
	}
}

LRESULT ComboBox::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	HBRUSH Bk = CreateSolidBrush(_Style.Background);
	HPEN Border = CreatePen(PS_SOLID, _Style.BorderThickness, _Style.BorderBrush);

	HFONT Font = CreateFontW(-MulDiv(_TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_TextStyle.Bold ? FW_BOLD : 0), _TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_TextStyle.FontFamily));
	SetTextColor(Dc, _TextStyle.Foreground);
	SetBkMode(Dc, TRANSPARENT);
	
	SelectObject(Dc, Bk);	
	SelectObject(Dc, Font);
	if (_Style.BorderThickness == 0)
		SelectObject(Dc, GetStockObject(NULL_PEN));
	else
		SelectObject(Dc, Border);

	Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

	WndRect.left += _Style.BorderThickness;
	WndRect.top += _Style.BorderThickness;
	WndRect.right -= _Style.BorderThickness;
	WndRect.bottom -= _Style.BorderThickness;

	const wchar_t* Text = _Items[_Current].ConstWchar();
	int TextLen = static_cast<int>(_Items[_Current].Length());
	DrawTextW(Dc, Text, TextLen, &WndRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

	RECT AccentArea = { WndRect.right - (WndRect.bottom - 8), 8, WndRect.right - 8, WndRect.bottom - 8 };
	
	DeleteObject(Bk);
	Bk = CreateSolidBrush(_Style.BorderBrush);
	SelectObject(Dc, Bk);
	Rectangle(Dc, AccentArea.left, AccentArea.top, AccentArea.right, AccentArea.bottom);

	DeleteObject(Bk);
	DeleteObject(Border);
	DeleteObject(Font);
	
	EndPaint(_Base, &ps);
	return 0;
}
LRESULT ComboBox::Click()
{
	IsPopout = !IsPopout;
	UpdateDrawer();

	return 0;
}
LRESULT ComboBox::Size()
{
	return 0;
}
LRESULT ComboBox::LostFocus()
{
	return 0;
}
LRESULT ComboBox::Command(WPARAM wp, LPARAM lp)
{
	Button* Sender = reinterpret_cast<Button*>(lp);
	if (!Sender)
		return 0;

	return 0;
}
LRESULT ComboBox::OnDestroy()
{
	DestroyWindow(_Wnd);
	return 0;
}