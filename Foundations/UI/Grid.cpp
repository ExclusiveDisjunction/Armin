#include "Grid.h"

Grid::Grid(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, StyleSheet Style)
{
    if (!_ThisAtom)
        InitBase(ins);

    _Style = Style;
    _Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, X, Y, Width, Height, Parent, NULL, ins, NULL);

    SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

ATOM Grid::_ThisAtom = ATOM();
void Grid::InitBase(HINSTANCE ins)
{
    WNDCLASSW wn = { 0 };
    wn.lpfnWndProc = Grid::WndProc;
    wn.hInstance = ins;
    wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wn.lpszMenuName = NULL;
    wn.lpszClassName = L"GridClass";
    wn.hIcon = NULL;
    wn.cbClsExtra = 0;
    wn.cbWndExtra = sizeof(Grid*);
    wn.style = CS_HREDRAW | CS_VREDRAW;

    _ThisAtom = RegisterClassW(&wn);
}
LRESULT __stdcall Grid::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
    if (Message == WM_COMMAND)
        return SendMessageW(GetParent(Window), Message, wp, lp);

    return CommonWndProc(Window, Message, wp, lp);
}

LRESULT Grid::Paint()
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
