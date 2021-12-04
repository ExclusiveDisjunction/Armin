#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\Components.h"

#include <iostream>
#include <Windows.h>
#include <Gdiplus.h>
using namespace Gdiplus;
using namespace std;

namespace Armin::Editors::Resources
{
	ViewImageEditor::ViewImageEditor(Files::Image* Target) : _Target(nullptr)
	{
		HGLOBAL Global = Target->GetHandle();
		if (!Target || !Global)
			return;

		_Target = Global;
	}
	ViewImageEditor::~ViewImageEditor()
	{
		GlobalFree(_Target);
	}

	LRESULT __stdcall ViewImageEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ViewImageEditor* This = reinterpret_cast<ViewImageEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ViewImageEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		Img = new ImageViewer(10, 60, WndRect.right - 20, WndRect.bottom - 70, _Base, ins, _Target);
	}

	LRESULT ViewImageEditor::Size()
	{
		if (!_Loaded)
			return 0;
				RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		Img->Move(10, 60, WndRect.right - 20, WndRect.bottom - 70);

		return 0;
	}
	LRESULT ViewImageEditor::KeyDown(WPARAM Key)
	{
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
	}
	LRESULT ViewImageEditor::Command(WPARAM wp, LPARAM lp)
	{
		return 0;
	}

	bool ViewImageEditor::TestOnCondition(Vector<void*> Args) const
	{
		return Args.Size == 1 && Args[0] == this->_Target;
	}
	bool ViewImageEditor::EquatableTo(EditorFrame* Other) const
	{
		ViewImageEditor* Temp;
		return (Temp = dynamic_cast<ViewImageEditor*>(Other)) != nullptr && Temp->_Target == _Target;
	}

	void ViewImageEditor::Reset()
	{

	}
}