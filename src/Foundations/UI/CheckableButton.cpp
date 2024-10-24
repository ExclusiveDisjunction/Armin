#include "CheckableButton.h"

CheckableButton::CheckableButton(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, bool IsChecked, String Text, CheckableButtonType Type, StyleSheet Style, TextStyleSheet TextStyle)
{
	if (!_ThisAtom)
		InitBase(ins);

	_Check = IsChecked;
	_Type = Type;
	_Style = Style;
	_TextStyle = TextStyle;
	_Text = Text;

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, X, Y, Width, Height, Parent, ID, ins, NULL);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

ATOM CheckableButton::_ThisAtom = ATOM();
void CheckableButton::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = CheckableButton::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"CheckableButtonClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(CheckableButton*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}
LRESULT __stdcall CheckableButton::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{	
	CheckableButton* This = reinterpret_cast<CheckableButton*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!This)
		return DefWindowProc(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_SIZE:
		return 0;
	case WM_ENABLE:
		This->IsEnabled = wp;
		return 0;
	case ACBM_UNCHECK:
		This->Uncheck(wp > 0);
		return 0;
	case ACBM_CHECK:
		This->Check();
		return 0;
	case WM_SHOWWINDOW:
		return 0;
	case WM_MOUSEMOVE:
		return This->MouseMove();
	case WM_MOUSELEAVE:
		return This->MouseLeave();
	default:
		return CommonWndProc(Window, Message, wp, lp);
	}
}

void CheckableButton::SetCheckState(bool New)
{
	if (_Type == CBT_Radio && New)
	{
		HWND Parent = GetParent(_Base);

		for (HWND Child = GetTopWindow(Parent); Child != nullptr; Child = GetNextWindow(Child, GW_HWNDNEXT))
			SendMessageW(Child, ACBM_UNCHECK, 1, 0);
	}

	if (New)
		Check();
	else
		Uncheck(false);

	RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	SendMessageW(GetParent(_Base), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(_Base)), static_cast<LPARAM>(_Check));
}

LRESULT CheckableButton::KeyDown(WPARAM wp)
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
LRESULT CheckableButton::Paint()
{
	PAINTSTRUCT Ps;
	HDC Dc = BeginPaint(_Base, &Ps);

	bool Checked = _Check;

	AaColor Hover = _Style.BorderBrush;
	Hover.Reduce(0.7f);
	AaColor BkColor = Checked ? _Style.BorderBrush : _Style.Background;
	AaColor ForegroundColor = _TextStyle.Foreground;
	AaColor BorderColor = _Style.BorderBrush;
	if (!IsEnabled || !::IsWindowEnabled(_Base))
	{
		BkColor.Reduce(0.7f);
		ForegroundColor.Reduce(0.7f);
		BorderColor.Reduce(0.7f);
	}

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	HBRUSH Base = CreateSolidBrush(_HasMouse ? Hover : _Style.BaseBackground), Bk = CreateSolidBrush(BkColor);
	FillRect(Dc, &WndRect, Base);

	HPEN Border = CreatePen(PS_SOLID, _Style.BorderThickness, BorderColor);

	HFONT Font = CreateFontW(-MulDiv(_TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_TextStyle.Bold ? FW_BOLD : 0), _TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_TextStyle.FontFamily));
	SetTextColor(Dc, ForegroundColor);
	SetBkMode(Dc, TRANSPARENT);

	SelectObject(Dc, Base);
	SelectObject(Dc, Font);
	if (_Style.BorderThickness == 0)
		SelectObject(Dc, GetStockObject(NULL_PEN));
	else
		SelectObject(Dc, Border);

	DeleteObject(Base);
	SelectObject(Dc, Bk);

	RECT DrawArea = { 3, 3, WndRect.bottom - 6, WndRect.bottom - 6 };

	switch (_Type)
	{
	case CBT_Radio:
		Ellipse(Dc, DrawArea.left, DrawArea.top, DrawArea.right, DrawArea.bottom);
		break;
	case CBT_CheckBox:
		Rectangle(Dc, DrawArea.left, DrawArea.top, DrawArea.right, DrawArea.bottom);
		break;
	}

	RECT TextArea = { DrawArea.left + DrawArea.right + 3, 0, WndRect.right - (DrawArea.left + DrawArea.right + 3), WndRect.bottom };

	const wchar_t* Text = _Text.ConstWchar();
	int TextLen = static_cast<int>(_Text.Length());

	SIZE TextSize;
	GetTextExtentPoint32W(Dc, Text, TextLen, &TextSize);

	int TextX = _Style.BorderThickness + 1;
	int TextY = (WndRect.bottom / 2) - (TextSize.cy / 2);
	DrawTextW(Dc, Text, TextLen, &TextArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

	DeleteObject(Bk);
	DeleteObject(Border);
	DeleteObject(Font);

	EndPaint(_Base, &Ps);
	return 0;
}
LRESULT CheckableButton::Click()
{
	if (_Type == CBT_CheckBox)
		SetCheckState(!_Check);
	else
		SetCheckState(true);
	return 0;
}
LRESULT CheckableButton::MouseMove()
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
LRESULT CheckableButton::MouseLeave()
{
	_HasMouse = false;
	Redraw();
	return 0;
}