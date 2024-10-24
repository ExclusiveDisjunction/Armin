#include "TrackedCoord.h"

TrackedCoord::TrackedCoord(Vector<Vector2d*> Bind, int Size, HWND Parent, COLORREF Border, COLORREF Bk, HINSTANCE ins)
{
	if (!_ThisAtom)
		InitBase(ins);

	if (Bind.Size == 0)
		return;

	Modify = Bind;
	_Track = false;
	this->Border = Border;
	this->Bk = Bk;

	Vector2d* Tar = Modify[0];
	for (uint i = 1; i < Modify.Size; i++)
		*Modify[i] = *Tar;

	int XCoord = int(Tar->X - (Size / 2.0));
	int YCoord = int(Tar->Y - (Size / 2.0));

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, XCoord, YCoord, Size, Size, Parent, NULL, ins, NULL);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

void TrackedCoord::Binding(Vector<Vector2d*> New)
{
	if (New.Size == 0)
		return;

	Modify = New;
	Vector2d* Tar = New[0];
	for (uint i = 1; i < Modify.Size; i++)
		*Modify[i] = *Tar;

	int X = int(Tar->X);
	int Y = int(Tar->Y);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	int WndX = X - (WndRect.right / 2);
	int WndY = Y - (WndRect.bottom / 2);

	MoveWindow(_Base, WndX, WndY, WndRect.right, WndRect.bottom, true);
}
void TrackedCoord::AddToBind(Vector2d* New)
{
	if (Modify.Size != 0)
		*New = *Modify[0];

	Modify.Add(New);
}

ATOM TrackedCoord::_ThisAtom = ATOM();
void TrackedCoord::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(TrackedCoord*);
	wn.hbrBackground = NULL;
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.hIcon = NULL;
	wn.hInstance = ins;
	wn.lpfnWndProc = TrackedCoord::WndProc;
	wn.lpszClassName = L"TrackedCoord";
	wn.lpszMenuName = NULL;
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}
LRESULT __stdcall TrackedCoord::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	TrackedCoord* Item = reinterpret_cast<TrackedCoord*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!Item)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_ERASEBKGND:
		return 1;
	case WM_TIMER:
		return Item->Timer(wp);
	case WM_MOVE:
	{
		POINTS Pt = MAKEPOINTS(lp);
		Vector2d New(static_cast<double>(Pt.x), static_cast<double>(Pt.y));
		for (uint i = 0; i < Item->Modify.Size; i++)
			*Item->Modify[i] = New;
		Item->OnMove(New);
		break;
	}
	default:
		return CommonWndProc(Window, Message, wp, lp);
	}

	return 0;
}

LRESULT TrackedCoord::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	HBRUSH Bk = CreateSolidBrush(this->Bk), Border = CreateSolidBrush(this->Border);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	FillRect(Dc, &WndRect, Border);
	WndRect.left += 3;
	WndRect.top += 3;
	WndRect.right -= 3;
	WndRect.bottom -= 3;

	FillRect(Dc, &WndRect, Bk);

	DeleteObject(Bk);
	DeleteObject(Border);
	EndPaint(_Base, &ps);
	return 0;
}
LRESULT TrackedCoord::Click()
{
	_Track = true;
	SetTimer(_Base, 1, 100, NULL);
	return 0;
}
LRESULT TrackedCoord::MouseUp()
{
	_Track = false;
	KillTimer(_Base, 1);
	return 0;
}
LRESULT TrackedCoord::Timer(WPARAM wp)
{
	if (!_Track || !(GetAsyncKeyState(VK_LBUTTON) & 0x8000))
	{
		_Track = false;
		return 0;
	}

	POINT Mouse;
	GetCursorPos(&Mouse);
	ScreenToClient(GetParent(_Base), &Mouse);
	int X = static_cast<int>(Mouse.x);
	int Y = static_cast<int>(Mouse.y);

	Vector2d* Old = Modify[0];
	Vector2d New;
	if (Old->X == X && Old->Y == Y)
		return 0;

	RECT ParentRect;
	GetClientRect(GetParent(_Base), &ParentRect);

	if (X < 0 || Y < 0 || X > ParentRect.right || Y > ParentRect.bottom)
		return 0;

	New = { static_cast<double>(X), static_cast<double>(Y) };
	for (uint i = 0; i < Modify.Size; i++)
		*Modify[i] = New;

	OnMove(New);

	RECT WndRect;
	GetClientRect(_Base, &WndRect);

	int WndX = X - (WndRect.right / 2);
	int WndY = Y - (WndRect.bottom / 2);

	MoveWindow(_Base, WndX, WndY, WndRect.right, WndRect.bottom, true);
	RedrawWindow(GetParent(_Base), NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
	return 0;
}