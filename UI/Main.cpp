#include "Main.h"

#include "Files\Stream.h"
#include "SideHost.h"
#include "..\Ins.h"
#include "..\UserRegistry.h"
#include "..\Editors\EditorFrame.h"
#include "..\Editors\EditorRegistry.h"
#include "..\Files\ArminSessions.h"
#include "..\Files\SessionControl.h"

namespace Armin::UI
{
	using namespace Files;
	using namespace Editors;

	Main::Main(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		EditorRegistry::SetHost(this);
		EditorRegistry::Track(static_cast<FooterHost*>(this));

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), static_cast<const wchar_t*>(L"Armin " + String(Version) + (Ins::IsLoaded() ? L" - " + FileFullName(LoadedProjectPath) : L" - Unloaded")), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1250, 700, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_MAXIMIZE);
		UpdateWindow(_Base);

		LoadControls();

		SetRibbonStatus();
	}

	ATOM Main::_ThisAtom = ATOM();
	void Main::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = Main::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"MainClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(Main*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall Main::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		Main* Target = reinterpret_cast<Main*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Target)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Target->Paint();
		case WM_CLOSE:
			return Target->OnClose();
		case WM_DESTROY:
			return Target->Destroy();
		case WM_SIZE:
			return Target->Size();
		case WM_KEYDOWN:
			return Target->KeyDown(wp);
		case WM_TIMER:
			return Target->Timer(wp);
		case WM_COMMAND:
			return Target->Command(wp, lp);
		case WM_GETMINMAXINFO:
			return Target->GetMinMaxInfo(lp);
		//case Editor_SetCurrentToNullptr:
		//{
		//	if (Target->Current)
		//		Target->RemoveEditorButton(Target->Current);
		//	Target->Current = nullptr;

		//	Target->OpenLastEditor();
		//	return 0;
		//}
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void Main::Open()
	{
		MSG msg;
		int Result;

		while ((Result = GetMessageW(&msg, NULL, NULL, NULL)) != 0)
		{
			if (Result < 0)
				continue;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}
	void Main::Close()
	{
		SendMessageW(_Base, WM_CLOSE, 0, 0);
	}

	void Main::LoadControls()
	{
		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		StyleSheet Style;
		Style.Background = Grey2;
		Style.BorderBrush = 0x00FFFFFF;
		Style.BorderThickness = 0;
		Style.Radius = 0;

		int XCoord = 0;
		int YCoord = 0;
		int Width = WndRect.right - XCoord;
		int Height = 80;

		RibbonBase = new Grid(XCoord, YCoord, Width, Height, _Base, ins, Style);

		YCoord += Height;
		Width = 240;
		Height = WndRect.bottom - YCoord - 32;
		Style.Background = Accent1;

		LoadEditorButtonsBase(_Base, ins, XCoord, YCoord, Width, Height, Accent1);

		XCoord += 20 + Width;
		YCoord += 20;

		EditorPlacement = { XCoord, YCoord, 20, 20 };

		Height = 30;
		Width = WndRect.right;
		XCoord = 0;
		YCoord = WndRect.bottom - Height;

		Style.Background = Accent2;

		TextStyleSheet TextStyle;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.FontSize = 13;
		TextStyle.Foreground = 0xFFFFFFFF;

		Footer = new Grid(XCoord, YCoord, Width, Height, _Base, ins, Style);
		FooterText = new Label(5, 3, Width, Height - 9, Footer, ins, L"Ready", Style.Background, TextStyle, false);

		EditorPlacement.bottom += Height;

		auto Function = [](HWND Param, UINT Message, UINT_PTR ID, DWORD Milliseconds) -> void
		{ //TODO: convert to multithreaded function.
			if (LoadedProject) //If a user file was loaded at start, then prompt a sign in.
			{
				UserSystem* Conv = dynamic_cast<UserSystem*>(LoadedProject);
				UniProject* InvConv = dynamic_cast<UniProject*>(LoadedProject);
				if (Conv)
				{
					KillTimer(Param, ID);
					SignIn();
				}
				else if (InvConv && InvConv->Config == UPC_Inventory)
					EditorRegistry::OpenEditor(new Inventory::InventoryEditor(nullptr), nullptr);
			}

			KillTimer(Param, ID);
		};
		SetTimer(_Base, 1, 200, Function);

		LoadRibbon();
	}

	LRESULT Main::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background1);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT Main::OnClose()
	{
		if (SessionControl::Close(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE))))
			DestroyWindow(_Base);

		return 0;
	}
	LRESULT Main::Destroy()
	{
		/*
		WINDOWPLACEMENT Place;
		GetWindowPlacement(_Base, &Place);

		InsInstance->LastWindowState = (WindowStates)Place.showCmd;
		*/

		PostQuitMessage(0);
		return 0;
	}
	LRESULT Main::Size()
	{
		if (!_Loaded)
			return 0;

		Vector<EditorFrame*> ThisEditors = EditorRegistry::GetEditors(EDT_All, this);
		for (uint i = 0; i < ThisEditors.Size; i++)
			ThisEditors[i]->ReSize();

		RECT WndRect;
		GetClientRect(_Base, &WndRect); //1200, 660

		int XCoord = 0;
		int YCoord = 0;
		int Width = WndRect.right - XCoord;
		int Height = 80;

		if (RibbonBase)
			RibbonBase->Move(XCoord, YCoord, Width, Height);

		YCoord += Height;
		Width = 240;
		Height = WndRect.bottom - YCoord - 32;

		if (EditorButtonsScroll)
		{
			EditorButtonsScroll->Move(XCoord, YCoord, Width, Height);
			this->ResizeEditorButtons();
		}

		Height = 30;
		Width = WndRect.right;
		XCoord = 0;
		YCoord = WndRect.bottom - Height;

		Footer->Move(XCoord, YCoord, Width, Height);
		FooterText->Move(5, 3, Width, Height - 9);

		ResizeRibbon();

		return 0;
	}
	LRESULT Main::KeyDown(WPARAM wp)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			if (wp == L'S')
				SessionControl::Save(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			else if ((GetKeyState(VK_SHIFT) & 0x8000) && wp == L'A')
				SessionControl::ApplyAll();

			if (Current)
			{
				if (wp == L'A')
					SessionControl::Apply(Current);
				else if ((GetKeyState(VK_SHIFT) & 0x8000) && wp == L'P')
					EditorRegistry::MoveEditor(Current, nullptr, true);
				else if (wp == L'E' && (GetKeyState(VK_SHIFT) & 0x8000))
					EditorRegistry::CloseEditor(Current);
			}
		}
		return 0;
	}
	LRESULT Main::Timer(WPARAM ID)
	{
		if (ID == 2)
			ResetFooterText();
		return 0;
	}
	LRESULT Main::Command(WPARAM wp, LPARAM lp)
	{
		ProcessRibbon(wp, lp);
		return 0;
	}
	LRESULT Main::GetMinMaxInfo(LPARAM lp)
	{
		MINMAXINFO* Item = (LPMINMAXINFO)lp;

		Item->ptMinTrackSize.x = 1200;
		Item->ptMinTrackSize.y = 660;
		return 0;
	}
}