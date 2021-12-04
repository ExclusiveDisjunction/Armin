#include "SignIn.h"

#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Main.h"

namespace Armin::UI::Users
{
	using namespace Files;

	SignIn::SignIn(Files::User* Target, bool ForAssurance, bool ForAdmin, bool Lock, HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		this->Target = Target;
		this->ForAssurance = ForAssurance;
		this->ForAdmin = ForAdmin;
		this->Lock = Lock;

		const int Width = 380;
		const int Height = 500;
		int XCoord = GetSystemMetrics(SM_CXSCREEN) / 2 - (Width / 2);
		int YCoord = GetSystemMetrics(SM_CYSCREEN) / 2 - (Height / 2);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Sign In", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, XCoord, YCoord, Width, Height, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		LoadControls();

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);
	}

	LRESULT __stdcall SignIn::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		SignIn* Item = reinterpret_cast<SignIn*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_DESTROY:
			return Item->Destroy();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_ERASEBKGND:
			return 1;
		case WM_KEYDOWN:
			return Item->KeyDown(wp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}
	ATOM SignIn::_ThisAtom = ATOM();
	void SignIn::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = SignIn::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"SignInClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(SignIn*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}

	Files::User* SignIn::Execute(Files::User* Target, bool ForAssurance, bool ForAdmin, bool Lock, HINSTANCE ins)
	{
		SignIn* Temp = new SignIn(Target, ForAssurance, ForAdmin, Lock, ins);

		MSG msg;
		int Result;
		while ((Result = GetMessageW(&msg, Temp->_Base, 0, 0)) != 0)
		{
			if (Result < 0)
				break;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		Files::User* Return = Temp->Return;
		delete Temp;
		return Return;
	}

	void SignIn::LoadControls()
	{
		int BaseYCoord = 190;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		{
			int XCoord = 50;
			int YCoord = BaseYCoord;
			int Width = WndRect.right - (XCoord * 2);
			int Height = 30;

			AaColor BaseBk = Grey1;

			TextStyleSheet TextStyle;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;

			StyleSheet Style;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 2;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.Radius = 20;

			Labels.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Username:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Username = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, this->Target ? Target->Username : String(), Style, TextStyle);
			EnableWindow(*Username, this->Target == nullptr);
			YCoord += 10 + Height;

			Labels.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Password:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Password = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, false, true);
			YCoord += 10 + Height;

			XCoord = 100;
			Width = WndRect.right - (XCoord * 2);

			Style.Radius = 20;
			Style.BorderThickness = 3;

			Login = new Button(XCoord, YCoord, Width, Height, L"Login", _Base, (HMENU)1, ins, Style, TextStyle);
		}

		SetFocus(*Username);
	}
	void SignIn::TestCondition()
	{
		UserSystem* File = dynamic_cast<UserSystem*>(LoadedProject);
		UserSet* Users = !File ? nullptr : File->Users;
		if (!Users)
		{
			MessageBoxW(_Base, L"This type of project does not support Users.", L"Sign In:", MB_OK | MB_ICONERROR);
			ToClose = true;
			return;
		}

		String Username = this->Username->GetText();
		String Password = this->Password->GetText();

		if (Username == L"" || Password == L"")
			MessageBoxW(_Base, L"Please supply a Username or Password.", L"Sign In:", MB_OK | MB_ICONERROR);

		if (Target == nullptr)
		{
			for (uint i = 0; i < Users->Count; i++)
			{
				Files::User* Current = Users->Item(i);
				if (Current->Username == Username && Current->Password == Password && (ForAdmin && !ForAssurance ? Current->IsAdmin : true) && (ForAssurance && !ForAdmin ? (Current->IsAssurance || Current->IsAdmin) : true))
				{
					Return = Current;
					ToClose = true;
					return;
				}
			}

			this->Username->SetText(String());
			this->Password->SetText(String());
			ToClose = false;
			if (ForAssurance)
				MessageBoxW(_Base, L"The Username or Password was incorrect, or the user was not assurance or admin. Please try again.", L"Sign In:", MB_OK | MB_ICONERROR);
			else if (ForAdmin)
				MessageBoxW(_Base, L"The Username or Password was incorrect, or the user was not an admin. Please try again.", L"Sign In:", MB_OK | MB_ICONERROR);
			else
				MessageBoxW(_Base, L"The Username or Password was incorrect. Please try again.", L"Sign In:", MB_OK | MB_ICONERROR);

			SetFocus(*this->Username);
			return;
		}
		else
		{
			if (Lock)
			{
				if (Target->Password == Password)
				{
					Return = Target;
					ToClose = true;
					return;
				}
				else
				{
					MessageBoxW(_Base, L"The password provided was not correct.", L"Sign In:", MB_OK | MB_ICONERROR);
					return;
				}
			}
			else
			{
				if (Target->Password == Password)
				{
					Return = Target;
					ToClose = true;
					return;
				}
				else
				{
					MessageBoxW(_Base, L"The password provided was not correct.", L"Sign In:", MB_OK | MB_ICONERROR);
					ToClose = true;
					return;
				}

			}		
		}
	}

	LRESULT SignIn::Paint()
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
		DrawTextW(Dc, L"Sign In", 8, &AccentArea, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP);

		POINT One = { TextArea.left, TextArea.bottom + TextArea.top }, Two = { TextArea.right + TextArea.left, TextArea.bottom + TextArea.top };
		MoveToEx(Dc, One.x, One.y, NULL);
		LineTo(Dc, Two.x, Two.y);

		DeleteObject(Bk);
		DeleteObject(GoldPen);
		DeleteObject(Font);

		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT SignIn::Destroy()
	{
		if (!ToClose)
			Return = nullptr;

		return 0;
	}
	LRESULT SignIn::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1:
			TestCondition();
			if (ToClose)
				DestroyWindow(_Base);
			return 0;
		}
		return 0;
	}
	LRESULT SignIn::KeyDown(WPARAM wp)
	{
		if (wp == VK_RETURN)
			return Command(1, 0);
		else
			return 0;
	}
}