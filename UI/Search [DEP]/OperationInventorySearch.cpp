#include "OperationInventorySearch.h"

#include "..\..\Files\ArminSessions.h"
#include "..\..\Windows\Tool\Information\ComponentListRenderer.h"

namespace Armin::Windows::Search
{
	using namespace Files;
	using namespace Windows::Tool;

	OperationInventorySearch::OperationInventorySearch(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Operation Inventory Search", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 975, 400, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadControls();
	}

	ATOM OperationInventorySearch::_ThisAtom = ATOM();
	void OperationInventorySearch::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = OperationInventorySearch::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"OperationInventorySearchClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(OperationInventorySearch*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall OperationInventorySearch::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		OperationInventorySearch* This = reinterpret_cast<OperationInventorySearch*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return This->Paint();
		case WM_SIZE:
			return This->Size();
		case WM_DESTROY:
			return This->Destroy();
		case WM_COMMAND:
			return This->Command(wp, lp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void OperationInventorySearch::LoadControls()
	{
        if (_Loaded)
            return;

        _Loaded = true;

        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        {
            int XCoord = 10;
            int YCoord = 10;
            int Width = 200;
            int Height = 27;

            AaColor BaseBk = Grey3;

            StyleSheet Style;
            Style.Background = Grey1;
            Style.BaseBackground = BaseBk;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 20;

            TextStyleSheet TextStyle;
            TextStyle.Foreground = FontColor;
            TextStyle.FontSize = 13;
            TextStyle.Alignment = TA_RightAlignment;
            TextStyle.FontFamily = StandardFontName;

            Run = new Button(XCoord, YCoord, (WndRect.right - 10) - XCoord, Height, L"Run Search", _Base, (HMENU)1, ins, Style, TextStyle);
            YCoord += 10 + Height;

            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
            YCoord += 10 + Height;

            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
            YCoord += 10 + Height;

            {
                int CurrentX = 10;
                int CurrentWidth = (WndRect.right - 12 - 10 - CurrentX) / 5;

                WorkingOrder = new CheckableButton(CurrentX, YCoord, CurrentWidth, Height, _Base, ins, NULL, false, L"Working Order", CBT_CheckBox, Style, TextStyle);
                CurrentX += CurrentWidth + 5;

                PartialWorkingOrder = new CheckableButton(CurrentX, YCoord, CurrentWidth, Height, _Base, ins, NULL, false, L"Partially Working", CBT_CheckBox, Style, TextStyle);
                CurrentX += CurrentWidth + 5;

                Broken = new CheckableButton(CurrentX, YCoord, CurrentWidth, Height, _Base, ins, NULL, false, L"Broken", CBT_CheckBox, Style, TextStyle);
                CurrentX += CurrentWidth + 5;

                UnderRepair = new CheckableButton(CurrentX, YCoord, CurrentWidth, Height, _Base, ins, NULL, false, L"Under Repair", CBT_CheckBox, Style, TextStyle);
                CurrentX += CurrentWidth + 5;

                NotInPossession = new CheckableButton(CurrentX, YCoord, CurrentWidth, Height, _Base, ins, NULL, false, L"Not In Possession", CBT_CheckBox, Style, TextStyle);
            }

            YCoord += 10 + Height;
            MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description Contains:", BaseBk, TextStyle, false));

            XCoord += 10 + Width;
            YCoord -= (10 + Height) * 3;
            Width = (WndRect.right - 10) - XCoord;

            SerialNumber = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle);
            YCoord += 10 + Height;

            Group = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle);
            YCoord += (10 + Height) * 2;

            Height = (WndRect.bottom - 10) - YCoord;
            DescriptionContains = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, L"", Style, TextStyle, true);
        }
    }
    void OperationInventorySearch::RunSearch()
    {
        Vector<Component*> Result;

        String RawSerial = this->SerialNumber->GetText();
        String RawGroup = this->Group->GetText();
        String Description = this->DescriptionContains->GetText();

        bool WorkingOrder = this->WorkingOrder->GetCheckState(), PartialWorkingOrder = this->PartialWorkingOrder->GetCheckState(), Broken = this->Broken->GetCheckState(), UnderRepair = this->UnderRepair->GetCheckState(), NotInPossession = this->NotInPossession->GetCheckState();

        auto Test = [WorkingOrder, PartialWorkingOrder, Broken, UnderRepair, NotInPossession](OperationInventoryItem* Current) -> bool
        {
            if (WorkingOrder)
                return Current->CurrentState == MIIS_WorkingOrder;
            else if (PartialWorkingOrder)
                return Current->CurrentState == MIIS_PartialWorkingOrder;
            else if (Broken)
                return Current->CurrentState == MIIS_Broken;
            else if (UnderRepair)
                return Current->CurrentState == MIIS_UnderRepair;
            else if (NotInPossession)
                return Current->CurrentState == MIIS_NotInPossession;

            return false;
        };

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
        OperationInventoryItemGroup* Inventory = !File ? nullptr : File->OperationInventoryItems();
        if (!Inventory || !File)
            return;

        for (uint i = 0; i < Inventory->Count(); i++)
        {
            OperationInventoryItem* Current = Inventory->Item(i);

            bool SerialCond = Serial.Size() == 1 && Serial[0] == L"" ? true : Serial.Contains(Current->SerialNumber);
            bool GroupCond = Group.Size() == 1 && Group[0] == L"" ? true : Group.Contains(Current->Group);
            bool DescriptionCond = Description == L"" ? true : Current->Description.Contains(Description);
            bool TypeCond = Test(Current);

            if (SerialCond && GroupCond && DescriptionCond)
                Result.Add(Current);
        }

        ComponentListRenderer* Window = new ComponentListRenderer(Result, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"Search Result");
	}

	LRESULT OperationInventorySearch::Paint()
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
	LRESULT OperationInventorySearch::Size()
	{
        if (!_Loaded)
            return 0;

        RECT WndRect;
        GetClientRect(_Base, &WndRect);

        {
            int XCoord = 10;
            int YCoord = 10;
            int Width = 200;
            int Height = 27;

            Run->Move(XCoord, YCoord, (WndRect.right - 10) - XCoord, Height);
            YCoord += 10 + Height;

            MiscControls[0]->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            MiscControls[1]->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            {
                int CurrentX = 10;
                int CurrentWidth = (WndRect.right - 12 - 10 - CurrentX) / 5;

                WorkingOrder->Move(CurrentX, YCoord, CurrentWidth, Height);
                CurrentX += CurrentWidth + 5;

                PartialWorkingOrder->Move(CurrentX, YCoord, CurrentWidth, Height);
                CurrentX += CurrentWidth + 5;

                Broken->Move(CurrentX, YCoord, CurrentWidth, Height);
                CurrentX += CurrentWidth + 5;

                UnderRepair->Move(CurrentX, YCoord, CurrentWidth, Height);
                CurrentX += CurrentWidth + 5;

                NotInPossession->Move(CurrentX, YCoord, CurrentWidth, Height);
            }

            YCoord += 10 + Height;
            MiscControls[2]->Move(XCoord, YCoord, Width, Height);

            XCoord += 10 + Width;
            YCoord -= (10 + Height) * 3;
            Width = (WndRect.right - 10) - XCoord;

            SerialNumber->Move(XCoord, YCoord, Width, Height);
            YCoord += 10 + Height;

            Group->Move(XCoord, YCoord, Width, Height);
            YCoord += (10 + Height) * 2;

            Height = (WndRect.bottom - 10) - YCoord;
            DescriptionContains->Move(XCoord, YCoord, Width, Height);
        }
		return 0;
	}
	LRESULT OperationInventorySearch::Command(WPARAM wp, LPARAM lp)
	{
        switch (wp)
        {
        case 1:
            RunSearch();
            break;
        }
		return 0;
	}
}