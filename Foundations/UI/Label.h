#pragma once

#include "Win32 Common.h"
#include "ControlBase.h"

class Label : public ControlBase
{
private:
	String Text;
	TextStyleSheet _Style;
	AaColor _Bk;
	StringList _Lines;
	bool ShowScroll;
	int TextPos;

	void Reset();

	SCROLLINFO si = { 0 };

	int HSUnit; //Horizontal Scroll Unit
	int VSUnit; //Vertical Scroll Unit
	int HUnitMax; //Average uppercase character height

	int NeedCWidth; //The Size of the Current text
	int ClientWidth;
	int ClientHeight;

	int XPos;
	int YPos;

	static ATOM _ThisAtom;
	static void InitBase(HINSTANCE ins);
	static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
public:
	Label(int X, int Y, int Width, int Height, HWND Window, HINSTANCE ins, String Text, AaColor BaseBk, TextStyleSheet Style, bool ShowScroll);
	Label(int X, int Y, int Width, int Height, ControlBase* Parent, HINSTANCE ins, String Text, AaColor BaseBk, TextStyleSheet Style, bool ShowScroll) : Label(X, Y, Width, Height, *Parent, ins, Text, BaseBk, Style, ShowScroll) {}
	~Label();

	void SetText(String New)
	{
		Text = New;
		_Lines = Text.Split(L'\n');
		Reset();
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
	}
	String GetText()
	{
		return Text;
	}
	TextStyleSheet GetTextStyle()
	{
		return _Style;
	}
	void SetTextStyle(TextStyleSheet New)
	{
		_Style = New;
		Reset();
		RedrawWindow(_Base, NULL, NULL, RDW_ERASENOW | RDW_INVALIDATE);
	}

	LRESULT Paint() override;
	LRESULT VScroll(WPARAM wp);
	LRESULT HScroll(WPARAM wp);
	LRESULT MouseScroll(WPARAM wp);
	LRESULT HMouseScroll(WPARAM wp);
	LRESULT Size() override;
	LRESULT KeyDown(WPARAM wp) override;
	LRESULT GotFocus() override;
};