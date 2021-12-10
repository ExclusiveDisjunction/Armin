#include "NewFile.h"

#include "UI\Label.h"
#include "..\..\Files\ArminSessions.h"

#include <iostream>
#include <thread>
#include <ShlObj.h>

using namespace std;

namespace Armin::UI
{
	using namespace Files;

	NewFile::NewFile() : _Base(nullptr), _Loaded(false)
	{

	}
	void NewFile::CreateBase(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowEx(0l, MAKEINTATOM(_ThisAtom), L"Create File:", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 550, nullptr, nullptr, ins, nullptr);
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

		AaColor BaseBk = Style.BaseBackground;
		int BaseYCoord = 10;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		_Loaded = true;
		
		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 130;
			int Height = 27;

			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Bold = true;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Project Name:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Place Project In:", BaseBk, TextStyle, false));

			BaseYCoord = YCoord;
			YCoord -= 10 + Height;
			XCoord += 10 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;
				
			Name = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			Width -= 5 + ButtonSize;
			Directory = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			XCoord += 5 + Width;
			SelectDirectory = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)1, ins, Style, TextStyle);
			XCoord -= 5 + ButtonSize;
			BaseYCoord = YCoord += 10 + Height;
			BaseYCoord += 30;
		}

		//Left Half
		{
			int XCoord = 15;
			int YCoord = BaseYCoord;
			int Width = (WndRect.right / 2) - (10 + XCoord);
			int Height = 27;

			InventorySys = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, nullptr, true, L"Inventory System", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			TaskSys = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)4, true, L"Task System", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			UserSys = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)5, true, L"User System", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			ResourceSys = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, nullptr, true, L"Resource System", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			Width = ((WndRect.right / 2) - 35) / 2; //35 is the sum of 10 from left bound, 15 from right bound, and 10 from spacing.

			StdProj = new Button(XCoord, YCoord, Width, Height, L"Standard Project", _Base, (HMENU)6, ins, Style, TextStyle);
			XCoord += 10 + Width;

			InvProj = new Button(XCoord, YCoord, Width, Height, L"Inventory Project", _Base, (HMENU)7, ins, Style, TextStyle);
			YCoord += 10 + Height;
			XCoord -= 10 + Width;
			XCoord += Width / 2;

			TaskProj = new Button(XCoord, YCoord, Width, Height, L"Team Project", _Base, (HMENU)8, ins, Style, TextStyle);
		}

		//Right Half
		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = WndRect.right - (15 + XCoord);
			int Height = 27;

			TextStyle.Bold = true;
			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Username:", BaseBk, TextStyle, false));
			TextStyle.Bold = false;
			YCoord += 10 + Height;

			Username = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 20 + Height;

			TextStyle.Bold = true;
			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Password:", BaseBk, TextStyle, false));
			TextStyle.Bold = false;
			YCoord += 10 + Height;

			Password = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, false, true);

			YCoord = WndRect.bottom - (15 + Height);
			XCoord = (WndRect.right / 2) + 10;
			Width = ((WndRect.right / 2) - 35) / 2; //35 is the sum of 10 from left bound, 15 from right bound, and 10 from spacing.

			Submit = new Button(XCoord, YCoord, Width, Height, L"Create Project", _Base, (HMENU)2, ins, Style, TextStyle);
			XCoord += 10 + Width;

			Style.BorderBrush = 0xFFFF0000;
			Cancel = new Button(XCoord, YCoord, Width, Height, L"Cancel", _Base, (HMENU)3, ins, Style, TextStyle);
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
			PostQuitMessage(0);
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
	
	LRESULT NewFile::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background1);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT NewFile::Size()
	{
		return 0;
	}
	LRESULT NewFile::Command(WPARAM wp, LPARAM lp)
	{
		auto EnableBttn = [](CheckableButton* UserSys, TextBox* Username, TextBox* Password) -> void
		{
			bool State = UserSys->GetCheckState();
			EnableWindow(*Username, State);
			EnableWindow(*Password, State);
		};

		switch (wp)
		{
		case 1: //Selete Directory
		{
			auto FolderCallback = [](HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData) -> int
			{
				if (uMsg == BFFM_INITIALIZED) {
					LPCTSTR path = reinterpret_cast<LPCTSTR>(lpData);
					::SendMessage(hwnd, BFFM_SETSELECTION, true, (LPARAM)path);
				}
				return 0;
			};

			HRESULT ThisResult = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
			if (ThisResult != S_OK)
				MessageBoxW(_Base, L"no ok", L"Other", MB_OK);

			TCHAR my_documents[MAX_PATH];
			HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

			BROWSEINFO binf = { 0 };

			binf.lParam = reinterpret_cast<LPARAM>(my_documents);
			binf.lpfn = FolderCallback;
			binf.ulFlags = BIF_USENEWUI;
			binf.hwndOwner = _Base;
			binf.lpszTitle = L"Please select your location for your new Project.";

			auto Result = SHBrowseForFolder(&binf);
			wchar_t* Text = new wchar_t[MAX_PATH];
			SHGetPathFromIDList(Result, Text);

			String NewText = Text;
			delete[] Text;

			if (NewText != L"")
			{
				NewText += L'\\';
				Directory->SetText(NewText);
			}

			break;
		}
		case 2: //Submit
		{
			bool Inv = InventorySys->GetCheckState(), User = UserSys->GetCheckState(), Task = TaskSys->GetCheckState(), Resource = ResourceSys->GetCheckState();

			if (Task && !User)
			{
				User = true;
				EnableWindow(*Username, true);
				EnableWindow(*Password, true);
			}

			if (User)
			{
				String Usern = Username->GetText();
				String Pass = Password->GetText();

				if (Usern == L"" || Pass == L"")
				{
					MessageBoxW(_Base, L"Please supply a Username and/or Password.", L"New File:", MB_OK | MB_ICONERROR);
					break;
				}

				if (Usern.Contains(L'~') || Pass.Contains(L'~'))
				{
					MessageBoxW(_Base, L"The Username and/or Password cannot contain a tilde ('~').\n\nPlease remove them and try again.", L"New File:", MB_OK | MB_ICONERROR);
					break;
				}

				_Username = Usern;
				_Password = Pass;
			}

			_Config = (Inv ? UPC_Inventory : 0) | (User ? UPC_Users : 0) | (Task ? UPC_Tasks : 0) | (Resource ? UPC_Resource : 0);
			String Dir = Directory->GetText();
			_Return = Dir + (Dir[Dir.Length() - 1] == L'\\' ? String() : L"\\") + Name->GetText() + L".arminuni";
		}
		case 3: //Cancel
			DestroyWindow(_Base);
			break;
		case 4: //TaskSys
		{
			if (TaskSys->GetCheckState())
				UserSys->SetCheckState(true);

			EnableBttn(UserSys, Username, Password);
			break;
		}
		case 5: //UserSys
		{
			if (!UserSys->GetCheckState())
				TaskSys->SetCheckState(false);

			EnableBttn(UserSys, Username, Password);
			break;
		}
		case 6: //Std Proj
		{
			UserSys->SetCheckState(true);
			TaskSys->SetCheckState(true);
			InventorySys->SetCheckState(true);
			ResourceSys->SetCheckState(true);

			EnableBttn(UserSys, Username, Password);
			break;
		}
		case 7: //Inv Proj
		{
			UserSys->SetCheckState(false);
			TaskSys->SetCheckState(false);
			InventorySys->SetCheckState(true);
			ResourceSys->SetCheckState(true);

			EnableBttn(UserSys, Username, Password);
			break;
		}
		case 8: //Team Proj
		{
			UserSys->SetCheckState(true);
			TaskSys->SetCheckState(true);
			InventorySys->SetCheckState(false);
			ResourceSys->SetCheckState(true);

			EnableBttn(UserSys, Username, Password);
			break;
		}
		}
		return 0;
	}
	LRESULT NewFile::KeyDown(WPARAM wp)
	{
		return 0;
	}

	LRESULT NewFile::RunMessageLoop(NewFile* Object, HINSTANCE ins, bool* Running)
	{
		Object->CreateBase(ins);
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
		return msg.wParam;
	}
	String NewFile::Execute(HINSTANCE ins, String& Username, String& Password, int& Config)
	{
		NewFile* Obj = new NewFile();

		bool* Running = new bool(true);
		thread Thread = thread(RunMessageLoop, Obj, ins, Running);
		while (*Running)
			this_thread::sleep_for(chrono::milliseconds(100));

		Thread.detach();
		delete Running;
		Username = Obj->_Username;
		Password = Obj->_Password;
		Config = Obj->_Config;
		String Return = String(Obj->Return);
		delete Obj;

		return Return;
	}
}