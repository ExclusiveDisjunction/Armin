#include "ToggleButton.h"

ToggleButton::ToggleButton(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, String Text, bool Multiselect, StyleSheet Style, TextStyleSheet TextStyle, bool CheckState)
{
	if (!_ThisAtom)
		InitBase(ins);

	_Checked = CheckState;
	_Multi = Multiselect;
	_Style = Style;
	_TextStyle = TextStyle;
	_Text = Text;

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, XCoord, YCoord, Width, Height, Parent, ID, ins, NULL);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

ATOM ToggleButton::_ThisAtom = ATOM();
void ToggleButton::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = ToggleButton::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hbrBackground = NULL;
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"ToggleButtonClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(ToggleButton*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}
LRESULT ToggleButton::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	ToggleButton* This = reinterpret_cast<ToggleButton*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!This)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_ENABLE:
		This->IsEnabled = wp;
		return 0;
	case ACTB_UNCHECK:
		This->Uncheck();
		return 0;
	case ACTB_CHECK:
		This->Check();
		return 0;
	case WM_MOUSEMOVE:
		return This->MouseMove();
	case WM_MOUSELEAVE:
		return This->MouseLeave();
	default:
		return CommonWndProc(Window, Message, wp, lp);
	}
}

void ToggleButton::Uncheck()
{
	_Checked = false;
	Redraw();
}
void ToggleButton::Check()
{
	_Checked = true;
	Redraw();
}
void ToggleButton::CheckState(bool New)
{
	if (!_Multi && New)
	{
		HWND Parent = GetParent(_Base);

		for (HWND Child = GetTopWindow(Parent); Child != nullptr; Child = GetNextWindow(Child, GW_HWNDNEXT))
			SendMessageW(Child, ACTB_UNCHECK, 0, 0);
	}

	if (New)
		Check();
	else
		Uncheck();

	SendMessageW(GetParent(_Base), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(_Base)), reinterpret_cast<LONG_PTR>(this));
}

LRESULT ToggleButton::KeyDown(WPARAM wp)
{
	if (wp == VK_RETURN)
		return SendMessageW(_Base, WM_LBUTTONDOWN, 0, 0);
	else if (wp == VK_TAB)
	{
		if (GetKeyState(VK_SHIFT) & 0x8000)
			SetFocus(GetWindow(_Base, GW_HWNDPREV));
		else
			SetFocus(GetWindow(_Base, GW_HWNDNEXT));
		return 0;
	}
	else
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
}
LRESULT ToggleButton::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	AaColor HoverColor = _Style.BorderBrush;
	HoverColor.Reduce(0.7f);

	AaColor BkColor = (_Checked) ? _Style.BorderBrush : (_HasMouse ? HoverColor : _Style.Background), ForegroundColor = _TextStyle.Foreground;
	if (!IsEnabled)
	{
		BkColor.Reduce(0.7f);
		ForegroundColor.Reduce(0.7f);
	}

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	HBRUSH Bk = CreateSolidBrush(BkColor), Foreground = CreateSolidBrush(ForegroundColor);
	FillRect(Dc, &WndRect, Bk);

	HFONT Font = CreateFontW(-MulDiv(_TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_TextStyle.Bold ? FW_BOLD : 0), _TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_TextStyle.FontFamily));
	SetTextColor(Dc, ForegroundColor);
	SetBkMode(Dc, TRANSPARENT);
	SelectObject(Dc, Font);

	const wchar_t* Text = _Text.ConstWchar();
	int TextLen = static_cast<int>(_Text.Length());

	SIZE TextSize;
	GetTextExtentPoint32W(Dc, Text, TextLen, &TextSize);

	int TextX = WndRect.left + 7;
	int TextY = (WndRect.bottom / 2) - (TextSize.cy / 2);
	RECT TextArea = { TextX, TextY, WndRect.right, WndRect.bottom };

	DrawTextW(Dc, Text, TextLen, &TextArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

	DeleteObject(Bk);
	DeleteObject(Font);
	EndPaint(_Base, &ps);
	return 0;
}
LRESULT ToggleButton::Click()
{
	if (_Multi)
		CheckState(!_Checked);
	else
		CheckState(true);
	return 0;
}
LRESULT ToggleButton::MouseMove()
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
LRESULT ToggleButton::MouseLeave()
{
	_HasMouse = false;
	Redraw();
	return 0;
}