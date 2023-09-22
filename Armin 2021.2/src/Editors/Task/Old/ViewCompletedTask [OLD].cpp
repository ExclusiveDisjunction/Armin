#include "ViewCompletedTask.h"

#include "ArminSessions.h"
#include "Components.h"
#include "ViewTask.h"
#include "ViewTaskRegistry.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Config\Ins.h"

namespace Armin::Editors::Task
{
	using namespace Config;
	using namespace Files;

	ViewCompletedTask::ViewCompletedTask(CompletedTask* Target)
	{
		_Target = Target;

		HINSTANCE ins = Ins::Instance();

		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"View Completed Task", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 850, 600, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		LoadControls();
		ViewTaskRegistry::ViewCompletedTasks.Add(this);

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);
	}
	ViewCompletedTask::~ViewCompletedTask()
	{
		SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);
		DestroyWindow(_Base);

		ViewTaskRegistry::ViewCompletedTasks.Remove(this);
		Destroy();
	}

	ATOM ViewCompletedTask::_ThisAtom = ATOM();
	void ViewCompletedTask::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(ViewCompletedTask*);
		wn.hInstance = ins;
		wn.hIcon = ArminIcon;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"ViewCompletedTaskClass";
		wn.lpfnWndProc = ViewCompletedTask::WndProc;
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall ViewCompletedTask::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ViewCompletedTask* Item = reinterpret_cast<ViewCompletedTask*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProc(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_DESTROY:
			return Item->Destroy();
		case WM_SIZE:
			return Item->Size();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_SPECIALCOMMAND:
			return Item->SpecialCommand(reinterpret_cast<HMENU>(HIWORD(wp)), static_cast<uint>(LOWORD(wp)), lp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void ViewCompletedTask::LoadControls()
	{
		if (_Loaded)
			return;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		AaColor BaseBk = Background1;
		int BaseYCoord = 10;

		MiscControls.Add(new Label(10, BaseYCoord, 180, 30, _Base, ins, L"View Completed Task:", BaseBk, { 0xFFFFFFFF, TitleFontName, 13, false, false, false }, false));
		BaseYCoord += 20 + 30;

		TextStyleSheet LabelStyle;
		TextStyleSheet TextStyle;
		StyleSheet Style;

		{
			LabelStyle.FontFamily = TextStyle.FontFamily = StandardFontName;
			LabelStyle.FontSize = TextStyle.FontSize = 13;
			LabelStyle.Foreground = TextStyle.Foreground = 0xFFFFFFFF;
			LabelStyle.Bold = true;
			TextStyle.Bold = false;

			TextStyle.Alignment = TA_LeftAlignment;
			LabelStyle.Alignment = TA_RightAlignment;

			Style.Background = Grey1;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;
		}

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Task:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Date Completed:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assured By:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			LabelStyle.Alignment = TA_LeftAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Completed By:", BaseBk, LabelStyle, false));
			LabelStyle.Alignment = TA_RightAlignment;
			YCoord += 10 + Height;

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			int ButtonSize = Height;

			Task = new ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, NULL, _Target->Tasks()->Item(0), _DummySelect, Style, TextStyle, false);
			YCoord += 10 + Height;

			DateCompleted = new Label(XCoord, YCoord, Width, Height, _Base, ins, _Target->DateCompleted.ToString(), BaseBk, TextStyle, false);
			YCoord += 10 + Height;

			AssuredBy = new ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, NULL, _Target->AssuredBy, _DummySelect, Style, TextStyle);

			YCoord += (10 + Height) * 2;
			XCoord = 10;
			Width = (WndRect.right / 2) - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			AaColor OldBackground = Style.Background;
			Style.Background = Grey2;

			CompletedByScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			CompletedByGrid = new Grid(0, 0, 910, 32, CompletedByScroll, ins, Style);

			Style.Background = OldBackground;
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			LabelStyle.Alignment = TA_LeftAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Reports:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			LabelStyle.Alignment = TA_RightAlignment;

			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			AaColor OldBackground = Style.Background;
			Style.Background = Grey3;
			Style.BaseBackground = Grey2;

			ReportsScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ReportsView = new Grid(0, 0, 910, 32, ReportsScroll, ins, Style);

			Style.Background = OldBackground;
		}

		{
			Style.BaseBackground = Grey2;
			Style.Background = Grey3;
			Vector<Files::User*>& Completed = _Target->CompletedBy;

			CompletedBy = ComponentViewer::GenerateList(Completed, CompletedByGrid, NULL, _DummySelect, true, Style, TextStyle, CompletedByScroll);
		}

		{
			Style.BaseBackground = Style.Background;
			Style.Background = Grey2;
			Vector<ObjectReport*> Reports = *_Target->ObjectReports();

			this->Reports = ComponentViewer::GenerateList(Reports, ReportsView, NULL, _DummySelect, true, Style, TextStyle, ReportsScroll);
		}
	}

	LRESULT ViewCompletedTask::Paint()
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
	LRESULT ViewCompletedTask::Command(WPARAM wp, LPARAM lp)
	{
		return 0;
	}
	LRESULT ViewCompletedTask::SpecialCommand(HMENU ID, uint Command, LPARAM Sender)
	{
		return 0;
	}
	LRESULT ViewCompletedTask::Destroy()
	{
		return 0;
	}
	LRESULT ViewCompletedTask::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseYCoord = 10;

		MiscControls[0]->Move(10, BaseYCoord, 180, 30);
		BaseYCoord += 20 + 30;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			int ButtonSize = Height;

			Task->Move(XCoord, YCoord, Width - (5 + ButtonSize), Height);
			YCoord += 10 + Height;

			DateCompleted->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			AssuredBy->Move(XCoord, YCoord, Width, Height);

			YCoord += (10 + Height) * 2;
			XCoord = 10;
			Width = (WndRect.right / 2) - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			CompletedByScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls[5]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ReportsScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			ComponentViewer::ReSizeList(CompletedBy, CompletedByGrid, CompletedByScroll);
			ComponentViewer::ReSizeList(Reports, ReportsView, ReportsScroll);

		}

		return 0;
	}
}