#include "ViewTask.h"

#include "Components.h"
#include "EditTask.h"
#include "ViewTaskRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Config\Ins.h"
#include "..\..\Editors\EditorRegistry.h"

namespace Armin::Editors::Task
{
    using namespace Config;
    using namespace Files;

    ViewTask::ViewTask(Files::Task* Target, bool EditMode)
    {
        Source = Target;
        _EditMode = EditMode;

        HINSTANCE ins = Ins::Instance();

        if (!_ThisAtom)
            InitBase(ins);

        _Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"View Task", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 850, 600, NULL, NULL, ins, NULL);
        SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        ShowWindow(_Base, SW_NORMAL);
        UpdateWindow(_Base);
        ViewTaskRegistry::ViewTasks.Add(this);

        LoadControls();
    }
    ViewTask::~ViewTask()
    {
        SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);
        DestroyWindow(_Base);

        ViewTaskRegistry::ViewTasks.Remove(this);
        Destroy();
    }

    ATOM ViewTask::_ThisAtom = ATOM();
    void ViewTask::InitBase(HINSTANCE ins)
    {
        WNDCLASS wn = { 0 };
        wn.cbClsExtra = 0;
        wn.cbWndExtra = sizeof(ViewTask*);
        wn.hInstance = ins;
        wn.hIcon = ArminIcon;
        wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wn.hbrBackground = NULL;
        wn.lpszMenuName = NULL;
        wn.lpszClassName = L"ViewTaskClass";
        wn.lpfnWndProc = ViewTask::WndProc;
        wn.style = CS_HREDRAW | CS_VREDRAW;

        _ThisAtom = RegisterClassW(&wn);
    }
    LRESULT __stdcall ViewTask::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
    {
        ViewTask* Target = reinterpret_cast<ViewTask*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
        if (!Target)
            return DefWindowProcW(Window, Message, wp, lp);

        switch (Message)
        {
        case WM_PAINT:
            return Target->Paint();
        case WM_DESTROY:
        {
            LRESULT Return = Target->Destroy();

            delete Target;
            SetWindowLongPtrW(Window, GWLP_USERDATA, 0);
            return Return;
        }
        case WM_COMMAND:
            return Target->Command(wp, lp);
        case WM_SIZE:
            return Target->Size();
        }

        return DefWindowProcW(Window, Message, wp, lp);
    }

    void ViewTask::LoadControls()
    {
        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        _Loaded = true;

        {
            int Width = 140;
            int Height = 27;
            int XCoord = WndRect.right - 10 - ((Width * 3) + 10);
            int YCoord = 10;

            TextStyleSheet TextStyle;
            TextStyle.FontFamily = TitleFontName;
            TextStyle.FontSize = 15;
            TextStyle.Foreground = 0xFFFFFFFF;

            StyleSheet Style;
            Style.Background = Grey1;
            Style.BaseBackground = Grey3;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 20;

            MiscControls.Add(new Label(10, 10, Width, Height, _Base, ins, L"View Task:", Style.BaseBackground, TextStyle, false));

            TextStyle.FontFamily = StandardFontName;
            TextStyle.FontSize = 13;

            CompleteTask = new Button(XCoord, YCoord, Width, Height, L"Complete Task", _Base, (HMENU)1, ins, Style, TextStyle);
            XCoord += 10 + Width;

            EditThis = new Button(XCoord, YCoord, Width, Height, L"Edit Task", _Base, (HMENU)3, ins, Style, TextStyle);
            EnableWindow(*EditThis, UserRegistry::CurrentUserType() == UT_Admin);
            XCoord += 10 + Width;

            EnableWindow(*CompleteTask, _EditMode);
            EnableWindow(*EditThis, _EditMode);

            AaColor OldBorder = Style.BorderBrush;
            Style.BorderBrush = 0xFFD81717; //Close Red

            Close = new Button(XCoord, YCoord, Width, Height, L"Close", _Base, (HMENU)2, ins, Style, TextStyle);

            XCoord = 10;
            int BaseYCoord = (YCoord += 10 + Height);
            Style.BorderBrush = OldBorder;
            Style.BorderBrush = Accent4;
            TextStyle.Bold = true;
            TextStyle.Alignment = TA_RightAlignment;

            Width = 150;

            {
                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", Style.BaseBackground, TextStyle, false));
                YCoord += 10 + Height;

                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Due Date:", Style.BaseBackground, TextStyle, false));
                YCoord += 10 + Height;

                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Needs Assurance?", Style.BaseBackground, TextStyle, false));
                YCoord += 10 + Height;

                TextStyle.Alignment = TA_LeftAlignment;

                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assigned To:", Style.BaseBackground, TextStyle, false));

                XCoord += Width + 5;
                YCoord = BaseYCoord;
                Width = (WndRect.right / 2) - 10 - XCoord;
                Style.Background = Grey2;
                TextStyle.Bold = false;

                Title = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Source ? L"[ERROR]" : Source->Title(), Style.BaseBackground, TextStyle, false);
                YCoord += 10 + Height;

                DueBy = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Source ? L"[ERROR]" : Source->DueBy.ToString(), Style.BaseBackground, TextStyle, false);
                YCoord += 10 + Height;

                RequiresAssurance = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Source ? L"[ERROR]" : (Source->RequiresAssurance ? L"Yes" : L"No"), Style.BaseBackground, TextStyle, false);
                YCoord += (10 + Height) * 2;
                XCoord = 10;
                Width = (WndRect.right / 2) - (XCoord + 10);
                Height = WndRect.bottom - (10 + YCoord);

                AssignedScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
                AssignedView = new Grid(0, 0, 910, 32, AssignedScroll, ins, Style);
                AssignedScroll->SetViewer(AssignedView);

                FillAssigned();
            }

            XCoord = (WndRect.right / 2) + 10;
            YCoord = BaseYCoord;
            Width = 120;
            Height = 27;

            {
                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Instructions:", Style.BaseBackground, TextStyle, false));
                YCoord += 10 + Height;
                int OldWidth = Width;
                int OldHeight = Height;

                Style.Background = Grey2;

                Width = WndRect.right - 10 - XCoord;
                Height = WndRect.bottom - 10 - YCoord;

                TextStyle.Bold = false;
                TextStyle.Alignment = TA_LeftAlignment;

                Instructions = new Label(XCoord, YCoord, Width, Height, _Base, ins, (Source ? Source->Instructions : L"[ERROR]"), Style.Background, TextStyle, true);

                TextStyle.Bold = true;
                TextStyle.Alignment = TA_RightAlignment;

                YCoord += Height + 10;
                Height = OldHeight;
                Width = OldWidth;
                Style.Background = Grey3;

                XCoord -= Width + 5;
            }
        }
    }
    void ViewTask::FillAssigned()
    {
        if (!Source || !_Loaded)
            return;

        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
        Vector<Files::User*> Assigned = Source->AssignedTo;
        CloseControls(AssignedObjects);

        RECT WndRect;
        GetClientRect(AssignedView, &WndRect);

        int XCoord = 5;
        int YCoord = 5;
        int Width = 900;
        int Height = 27;

        TextStyleSheet TextStyle;
        TextStyle.FontSize = 13;
        TextStyle.Foreground = 0xFFFFFFFF;
        TextStyle.FontFamily = StandardFontName;

        StyleSheet Style;
        Style.Background = Grey2;
        Style.BaseBackground = Grey2;
        Style.BorderBrush = Accent4;

        AssignedObjects = ComponentViewer::GenerateList(Assigned, AssignedView, NULL, _DummySelect, true, Style, TextStyle, AssignedScroll);
    }

    LRESULT ViewTask::Paint()
    {
        PAINTSTRUCT ps;
        HDC Dc = BeginPaint(_Base, &ps);

        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        HBRUSH Bk = CreateSolidBrush(Grey3);
        FillRect(Dc, &WndRect, Bk);

        DeleteObject(Bk);
        EndPaint(_Base, &ps);
        return 0;
    }
    LRESULT ViewTask::Destroy()
    {
        return 0;
    }
    LRESULT ViewTask::Command(WPARAM wp, LPARAM lp)
    {
        switch (wp)
        {
        case 1:
            EditorRegistry::OpenEditorArgs(EDT_CompleteTask, { Source });
            DestroyWindow(_Base);
            break;
        case 2:
            DestroyWindow(_Base);
            break;
        case 3:
        {
            if (!UserRegistry::CurrentUserType() == UT_Admin)
            {
                MessageBoxW(_Base, L"You do not have the proper user privileges to preform this task.", L"Edit Task:", MB_OK | MB_ICONERROR);
                EnableWindow(*EditThis, false);
                break;
            }

            new EditTask(this->Source);
            DestroyWindow(_Base);
        }
        }
        return 0;
    }
    LRESULT ViewTask::Size()
    {
        if (!_Loaded)
            return 0;

        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        {
            int Width = 140;
            int Height = 27;
            int XCoord = WndRect.right - 10 - ((Width * 3) + 10);
            int YCoord = 10;

            MiscControls[0]->Move(10, 10, Width, Height);

            CompleteTask->Move(XCoord, YCoord, Width, Height);
            XCoord += 10 + Width;

            EditThis->Move(XCoord, YCoord, Width, Height);
            XCoord += 10 + Width;

            Close->Move(XCoord, YCoord, Width, Height);

            XCoord = 10;
            int BaseYCoord = (YCoord += 10 + Height);
            Width = 150;

            {
                MiscControls[1]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                MiscControls[2]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                MiscControls[3]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                MiscControls[4]->Move(XCoord, YCoord, Width, Height);

                XCoord += Width + 5;
                YCoord = BaseYCoord;
                Width = (WndRect.right / 2) - 10 - XCoord;

                Title->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                DueBy->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                RequiresAssurance->Move(XCoord, YCoord, Width, Height);
                YCoord += (10 + Height) * 2;
                XCoord = 10;
                Width = (WndRect.right / 2) - (XCoord + 10);

                Height = WndRect.bottom - (10 + YCoord);

                AssignedScroll->Move(XCoord, YCoord, Width, Height);
            }

            XCoord = (WndRect.right / 2) + 10;
            YCoord = BaseYCoord;
            Height = 27;

            {
                MiscControls[5]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;
                int OldWidth = Width;
                int OldHeight = Height;

                Width = WndRect.right - 10 - XCoord;
                Height = WndRect.bottom - 10 - YCoord;

                Instructions->Move(XCoord, YCoord, Width, Height);
            }
        }

        {
            ComponentViewer::ReSizeList(AssignedObjects, AssignedView, AssignedScroll);
        }

        return 0;
    }
}