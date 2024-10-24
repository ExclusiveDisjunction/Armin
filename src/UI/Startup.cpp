#include "Startup.h"

#include "..\Ins.h"
#include "..\UICommon.h"
#include "..\Files\SessionControl.h"
#include "..\resource.h"

#include <time.h>
#include <ObjIdl.h>
#include <Gdiplus.h>

#include "Color.h"

using namespace Gdiplus;

namespace Armin::UI
{	
	Startup::Startup(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		const int Width = 700;
		const int Height = 400;
		int X = (GetSystemMetrics(SM_CXSCREEN) / 2) - (Width / 2);
		int Y = (GetSystemMetrics(SM_CYSCREEN) / 2) - (Height / 2);

		_Base = CreateWindowExW(WS_EX_TOOLWINDOW, L"StartupClass", L"Startup", WS_VISIBLE | WS_POPUP, X, Y, Width, Height, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		SetTimer(_Base, 1, 3000, NULL);
	}

	LRESULT __stdcall Startup::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		Startup* This = reinterpret_cast<Startup*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return This->Paint();
		case WM_TIMER:
			return This->Timer(wp);
		case WM_DESTROY:
			return This->Destroy();
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}
	ATOM Startup::_ThisAtom = ATOM();
	void Startup::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.cbClsExtra = 0;
		wn.cbWndExtra = 0;
		wn.hInstance = ins;
		wn.hIcon = ArminIcon;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wn.lpszMenuName = 0;
		wn.lpszClassName = L"StartupClass";
		wn.lpfnWndProc = Startup::WndProc;
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}

	void Startup::Open()
	{
		MSG msg;
		int Result;
		while ((Result = GetMessage(&msg, _Base, 0, 0)) != 0 && _Base)
		{
			if (Result < -1)
				return;

			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	LRESULT Startup::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);
		Graphics* Graph = Graphics::FromHDC(Dc);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		Rect Area(WndRect.left, WndRect.top, WndRect.right, WndRect.bottom);

		Image* img;
		{
			HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
			HRSRC hResource = FindResourceW(ins, MAKEINTRESOURCE(Armin_StartupImage), L"PNG");

			DWORD ImageSize = SizeofResource(ins, hResource);
			HGLOBAL GlobalResource = LoadResource(ins, hResource);
			
			const void* pResourceData = LockResource(GlobalResource);
			HGLOBAL Buffer = GlobalAlloc(GMEM_MOVEABLE, ImageSize);
			if (Buffer)
			{
				void* BufferData = GlobalLock(Buffer);
				if (BufferData)
				{
					CopyMemory(BufferData, pResourceData, ImageSize);
					UnlockResource(GlobalResource);
					FreeResource(GlobalResource);

					IStream* pStream = NULL;
					if (CreateStreamOnHGlobal(BufferData, FALSE, &pStream) == S_OK)
					{
						img = Image::FromStream(pStream);
						pStream->Release();
						if (img->GetLastStatus() == Ok)
						{
							Graph->DrawImage(img, Area);
						}

						delete img;
						img = nullptr;
					}
					img = nullptr;
					GlobalUnlock(Buffer);
				}
				GlobalFree(Buffer);
				Buffer = nullptr;
			}
		}

		SolidBrush HeaderBrush(Color(0xFF, Accent1.R(), Accent1.G(), Accent1.B()));
		Rect HeaderArea(0, 23, 276, 111);
		Graph->FillRectangle(&HeaderBrush, HeaderArea);

		SolidBrush WhiteBrush = SolidBrush(0xFFFFFFFF);

		PointF TitleTextOrig(0.0f, 23.0f);
		FontFamily TitleFam(static_cast<const wchar_t*>(TitleFontName));
		Font TitleFont(&TitleFam, 50);
		Graph->DrawString(L"Armin", -1, &TitleFont, TitleTextOrig, &WhiteBrush);

		PointF VersTextOrig(0.0f, 99.0f);
		FontFamily StdFam(static_cast<const wchar_t*>(StandardFontName));
		Font StdFont(&StdFam, 20);
		Graph->DrawString(static_cast<const wchar_t*>(Version), -1, &StdFont, VersTextOrig, &WhiteBrush);

		String FooterText = L"Armin | Artwork By: Hollan Connor Sellars";
		Font FooterFont(&StdFam, 13);

		RectF FooterRect(0.0f, Area.Height - 30.0f, static_cast<float>(Area.Width), 30.0f);
		SolidBrush FooterBrush(Color(0xc5292929));
		Graph->FillRectangle(&FooterBrush, FooterRect);
		Graph->DrawString(static_cast<const wchar_t*>(FooterText), -1, &FooterFont, PointF(FooterRect.X, FooterRect.Y), &WhiteBrush);

		delete Graph;
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT Startup::Destroy()
	{
		_Base = nullptr;
		return 0;
	}
	LRESULT Startup::Timer(WPARAM wp)
	{
		KillTimer(_Base, wp);
		DestroyWindow(_Base);

		return 0;
	}
}