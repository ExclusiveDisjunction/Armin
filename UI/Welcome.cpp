#include "Welcome.h"

#include "Files\Stream.h"
#include "UI\Controls.h"
#include "..\UICommon.h"
#include "..\Config\Ins.h"
#include "..\Config\Recent.h"
#include "..\Files\SessionControl.h"

namespace Armin::UI
{
	using namespace Config;
	using namespace Files;

	Welcome::Welcome(HINSTANCE ins)
	{
		DoQuit = false;

		if (!_ThisAtom)
			InitBase(ins);

		const int Width = 940;
		const int Height = 540;
		int XCoord = GetSystemMetrics(SM_CXSCREEN) / 2 - (Width / 2);
		int YCoord = GetSystemMetrics(SM_CYSCREEN) / 2 - (Height / 2);
		_Base = CreateWindowExW(0l, L"WelcomeClass", L"Welcome", WS_OVERLAPPEDWINDOW, XCoord, YCoord, Width, Height, NULL, NULL, ins, NULL);

		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		LoadControls();
	}

	void Welcome::Open()
	{
		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		MSG msg;
		int Result;
		while ((Result = GetMessageW(&msg, _Base, NULL, NULL)) != 0 && _Base)
		{
			if (Result < 0)
				return;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	void Welcome::Close()
	{
		DestroyWindow(_Base);
	}

	void Welcome::LoadControls()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 230;
			int Height = 50;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = TitleFontName;
			TextStyle.FontSize = 23;
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Welcome!", Grey1, TextStyle, false));

			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Bold = false;

			Width = 220;
			XCoord = WndRect.right - 10 - Width;
			int PreYCoord = (YCoord += Height + 10);
			Height = 70;

			StyleSheet Style;
			Style.Background = Grey1;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.BaseBackground = Grey1;
			Style.Radius = 20;

			TextStyle.FontSize = 17;

			OpenCmd = new Button(XCoord, YCoord, Width, Height, L"Open Project", _Base, (HMENU)2, ins, Style, TextStyle);
			YCoord += 7 + Height;

			NewCmd = new Button(XCoord, YCoord, Width, Height, L"New Project", _Base, (HMENU)3, ins, Style, TextStyle);
			YCoord += 15 + Height;

			TextStyle.FontSize = 15;
			Height = 40;

			OpenLast = new Button(XCoord, YCoord, Width, Height, L"Open Last", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 7 + Height;

			OpenWithout = new Button(XCoord, YCoord, Width, Height, L"Open Without Project", _Base, (HMENU)6, ins, Style, TextStyle);
			YCoord += 7 + Height;

			Quit = new Button(XCoord, YCoord, Width, Height, L"Quit", _Base, (HMENU)7, ins, Style, TextStyle);

			XCoord = 10;
			YCoord = PreYCoord;
			Width = WndRect.right - (20 + XCoord + Width);
			Height = WndRect.bottom - 10 - YCoord;

			Style.Background = Grey2;

			ViewerScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			Viewer = new Grid(0, 0, Width, Height, ViewerScroll, ins, Style);
			ViewerScroll->SetViewer(Viewer);

			{
				GetClientRect(Viewer, &WndRect);

				XCoord = 5;
				YCoord = 5;
				Height = 27;
				Width = WndRect.right - 5 - XCoord;

				TextStyle.FontSize = 13;
				Style.BaseBackground = Style.Background = Grey2;
				Style.BorderThickness = 0;
				Style.Radius = 0;

				StringList Paths = RecentInstance->GetRecentsForUI();
				this->Paths = RecentInstance->RecentPaths;

				Viewer->Move(WndRect.left, WndRect.top, WndRect.right, static_cast<int>((Height + 5) * (Paths.Size + 1)));
				ViewerScroll->Reset();

				LONG_PTR Item = 19;
				for (uint i = 0; i < Paths.Size; i++)
				{
					Item++;
					Recents.Add(new Button(XCoord, YCoord, Width, Height, Paths[i], Viewer, (HMENU)Item, ins, Style, TextStyle, TA_LeftAlignment));
					YCoord += 3 + Height;
				}
			}
		}
	}
	void Welcome::MoveControls()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 230;
			int Height = 50;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);

			Width = 220;
			XCoord = WndRect.right - 10 - Width;
			int PreYCoord = (YCoord += Height + 10);
			Height = 70;

			OpenCmd->Move(XCoord, YCoord, Width, Height);
			YCoord += 7 + Height;

			NewCmd->Move(XCoord, YCoord, Width, Height);
			YCoord += 15 + Height;

			Height = 40;

			OpenLast->Move(XCoord, YCoord, Width, Height);
			YCoord += 7 + Height;

			OpenWithout->Move(XCoord, YCoord, Width, Height);
			YCoord += 7 + Height;

			Quit->Move(XCoord, YCoord, Width, Height);

			XCoord = 10;
			YCoord = PreYCoord;
			Width = WndRect.right - (20 + XCoord + Width);
			Height = WndRect.bottom - 10 - YCoord;

			ViewerScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			RECT ClientRect;
			GetClientRect(ViewerScroll, &ClientRect);

			int XCoord = 5;
			int YCoord = 5;
			int Width = ClientRect.right - (XCoord * 2);
			int Height = 27;

			Viewer->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Recents.Size)));
			ViewerScroll->Reset();
			for (uint i = 0; i < Recents.Size; i++)
			{
				Recents[i]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;
			}
		}
	}
	void Welcome::RefreshList()
	{
		RECT WndRect;
		GetClientRect(Viewer, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		int XCoord = 5;
		int YCoord = 5;
		int Height = 27;
		int Width = WndRect.right - 5 - XCoord;

		TextStyleSheet TextStyle;
		TextStyle.FontSize = 13;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.Foreground = FontColor;

		StyleSheet Style;
		Style.BaseBackground = Style.Background = Grey2;
		Style.BorderThickness = 0;
		Style.Radius = 0;

		StringList Paths = RecentInstance->GetRecentsForUI();
		this->Paths = RecentInstance->RecentPaths;

		Viewer->Move(0, 0, WndRect.right, static_cast<int>((Height + 5) * (Paths.Size + 1)));

		LONG_PTR Item = 19;
		for (uint i = 0; i < Paths.Size; i++)
		{
			Item++;
			Recents.Add(new Button(XCoord, YCoord, Width, Height, Paths[i], Viewer, (HMENU)Item, ins, Style, TextStyle, TA_LeftAlignment));
			YCoord += 5 + Height;
		}
	}

	LRESULT Welcome::Paint()
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
	LRESULT Welcome::Command(WPARAM wp, LPARAM lp)
	{
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		switch (wp)
		{
		case 2: //Open
			if (SessionControl::Open(ins))
				DestroyWindow(_Base);
			break;
		case 3: //New
			if (SessionControl::NewFile(ins))
				DestroyWindow(_Base);
			break;
		case 4: //Unused
			break;
		case 5: //Open Last
		{
			String Path = InsInstance->LastLoaded;
			if (SessionControl::Open(Path, ins))
				DestroyWindow(_Base);
			else
				MessageBoxW(_Base, L"The last loaded file does not exist.\n\nPlease try again later.", L"Welcome:", MB_OK | MB_ICONERROR);

			break;
		}
		case 6: //Open Without
			DestroyWindow(_Base);
			break;
		case 7: //Quit
			Welcome::DoQuit = true;
			DestroyWindow(_Base);
			break;
		}

		static WPARAM LastWp = 0;

		if (wp >= 20) //Recent Item
		{
			if (wp == LastWp)
				return 0;

			LastWp = wp;

			uint Index = static_cast<uint>(wp) - 20;

			if (Index >= Paths.Size || Index < 0)
				return 0;

			String Current = Paths[Index];
			if (!FileExists(Current))
			{
				MessageBoxW(_Base, L"The path could not be opened.\n\nIt will be removed from recents.", L"Welcome:", MB_OK | MB_ICONERROR);

				RecentInstance->DeleteRecent(Index);
				Button* Item = reinterpret_cast<Button*>(lp);
				if (!Item)
					return 0;

				Item->Text(L"[FILE NOT FOUND]");
			}
			else
			{
				SessionControl::Open(Current, ins);
				DestroyWindow(_Base);
			}
		}

		return 0;
	}
	LRESULT Welcome::Size()
	{
		MoveControls();

		return 0;
	}
	LRESULT Welcome::Destroy()
	{
		return 0;
	}

	bool Welcome::DoQuit = false;
	ATOM Welcome::_ThisAtom = ATOM();
	void Welcome::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(Welcome*);
		wn.hbrBackground = NULL;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hIcon = ArminIcon;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"WelcomeClass";
		wn.lpfnWndProc = Welcome::WndProc;
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall Welcome::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		Welcome* Item = reinterpret_cast<Welcome*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_SIZE:
			return Item->Size();
		case WM_DESTROY:
			return Item->Destroy();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		default:
			return DefWindowProc(Window, Message, wp, lp);
		}
	}
}