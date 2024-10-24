#include "ImageViewer.h"

#include <gdiplus.h>
using namespace Gdiplus;

ImageViewer::ImageViewer(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HGLOBAL Target)
{
	if (!_ThisAtom)
		InitBase(ins);

	_Target = Target;

	_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), nullptr, WS_CHILD | WS_VISIBLE, XCoord, YCoord, Width, Height, Parent, nullptr, ins, nullptr);
	SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}
ImageViewer::ImageViewer(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE ins, HGLOBAL Target) : ImageViewer(XCoord, YCoord, Width, Height, !Parent ? nullptr : *Parent, ins, Target) {}

LRESULT __stdcall ImageViewer::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
{
	ImageViewer* This = reinterpret_cast<ImageViewer*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
	if (!This)
		return DefWindowProcW(Window, Message, wp, lp);

	switch (Message)
	{
	case WM_PAINT:
		return This->Paint();
	default:
		return DefWindowProcW(Window, Message, wp, lp);
	}
}
ATOM ImageViewer::_ThisAtom = ATOM();
void ImageViewer::InitBase(HINSTANCE ins)
{
	WNDCLASSW wn = { 0 };
	wn.lpfnWndProc = ImageViewer::WndProc;
	wn.lpszClassName = L"ImageViewerClass";
	wn.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wn.hInstance = ins;
	wn.cbClsExtra = 0;
	wn.cbWndExtra = sizeof(ImageViewer*);
	wn.hbrBackground = nullptr;
	wn.hIcon = nullptr;
	wn.lpszMenuName = nullptr;
	wn.style = CS_HREDRAW | CS_VREDRAW;

	_ThisAtom = RegisterClassW(&wn);
}

LRESULT ImageViewer::Paint()
{
	PAINTSTRUCT ps;
	HDC Dc = BeginPaint(_Base, &ps);

	RECT RWndRect;
	GetClientRect(_Base, &RWndRect);

	Graphics* Graph = Graphics::FromHDC(Dc);
	Rect WndRect{ RWndRect.left, RWndRect.top, RWndRect.right, RWndRect.bottom };

	IStream* Stream = nullptr;
	if (CreateStreamOnHGlobal(_Target, false, &Stream) != S_OK)
	{
		delete Graph;
		ReleaseDC(_Base, Dc);
		return 0;
	}

	Gdiplus::Image* img = Bitmap::FromStream(Stream);
	Stream->Release();

	int Width = WndRect.Width;
	int Height = WndRect.Height;
	float RateX = (img->GetWidth() <= Width ? 1.0f : img->GetWidth() / (float)Width);
	float RateY = (img->GetHeight() <= Height ? 1.0f : img->GetHeight() / (float)Height);

	int ImgWidth = img->GetWidth(), ImgHeight = img->GetHeight();
	if (RateX != 1)
	{
		int Overlap = Width / (RateX - (int)RateX + 1.0f);
		ImgWidth -= Overlap;
	}
	if (RateY != 1)
	{
		int Overlap = Height / (RateY - (int)RateY + 1.0f);
		ImgHeight -= Overlap;
	}

	Graph->DrawImage(img, 10 + ((WndRect.Width / 2) - (ImgWidth / 2)), 110 + ((WndRect.Width / 2) - (ImgWidth / 2)), ImgWidth, ImgHeight);
	//auto Result = Graph->DrawImage(img, 10.0f, 110.0f, img->GetWidth() / RateX, img->GetHeight() / RateY);

	delete img;
	delete Graph;
	EndPaint(_Base, &ps);
	return 0;
}