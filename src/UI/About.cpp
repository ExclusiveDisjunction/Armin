#include "About.h"

#include "..\UICommon.h"
#include "..\resource.h"
#include "..\Ins.h"

namespace Armin::UI
{
	About::About(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), static_cast<LPCWSTR>(L"About Armin " + Version), WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 550, 400, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadControls();
	}
	About::~About()
	{
		SetWindowLongPtrW(_Base, GWLP_USERDATA, 0l);
		DestroyWindow(_Base);
	}

	ATOM About::_ThisAtom = ATOM();
	void About::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = About::WndProc;
		wn.hInstance = ins;
		wn.hCursor = ArminCursor;
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"AboutWindowClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(About*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall About::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		About* This = reinterpret_cast<About*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_DESTROY:
		{
			delete This;
			return 0;
		}
		case WM_PAINT:
			return This->Paint();
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	LRESULT About::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		HICON ArminIconForRender;
		{
			const int IconSize = 100;

			HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
			HRSRC FindR;
			HGLOBAL LoadR;
			LPVOID RPointer;
			int ID;

			FindR = FindResourceW(ins, MAKEINTRESOURCE(ID_ARMINICO), RT_GROUP_ICON);
			LoadR = LoadResource(ins, FindR);
			RPointer = LockResource(LoadR);

			ID = LookupIconIdFromDirectoryEx(static_cast<PBYTE>(RPointer), true, IconSize, IconSize, LR_DEFAULTCOLOR);
			UnlockResource(LoadR);
			FreeResource(LoadR);

			FindR = FindResourceW(ins, MAKEINTRESOURCE(ID), RT_ICON);
			LoadR = LoadResource(ins, FindR);
			RPointer = LockResource(LoadR);

			ArminIconForRender = CreateIconFromResourceEx(static_cast<PBYTE>(RPointer), SizeofResource(ins, FindR), true, 0x00030000, IconSize, IconSize, LR_DEFAULTCOLOR);

			UnlockResource(LoadR);
			FreeResource(LoadR);
		}

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background1);
		FillRect(Dc, &WndRect, Bk);

		DrawIconEx(Dc, 20, 20, ArminIconForRender, 75, 75, 0, NULL, DI_NORMAL);
		DestroyIcon(ArminIconForRender);
		return 0;
	}
	void About::LoadControls()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		AaColor BaseBk = Background1;		
		TextStyleSheet TextStyle;

		int BaseYCoord = 0;

		{
			TextStyle.FontFamily = TitleFontName;
			TextStyle.FontSize = 24;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = false;

			int XCoord = 130;
			int YCoord = 20;
			int Width = WndRect.right - (10 + XCoord);
			int Height = 30;

			String OutputText = L"Armin ";
			StringList Parts = Version.Split(L'.');
			OutputText += Parts[0];

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, OutputText, BaseBk, TextStyle, false));
			BaseYCoord = 130;
		}

		{
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;
			TextStyle.Alignment = TA_RightAlignment;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Version:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Licenced To:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Created By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Developer Mode:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Is Loaded:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Loaded Path:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			XCoord += 5 + Width;
			YCoord = BaseYCoord;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Bold = false;
			TextStyle.Alignment = TA_LeftAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, Version, BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Blue Water Ventures International", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Hollan Connor Sellars", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, InsInstance->DevMode ? L"Yes" : L"No", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, Ins::IsLoaded() ? L"Yes" : L"No", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, Ins::IsLoaded() ? LoadedProjectPath : L"No Data", BaseBk, TextStyle, false));
		}
	}
}