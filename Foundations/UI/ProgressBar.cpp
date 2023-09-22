#include "ProgressBar.h"

LRESULT __stdcall ProgressBar::_ProgressBarProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	switch (Message)
	{
	case WM_PAINT:
	{
		int Progress = (int)GetWindowLongPtrW(Window, GWLP_USERDATA);
		DWORD Style = (DWORD)GetWindowLongPtrW(Window, GWL_STYLE);
		
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(Window, &ps);
		RECT& Area = ps.rcPaint;

		RECT WndRect;
		GetClientRect(Window, &WndRect);
		SetBkMode(Dc, TRANSPARENT);

		if (Style & APBS_BORDER)
		{
			FillRect(Dc, &WndRect, CreateSolidBrush((Style & APBS_DARKBK ? 0x505050 : 0x000000)));
			WndRect.left += 3;
			WndRect.top += 3;
			WndRect.right -= 3;
			WndRect.bottom -= 3;
		}

		double Rate = Progress / 100.0;	
		int Loc;
		if (Style & APBS_VERTICAL)
			Loc = static_cast<int>(Rate * Area.bottom);
		else 
			Loc = static_cast<int>(Rate * Area.right);

		if (Progress < 0)
			Loc = 0;

		COLORREF BkColor;
		COLORREF BarColor;

		if (Style & APBS_COLORGREEN)
			BarColor = 0x00FF00;
		else if (Style & APBS_COLORRED)
			BarColor = 0x0000FF;
		else if (Style & APBS_COLORYELLOW)
			BarColor = 0x00FFFF;

		if (Style & APBS_LIGHTBK)
			BkColor = 0xCCCCCC;
		else if (Style & APBS_DARKBK)
			BkColor = 0x353535;
		
		RECT ToDraw = { 0 };
		const int ZeroPos = (Style & APBS_BORDER ? 3 : 0);
      if (Loc == Area.bottom)
         Loc -= ZeroPos;
		if (Style & APBS_VERTICAL)
		{
			if (Style & APBS_INVERTDIRECTION)
			{
				ToDraw.left = ZeroPos;
				ToDraw.top = Loc;
				ToDraw.right = WndRect.right;
				ToDraw.bottom = ZeroPos;
			}
			else
			{
				ToDraw.left = ZeroPos;
				ToDraw.top = WndRect.bottom + Loc;
				ToDraw.right = WndRect.right;
				ToDraw.bottom = WndRect.bottom;
			}			
		}
		else
		{
			if (Style & APBS_INVERTDIRECTION)
			{
				ToDraw.left = WndRect.right - Loc;
				ToDraw.top = ZeroPos;
				ToDraw.right = WndRect.right;
				ToDraw.bottom = WndRect.bottom;
			}
			else
			{
				ToDraw.left = ZeroPos;
				ToDraw.top = ZeroPos;
				ToDraw.right = Loc;
				ToDraw.bottom = WndRect.bottom;
			}
		}

		FillRect(Dc, &WndRect, CreateSolidBrush(BkColor));
		FillRect(Dc, &ToDraw, CreateSolidBrush(BarColor));

		EndPaint(Window, &ps);
		return 0;
	}
	case WM_SIZE:
	{
		RECT Rect;
		GetClientRect(Window, &Rect);
		InvalidateRect(Window, &Rect, TRUE);
		return 0;
	}
	case APBM_SETPROGRESS:
	{
		int Value = (int)wp;
		if (Value < 0 || Value > 100)
		{
			SetLastError(87l);
			return 1;
		}

		SetWindowLongPtrW(Window, GWLP_USERDATA, (LONG)Value);

		RECT Bounds;
		GetClientRect(Window, &Bounds);
		InvalidateRect(Window, &Bounds, TRUE);

		return 0;
	}
	case APBM_GETPROGRESS:
	{
		return GetWindowLongPtrW(Window, GWLP_USERDATA);
	}
	default:
		return DefWindowProcW(Window, Message, wp, lp);
	}
}

ProgressBar ProgressBar::Create(HWND Parent, int X, int Y, int Width, int Height, HINSTANCE Ins, DWORD Style)
{
	HWND Wnd = CreateWindowExW(0l, ProgressBarClass, L"", WS_CHILD | WS_VISIBLE | Style, X, Y, Width, Height, Parent, NULL, Ins, NULL);
	if (!Wnd)
		return ProgressBar();

	ProgressBar Return;
	Return._Base = Wnd;

	return Return;
}
void ProgressBar::InitateBase(HINSTANCE Ins)
{
	WNDCLASSW wn;
	wn.style = CS_VREDRAW | CS_HREDRAW;
	wn.lpfnWndProc = _ProgressBarProc;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = 0;
	wn.hInstance = Ins;
	wn.hIcon = LoadIconW(NULL, IDI_APPLICATION);
	wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
	wn.lpszClassName = ProgressBarClass;
	wn.lpszMenuName = L"";
	wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	ATOM Thing = RegisterClassW(&wn);
}