#include "Label.h"

Label::Label(int X, int Y, int Width, int Height, HWND Window, HINSTANCE ins, String Text, AaColor BaseBk, TextStyleSheet Style, bool ShowScroll)
{
	if (!_ThisAtom)
		InitBase(ins);

	this->Text = Text;
	this->_Style = Style;
	this->_Bk = BaseBk;
	this->ShowScroll = ShowScroll;
	this->_Lines = Text.Split(L'\n');

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, X, Y, Width, Height, Window, NULL, ins, NULL);
	int Result = GetLastError();

	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	Reset();
}
Label::~Label()
{
	_Lines.Clear();
}

ATOM Label::_ThisAtom = ATOM();
void Label::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = Label::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"LabelClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(Label*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}
LRESULT __stdcall Label::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	Label* Item = reinterpret_cast<Label*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!Item)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_ERASEBKGND:
		return TRUE;
	case WM_VSCROLL:
		return Item->VScroll(wp);
	case WM_HSCROLL:
		return Item->HScroll(wp);
	case WM_MOUSEWHEEL:
		return Item->MouseScroll(wp);
	case WM_MOUSEHWHEEL:
		return Item->HMouseScroll(wp);
	default:
		return CommonWndProc(Window, Message, wp, lp);
	}
}

void Label::Reset()
{
	auto LongestLine = [](StringList& Lines) -> uint
	{
		uint Last = 0u;
		for (uint i = 0; i < Lines.Size; i++)
		{
			uint Current = Lines[i].Length();
			if (Current > Last)
				Last = Current;
		}

		return Last;
	};

	StringList& Lines = _Lines;
	uint LargestLen = LongestLine(Lines);
	uint LinesCount = Lines.Size;

	HDC Dc = GetDC(_Base);

	TEXTMETRIC tm;
	GetTextMetricsW(Dc, &tm);
	HSUnit = tm.tmAveCharWidth + 1;
	VSUnit = tm.tmHeight + 8;
	HUnitMax = ((tm.tmPitchAndFamily & 1 ? 3 : 2) * HSUnit) / 2;

	ReleaseDC(_Base, Dc);

	NeedCWidth = static_cast<int>((LargestLen * HSUnit) + LargestLen);

	RECT Bounds;
	GetClientRect(_Base, &Bounds);
	ClientHeight = Bounds.bottom;
	ClientWidth = Bounds.right;

	if (NeedCWidth <= ClientWidth)
		ShowScrollBar(_Base, SB_HORZ, false);

	if (!ShowScroll)
		ShowScrollBar(_Base, SB_VERT, false);
	else
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = LinesCount - 1;
		si.nPage = ClientHeight / VSUnit;
		SetScrollInfo(_Base, SB_VERT, &si, TRUE);
	}

	if (!ShowScroll)
		ShowScrollBar(_Base, SB_HORZ, false);
	else
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + NeedCWidth / HSUnit;
		si.nPage = ClientWidth / HSUnit;
		SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
	}	
}

LRESULT Label::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	AaColor Foreground = _Style.Foreground;
	if (!IsEnabled)
		Foreground.Reduce(0.7f);

	HBRUSH Base = CreateSolidBrush(_Bk);
	HFONT Font = CreateFontW(-MulDiv(_Style.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_Style.Bold ? FW_BOLD : 0), _Style.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_Style.FontFamily));
	SetTextColor(Dc, Foreground);

	SelectObject(Dc, Base);
	SelectObject(Dc, Font);
	SetBkColor(Dc, _Bk);

	SetTextCharacterExtra(Dc, GetTextCharacterExtra(Dc) + _Style.CharSpacing);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	FillRect(Dc, &WndRect, Base);

	if (!ShowScroll)
	{
		const wchar_t* Text = this->Text.ConstWchar();
		int TextLen = static_cast<int>(this->Text.Length());
		if (TextPos > TextLen)
			TextPos = TextLen;

		DrawTextW(Dc, &Text[TextPos], TextLen - TextPos, &WndRect, (_Style.Alignment == TA_CenterAlignment ? DT_CENTER : _Style.Alignment == TA_LeftAlignment ? DT_LEFT : DT_RIGHT) | (_Style.Alignment == TA_CenterAlignment ? DT_VCENTER : 0) | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
	}
	else
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(_Base, SB_VERT, &si);
		YPos = si.nPos; //Gets the current YPosition of the scrolling.

		GetScrollInfo(_Base, SB_HORZ, &si);
		XPos = si.nPos; //Gets the currernt XPosition of the scrolling.

		StringList& Lines = _Lines;
		int LineCount = static_cast<int>(Lines.Size); //Gets the lines and line count.

		int FirstLine = max(0, YPos + ps.rcPaint.top / VSUnit); //Deterimines the index of the first line to draw.
		int LastLine = min(LineCount - 1, YPos + ps.rcPaint.bottom / VSUnit); //Determines the index of the last line to draw.

		int X, Y; //The coords of the text to be drawn.
		for (int i = FirstLine; i <= LastLine; i++)
		{
			X = HSUnit * (1 - XPos); //Gets the XPosition
			Y = VSUnit * (i - YPos); //Gets the YPosition

			String& ThisLine = Lines[i]; //Gets this line
			int TabIndex = ThisLine.TabIndex();
			if (TabIndex > 0)
				X += (TabIndex * HSUnit * 5); //Adds some margin

			RECT Pos = { X, Y, 0, 0 };
			DrawTextW(Dc, (const wchar_t*)ThisLine, ThisLine.Length(), &Pos, DT_NOCLIP | DT_LEFT | DT_NOPREFIX); //Draws the text
		}
	}	

	DeleteObject(Base);
	DeleteObject(Font);

	EndPaint(_Base, &ps);
	return 0;
}
LRESULT Label::VScroll(WPARAM wp)
{
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(_Base, SB_VERT, &si);

	YPos = si.nPos;
	int Action = LOWORD(wp);
	switch (LOWORD(wp))
	{
	case SB_TOP:
		si.nPos = si.nMin;
		break;
	case SB_BOTTOM:
		si.nPos = si.nMax;
		break;
	case SB_LINEUP:
		si.nPos--;
		break;
	case SB_LINEDOWN:
		si.nPos++;
		break;
	case SB_PAGEUP:
		si.nPos -= si.nPage;
		break;
	case SB_PAGEDOWN:
		si.nPos += si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(_Base, SB_VERT, &si, TRUE);
	GetScrollInfo(_Base, SB_VERT, &si);

	if (si.nPos != YPos)
	{
		ScrollWindow(_Base, 0, VSUnit * (YPos - si.nPos), NULL, NULL);
		UpdateWindow(_Base);
	}
	return 0;
}
LRESULT Label::HScroll(WPARAM wp)
{
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	GetScrollInfo(_Base, SB_HORZ, &si);
	XPos = si.nPos;

	switch (LOWORD(wp))
	{
	case SB_LINELEFT:
		si.nPos--;
		break;
	case SB_LINERIGHT:
		si.nPos++;
		break;
	case SB_PAGELEFT:
		si.nPos -= si.nPage;
		break;
	case SB_THUMBTRACK:
		si.nPos = si.nTrackPos;
		break;
	default:
		break;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
	GetScrollInfo(_Base, SB_HORZ, &si);

	if (si.nPos != XPos)
		ScrollWindow(_Base, HSUnit * (XPos - si.nPos), 0, NULL, NULL);

	return 0;
}
LRESULT Label::MouseScroll(WPARAM wp)
{
	if (!ShowScroll && (GetKeyState(VK_CONTROL) & 0x8000))
	{
		int Change = GET_WHEEL_DELTA_WPARAM(wp);
		if (Change < 0)
			TextPos++;
		else
			TextPos--;

		if (TextPos < 0)
			TextPos = 0;

		Redraw();
		return 0;
	}
	else if (!ShowScroll)
		return 0;

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	int Change = GET_WHEEL_DELTA_WPARAM(wp); //Gets the amount that the user scrolled. If the change is positive, they are scrolling up, and if the change is negitive, then they are scrolling down.

	if (GET_KEYSTATE_WPARAM(wp) & MK_SHIFT) //When the shift key is pressed down, the scrolling is directed in the horizontal direction.
	{
		GetScrollInfo(_Base, SB_HORZ, &si);
		XPos = si.nPos;

		if (Change < 0)
			si.nPos -= 2;
		else if (Change > 0)
			si.nPos += 2;

		si.fMask = SIF_POS;
		SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
		GetScrollInfo(_Base, SB_HORZ, &si);

		if (si.nPos != XPos)
			ScrollWindow(_Base, HSUnit * (XPos - si.nPos), 0, NULL, NULL); //Sets the scroll pos
	}
	else //Else, its vertical.
	{
		GetScrollInfo(_Base, SB_VERT, &si);
		YPos = si.nPos;

		if (Change < 0)
			si.nPos += 2;
		else if (Change > 0)
			si.nPos -= 2;

		si.fMask = SIF_POS;
		SetScrollInfo(_Base, SB_VERT, &si, TRUE);
		GetScrollInfo(_Base, SB_VERT, &si);

		if (si.nPos != YPos)
		{
			ScrollWindow(_Base, 0, VSUnit * (YPos - si.nPos), NULL, NULL);
			UpdateWindow(_Base);
		}
	}
	return 0;
}
LRESULT Label::HMouseScroll(WPARAM wp)
{
	if (!ShowScroll)
		return 0;

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	int Change = GET_WHEEL_DELTA_WPARAM(wp); //Gets the amount that the user scrolled. If the change is positive, they are scrolling up, and if the change is negitive, then they are scrolling down.

	GetScrollInfo(_Base, SB_HORZ, &si);
	XPos = si.nPos;

	if (Change < 0)
		si.nPos -= 2;
	else if (Change > 0)
		si.nPos += 2;

	si.fMask = SIF_POS;
	SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
	GetScrollInfo(_Base, SB_HORZ, &si);

	if (si.nPos != XPos)
		ScrollWindow(_Base, HSUnit * (XPos - si.nPos), 0, NULL, NULL); //Sets the scroll pos
	return 0;
}
LRESULT Label::Size()
{
	Reset();
	return 0;
}
LRESULT Label::KeyDown(WPARAM wp)
{
	if (wp == VK_TAB || wp == VK_RETURN)
	{
		SetFocus(GetWindow(_Base, GW_HWNDNEXT));
		return 0;
	}
	else
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
}
LRESULT Label::GotFocus()
{
	SetFocus(GetWindow(_Base, GW_HWNDNEXT));
	return 0;
}