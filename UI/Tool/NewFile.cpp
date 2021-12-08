#include "NewFile.h"

#include <iostream>
#include <thread>

using namespace std;

namespace Armin::UI
{
	NewFile::NewFile() : _Base(nullptr), _Loaded(false)
	{

	}
	void NewFile::CreateBase(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowEx(0l, MAKEINTATOM(_ThisAtom), L"Create File:", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 700, 550, nullptr, nullptr, ins, nullptr);
		ShowWindow(_Base, SW_NORMAL);
		SetWindowLongPtr(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
		UpdateWindow(_Base);

		StyleSheet Style;
		Style.Background = Grey3;
		Style.BorderBrush = Accent4;
		Style.BorderThickness = 3;
		Style.Radius = 20;
		Style.BaseBackground = Background1;

		TextStyleSheet TextStyle;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.FontSize = 13;
		TextStyle.Foreground = FontColor;

		_Loaded = true;
		
		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 130;
			int Height = 27;


		}
	}

	LRESULT __stdcall NewFile::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		NewFile* This = reinterpret_cast<NewFile*>(GetWindowLongPtr(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProc(Window, Message, wp, lp);
		
		switch (Message)
		{
		case WM_PAINT:
			return This->Paint();
		case WM_SIZE:
			return This->Size();
		case WM_COMMAND:
			return This->Command(wp, lp);
		case WM_KEYDOWN:
			return This->KeyDown(wp);
		case WM_DESTROY:
		{
			SetWindowLongPtr(Window, GWLP_USERDATA, 0);
			delete This;
			return 0;
		}
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}
	ATOM NewFile::_ThisAtom = ATOM();
	void NewFile::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(NewFile*);
		wn.lpfnWndProc = WndProc;
		wn.lpszClassName = L"NewFileClass";
		wn.hCursor = ArminCursor;
		wn.hInstance = ins;
		wn.hbrBackground = nullptr;
		wn.hIcon = ArminIcon;
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}

	LRESULT NewFile::RunMessageLoop(NewFile* Object, HINSTANCE ins, bool* Running)
	{
		Object->CreateBase(ins);
		*Running = true;

		int Result;
		MSG msg;
		while ((Result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
		{
			if (Result < 0 || *Running == false)
				break;
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		*Running = false;
		return msg.wParam;
	}
	String NewFile::Execute(HINSTANCE ins)
	{
		NewFile* Obj = new NewFile();

		bool* Running = new bool(true);
		thread Thread = thread(RunMessageLoop, Obj, ins, Running);
		while (*Running)
			this_thread::sleep_for(chrono::milliseconds(100));

		Thread.detach();
		delete Running;
		String Return = Obj->Return;
		delete Obj;

		return Return;
	}
}