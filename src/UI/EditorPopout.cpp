#include "EditorPopout.h"

#include "..\Ins.h"
#include "..\Files\SessionControl.h"
#include "..\Editors\EditorFrame.h"
#include "..\Editors\EditorRegistry.h"

namespace Armin::UI
{
	using namespace Files;
	using namespace Editors;

	EditorPopout::EditorPopout(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		Current = nullptr;
		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Editor Popout", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 960, 640, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		EditorPlacement = { 10, 10, 10, 10 };
	}
	EditorPopout::~EditorPopout()
	{
		SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);
		DestroyWindow(_Base);

		Destroy();
	}

	ATOM EditorPopout::_ThisAtom = ATOM();
	void EditorPopout::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.cbClsExtra = 0;
		wn.cbClsExtra = sizeof(EditorPopout*);
		wn.hbrBackground = NULL;
		wn.lpfnWndProc = EditorPopout::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.lpszClassName = L"EditorPopoutClass";
		wn.hIcon = ArminIcon;
		wn.lpszMenuName = NULL;
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall EditorPopout::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		EditorPopout* Item = reinterpret_cast<EditorPopout*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_KEYDOWN:
			return Item->KeyDown(wp);
		case WM_CLOSE:
			return Item->OnClose();
		case WM_DESTROY:
		{
			LRESULT Return = Item->Destroy();
			SetWindowLongPtrW(Window, GWLP_USERDATA, 0);
			
			delete Item;
			return Return;
		}
		case WM_SIZE:
			return Item->Size();
		case WM_ERASEBKGND:
			return 1;
		case WM_GETMINMAXINFO:
			return Item->GetMinMaxInfo(lp);
		//case Editor_SetCurrentToNullptr:
		//	Item->Current = nullptr;
		//	DestroyWindow(Window);
			return 0;
		}

		return DefWindowProcW(Window, Message, wp, lp);
	}

	void EditorPopout::ProcessOpenEditor(EditorFrame* Editor)
	{
		RECT Place = EditorPlacement;

		if (Current)
		{
			delete Current;
			Current = nullptr;
		}

		Current = Editor;

		if (Current)
			SetWindowTextW(_Base, static_cast<LPCWSTR>(Editor->GetName()));
	}

	LRESULT EditorPopout::Paint()
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
	LRESULT EditorPopout::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1:
			if (SessionControl::Apply(Current))
			{
				Current = nullptr;
				DestroyWindow(_Base);
			}
			break;
		}

		return 0;
	}
	LRESULT EditorPopout::KeyDown(WPARAM Key)
	{
		if (Key == VK_ESCAPE)
			OnClose();
		else if (Key == 0x53 && (GetKeyState(VK_CONTROL) & 0x8000))
			Command(1, 0);

		if (Current)
			Current->BasicKeyDown(Key);

		return 0;
	}
	LRESULT EditorPopout::OnClose()
	{
		if (EditorRegistry::CloseEditor(Current))
		{
			Current = nullptr;
			DestroyWindow(_Base);
		}
		return 0;
	}
	LRESULT EditorPopout::Destroy()
	{
		return 0;
	}
	LRESULT EditorPopout::Size()
	{
		if (Current)
			Current->ReSize();

		return 0;
	}
	LRESULT EditorPopout::GetMinMaxInfo(LPARAM lp)
	{
		MINMAXINFO* Item = (LPMINMAXINFO)lp;

		Item->ptMinTrackSize.x = 960;
		Item->ptMinTrackSize.y = 640;
		return 0;
	}

	void EditorPopout::Close()
	{
		OnClose();
	}

}