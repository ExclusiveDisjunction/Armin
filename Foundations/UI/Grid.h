#pragma once

#include "Win32 Common.h"

class Grid : public ControlBase
{
private:
	static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
	static ATOM _ThisAtom;

	StyleSheet _Style;
public:
	Grid(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, StyleSheet Style);
	Grid(int X, int Y, int Width, int Height, ControlBase* Parent, HINSTANCE ins, StyleSheet Style) : Grid(X, Y, Width, Height, *Parent, ins, Style) {}

	StyleSheet GetStyle()
	{
		return _Style;
	}
	void SetStyle(StyleSheet Style)
	{
		_Style = Style;

		RedrawWindow(_Base, NULL, NULL, RDW_ERASENOW | RDW_INVALIDATE);
	}

	LRESULT Paint() override;

	static void InitBase(HINSTANCE ins);
};