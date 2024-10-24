#pragma once

#include "ControlBase.h"

#define ACTB_UNCHECK WM_USER + 3
#define ACTB_CHECK WM_USER + 4

class ToggleButton : public ControlBase
{
private:
	bool _HasMouse, _Checked, _Multi;
	StyleSheet _Style;
	TextStyleSheet _TextStyle;
	String _Text;

	void Uncheck();
	void Check();

	static ATOM _ThisAtom;
	static void InitBase(HINSTANCE ins);
	static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

public:
	ToggleButton(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, String Text, bool Multiselect, StyleSheet Style, TextStyleSheet TextStyle, bool CheckState = false);
	ToggleButton(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE ins, HMENU ID, String Text, bool Multiselect, StyleSheet Style, TextStyleSheet TextStyle, bool CheckState = false) : ToggleButton(XCoord, YCoord, Width, Height, *Parent, ins, ID, Text, Multiselect, Style, TextStyle, CheckState) {}

	bool CheckState() const { return _Checked; }
	void CheckState(bool New);
	StyleSheet const& Style = _Style;
	TextStyleSheet const& TextStyle = _TextStyle;

	String Text() const { return _Text; }
	void Text(const String& New) { _Text = New; Redraw(); }

	LRESULT KeyDown(WPARAM ID) override;
	LRESULT Paint() override;
	LRESULT Click() override;
	LRESULT MouseMove();
	LRESULT MouseLeave();
};