#include "FileProgress.h"

#include "..\..\Common.h"

namespace Armin::UI::Tool
{
	ATOM FileProgress::_ThisAtom = ATOM();

	LRESULT __stdcall FileProgress::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		if (Message == WM_PAINT)
		{
			PAINTSTRUCT ps;
			HDC Dc = BeginPaint(Window, &ps);

			HBRUSH Bk = CreateSolidBrush(Accent1);
			HPEN Border = CreatePen(PS_SOLID, 3, Accent4);

			HFONT Font = CreateFontW(-MulDiv(15, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, 0, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(StandardFontName));
			SelectObject(Dc, Font);
			SetTextColor(Dc, FontColor);
			SetBkMode(Dc, TRANSPARENT);

			SelectObject(Dc, Bk);
			SelectObject(Dc, Border);

			RECT WndRect;
			GetClientRect(Window, &WndRect);

			bool Mode = false;
			FileProgress* Progress = reinterpret_cast<FileProgress*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
			if (!Progress)
				Mode = false;
			else
				Mode = Progress->Mode;

			const wchar_t* Text = (Mode ? L"Please wait while the project saves..." : L"Please wait while the project loads...");
			int TextLen = 39; //Both are 39 chars long

			SIZE TextSize;
			GetTextExtentPoint32W(Dc, Text, TextLen, &TextSize);
			POINT TextOrig = { 10, (WndRect.bottom / 2) - (TextSize.cy / 2) };

			Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);
			TextOutW(Dc, TextOrig.x, TextOrig.y, Text, TextLen);

			DeleteObject(Bk);
			DeleteObject(Border);
			DeleteObject(Font);

			EndPaint(Window, &ps);
			return 0;
		}

		return DefWindowProcW(Window, Message, wp, lp);
	}
	void FileProgress::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = FileProgress::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"FileProgress";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(FileProgress*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}

	FileProgress::FileProgress(bool Mode, HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		this->Mode = Mode;

		const int Width = 700;
		const int Height = 200;
		int X = (GetSystemMetrics(SM_CXSCREEN) / 2) - (Width / 2);
		int Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (Height / 2);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Please Wait:", WS_POPUP | WS_VISIBLE, X, Y, Width, Height, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);
	}
}