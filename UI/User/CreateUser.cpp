#include "CreateUser.h"

#include "..\Tool\SearchByName.h"
#include "..\..\UICommon.h"
#include "..\..\Config\Ins.h"
#include "..\..\Editors\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"

namespace Armin::UI::Users
{
	using namespace Config;
	using namespace Files;
	using namespace Editors;
	using namespace Search;

	CreateUser::CreateUser(bool AdminOnly, HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Admin = AdminOnly;

		const int Width = 380;
		const int Height = 550;
		int XCoord = GetSystemMetrics(SM_CXSCREEN) / 2 - (Width / 2);
		int YCoord = GetSystemMetrics(SM_CYSCREEN) / 2 - (Height / 2);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Create User", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, XCoord, YCoord, Width, Height, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadControls();
	}

	bool CreateUser::Execute(bool AdminOnly, HINSTANCE ins)
	{
		CreateUser* Item = new CreateUser(AdminOnly, ins);
		EnableWindow(*Item->UserType, !AdminOnly);
		Item->Open();

		bool Return = Item->Return;
		delete Item;

		return Return;
	}

	ATOM CreateUser::_ThisAtom = ATOM();
	void CreateUser::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = CreateUser::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"CreateUserClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(CreateUser*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall CreateUser::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		CreateUser* Item = reinterpret_cast<CreateUser*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_KEYDOWN:
			return Item->KeyDown(wp);
		case WM_COMMAND:
			return Item->Command(wp);
		case WM_DESTROY:
			return Item->Destroy();
		case WM_ERASEBKGND:
			return 1;
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void CreateUser::LoadControls()
	{
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{ //Text Boxes	

			int Height = 25;
			int Width = WndRect.right - 30;
			int XCoord = 15;
			int YCoord = 180;

			StyleSheet Style;
			Style.Background = Grey1;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 2;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;
			TextStyle.FontSize = 13;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Username:", Grey1, TextStyle, false));
			YCoord += 5 + Height;

			Username = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, false);
			YCoord = 260;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Password & Confirm:", Grey1, TextStyle, false));
			YCoord += 5 + Height;

			Password = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, false, true);
			YCoord += 10 + Height;

			PasswordConfirm = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, false, true);
			YCoord += 15 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"First, Middle, and Last Names:", Grey1, TextStyle, false));
			YCoord += 5 + Height;
			Width = (WndRect.right - (20 + XCoord)) / 3;

			First = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, false);
			XCoord += Width + 5;

			Middle = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, false);
			XCoord += Width + 5;

			Last = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, false);

			YCoord += 15 + Height;
			Width = WndRect.right - 30;
			XCoord = 15;
			Style.BaseBackground = Grey2;

			Vector<String> StrList = _Admin ? StringList(L"Admin") : StringList({ L"Standard", L"Assurance", L"Admin" });

			UserType = new ComboBox(XCoord, YCoord, Width, 30, _Base, NULL, ins, StrList, Style, TextStyle);

			Width = 90;
			Height = 30;
			XCoord = (WndRect.right / 2) - (Width + 10);
			YCoord = WndRect.bottom - (Height + 10);

			Style.Background = Grey1;
			Style.Radius = 20;
			Style.BorderThickness = 3;
			Style.BorderBrush = Accent4;

			this->CreateCmd = new Button(XCoord, YCoord, Width, Height, L"Create", _Base, (HMENU)1, ins, Style, TextStyle);
			XCoord += Width + 10;

			this->CancelCmd = new Button(XCoord, YCoord, Width, Height, L"Cancel", _Base, (HMENU)2, ins, Style, TextStyle);
		}
	}
	void CreateUser::Open()
	{
		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		MSG msg;
		int Result = 0;
		while ((Result = GetMessageW(&msg, NULL, NULL, NULL)) != 0 && _Base)
		{
			if (Result < 0)
				return;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	LRESULT CreateUser::Destroy()
	{
		_Base = nullptr;
		return LRESULT();
	}
	LRESULT CreateUser::Command(WPARAM ID)
	{
		switch (ID)
		{
		case 1:
		{
			File = LoadedProject;

			if (!Ins::IsLoaded() || !File)
			{
				MessageBoxW(_Base, L"A user cannot be created if there is no Project loaded.", L"Create User:", MB_OK | MB_ICONERROR);
				break;
			}
			UserSystem* UserParent = dynamic_cast<UserSystem*>(File);
			Users = UserParent->Users;

			String Username = this->Username->GetText();
			String Password = this->Password->GetText();
			String ConfirmPassword = this->PasswordConfirm->GetText();

			if (Users)
			{
				for (uint i = 0u; i < Users->Count; i++)
				{
					Files::User* Current = Users->operator[](i);
					if (Current->Username == Username)
					{
						MessageBoxW(_Base, L"There is already a User with the Username provided.\n\nPlease provide a unique Username.", L"Create Users:", MB_OK | MB_ICONERROR);
						break;
					}
				}
			}

			if (Password != ConfirmPassword)
			{
				MessageBoxW(_Base, L"The passwords do not match.", L"Create User:", MB_OK | MB_ICONERROR);
				break;
			}

			Files::User* New = new User(File, Users);
			New->Username = Username;
			New->Password = Password;
			New->FirstName = First->GetText();
			New->MiddleName = Middle->GetText();
			New->LastName = Last->GetText();

			String Type = UserType->Current();
			New->IsAdmin = Type == L"Admin";
			New->IsAssurance = Type == L"Assurance";

			if (!New->IsAdmin && UserParent->Positions && UserParent->Positions->Count)
			{
				MessageBoxW(_Base, L"Please select the Job Positions that you want associated with this user.", L"Create User:", MB_OK | MB_ICONINFORMATION);

				SearchCriteria Criteria;
				Criteria.AllowedTypes = CT_JobPosition;
				Criteria.Multiselect = true;
				Vector<Component*> Items = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

				Vector<JobPosition*> NewPositions;
				for (uint i = 0; i < Items.Size; i++)
				{
					JobPosition* Current = dynamic_cast<JobPosition*>(Items[i]);
					if (!Current)
						continue;

					NewPositions.Add(Current);
				}

				New->Positions = ComponentReference::Generate(NewPositions);
			}

			AppState |= APS_HasEdit;

			MessageBoxW(_Base, L"The user has been created successfully.", L"Create User:", MB_OK | MB_ICONINFORMATION);
			Return = true;
			DestroyWindow(_Base);

			EditorRegistry::ResetEditorOfType(EDT_Users);

			return 0;
		}
		case 2:
			DestroyWindow(_Base);
			return 0;
		}

		return 0;
	}
	LRESULT CreateUser::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Grey1);
		HPEN Border = CreatePen(PS_SOLID, 1, Grey1);
		SelectObject(Dc, Bk);
		SelectObject(Dc, Border);

		Rectangle(Dc, WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

		RECT AccentArea = { 0, 0, WndRect.right, 170 };
		{ //Gradient		
			int R1 = Accent1.R(), G1 = Accent1.G(), B1 = Accent1.B();
			int R2 = Accent3.R(), G2 = Accent3.G(), B2 = Accent3.B();

			for (int i = 0; i < AccentArea.right; i++)
			{
				int R, G, B;
				R = R1 + (i * (R2 - R1) / AccentArea.right);
				G = G1 + (i * (G2 - G1) / AccentArea.right);
				B = B1 + (i * (B2 - B1) / AccentArea.right);

				RECT Temp = { AccentArea.left + i, AccentArea.top, AccentArea.left + i + 1, AccentArea.bottom };
				HBRUSH Color = CreateSolidBrush(RGB(R, G, B));

				FillRect(Dc, &Temp, Color);
				DeleteObject(Color);
			}
		}

		HFONT Font = CreateFontW(-MulDiv(34, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, 0, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(TitleFontName));
		SelectObject(Dc, Font);
		SetTextColor(Dc, 0xFFFFFF);
		SetBkMode(Dc, TRANSPARENT);

		DeleteObject(Border);

		HPEN GoldPen = CreatePen(PS_SOLID, 4, Accent4);
		SelectObject(Dc, GoldPen);

		RECT TextArea = { 50, 39, WndRect.right - 100, 72 };
		DrawTextW(Dc, L"Create User", 12, &AccentArea, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

		POINT One = { TextArea.left, TextArea.bottom + TextArea.top }, Two = { TextArea.right + TextArea.left, TextArea.bottom + TextArea.top };
		MoveToEx(Dc, One.x, One.y, NULL);
		LineTo(Dc, Two.x, Two.y);

		DeleteObject(Bk);
		DeleteObject(GoldPen);
		DeleteObject(Font);

		EndPaint(_Base, &ps);
		return 0;
	}
}