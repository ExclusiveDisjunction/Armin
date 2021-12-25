#include "ArminControls.h"

#include "..\Editors\EditorFrame.h"
#include "..\Editors\EditorRegistry.h"
#include "..\UI\Main.h"

namespace Armin::UI
{
	using namespace Editors;

	EditorButton::EditorButton(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, EditorFrame* Source, EditorHost* Host, StyleSheet Style, TextStyleSheet TextStyle)
	{
		if (!_ThisAtom)
			InitBase(ins);

		this->Style = Style;
		this->TextStyle = TextStyle;
		this->Source = Source;
		this->Host = Host;

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), NULL, WS_CHILD | WS_VISIBLE, XCoord, YCoord, Width, Height, Parent, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}

	ATOM EditorButton::_ThisAtom = ATOM();
	void EditorButton::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = EditorButton::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"EditorButtonClass";
		wn.hIcon = NULL;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(EditorButton*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall EditorButton::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		EditorButton* This = reinterpret_cast<EditorButton*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_MOUSEMOVE:
			return This->MouseMove();
		case WM_MOUSELEAVE:
			return This->MouseLeave();
		default:
			return CommonWndProc(Window, Message, wp, lp);
		}		
	}

	LRESULT EditorButton::Paint()
	{
		if (!Source || !this)
			return 0;

		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);
		SetBkMode(Dc, TRANSPARENT);

		AaColor BkColor = HasMouse ? Accent4 : ((_Clicked || (GetForegroundWindow() == _Base) ? Style.BorderBrush : Style.Background)), BorderColor = Style.BorderBrush, ForegroundColor = TextStyle.Foreground;

		if (GetAncestor(*Source, GA_ROOT) != GetAncestor(_Base, GA_ROOT))
			BkColor = Accent2;

		if (!IsEnabled)
		{
			BorderColor.Reduce(0.7f);
			ForegroundColor.Reduce(0.7f);
		}

		HBRUSH BkBrush = CreateSolidBrush(BkColor);
		HPEN Border = CreatePen(PS_SOLID, Style.BorderThickness, BorderColor);

		HFONT Font = CreateFontW(-MulDiv(TextStyle.FontSize, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, (TextStyle.Bold ? FW_BOLD : 0), TextStyle.Italic, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(TextStyle.FontFamily));
		SetTextColor(Dc, ForegroundColor);

		SelectObject(Dc, BkBrush);
		SelectObject(Dc, Font);
		if (Style.BorderThickness == 0)
			SelectObject(Dc, GetStockObject(NULL_PEN));
		else
			SelectObject(Dc, Border);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (Style.Radius > 0)
			RoundRect(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom, Style.Radius, Style.Radius);
		else
			Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

		String RawName = Source->GetName();
		const wchar_t* Text = static_cast<const wchar_t*>(RawName);
		int TextLen = lstrlenW(Text);

		SIZE TextSize;
		GetTextExtentPoint32W(Dc, Text, TextLen, &TextSize);
		DrawTextW(Dc, Text, TextLen, &WndRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

		DeleteObject(BkBrush);
		DeleteObject(Border);
		DeleteObject(Font);

		EndPaint(_Base, &ps);

		return 0;
	}
	LRESULT EditorButton::Click()
	{
		_Clicked = true;
		SetTimer(_Base, 1, 100, NULL);
		
		Redraw();
		return 0;
	}
	LRESULT EditorButton::MouseMove()
	{
		if (!HasMouse)
		{
			HasMouse = true;
			TRACKMOUSEEVENT tme = { sizeof(tme) };
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = _Base;
			TrackMouseEvent(&tme);

			Redraw();
		}

		return 0;
	}
	LRESULT EditorButton::MouseLeave()
	{
		HasMouse = false;
		Redraw();
		return 0;
	}
	LRESULT EditorButton::RightClick()
	{
		if (GetAncestor(_Base, GA_ROOT) == *Source->Host || (GetKeyState(VK_CONTROL) & 0x8000))
			EditorRegistry::MoveEditor(Source, nullptr, true);
		else
			EditorRegistry::MoveEditor(Source, reinterpret_cast<EditorHost*>(GetWindowLongPtrW(GetAncestor(_Base, GA_ROOT), GWLP_USERDATA)), false);
		return 0;
	}
	LRESULT EditorButton::TimerTick(LONG ID)
	{
		KillTimer(_Base, ID);

		EditorHost* Host = Source->Host;
		EditorButtonHost* BttnHost = dynamic_cast<EditorButtonHost*>(Host);

		if (BttnHost)
		{
			if (BttnHost->Current)
				ShowWindow(*BttnHost->Current, SW_HIDE);
			ShowWindow(*Source, SW_SHOW);
			BttnHost->LastEditor = BttnHost->Current;
			BttnHost->Current = Source;
		}

		HWND Parent = *Host;

		SetWindowPos(Parent, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		if (GetWindowLongPtrW(Parent, GWL_STYLE) & WS_MINIMIZE)
			ShowWindow(Parent, SW_RESTORE);

		_Clicked = false;
		Redraw();
		return 0;
	}
}