#pragma once

#include "ControlBase.h"
#include <Windows.h>

class ImageViewer : public ControlBase
{
private:
	HGLOBAL _Target;

	static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
	static ATOM _ThisAtom;
	static void InitBase(HINSTANCE ins);
public:
	ImageViewer(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HGLOBAL _Target);
	ImageViewer(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE ins, HGLOBAL _Target);
	
	LRESULT Paint() override;
};