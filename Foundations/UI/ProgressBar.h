#pragma once

#include "ControlBase.h"

#define APBS_COLORGREEN 1 //Draws with a green bar color
#define APBS_COLORRED 2 //Draws with a red bar color
#define APBS_COLORYELLOW 4 //Draws with a yellow bar color
#define APBS_LIGHTBK 8 //Draws with light Background
#define APBS_DARKBK 16 //Draws with dark Background
#define APBS_VERTICAL 32 //Draws the ProgressBar in the vertical plane.
#define APBS_INVERTDIRECTION 64 //Reverses the bar's direction
#define APBS_BORDER 128
#define APBS_STANDARD (APBS_COLORGREEN | APBS_DARKBK | APBS_BORDER) //Draws a standard ProgressBar (DarkBk, GreenBar)
#define APBS_STANDARDR (APBS_COLORRED | APBS_DARKBK | APBS_BORDER) //Draws a standard ProgresBar (DarkBk, RedBar)
#define APBS_STANDARDY (APBS_COLORYELLOW | APBS_DARKBK | APBS_BORDER) //Draws a standard ProgressBar (DarkBk, YellowBar)

#define APBM_SETPROGRESS 0x0401 //Tells the ProgressBar window to re-size its progress. It will repaint after. WPARAM = Target Value, LPARAM = Not Used.
#define APBM_GETPROGRESS 0x0402 //Retrive's the ProgressBar's current progress. WPARAM & LPARAM are not used.

constexpr const wchar_t ProgressBarClass[] = L"AaronProgressBar";

class ProgressBar : public ControlBase
{
private:
	static LRESULT __stdcall _ProgressBarProc(HWND, UINT, WPARAM, LPARAM);
public:
	ProgressBar() : ControlBase()
	{
	}

	//Creates a single ProgressBar with the default style. Set style to different value to change appearance.
	static ProgressBar Create(HWND Parent, int X, int Y, int Width, int Height, HINSTANCE Ins, DWORD Style = APBS_STANDARD);
	//Call at app entry. Used to setup the first WndClass for all later controls
	static void InitateBase(HINSTANCE Instance);

	//Returns the progress of the Bar.
	int Progress()
	{
		return (int)GetWindowLongPtrW(_Base, GWLP_USERDATA);
	}
	//Sets and 
	void Progress(int New)
	{
		SendMessageW(_Base, APBM_SETPROGRESS, (WPARAM)New, 0);
	}

	RECT LocSize()
	{
		RECT Return;
		GetWindowRect(_Base, &Return);
		return Return;
	}
	void LocSize(RECT New)
	{
		MoveWindow(_Base, New.top, New.left, New.right, New.bottom, TRUE);
	}

	DWORD Style()
	{
		DWORD Val = GetWindowLongW(_Base, GWL_STYLE);
		Val &= ~WS_VISIBLE;
		Val &= ~WS_CHILD;
	}
	void Style(DWORD New)
	{
		SetWindowLongW(_Base, GWL_STYLE, WS_CHILD | WS_VISIBLE | New);
	}
};