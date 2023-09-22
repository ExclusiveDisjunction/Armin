#include "Button.h"

Button::Button(int X, int Y, int Width, int Height, String Text, HWND Parent, HMENU ID, HINSTANCE ins, StyleSheet _Style, TextStyleSheet _Text, TextAlignment Alignment)
{
	if (!_ThisAtom)
		InitBase(ins);

	this->_Text = Text;
	this->_Style = _Style;
	this->_TextStyle = _Text;
	this->_Alignment = Alignment;

	_Base = CreateWindowExW(0l, L"ButtonClass", L"", WS_CHILD | WS_VISIBLE, X, Y, Width, Height, Parent, ID, ins, NULL);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

ATOM Button::_ThisAtom = ATOM();
void Button::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.cbWndExtra = sizeof(Button*);
	wn.cbClsExtra = 0;
	wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wn.lpfnWndProc = Button::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hIcon = NULL;
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"ButtonClass";
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}

LRESULT Button::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);
	SetBkMode(Dc, TRANSPARENT);

	AaColor BkColor = (_Clicked || _HasMouse ? _Style.BorderBrush : _Style.Background), BorderColor = _Style.BorderBrush, ForegroundColor = _TextStyle.Foreground;

	if (!IsEnabled)
	{
		BorderColor.Reduce(0.7f);
		ForegroundColor.Reduce(0.7f);
		BkColor.Reduce(0.7f);
	}

	HBRUSH BkBrush = CreateSolidBrush(BkColor);
	HPEN Border = CreatePen(PS_SOLID, _Style.BorderThickness, BorderColor);

	HFONT Font = CreateFontW(-MulDiv(_TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_TextStyle.Bold ? FW_BOLD : 0), _TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_TextStyle.FontFamily));
	SetTextColor(Dc, ForegroundColor);

	SelectObject(Dc, BkBrush);
	SelectObject(Dc, Font);

	SetTextCharacterExtra(Dc, GetTextCharacterExtra(Dc) + _TextStyle.CharSpacing);

	if (_Style.BorderThickness == 0)
		SelectObject(Dc, GetStockObject(NULL_PEN));
	else
		SelectObject(Dc, Border);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	if (_Style.Radius > 0)
		RoundRect(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom, _Style.Radius, _Style.Radius);
	else
		Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

	const wchar_t* Text = _Text.ConstWchar();
	int TextLen = lstrlenW(Text);

	DrawTextW(Dc, Text, TextLen, &WndRect, (_Alignment == TA_CenterAlignment ? DT_CENTER : _Alignment == TA_LeftAlignment ? DT_LEFT : DT_RIGHT) | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
	
	DeleteObject(BkBrush);
	DeleteObject(Border);
	DeleteObject(Font);
	
	EndPaint(_Base, &ps);

	return 0;
}
LRESULT Button::Click()
{
	_Clicked = true;
	SendMessageW(::GetParent(_Base), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(_Base)), reinterpret_cast<LPARAM>(this));
	SetTimer(_Base, 1, 100, NULL);

	RedrawWindow(_Base, NULL, NULL, RDW_ERASENOW | RDW_UPDATENOW | RDW_INVALIDATE);

	return 0;
}
LRESULT Button::MouseLeave()
{
	_HasMouse = false;
	Redraw();
	return 0;
}
LRESULT Button::MouseMove()
{
	if (!_HasMouse)
	{
		_HasMouse = true;
		TRACKMOUSEEVENT tme = { sizeof(tme) };
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = _Base;
		TrackMouseEvent(&tme);

		Redraw();
	}

	return 0;
}
LRESULT Button::TimerTick(LONG ID)
{
	if (ID == 1)
	{
		_Clicked = false;

		RedrawWindow(_Base, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);

		KillTimer(_Base, ID);
	}	

	return 0;
}
LRESULT Button::KeyDown(WPARAM wp)
{
	if (wp == VK_RETURN)
		return Click();
	else if (wp == VK_TAB)
	{
		HWND Other;
		if (GetKeyState(VK_SHIFT) & 0x800)
			Other = GetWindow(_Base, GW_HWNDLAST);
		else
			Other = GetWindow(_Base, GW_HWNDNEXT);
		SetFocus(Other);
		return 0;
	}
	else
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
}
LRESULT Button::GotFocus()
{
	Redraw();
	return 0;
}
LRESULT Button::LostFocus()
{
	Redraw();
	return 0;
}