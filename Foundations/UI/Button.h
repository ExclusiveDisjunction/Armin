#pragma once

#include "ControlBase.h"
#include "Win32 Common.h"

class Button : public ControlBase
{
private:
	StyleSheet _Style;
	TextStyleSheet _TextStyle;
	String _Text;
	bool _Clicked = false, _HasMouse = false;
	TextAlignment _Alignment;

	bool FocusOnClick() const override { return false; }

	static ATOM _ThisAtom;

	inline static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		Button* This = reinterpret_cast<Button*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return ControlBase::CommonWndProc(Window, Message, wp, lp);

		if (Message == WM_MOUSEMOVE)
			return This->MouseMove();
		else if (Message == WM_MOUSELEAVE)
			return This->MouseLeave();
		else if (Message == WM_ERASEBKGND)
			return true;

		return ControlBase::CommonWndProc(Window, Message, wp, lp);
	}

public:
	Button(int X, int Y, int Width, int Height, String Text, HWND Parent, HMENU ID, HINSTANCE ins, StyleSheet _Style, TextStyleSheet _Text, TextAlignment Alignment = TA_CenterAlignment);
	Button(int X, int Y, int Width, int Height, String Text, ControlBase* Parent, HMENU ID, HINSTANCE ins, StyleSheet _Style, TextStyleSheet _Text, TextAlignment Alignment = TA_CenterAlignment) : Button(X, Y, Width, Height, Text, *Parent, ID, ins, _Style, _Text, Alignment) {}

	StyleSheet& Style() { return _Style; }
	TextStyleSheet& TextStyle() { return _TextStyle; }
	String Text() { return _Text; }
	void Text(String New)
	{
		_Text = New;

		Redraw();
	}

	LRESULT Paint() override;
	LRESULT Click() override;
	LRESULT TimerTick(LONG ID) override;
	LRESULT MouseLeave();
	LRESULT MouseMove();
	LRESULT KeyDown(WPARAM wp) override;
	LRESULT GotFocus() override;
	LRESULT LostFocus() override;

	static void InitBase(HINSTANCE ins);
};