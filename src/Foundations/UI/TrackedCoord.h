#pragma once

#include <Windows.h>

#include "ControlBase.h"
#include "..\Arithmetic.h"
#include "..\Container.h"

class TrackedCoord : public ControlBase
{
protected:
	Vector<Vector2d*> Modify;
	HWND _Base;
	bool _Track;
	COLORREF Border, Bk;

	static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
	static ATOM _ThisAtom;
	static void InitBase(HINSTANCE ins);

	LRESULT Paint() override;
	LRESULT Click() override;
	LRESULT MouseUp() override;
	LRESULT Timer(WPARAM wp);

	virtual void OnMove(Vector2d NewPlace) {}
public:
	TrackedCoord(Vector<Vector2d*> Bind, int Size, HWND Parent, COLORREF Border, COLORREF Bk, HINSTANCE ins);

	Vector<Vector2d*> Binding() const { return Modify; }
	void Binding(Vector<Vector2d*> New);
	void AddToBind(Vector2d* New);

	operator HWND() const { return _Base; }
};