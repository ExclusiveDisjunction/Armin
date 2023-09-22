#include "TextBox.h"

#include "..\Arithmetic.h"
#include "..\Container.h"

#include <Windowsx.h>

#define PointEquals(X, Y) (X.x == Y.x && X.y == Y.y)

TextBox::TextBox(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, String Text, StyleSheet Style, TextStyleSheet TextStyle, bool HasScrolls, bool IsPassword, int MaxCompacity)
{
	_Style = Style;
	_TextStyle = TextStyle;
	_HasScrolls = HasScrolls;
	_IsPassword = IsPassword;
	_MaxComp = MaxCompacity;
	_Multiline = HasScrolls;

	SetText(Text);

	if (!_ThisAtom)
		InitBase(ins);

	if (!_Multiline)
		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, X, Y, Width, Height, Parent, NULL, ins, NULL);
	else
		_Base = CreateWindowExW(0l, L"EDIT", static_cast<LPCWSTR>(Text), WS_CHILD | WS_VISIBLE | ES_WANTRETURN | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | (IsPassword ? ES_PASSWORD : 0), X, Y, Width, Height, Parent, NULL, ins, NULL);
	int Result = GetLastError();

	{
		HDC Dc = GetDC(_Base);

		Font = CreateFontW(-MulDiv(_TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (_TextStyle.Bold ? FW_BOLD : 0), _TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(_TextStyle.FontFamily));

		SelectObject(Dc, Font);
		TEXTMETRIC tm;
		GetTextMetricsW(Dc, &tm);
		FontHeight = tm.tmHeight;

		ReleaseDC(_Base, Dc);
	}

	if (!_Multiline)
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	if (!_Multiline)
		ResetScrolling();

}
TextBox::~TextBox()
{
	SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);

	free(Text);
	DeleteObject(Font);
	DestroyWindow(_Base);
}

LRESULT __stdcall TextBox::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	TextBox* This = reinterpret_cast<TextBox*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!This)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_GETDLGCODE:
		return This->GetDlgCode();
	case WM_VSCROLL:
		return This->VScroll(wp, lp);
	case WM_HSCROLL:
		return This->HScroll(wp, lp);
	case WM_MOUSEWHEEL:
		return This->MouseScroll(wp, lp);
	case WM_MOUSEHWHEEL:
		return This->HMouseScroll(wp, lp);
	case WM_SIZE:
		if (This->_HasScrolls)
			This->ResetScrolling();
		return 0;
	}
	if (Message == WM_GETDLGCODE)
		return This->GetDlgCode();

	return CommonWndProc(Window, Message, wp, lp);
}
ATOM TextBox::_ThisAtom = ATOM();
void TextBox::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = TextBox::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_IBEAM);
	wn.hbrBackground = NULL;
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"TextBoxClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(TextBox*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}

void TextBox::AdjustCaretPos()
{
	HDC Dc = GetDC(_Base);
	SelectObject(Dc, Font);

	int Size = lstrlenW(this->Text) + 1;

	wchar_t* Text = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * Size));
	if (_IsPassword)
	{
		for (int i = 0; i < Size - 1; i++)
			Text[i] = L'*';
		Text[Size - 1] = 0;
	}
	else
		memcpy(Text, this->Text, sizeof(wchar_t) * Size);

	int CaretPosX = CaretPos.x;
	int CaretPosY = CaretPos.y * FontHeight;

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	SIZE Extent;

	if (_HasScrolls)
	{
		uint PrePos = CaretPos.x;

		if (CaretPos.y != 0)
		{
			StringList Lines = String(Text).Split(L'\n');
			for (uint i = 0; i < Lines.Size && i < static_cast<uint>(CaretPos.y - 1); i++)
				PrePos += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;
		}

		GetTextExtentPoint32W(Dc, &Text[PrePos - CaretPosX], CaretPosX, &Extent);
	}
	else
		GetTextExtentPoint32W(Dc, Text, CaretPosX, &Extent);

	if (_HasScrolls && _LoadedScrolls)
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_POS;
		GetScrollInfo(_Base, SB_VERT, &si);

		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = (CaretPos.y > (WndRect.bottom / VSUnit) ? si.nPos : CaretPos.y);
		SetScrollInfo(_Base, SB_VERT, &si, true);

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_POS;
		GetScrollInfo(_Base, SB_HORZ, &si);

		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		si.nPos = (CaretPos.y > (WndRect.right / HSUnit) ? si.nPos : CaretPos.x);
		SetScrollInfo(_Base, SB_HORZ, &si, true);

	}
	
	if (_HasScrolls)
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(_Base, SB_VERT, &si);
		YPos = si.nPos;

		SetCaretPos((_Style.BorderThickness + 5) + Extent.cx, _Style.BorderThickness * 2 + VSUnit * (CaretPos.y - YPos) + (_Style.BorderThickness + 2));
	}
	else
		SetCaretPos((_Style.BorderThickness + 5) + Extent.cx, _Style.BorderThickness * 2);
	
	free(Text);
	ReleaseDC(_Base, Dc);

	Redraw();
}
void TextBox::ResetScrolling()
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

	StringList Lines = String(Text).Split(L'\n');
	uint LargestLen = LongestLine(Lines);
	uint LinesCount = Lines.Size;

	HDC Dc = GetDC(_Base);
	SelectObject(Dc, Font);

	TEXTMETRIC tm;
	GetTextMetricsW(Dc, &tm);
	HSUnit = tm.tmAveCharWidth + 1;
	VSUnit = tm.tmHeight + 8;
	HUnitMax = ((tm.tmPitchAndFamily & 1 ? 3 : 2) * HSUnit) / 2;

	ReleaseDC(_Base, Dc);

	RECT Bounds;
	GetClientRect(_Base, &Bounds);

	NeedCWidth = static_cast<int>((LargestLen * HSUnit) + LargestLen);
	NeedCHeight = max(Bounds.bottom, static_cast<int>(Lines.Size * tm.tmHeight));

	int ClientHeight = Bounds.bottom;
	int ClientWidth = Bounds.right;

	if (NeedCWidth <= ClientWidth)
		ShowScrollBar(_Base, SB_HORZ, false);

	if (!_HasScrolls)
		ShowScrollBar(_Base, SB_VERT, false);
	else
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nMax = LinesCount - 1;
		si.nPage = ClientHeight / VSUnit;
		si.nPos = CaretPos.y;
		SetScrollInfo(_Base, SB_VERT, &si, TRUE);
	}

	if (!_HasScrolls)
		ShowScrollBar(_Base, SB_HORZ, false);
	else
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
		si.nMin = 0;
		si.nMax = 2 + NeedCWidth / HSUnit;
		si.nPage = ClientWidth / HSUnit;
		si.nPos = CaretPos.x;
		SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
	}

	_LoadedScrolls = true;
}

LRESULT TextBox::Command(WPARAM wp, LPARAM lp)
{
	return 0;
}
LRESULT TextBox::KeyDown(WPARAM wp)
{
	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		switch (wp)
		{
		case L'A':
		{
			SelectionPos.x = 0;
			SelectionPos.y = 0;

			StringList Lines = String(Text).Split(L'\n');
			CaretPos.x = Lines[Lines.Size - 1].Length();
			CaretPos.y = Lines.Size - 1;

			AdjustCaretPos();
			return 0;
		}
		case L'C':
		case L'X':
		case L'V':
		{
			if (this->_IsPassword)
				return 0;

			if (PointEquals(SelectionPos, CaretPos) && (wp == L'C' || wp == L'X'))
				return 0;

			wchar_t* RawSelected;

			int Pos = CaretPos.x;
			StringList Lines = String(Text).Split(L'\n');
			for (uint i = 0; i < Lines.Size && i < static_cast<uint>(CaretPos.y); i++)
				Pos += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;

			int Selection = SelectionPos.x;
			for (uint i = 0; i < Lines.Size && i < static_cast<uint>(SelectionPos.y); i++)
				Selection += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;

			int One = min(Pos, Selection);
			int Two = max(Pos, Selection);
			int Size = Two - One + 1;

			HGLOBAL ClipboardHandle;

			if (wp == L'C' || wp == L'X')
			{
				if (!OpenClipboard(_Base))
					return 0;
				EmptyClipboard();

				ClipboardHandle = GlobalAlloc(GMEM_MOVEABLE, Size * sizeof(TCHAR));

				RawSelected = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * Size));
				memcpy(RawSelected, &Text[One], sizeof(wchar_t) * Size);				

				if (ClipboardHandle == nullptr)
					return 0;

				LPTSTR ToPasteTo = static_cast<LPTSTR>(GlobalLock(ClipboardHandle));
				memcpy(ToPasteTo, &Text[One], sizeof(wchar_t) * (Size - 1));
				ToPasteTo[Size - 1] = L'\0';

				GlobalUnlock(ClipboardHandle);
				SetClipboardData(CF_UNICODETEXT, ClipboardHandle);

				CloseClipboard();

				if (wp == L'X')
					this->RemoveItem(true);
				return 0;
			}
			else
			{
				if (!IsClipboardFormatAvailable(CF_UNICODETEXT))
					return 0;
				if (!OpenClipboard(_Base))
					return 0;

				ClipboardHandle = GetClipboardData(CF_UNICODETEXT);
				LPTSTR RetrivedText;

				if (ClipboardHandle)
				{
					RetrivedText = static_cast<LPTSTR>(GlobalLock(ClipboardHandle));
					{
						String TempText = RetrivedText;
						String NewTempText;
						for (uint i = 0; i < TempText.Length(); i++)
						{
							if (i != (TempText.Length() - 1) && TempText[i] == L'\n' && TempText[i + 1] == L'\r')
							{
								NewTempText += L'\n';
								i++;
							}
							else
								NewTempText += TempText[i];

						}
						const wchar_t* Transition = static_cast<const wchar_t*>(NewTempText);
						memset(RetrivedText, 0, sizeof(wchar_t) * TempText.Length());
						memcpy(RetrivedText, Transition, sizeof(wchar_t) * NewTempText.Length());
					}

					if (RetrivedText)
					{
						String Base(Text);

						String First = Base.SubString(0, One);
						String Last = Base.SubString(Two == One ? Two + 1 : Two);
						String Middle = String(RetrivedText);

						String Final = First + Middle + Last;

						free(Text);
						ulong ThisSize = sizeof(wchar_t) * (Final.Length() + 4), OtherSize = Final.Length() * sizeof(wchar_t);
						Text = static_cast<wchar_t*>(malloc(ThisSize));

						memset(Text, 0, ThisSize);
						memcpy(Text, static_cast<const wchar_t*>(Final), OtherSize);

						_Size = Final.Length();
						_Compacity = Final.Length() + 4;						
					}
					GlobalUnlock(ClipboardHandle);
				}

				Redraw();

				CloseClipboard();
				return 0;
			}

			return 0;
		}
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}
	}

	switch (wp)
	{
	case VK_LEFT:
	{
		if (CaretPos.x == 0)
		{
			if (CaretPos.y == 0)
				break;
			else
			{
				CaretPos.x = 0;
				CaretPos.y--;
			}
		}
		else
			CaretPos.x--;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			SelectionPos = CaretPos;

		AdjustCaretPos();
		break;
	}
	case VK_RIGHT:
	{
		if (_HasScrolls)
		{
			if (CaretPos.x == lstrlenW(Text))
				break;
			else if (Text[CaretPos.x + 1] == L'\n')
			{
				CaretPos.x = 0;
				CaretPos.y++;
			}
			else
				CaretPos.x++;
		}
		else
		{
			if (CaretPos.x == lstrlenW(Text))
				break;

			CaretPos.x++;
		}

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			SelectionPos = CaretPos;

		AdjustCaretPos();
		break;
	}
	case VK_UP:
	{
		if (!_HasScrolls)
			break;

		auto LastLine = [](wchar_t* Text, int CaretYPos) -> int
		{
			StringList Lines = String(Text).Split(L'\n');
			return Lines[CaretYPos - 1].Length() - 1;
		};

		if (CaretPos.y == 0)
			break;

		CaretPos.x = min(CaretPos.x, LastLine(Text, CaretPos.y));
		CaretPos.y--;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			SelectionPos = CaretPos;

		AdjustCaretPos();
		break;
	}
	case VK_DOWN:
	{
		if (!_HasScrolls)
			break;

		StringList Lines = String(Text).Split(L'\n');

		auto NextLine = [](StringList& Lines, int CaretYPos) -> int
		{			
			return Lines[CaretYPos + 1].Length() - 1;
		};

		if (CaretPos.y + 1 == Lines.Size)
			break;

		CaretPos.x = min(CaretPos.x, NextLine(Lines, CaretPos.y));
		CaretPos.y++;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			SelectionPos = CaretPos;

		AdjustCaretPos();
		break;
	}
	case VK_DELETE:
		RemoveItem(false);
		break;
	case VK_HOME:
		CaretPos.x = 0;
		if (GetKeyState(VK_CONTROL) & 0x8000)
			CaretPos.y = 0;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			SelectionPos = CaretPos;

		AdjustCaretPos();
		break;
	case VK_END:
	{
		if (_Size == 0)
			break;

		StringList Lines = String(Text).Split(L'\n');
		CaretPos.x = lstrlenW(static_cast<LPCWSTR>(Lines[CaretPos.y]));
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			auto Func = [](wchar_t* Text) -> int
			{
				int Return = 0;
				int Len = lstrlenW(Text);
				for (int i = 0; i <= Len; i++)
				{
					if (Text[i] == L'\n')
						Return++;
				}

				return Return;
			};

			CaretPos.y = Func(this->Text);
		}

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			SelectionPos = CaretPos;

		AdjustCaretPos();
		break;
	}
	case VK_ESCAPE:
		SetFocus(GetParent(_Base));
	}
	return 0;
}
LRESULT TextBox::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);
	SetBkMode(Dc, TRANSPARENT);

	AaColor BkColor = _Style.Background, BorderColor = _Style.BorderBrush, ForegroundColor = _TextStyle.Foreground;

	if (!IsEnabled)
	{
		BorderColor.Reduce(0.7f);
		ForegroundColor.Reduce(0.7f);
	}

	HBRUSH BkBrush = CreateSolidBrush(BkColor);
	HPEN Border = CreatePen(PS_SOLID, _Style.BorderThickness, BorderColor);

	SetTextColor(Dc, ForegroundColor);
	SelectObject(Dc, BkBrush);
	SelectObject(Dc, Font);
	if (_Style.BorderThickness == 0)
		SelectObject(Dc, GetStockObject(NULL_PEN));
	else
		SelectObject(Dc, Border);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);
	WndRect.bottom = NeedCHeight;

	if (_HasScrolls)
	{
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS | SIF_RANGE;

		GetScrollInfo(_Base, SB_HORZ, &si);
		XPos = si.nPos; //Gets the currernt XPosition of the scrolling.

		GetScrollInfo(_Base, SB_VERT, &si);
		YPos = si.nPos; //Gets the current YPosition of the scrolling.

		WndRect.top = min(0, -static_cast<int>((si.nPos * si.nPage)));
	}

	if (_Style.Radius > 0)
		RoundRect(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom, _Style.Radius, _Style.Radius);
	else
		Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

	int LimitBy = _Style.BorderThickness + 2;
	WndRect.left += LimitBy + 3;
	WndRect.top += LimitBy;
	WndRect.right -= LimitBy;
	WndRect.bottom -= LimitBy;

	auto Contains = [](wchar_t* Items, wchar_t Search) -> bool
	{
		int StrLen = lstrlenW(Items);
		for (uint i = 0; i <= static_cast<uint>(StrLen); i++)
		{
			if (Items[i] == Search)
				return true;
		}

		return false;
	};

	RECT SelArea = { (CaretPos.x == 0 ? WndRect.left : 0) + (CaretPos.x * HSUnit), WndRect.top + (CaretPos.y * VSUnit), (SelectionPos.x == 0 ? WndRect.left : 0) + (SelectionPos.x * HSUnit), WndRect.top + ((SelectionPos.y + 1) * VSUnit) };

	if (HasFocus)
	{
		AaColor Sel = _Style.BorderBrush;
		Sel.Reduce(0.6f);
		HBRUSH ThisBrush = CreateSolidBrush(Sel);
		HGDIOBJ Old = SelectObject(Dc, ThisBrush);

		FillRect(Dc, &SelArea, ThisBrush);

		SelectObject(Dc, Old);
		DeleteObject(ThisBrush);
	}

	int Size = lstrlenW(this->Text) + 1;
	wchar_t* Text = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * Size));

	if (!_HasScrolls)
	{		
		if (_IsPassword)
		{
			for (int i = 0; i < Size - 1; i++)
				Text[i] = L'*';
			Text[Size - 1] = L'\0';
		}
		else
			memcpy(Text, this->Text, sizeof(wchar_t) * Size);

		RECT CaretPos;
		CopyRect(&CaretPos, &WndRect);
		int Result = DrawTextW(Dc, Text, Size, &CaretPos, DT_VCENTER | DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
	}
	else
	{
		memcpy(Text, this->Text, sizeof(wchar_t) * Size);

		StringList Lines = String(Text).Split(L'\n');
		int LineCount = static_cast<int>(Lines.Size); //Gets the lines and line count.

		int FirstLine = max(0, YPos + ps.rcPaint.top / VSUnit); //Deterimines the index of the first line to draw.
		int LastLine = min(LineCount - 1, YPos + ps.rcPaint.bottom / VSUnit); //Determines the index of the last line to draw.

		int X, Y; //The coords of the text to be drawn.
		for (int i = FirstLine; i <= LastLine; i++)
		{
			X = HSUnit * (1 - XPos); //Gets the XPosition
			Y = VSUnit * (i - YPos) + LimitBy; //Gets the YPosition

			String& ThisLine = Lines[i]; //Gets this line
			int TabIndex = ThisLine.TabIndex();
			if (TabIndex > 0)
				X += (TabIndex * HSUnit * 5); //Adds some margin

			RECT Pos = { X, Y, 0, 0 };
			DrawTextW(Dc, (const wchar_t*)ThisLine, ThisLine.Length(), &Pos, DT_NOCLIP | DT_LEFT | DT_NOPREFIX); //Draws the text
		}

	}

	

	DeleteObject(BkBrush);
	DeleteObject(Border);

	EndPaint(_Base, &ps);

	free(Text);

	return 0;
}
LRESULT TextBox::Click()
{
	::SetFocus(_Base);
	return 0;
}
LRESULT TextBox::LostFocus()
{
	HasFocus = false;
	DestroyCaret();
	Redraw();
	return 0;
}
LRESULT TextBox::GotFocus()
{
	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	SelectionPos = CaretPos = { 0, 0 };
	CreateCaret(_Base, NULL, 2, FontHeight);
	SetCaretPos(_Style.BorderThickness + 5, _Style.BorderThickness * 2);
	ShowCaret(_Base);

	HasFocus = true;

	return 0;
}
LRESULT TextBox::Char(WPARAM wp)
{
	if (GetKeyState(VK_CONTROL) & 0x8000)
		return 0;

	if (wp == L'\b')
		RemoveItem(true);
	else if (wp == L'\n' || wp == L'\r')
	{
		if (!_HasScrolls)
		{
			SendMessageW(GetParent(_Base), WM_KEYDOWN, VK_RETURN, 0);
			SetFocus(GetParent(_Base));
		}
		else
		{
			AppendKey(L'\n');
			AdjustCaretPos();
			ResetScrolling();
		}
	}
	else if (wp == L'\t')
	{
		HWND Other;
		if (GetKeyState(VK_SHIFT) & 0x8000)
			Other = GetWindow(_Base, GW_HWNDLAST);
		else
			Other = GetWindow(_Base, GW_HWNDNEXT);

		SetFocus(Other);
	}
	else
		AppendKey(static_cast<wchar_t>(wp));
	return 0;
}
LRESULT TextBox::EraseBackground()
{
	return 1;
}
LRESULT TextBox::VScroll(WPARAM wp, LPARAM lp)
{
	if (!_HasScrolls)
		return 0;

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
LRESULT TextBox::HScroll(WPARAM wp, LPARAM lp)
{
	if (!_HasScrolls)
		return 0;

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
LRESULT TextBox::MouseScroll(WPARAM wp, LPARAM lp)
{
	if (!_HasScrolls)
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
LRESULT TextBox::HMouseScroll(WPARAM wp, LPARAM lp)
{
	if (!_HasScrolls)
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

String TextBox::GetText()
{
	if (_Multiline)
	{
		int Len = GetWindowTextLengthW(_Base);
		if (Len == 0)
			return String();
		else
		{
			wchar_t* Arr = new wchar_t[Len];
			memset(Arr, 0, Len * sizeof(wchar_t));
			GetWindowTextW(_Base, Arr, Len);

			String Return(Arr);
			delete[] Arr;

			return Return;
		}
	}
	else
		return String(Text);
}
void TextBox::SetText(String New)
{
	if (_Multiline)
		SetWindowTextW(_Base, static_cast<const wchar_t*>(New));
	else
	{
		const wchar_t* RawNew = static_cast<const wchar_t*>(New);
		delete[] Text;

		_Compacity = New.Length() + 4;
		_Size = New.Length() + 1;

		Text = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * _Compacity));
		memset(Text, 0, sizeof(wchar_t) * _Compacity);
		memcpy(Text, RawNew, sizeof(wchar_t) * _Size);

		CaretPos = { 0, 0 };
		AdjustCaretPos();
		Redraw();
	}
}
void TextBox::AppendKey(wchar_t New)
{
	if (lstrlenW(Text) >= _MaxComp && _MaxComp >= 0)
		return;

	if ((_Size + 1) > _Compacity)
	{
		_Compacity += 10;
		wchar_t* New = static_cast<wchar_t*>(realloc(Text, sizeof(wchar_t) * _Compacity));
		Text = New;
		memset(&Text[_Size], 0, sizeof(wchar_t) * (_Compacity - _Size));
	}

	int Pos = CaretPos.x;
	StringList Lines = String(Text).Split(L'\n');
	for (uint i = 0; i < Lines.Size && i < static_cast<uint>(CaretPos.y); i++)
		Pos += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;

	if (CaretPos.x == SelectionPos.x && CaretPos.y == SelectionPos.y)
	{
		uint TempSize = _Compacity - Pos;
		wchar_t* TempText = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * TempSize));
		memset(TempText, 0, sizeof(wchar_t) * TempSize);
		memcpy(TempText, &Text[Pos], sizeof(wchar_t) * TempSize);
		memcpy(&Text[Pos + 1], TempText, sizeof(wchar_t) * (TempSize - 1));
		Text[Pos] = New;

		free(TempText);
	}
	else
	{
		int Selection = SelectionPos.x;
		for (uint i = 0; i < Lines.Size && i < static_cast<uint>(SelectionPos.y); i++)
			Selection += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;

		int One = min(Pos, Selection);
		int Two = max(Pos, Selection);

		uint TempSize = _Compacity - Two;
		wchar_t* TempText = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * TempSize));
		memset(TempText, 0, sizeof(wchar_t) * TempSize);
		memcpy(TempText, &Text[Two], sizeof(wchar_t) * TempSize);

		memcpy(&Text[One + 1], TempText, sizeof(wchar_t) * (_Compacity - One + 1));
		Text[One] = New;

		free(TempText);
	}

	if (New != L'\n')
		CaretPos.x++;
	else
	{
		CaretPos.y++;
		CaretPos.x = 0;
	}
	SelectionPos = CaretPos;
	_Size++;

	AdjustCaretPos();
	Redraw();
}
void TextBox::RemoveItem(bool IsBackspace)
{
	if (_Size == 0)
		return;

	StringList Lines = String(Text).Split(L'\n');

	int Pos = CaretPos.x;
	for (uint i = 0; i < Lines.Size && i < static_cast<uint>(CaretPos.y); i++)
		Pos += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;

	int SelPos = this->SelectionPos.x;	
	for (uint i = 0; i < Lines.Size && i < static_cast<uint>(SelectionPos.y); i++)
		Pos += lstrlenW(static_cast<LPCWSTR>(Lines[i])) + 1;

	if (IsBackspace)
	{
		if (Pos == SelPos && Pos == 0)
			return;
		else if (Pos != SelPos)
		{
			int One = min(Pos, SelPos);
			int Two = max(Pos, SelPos);

			if (One == 0 && Two == (Lines.Size == 1 ? lstrlenW(Text) : lstrlenW(Text) + 1))
			{
				memset(Text, 0, sizeof(wchar_t) * _Compacity);
				CaretPos.x = SelectionPos.x = 0;
				CaretPos.y = SelectionPos.y = 0;

				AdjustCaretPos();
				Redraw();

				return;
			}

			if (Two == (Lines.Size == 1 ? lstrlenW(Text) : lstrlenW(Text) + 1))
			{
				memset(&Text[One], 0, sizeof(wchar_t) * (_Compacity - One));

				POINT OldCaret = CaretPos, OldSelection = SelectionPos;

				int X = min(CaretPos.x, SelectionPos.x);
				int Y = min(CaretPos.y, SelectionPos.y);

				CaretPos.x = SelectionPos.x = X;
				CaretPos.y = SelectionPos.y = Y;

				AdjustCaretPos();
				Redraw();

				return;
			}

			uint FirstSize = One + 1;
			wchar_t* FirstPart = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * FirstSize));
			memcpy(FirstPart, Text, sizeof(wchar_t) * FirstSize);

			uint SecondSize = _Size - Two + 1;
			wchar_t* SecondPart = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * SecondSize));
			memcpy(SecondPart, &Text[Two], sizeof(wchar_t) * SecondSize);

			memset(Text, 0, sizeof(wchar_t) * _Compacity);
			memcpy(Text, FirstPart, sizeof(wchar_t) * FirstSize);
			memcpy(&Text[One], SecondPart, sizeof(wchar_t) * SecondSize);

			_Size -= (Two - One);

			int X = min(CaretPos.x, SelectionPos.x);
			int Y = min(CaretPos.y, SelectionPos.y);

			CaretPos.x = SelectionPos.x = X;
			CaretPos.y = SelectionPos.y = Y;
			AdjustCaretPos();
		}
		else
		{
			memcpy(&Text[Pos - 1], &Text[Pos], sizeof(wchar_t) * (_Size - Pos + 1));
			memset(&Text[_Size - 1], 0, sizeof(wchar_t) * (_Compacity - _Size));
		}

		if (CaretPos.x == 0 && CaretPos.y != 0)
			KeyDown(VK_UP);
	}
	else
	{
		if (Pos == _Size && Pos == SelPos)
			return;
		else if (Pos != SelPos)
		{
			int One = min(Pos, SelPos);
			int Two = max(Pos, SelPos);

			if (One == 0 && Two == (Lines.Size == 1 ? lstrlenW(Text) : lstrlenW(Text) + 1))
			{
				memset(Text, 0, sizeof(wchar_t) * _Compacity);
				CaretPos.x = SelectionPos.x = 0;
				CaretPos.y = SelectionPos.y = 0;

				AdjustCaretPos();
				Redraw();

				return;
			}

			if (Two == (Lines.Size == 1 ? lstrlenW(Text) : lstrlenW(Text) + 1))
			{
				memset(&Text[One], 0, sizeof(wchar_t) * (_Compacity - One));
				
				POINT OldCaret = CaretPos, OldSelection = SelectionPos;

				int X = min(CaretPos.x, SelectionPos.x);
				int Y = min(CaretPos.y, SelectionPos.y);

				CaretPos.x = SelectionPos.x = X;
				CaretPos.y = SelectionPos.y = Y;

				AdjustCaretPos();
				Redraw();

				return;
			}

			uint FirstSize = One + 1;
			wchar_t* FirstPart = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * FirstSize));
			memcpy(FirstPart, Text, sizeof(wchar_t) * FirstSize);

			uint SecondSize = _Size - Two + 1;
			wchar_t* SecondPart = static_cast<wchar_t*>(malloc(sizeof(wchar_t) * SecondSize));
			memcpy(SecondPart, &Text[Two], sizeof(wchar_t) * SecondSize);

			memset(Text, 0, sizeof(wchar_t) * _Compacity);
			memcpy(Text, FirstPart, sizeof(wchar_t) * FirstSize);
			memcpy(&Text[One], SecondPart, sizeof(wchar_t) * SecondSize);

			_Size -= (Two - One);
		}
		else
		{
			memcpy(&Text[Pos], &Text[Pos + 1], sizeof(wchar_t) * (_Size - Pos));
			memset(&Text[_Size - 1], 0, sizeof(wchar_t) * (_Compacity - _Size));

			SelectionPos = CaretPos;
			Redraw();
			return;
		}
	}

	_Size--;
	if (CaretPos.x != 0)
	{
		CaretPos.x--;
		AdjustCaretPos();
	}
	SelectionPos = CaretPos;

	Redraw();
}