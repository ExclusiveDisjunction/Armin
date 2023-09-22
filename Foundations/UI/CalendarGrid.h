#pragma once

#include "ControlBase.h"

class CalendarGrid : public ControlBase
{
private:
	int Date;
	String Memo;
	StyleSheet _Style;
	TextStyleSheet _TextStyle;
	bool _HasMouse;

	static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
	static ATOM _ThisAtom;
	static void InitBase(HINSTANCE ins);
public:
	CalendarGrid(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, int Day, String Memo, StyleSheet Style, TextStyleSheet TextStyle);
	CalendarGrid(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE ins, HMENU ID, int Day, String Memo, StyleSheet Style, TextStyleSheet TextStyle) : CalendarGrid(XCoord, YCoord, Width, Height, *Parent, ins, ID, Day, Memo, Style, TextStyle) {}

	LRESULT Paint() override;
	LRESULT Click() override;
	LRESULT MouseMove();
	LRESULT MouseLeave();

	int GetDate() const { return Date; }
	String GetMemo() const { return Memo; }
	void SetDate(int New)
	{
		Date = New;
		Redraw();
	}
	void SetMemo(String New)
	{
		Memo = New;
		Redraw();
	}
	void SetParams(int New, String NewMemo)
	{
		Date = New;
		Memo = NewMemo;
		Redraw();
	}
};