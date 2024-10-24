#include "CalendarGrid.h"

CalendarGrid::CalendarGrid(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, int Day, String Memo, StyleSheet Style, TextStyleSheet TextStyle)
{
	if (!_ThisAtom)
		InitBase(ins);

	this->Memo = Memo;
	this->Date = Day;
	_Style = Style;
	_TextStyle = TextStyle;

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), nullptr, WS_CHILD | WS_VISIBLE, XCoord, YCoord, Width, Height, Parent, ID, ins, nullptr);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}	

ATOM CalendarGrid::_ThisAtom = ATOM();
void CalendarGrid::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = WndProc;
	wn.lpszClassName = L"CalendarGridClass";
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(CalendarGrid*);
	wn.hbrBackground = nullptr;
	wn.hIcon = nullptr;
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}
LRESULT __stdcall CalendarGrid::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	CalendarGrid* Obj = reinterpret_cast<CalendarGrid*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!Obj)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_MOUSEMOVE:
		return Obj->MouseMove();
	case WM_MOUSELEAVE:
		return Obj->MouseLeave();
	default:
		return CommonWndProc(Window, Message, wp, lp);
	}
}

LRESULT CalendarGrid::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	AaColor BkColor = (_HasMouse ? _Style.BorderBrush : _Style.Background), ForegroundColor = _TextStyle.Foreground;

	if (!IsEnabled)
	{
		BkColor.Reduce(0.7f);
		ForegroundColor.Reduce(0.7f);
	}

	HBRUSH BkBrush = CreateSolidBrush(BkColor);
		
	HFONT Font = CreateFontW(-MulDiv(_TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_TextStyle.Bold ? FW_BOLD : 0), _TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_TextStyle.FontFamily));
	SetTextColor(Dc, ForegroundColor);
	SetBkMode(Dc, TRANSPARENT);

	SelectObject(Dc, BkBrush);
	SelectObject(Dc, Font);

	SetTextCharacterExtra(Dc, GetTextCharacterExtra(Dc) + _TextStyle.CharSpacing);

	SelectObject(Dc, GetStockObject(NULL_PEN));

	RECT WndRect;
	GetClientRect(_Base, &WndRect);
	FillRect(Dc, &WndRect, BkBrush);

	String NumTextR(Date);
	const wchar_t* NumText = static_cast<const wchar_t*>(NumTextR);
	int TextLen = lstrlenW(NumText);

	if (Date != 0)
	{
		RECT NumPos = { 2, 2, 20, 20 };
		DrawTextW(Dc, NumText, TextLen, &NumPos, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	NumText = static_cast<const wchar_t*>(this->Memo);
	TextLen = lstrlenW(NumText);

	SIZE Size;
	GetTextExtentPoint32W(Dc, NumText, TextLen, &Size);
	RECT BoundingBox = { 2, 23, WndRect.right - 2 - 2, WndRect.bottom - 23 - 2};
	DrawTextW(Dc, NumText, TextLen, &BoundingBox, DT_CENTER | DT_WORDBREAK | DT_NOPREFIX);

	DeleteObject(BkBrush);
	DeleteObject(Font);

	EndPaint(_Base, &ps);
	return 0;
}
LRESULT CalendarGrid::Click()
{
	return SendMessageW(GetParent(_Base), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(_Base)), reinterpret_cast<LPARAM>(this));
}
LRESULT CalendarGrid::MouseLeave()
{
	_HasMouse = false;
	Redraw();
	return 0;
}
LRESULT CalendarGrid::MouseMove()
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