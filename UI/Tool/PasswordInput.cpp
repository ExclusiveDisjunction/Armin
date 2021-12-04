#include "PasswordInput.h"

namespace Armin::UI::Tool
{
	PasswordInput::PasswordInput(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Password Input", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 600, 250, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadControls();
	}

	String PasswordInput::Execute(HINSTANCE ins)
	{
		PasswordInput* This = new PasswordInput(ins);
		
		MSG msg;
		int Result;
		while ((Result = GetMessageW(&msg, This->_Base, 0, 0)) != 0)
		{
			if (Result < 0)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		String Return = This->Return;
		delete This;

		return Return;
	}

	ATOM PasswordInput::_ThisAtom = ATOM();
	void PasswordInput::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = PasswordInput::WndProc;
		wn.hInstance = ins;
		wn.hCursor = ArminCursor;
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"PasswordInputClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(PasswordInput*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall PasswordInput::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		PasswordInput* This = reinterpret_cast<PasswordInput*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return This->Paint();
		case WM_KEYDOWN:
			return This->KeyDown(wp);
		case WM_COMMAND:
			return This->Command(wp, lp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void PasswordInput::LoadControls()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		TextStyleSheet TextStyle;
		StyleSheet Style;
		AaColor BaseBk = Grey1;

		{
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;
			TextStyle.FontSize = 13;
			
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;
		}

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = WndRect.right - (10 + XCoord);
			int Height = 27;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Please input the password for this project.", BaseBk, TextStyle, false));
			YCoord += 20 + Height;

			Password = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, false, true);
			YCoord += 20 + Height;

			Width = 130;
			XCoord = (WndRect.right / 2) - (Width + 5);

			Submit = new Button(XCoord, YCoord, Width, Height, L"Submit", _Base, (HMENU)1, ins, Style, TextStyle);
			XCoord += 10 + Width;

			Close = new Button(XCoord, YCoord, Width, Height, L"Cancel", _Base, (HMENU)2, ins, Style, TextStyle);
		}

		SetFocus(*Password);
	}

	LRESULT PasswordInput::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Grey1);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT PasswordInput::KeyDown(WPARAM wp)
	{
		switch (wp)
		{
		case VK_RETURN:
			Command(1, 0);
			break;
		case VK_ESCAPE:
			Command(2, 0);
			break;
		}

		return 0;
	}
	LRESULT PasswordInput::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1:
			Return = Password->GetText();
			DestroyWindow(_Base);
			break;
		case 2:
			Return = L"";
			DestroyWindow(_Base);
			break;
		}
		return 0;
	}
}