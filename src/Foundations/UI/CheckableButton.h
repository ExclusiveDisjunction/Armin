#pragma once

#include "Win32 Common.h"

#define ACBM_UNCHECK WM_USER + 5 //Unchecks the current control. Do not use, use ARBM_SETCHECK (this is internal) WPARAM and LPARAM are 0.
#define ACBM_CHECK WM_USER + 6 //Checks the current control. Do not use, use ARBM_SETCHECK (this is internal) WPARAM and LPARAM are 0.

enum CheckableButtonType
{
	CBT_Radio,
	CBT_CheckBox
};

class CheckableButton : public ControlBase
{
private:
	static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM); //The window procedure for thsi button.
	static ATOM _ThisAtom;
	static void InitBase(HINSTANCE ins);
	
	bool _Check, _HasMouse;
	CheckableButtonType _Type = CBT_Radio;
	StyleSheet _Style;
	TextStyleSheet _TextStyle;
	String _Text;

	void Uncheck(bool SenderType)
	{
		if (SenderType && _Type == CBT_CheckBox)
			return;

		_Check = false;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
	void Check()
	{
		_Check = true;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}

public:
	CheckableButton(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, bool IsChecked, String Text, CheckableButtonType Type, StyleSheet Style, TextStyleSheet TextStyle);
	CheckableButton(int X, int Y, int Width, int Height, ControlBase* Parent, HINSTANCE ins, HMENU ID, bool IsChecked, String Text, CheckableButtonType Type, StyleSheet Style, TextStyleSheet TextStyle) : CheckableButton(X, Y, Width, Height, *Parent, ins, ID, IsChecked, Text, Type, Style, TextStyle) {}

	bool GetCheckState()
	{
		return _Check;
	}
	void SetCheckState(bool New);
	StyleSheet Style()
	{
		return _Style;
	}
	void Style(StyleSheet New)
	{
		_Style = New;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
	TextStyleSheet TextStyle()
	{
		return _TextStyle;
	}
	void TextStyle(TextStyleSheet New)
	{
		_TextStyle = New;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
	String Text()
	{
		return _Text;
	}
	void Text(String New)
	{
		_Text = New;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
	CheckableButtonType Type()
	{
		return _Type;
	}
	void Type(CheckableButtonType New)
	{
		_Type = New;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}

	void Move(int X, int Y, int Width, int Height) override
	{
		MoveWindow(_Base, X, Y, Width, Height, TRUE);
	}

	LRESULT KeyDown(WPARAM ID) override;
	LRESULT Paint() override;
	LRESULT Click() override;
	LRESULT MouseMove();
	LRESULT MouseLeave();
};