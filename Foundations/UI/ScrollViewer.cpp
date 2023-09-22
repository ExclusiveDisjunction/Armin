#include "ScrollViewer.h"

ScrollViewer::ScrollViewer(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, StyleSheet Style)
{
	if (!_ThisAtom)
		InitBase(ins);

	this->_Style = Style;

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL, X, Y, Width, Height, Parent, NULL, ins, NULL);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	Reset();
}

void ScrollViewer::Reset()
{
	HScrollUnit = 15;
	VScrollUnit = 20;

	HWND Children = (!Viewer ? nullptr : *Viewer);
	RECT WndRect, ChildRect;
	GetClientRect(_Base, &WndRect);
	GetClientRect(Children, &ChildRect);

	ScrollWindow(_Base, 0, 0, NULL, NULL);

	ClientWidth = WndRect.right;
	ClientHeight = WndRect.bottom;
	ScrollWidth = ChildRect.right;

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = (ChildRect.bottom / VScrollUnit) + 1;//ChildRect.bottom;
	si.nPage = VScrollUnit;
	SetScrollInfo(_Base, SB_VERT, &si, TRUE);

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	si.nMin = 0;
	si.nMax = ChildRect.right / HScrollUnit; //2 + (ScrollWidth / HScrollUnit);
	si.nPage = ClientWidth / HScrollUnit;
	SetScrollInfo(_Base, SB_HORZ, &si, TRUE);

	//ShowScrollBar(_Base, SB_HORZ, ChildRect.right <= WndRect.right);
	//ShowScrollBar(_Base, SB_VERT, ChildRect.bottom <= WndRect.bottom);
}

LRESULT ScrollViewer::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	AaColor Background = _Style.Background;
	if (!IsEnabled)
		Background.Reduce(0.7f);

	HBRUSH Bk = CreateSolidBrush(Background);
	FillRect(Dc, &WndRect, Bk);

	DeleteObject(Bk);
	EndPaint(_Base, &ps);
	return 0;
}
LRESULT ScrollViewer::HScroll(WPARAM wp)
{
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	GetScrollInfo(_Base, SB_HORZ, &si);
	XCoord = si.nPos;

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

	if (si.nPos != XCoord)
		ScrollWindow(_Base, HScrollUnit * (XCoord - si.nPos), 0, NULL, NULL);
	return 0;
}
LRESULT ScrollViewer::VScroll(WPARAM wp)
{
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	GetScrollInfo(_Base, SB_VERT, &si);

	YCoord = si.nPos;
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

	if (si.nPos != YCoord)
	{
		ScrollWindow(_Base, 0, VScrollUnit * (YCoord - si.nPos), NULL, NULL);
		UpdateWindow(_Base);
	}
	return 0;
}
LRESULT ScrollViewer::MouseWheel(WPARAM wp)
{
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	int Change = GET_WHEEL_DELTA_WPARAM(wp); //Gets the amount that the user scrolled. If the change is positive, they are scrolling up, and if the change is negitive, then they are scrolling down.

	if (GET_KEYSTATE_WPARAM(wp) & MK_SHIFT) //When the shift key is pressed down, the scrolling is directed in the horizontal direction.
	{
		GetScrollInfo(_Base, SB_HORZ, &si);
		XCoord = si.nPos;

		if (Change < 0)
			si.nPos -= 2;
		else if (Change > 0)
			si.nPos += 2;

		si.fMask = SIF_POS;
		SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
		GetScrollInfo(_Base, SB_HORZ, &si);

		if (si.nPos != XCoord)
			ScrollWindow(_Base, HScrollUnit * (XCoord - si.nPos), 0, NULL, NULL); //Sets the scroll pos
	}
	else //Else, its vertical.
	{
		GetScrollInfo(_Base, SB_VERT, &si);
		YCoord = si.nPos;

		if (Change < 0)
			si.nPos += 2;
		else if (Change > 0)
			si.nPos -= 2;

		si.fMask = SIF_POS;
		SetScrollInfo(_Base, SB_VERT, &si, TRUE);
		GetScrollInfo(_Base, SB_VERT, &si);

		if (si.nPos != YCoord)
		{
			ScrollWindow(_Base, 0, VScrollUnit * (YCoord - si.nPos), NULL, NULL);
			UpdateWindow(_Base);
		}
	}
	return 0;
}
LRESULT ScrollViewer::HMouseWheel(WPARAM wp)
{
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	int Change = GET_WHEEL_DELTA_WPARAM(wp); //Gets the amount that the user scrolled. If the change is positive, they are scrolling up, and if the change is negitive, then they are scrolling down.

	GetScrollInfo(_Base, SB_HORZ, &si);
	XCoord = si.nPos;

	if (Change < 0)
		si.nPos -= 2;
	else if (Change > 0)
		si.nPos += 2;

	si.fMask = SIF_POS;
	SetScrollInfo(_Base, SB_HORZ, &si, TRUE);
	GetScrollInfo(_Base, SB_HORZ, &si);

	if (si.nPos != XCoord)
		ScrollWindow(_Base, HScrollUnit * (XCoord - si.nPos), 0, NULL, NULL); //Sets the scroll pos
	return 0;
}
LRESULT ScrollViewer::Size()
{
	Reset();
	return 0;
}

ATOM ScrollViewer::_ThisAtom = ATOM();
void ScrollViewer::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = ScrollViewer::WndProc;
	wn.hInstance = ins;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hbrBackground = NULL;
	wn.lpszMenuName = NULL;
	wn.lpszClassName = L"ScrollViewerClass";
	wn.hIcon = NULL;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(ScrollViewer*);
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}
LRESULT __stdcall ScrollViewer::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	ScrollViewer* This = reinterpret_cast<ScrollViewer*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!This)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_VSCROLL:
		return This->VScroll(wp);
	case WM_HSCROLL:
		return This->HScroll(wp);
	case WM_MOUSEWHEEL:
		return This->MouseWheel(wp);
	case WM_MOUSEHWHEEL:
		return This->HMouseWheel(wp);
	default:
		return ControlBase::CommonWndProc(Window, Message, wp, lp);
	}
}
