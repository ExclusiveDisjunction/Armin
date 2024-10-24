#include "InventorySearch.h"

#include "..\..\Files\ArminSessions.h"
#include "..\..\Windows\Tool\Information\ComponentListRenderer.h"

namespace Armin::Windows::Search
{
    using namespace Files;
    using namespace Windows::Tool;

    InventorySearch::InventorySearch(HINSTANCE ins)
    {
        if (!_ThisAtom)
            InitBase(ins);

        _Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Inventory Search", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 900, 400, NULL, NULL, ins, NULL);
        SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

        ShowWindow(_Base, SW_NORMAL);
        UpdateWindow(_Base);

        LoadControls();
    }

    ATOM InventorySearch::_ThisAtom = ATOM();

    LRESULT __stdcall InventorySearch::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
    {
        InventorySearch* Search = reinterpret_cast<InventorySearch*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
        if (!Search)
            return DefWindowProcW(Window, Message, wp, lp);

        switch (Message)
        {
        case WM_PAINT:
            return Search->Paint();
        case WM_DESTROY:
        {
            LRESULT Return = Search->Destroy();
            delete Search;
            return Return;
        }
        case WM_COMMAND:
            return Search->Command(wp, lp);
        case WM_SIZE:
            return Search->Size();
        default:
            return DefWindowProcW(Window, Message, wp, lp);
        }
    }
    void InventorySearch::InitBase(HINSTANCE ins)
    {
        WNDCLASSW wn = { 0 };
        wn.lpfnWndProc = InventorySearch::WndProc;
        wn.hInstance = ins;
        wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
        wn.hbrBackground = NULL;
        wn.lpszMenuName = NULL;
        wn.lpszClassName = L"InventorySearchClass";
        wn.hIcon = ArminIcon;
        wn.cbClsExtra = 0;
        wn.cbWndExtra = sizeof(InventorySearch*);
        wn.style = CS_HREDRAW | CS_VREDRAW;

        _ThisAtom = RegisterClassW(&wn);
    }

    void InventorySearch::LoadControls()
    {
        if (_Loaded)
            return;

        _Loaded = true;

        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        AaColor BaseBk = Background2;

        {
            int XCoord = 20;
            int YCoord = 20;
            int Width = 120;
            int Height = 27;

            TextStyleSheet TextStyle;
            TextStyle.Alignment = TA_RightAlignment;
            TextStyle.FontSize = 13;
            TextStyle.FontFamily = StandardFontName;
            TextStyle.Foreground = FontColor;

            StyleSheet Style;
            Style.Background = Grey3;
            Style.BaseBackground = BaseBk;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 20;

            Width = 250;
            XCoord = (WndRect.right / 2) - (Width / 2);

            Run = new Button(XCoord, YCoord, Width, Height, L"Run Search", _Base, (HMENU)1, ins, Style, TextStyle);
            int ResetY = YCoord += 20 + Height;

            XCoord = 20;
            Width = 140;
            TextStyle.Bold = true;

            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
            YCoord += 10 + Height;

            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
            YCoord += 10 + Height;

            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));
            
            YCoord = ResetY;
            XCoord += 5 + Width;
            Width = WndRect.right - (10 + XCoord);

            SerialNumber = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
            YCoord += 10 + Height;

            Group = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
            YCoord += 10 + Height;

            Height = WndRect.bottom - (10 + YCoord);
            DescriptionContains = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, true);
        }

        InvalidateRect(_Base, &WndRect, true);
    }
    void InventorySearch::RunSearch()
    {
        Vector<Component*> Result;

        String RawSerial = this->SerialNumber->GetText();
        String RawGroup = this->Group->GetText();
        String Description = this->DescriptionContains->GetText();

        StringList Serial = RawSerial.Split(L',');
        for (uint i = 0; i < Serial.Size(); i++)
        {
            String& Current = Serial[i];
            Current = Current.RemoveFromFront(L' ');
            Current = Current.RemoveFromEnd(L' ');
        }

        StringList Group = RawGroup.Split(L',');
        for (uint i = 0; i < Group.Size(); i++)
        {
            String& Current = Group[i];
            Current = Current.RemoveFromFront(L' ');
            Current = Current.RemoveFromEnd(L' ');
        }

        InventoryFile* File = dynamic_cast<InventoryFile*>(LoadedSession);
        InventoryItemGroup* Inventory = !File ? nullptr : File->InventoryItems();
        if (!Inventory || !File)
            return;

        for (uint i = 0; i < Inventory->Count(); i++)
        {
            InventoryItem* Current = Inventory->Item(i);

            bool SerialCond = Serial.Size() == 1 && Serial[0] == L"" ? true : Serial.Contains(Current->SerialNumber);
            bool GroupCond = Group.Size() == 1 && Group[0] == L"" ? true : Group.Contains(Current->Group);
            bool DescriptionCond = Description == L"" ? true : Current->Description.Contains(Description);

            if (SerialCond && GroupCond && DescriptionCond)
                Result.Add(Current);
        }

        ComponentListRenderer* Window = new ComponentListRenderer(Result, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"Search Result");
    }

    LRESULT InventorySearch::Size()
    {
        if (!_Loaded)
            return 0;

        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        {
            int XCoord = 20;
            int YCoord = 20;
            int Width = 120;
            int Height = 27;

            Width = 250;
            XCoord = (WndRect.right / 2) - (Width / 2);

            Run->Move(XCoord, YCoord, Width, Height);
            int ResetY = YCoord += 20 + Height;

            XCoord = 20;
            Width = 140;

            MiscControls[0]->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            MiscControls[1]->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            MiscControls[2]->Move(XCoord, YCoord, Width, Height);

            YCoord = ResetY;
            XCoord += 5 + Width;
            Width = WndRect.right - (10 + XCoord);

            SerialNumber->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            Group->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            Height = WndRect.bottom - (10 + YCoord);
            DescriptionContains->Move(XCoord, YCoord, Width, Height);
        }

        return 0;
    }
    LRESULT InventorySearch::Paint()
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
    LRESULT InventorySearch::Command(WPARAM wp, LPARAM lp)
    {
        switch (wp)
        {
        case 1: //RunSearch
        {
            RunSearch();
            break;
        }
        }

        return 0;
    }
    LRESULT InventorySearch::Destroy()
    {
        return 0;
    }
}