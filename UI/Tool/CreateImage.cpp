#include "CreateImage.h"

#include "Files\Stream.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>
using namespace std;

#include "Files\Stream.h"
#include "..\..\Common.h"
#include "..\..\UICommon.h"
#include "..\..\Editors\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\Components.h"

namespace Armin::UI::Tool
{
	using namespace Files;

	CreateImage::CreateImage()
	{
	}
	CreateImage::~CreateImage()
	{
		SetWindowLongPtr(_Base, GWLP_USERDATA, 0);
		DestroyWindow(_Base);
	}

	void CreateImage::Construct(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Create Image", WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 900, 200, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_SHOW);
		UpdateWindow(_Base);

		LoadControls();
	}

	ATOM CreateImage::_ThisAtom = ATOM();
	void CreateImage::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = CreateImage::WndProc;
		wn.hInstance = ins;
		wn.hCursor = ArminCursor;
		wn.hbrBackground = nullptr;
		wn.lpszMenuName = nullptr;
		wn.lpszClassName = L"CreateImageClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(CreateImage*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall CreateImage::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		CreateImage* This = reinterpret_cast<CreateImage*>(GetWindowLongPtrW(Window, GWLP_USERDATA));

		switch (Message)
		{
		case WM_PAINT:
			return This->Paint();
		case WM_SIZE:
			return This->Size();
		case WM_COMMAND:
			return This->Command(wp, lp);
		case WM_DESTROY:
			if (This->_Mondal)
				PostQuitMessage(0);
			return 0;
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void CreateImage::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		StyleSheet Style;
		Style.Background = Grey3;
		Style.BaseBackground = Background2;
		Style.BorderBrush = Accent4;
		Style.BorderThickness = 4;
		Style.Radius = 20;

		TextStyleSheet TextStyle;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.FontSize = 13;
		TextStyle.Foreground = FontColor;

		AaColor& BaseBk = Style.BaseBackground;

		{
			int XCoord = 20;
			int YCoord = 40;
			int Width = 140;
			int Height = 27;

			TextStyle.Alignment = TA_RightAlignment;

			Text = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Image Path:", BaseBk, TextStyle, false);
			XCoord += 5 + Width;

			int ButtonSize = Height;
			Width = WndRect.right - (10 + XCoord) - (5 + ButtonSize);
			TextStyle.Alignment = TA_LeftAlignment;

			FilePath = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			XCoord += 5 + Width;

			SelectPath = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)1, ins, Style, TextStyle);
			YCoord += 10 + Height;

			XCoord = 20;
			Width = 140;
			Height = 27;
			TextStyle.Alignment = TA_RightAlignment;

			TitleTxt = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;

			Title = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);

			XCoord = 20;
			YCoord += 20 + Height;
			Width = (WndRect.right - (XCoord * 2)) / 2 - 5;
			TextStyle.Bold = true;

			BeginImport = new Button(XCoord, YCoord, Width, Height, L"Import Image", _Base, (HMENU)2, ins, Style, TextStyle);
			XCoord += 10 + Width;

			Cancel = new Button(XCoord, YCoord, Width, Height, L"Cancel", _Base, (HMENU)3, ins, Style, TextStyle);
		}
	}

	LRESULT CreateImage::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background2);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT CreateImage::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{
			int XCoord = 20;
			int YCoord = 40;
			int Width = 140;
			int Height = 27;

			Text->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			int ButtonSize = Height;
			Width = WndRect.right - (10 + XCoord) - (5 + ButtonSize);

			FilePath->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			SelectPath->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 10 + Height;

			XCoord = 20;
			Width = 140;
			Height = 27;

			TitleTxt ->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			Title->Move(XCoord, YCoord, Width, Height);

			XCoord = 20;
			YCoord += 20 + Height;
			Width = (WndRect.right - (XCoord * 2)) / 2 - 5;

			BeginImport->Move(XCoord, YCoord, Width, Height);
			XCoord += 10 + Width;

			Cancel->Move(XCoord, YCoord, Width, Height);
		}
		return 0;
	}
	LRESULT CreateImage::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1:
		{
			String Path = OpenFileEx(_Base, L"Image Files\0*.jpg;*.jpeg;*.png\0", L"");
			FilePath->SetText(Path);
			Title->SetText(FileName(Path));
			break;
		}
		case 2:
		{
			Image* Returned = GenerateImage();
			if (!Returned)
				break;

			_Created = Returned;
			DestroyWindow(_Base);
			return 0;
		}
		case 3:
			if (_Created)
				delete _Created;

			_Created = nullptr;
			DestroyWindow(_Base);
			break;
		}
		return 0;
	}

	Image* CreateImage::GenerateImage()
	{
		String Path = FilePath->GetText();
		if (!FileExists(Path))
		{
			MessageBoxW(_Base, L"Please proivde a valid path.", L"Create Image:", MB_OK | MB_ICONERROR);
			return nullptr;
		}

		ResourceSystem* System = dynamic_cast<ResourceSystem*>(LoadedProject);
		ImageList* Images = !System ? nullptr : System->Images;
		if (!System || !Images)
		{
			MessageBoxW(_Base, L"The current project does not support images or resources.\n\nPlease load a resource file and try again.", L"Create Image:", MB_OK | MB_ICONERROR);
			DestroyWindow(_Base);
			return nullptr;
		}

		AString ThisPath = !FileExists((String)System->ResourcePath) ? static_cast<AString>(::FilePath(LoadedProjectPath)) + "\\" + (AString)FileFullName(LoadedProjectPath) + ".arminresource" : System->ResourcePath;
		System->ResourcePath = ThisPath;

		ifstream Stream(Path, ios::binary | ios::ate);
		unsigned long long Size = Stream.tellg();
		char* Data = new char[Size];

		Stream.seekg(0, ios::beg);
		Stream.read(Data, Size);

		ofstream Out(ThisPath, ios::binary | ios::app | ios::ate);
		streampos Begin = Out.tellp();
		Out.write(Data, Size);
		streampos End = Out.tellp();

		Stream.close();
		Out.close();
		delete[] Data;

		Image* NewImage = new Image(LoadedProject, Images);
		NewImage->Title(Title->GetText());
		NewImage->Begin = Begin;
		NewImage->End = End;

		AppState |= APS_HasEdit;
		Editors::EditorRegistry::ResetEditorOfType(EDT_Images);
		return NewImage;
	}

	Image* CreateImage::Execute(HINSTANCE ins, bool Mondal)
	{
		CreateImage* This = new CreateImage();
		This->_Mondal = true;

		bool* Running = new bool(true);
		thread Thread = thread(RunMessageLoop, This, ins, Running);

		if (Mondal)
		{
			while (*Running)
				this_thread::sleep_for(chrono::milliseconds(100));

			Thread.detach();
			delete Running;
			Image* Return = This->_Created;
			delete This;

			return Return;
		}
		else
			return nullptr;
	}
	LRESULT CreateImage::RunMessageLoop(CreateImage* Object, HINSTANCE ins, bool* Running)
	{
		Object->Construct(ins);
		*Running = true;

		int Result;
		MSG msg;
		while ((Result = GetMessage(&msg, nullptr, 0, 0)) != 0)
		{
			if (Result < 0)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		*Running = false;
		if (Object->_Mondal)
			delete Running; //The execute function will not clean up this memory, so this function will.
		return msg.wParam;
	}
}