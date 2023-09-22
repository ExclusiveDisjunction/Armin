#include "DatePicker.h"

namespace Armin::UI::Tool
{
    DatePicker::DatePicker(const DateTime& Prev, HINSTANCE ins, bool HasDate) : _HasDate(HasDate)
    {
        Return = Prev;

        if (!_ThisAtom)
            InitBase(ins);

        _Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Date Picker", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 470, 250, NULL, NULL, ins, NULL);
        SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        
        ShowWindow(_Base, SW_NORMAL);
        UpdateWindow(_Base);

        LoadControls();
    }

    ATOM DatePicker::_ThisAtom = ATOM();
    void DatePicker::InitBase(HINSTANCE ins)
    {
        WNDCLASSW wn = { 0 };
        wn.lpfnWndProc = DatePicker::WndProc;
        wn.hInstance = ins;
        wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wn.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wn.lpszMenuName = NULL;
        wn.lpszClassName = L"DatePicker";
        wn.hIcon = ArminIcon;
        wn.cbClsExtra = 0;
        wn.cbWndExtra = sizeof(DatePicker*);
        wn.style = CS_HREDRAW | CS_VREDRAW;

        _ThisAtom = RegisterClassW(&wn);
    }

    LRESULT __stdcall DatePicker::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
    {
        DatePicker* Item = reinterpret_cast<DatePicker*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
        if (!Item)
            return DefWindowProcW(Window, Message, wp, lp);

        switch (Message)
        {
        case WM_PAINT:
            return Item->Paint();
        case WM_COMMAND:
            return Item->Command(wp, lp);
        case WM_DESTROY:
            return Item->Destroy();
        case WM_KEYDOWN:
            return Item->KeyDown(wp);
        default:
            return DefWindowProcW(Window, Message, wp, lp);
        }
    }

    void DatePicker::LoadControls()
    {
        if (_Loaded)
            return;

        _Loaded = true;

        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        {
            TextStyleSheet Style;
            Style.FontFamily = TitleFontName;
            Style.FontSize = 17;
            Style.Foreground = FontColor;

            int X = 10;
            int Y = 10;
            int Width = WndRect.right - 10 - X;
            int Height = 50;

            int Result = GetLastError();

            Title = new Label(X, Y, Width, Height, _Base, ins, L"Date Picker", Background1, Style, false);
            Result = GetLastError();
        }

        {
            int XCoord = 10;
            int YCoord = 110;
            int Width = 50;
            int Height = 30;

            int Width2 = 10;

            StyleSheet Style;
            Style.Background = Grey1;
            Style.BaseBackground = Background1;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 20;

            TextStyleSheet TextStyle;
            TextStyle.Foreground = FontColor;
            TextStyle.FontSize = 12;
            TextStyle.FontFamily = StandardFontName;
            TextStyle.Alignment = TA_CenterAlignment;

            MM = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(static_cast<int>(Return.Month())), Style, TextStyle, false, false, 2);
            XCoord += 3 + Width;
            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"/", Background1, TextStyle, false));
            XCoord += 3 + Width2;

            DD = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(Return.Day()), Style, TextStyle, false, false, 2);
            XCoord += 3 + Width;
            MiscControls.Add(new Label(XCoord, YCoord, Width2, Height, _Base, ins, L"/", Background1, TextStyle, false));
            XCoord += 3 + Width2;

            YYYY = new TextBox(XCoord, YCoord, Width + 10, Height, _Base, ins, String(Return.Year()), Style, TextStyle, false, false, 4);
            XCoord += 25 + Width + 10;

            HH = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(Return.Hour() > 12 ? Return.Hour() - 12 : Return.Hour()), Style, TextStyle, false, false, 2);
            XCoord += 3 + Width;
            MiscControls.Add(new Label(XCoord, YCoord, Width2, Height, _Base, ins, L":", Background1, TextStyle, false));
            XCoord += 3 + Width2;

            MiMi = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(Return.Minute()), Style, TextStyle, false, false, 2);
            XCoord += 10 + Width;

            {
                int NWidth = 100;
                int NHeight = 30;
                int NXCoord = XCoord + 10;
                int NYCoord = YCoord - (NHeight - 6);

                IsAM = new CheckableButton(NXCoord, NYCoord, NWidth, NHeight, _Base, ins, NULL, Return.Hour() <= 12, L"AM", CBT_Radio, Style, TextStyle);
                NYCoord += NHeight + 5;

                IsPM = new CheckableButton(NXCoord, NYCoord, NWidth, NHeight, _Base, ins, NULL, Return.Hour() > 12, L"PM", CBT_Radio, Style, TextStyle);
            }
        }

        {
            int Height = 30;
            int Width = 100;
            int XCoord = 10;
            int YCoord = WndRect.bottom - 10 - Height;

            StyleSheet Style;
            Style.Background = Background1;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 25;
            TextStyleSheet TextStyle;
            TextStyle.FontFamily = StandardFontName;
            TextStyle.FontSize = 12;
            TextStyle.Foreground = FontColor;

            Submit = new Button(XCoord, YCoord, Width, Height, L"Submit", _Base, (HMENU)1, ins, Style, TextStyle);
            XCoord += Width + 5;

            Cancel = new Button(XCoord, YCoord, Width, Height, L"Cancel", _Base, (HMENU)2, ins, Style, TextStyle);
            XCoord = WndRect.right - 10 - Width;

            Now = new Button(XCoord, YCoord, Width, Height, L"Now", _Base, (HMENU)3, ins, Style, TextStyle);
        }
    }

    bool DatePicker::Process()
    {
        if (HasDate->GetCheckState())
        {
            int Month, Day, Year, Hour, Minute;
            bool AMPM = IsAM->GetCheckState();

            Month = MM->GetText().ToInt();
            Day = DD->GetText().ToInt();
            Year = YYYY->GetText().ToInt();
            Hour = HH->GetText().ToInt();
            Minute = MiMi->GetText().ToInt();

            if (Month < 1 || Month > 12)
            {
                MessageBoxW(_Base, L"The month must be between 1 and 12.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }
            if (Day < 1)
            {
                MessageBoxW(_Base, L"The day must be greater than 1.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }

            if (Day > 30 && (Month == 4 || Month == 6 || Month == 9 || Month == 11))
            {
                MessageBoxW(_Base, L"In the months of April, June, September, and November, there are only 30 days.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }
            else if (Day > 31 && (Month == 1 || Month == 3 || Month == 5 || Month == 7 || Month == 8 || Month == 10 || Month == 12))
            {
                MessageBoxW(_Base, L"In the months of January, March, May, July, August, October, and December, there are only 31 days.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }
            else if (Day > 29 && Month == 2 && Year % 4 == 0)
            {
                MessageBoxW(_Base, L"In February, there are only 29 days in a leap year.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }
            else if (Day > 28 && Month == 2)
            {
                MessageBoxW(_Base, L"In February, there are only 28 days in a non-leap year.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }

            if (Hour > 12 || Hour < 1)
            {
                MessageBoxW(_Base, L"The hour must be between 1 and 12.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }
            if (Minute > 59 || Minute < 0)
            {
                MessageBoxW(_Base, L"The monute must be between 0 and 59.", L"Date Picker:", MB_OK | MB_ICONERROR);
                return false;
            }

            if (!AMPM)
                Hour += 12;

            Return = DateTime(Month, Day, Year, Hour, Minute, 0, 0);
        }
        else
            Return = DateTime();

        return true;
    }
    void DatePicker::MessageLoop()
    {
        MSG Msg;
        int Result;
        while ((Result = GetMessageW(&Msg, _Base, NULL, NULL)) != 0)
        {
            if (Result < 0)
                return;

            TranslateMessage(&Msg);
            DispatchMessageW(&Msg);
        }
    }
    LRESULT DatePicker::Paint()
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
    LRESULT DatePicker::Destroy()
    {
        return 0;
    }
    LRESULT DatePicker::Command(WPARAM wp, LPARAM lp)
    {
        switch (wp)
        {
        case 1: //Submit
            if (Process())
            {
                ProperReturn = true;
                DestroyWindow(_Base);
            }
            break;
        case 2: //Cancel
            DestroyWindow(_Base);
            break;
        case 3: //Now
            Return = DateTime::Now();

            ProperReturn = true;
            DestroyWindow(_Base);
            break;
        case 4:
        {
            bool IsChecked = lp > 0;

            MM->Enable(IsChecked);
            DD->Enable(IsChecked);
            YYYY->Enable(IsChecked);
            HH->Enable(IsChecked);
            MiMi->Enable(IsChecked);
            IsAM->Enable(IsChecked);
            IsPM->Enable(IsChecked);
            break;
        }
        }

        return 0;
    }
    LRESULT DatePicker::KeyDown(WPARAM key)
    {
        if (key == VK_RETURN)
            Command(1, 0);
        if (key == VK_ESCAPE)
            Command(2, 0);

        return 0;
    }

    DateTime DatePicker::Execute(const DateTime& Prev, HINSTANCE ins, bool HasDate)
    {
        DatePicker* Item = new DatePicker(Prev, ins, HasDate);
        Item->MessageLoop();

        DateTime Return = Item->Return;
        delete Item;

        return Return;
    }
}