#include "..\EditorFrame.h"

#include "Sort.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"

namespace Armin::Editors::Users
{
    using namespace Files;
    using namespace UI;

    JobPositionsEditor::JobPositionsEditor(UserSystem* System)
    {
        if (!System)
            _System = dynamic_cast<UserSystem*>(LoadedProject);
        else
            _System = System;
    }

    LRESULT __stdcall JobPositionsEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
    {
        JobPositionsEditor* This = reinterpret_cast<JobPositionsEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
        if (!This)
            return DefWindowProcW(Window, Message, wp, lp);

        return EditorProc(This, Window, Message, wp, lp);
    }

    void JobPositionsEditor::LoadControls()
    {
        if (_Loaded)
            return;

        UserSystem* File = dynamic_cast<class UserSystem*>(LoadedProject);
        if (!File)
            return;

        JobPositionList* Positions = File->Positions;
        if (!Positions)
            return;

        _Loaded = true;

        RECT WndRect;
        GetClientRect(_Base, &WndRect);
        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

        int BaseYCoord = 110;
        AaColor BaseBk = EditorGrey;
        LoadUpperButtons(WndRect, ins);

        {
            int XCoord = 10;
            int YCoord = BaseYCoord;
            int Width = 130;
            int Height = 27;

            TextStyleSheet TextStyle;
            TextStyle.Foreground = FontColor;
            TextStyle.FontFamily = StandardFontName;
            TextStyle.FontSize = 13;
            TextStyle.Alignment = TA_RightAlignment;
            TextStyle.Bold = true;

            StyleSheet Style;
            Style.Background = Grey3;
            Style.BaseBackground = BaseBk;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 20;

            int ResetY = 0;

            {
                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Positions Count:", BaseBk, TextStyle, false));

                TextStyle.Bold = false;
                TextStyle.Alignment = TA_LeftAlignment;
                ObjectCount = new Label(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height, _Base, ins, L"0", BaseBk, TextStyle, false);

                YCoord += 10 + Height;
                Width = (WndRect.right - (10 + XCoord + 10)) / 2;
                XCoord += Width / 2;

                Search = new Button(XCoord, YCoord, Width, Height, L"Positions Search", _Base, (HMENU)4, ins, Style, TextStyle);
                XCoord -= Width / 2;
                ResetY = YCoord += 10 + Height;
            }

            int ButtonSize = 40;

            {
                XCoord = WndRect.right - (ButtonSize + 10);

                int OldWidth = Width, OldHeight = Height;
                Width = Height = ButtonSize;

                Add = new Button(XCoord, YCoord, Width, Height, L"+", _Base, (HMENU)5, ins, Style, TextStyle);
                EnableWindow(*Add, (AppState & APS_HasAdminUser));
                YCoord += 5 + ButtonSize;

                Remove = new Button(XCoord, YCoord, Width, Height, L"-", _Base, (HMENU)6, ins, Style, TextStyle);
                EnableWindow(*Remove, (AppState & APS_HasAdminUser));
                YCoord += 10 + ButtonSize;

                View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
                YCoord += 5 + ButtonSize;

                Edit = new Button(XCoord, YCoord, Width, Height, L"ED", _Base, (HMENU)8, ins, Style, TextStyle);
                EnableWindow(*Edit, (AppState & APS_HasAdminUser));
                YCoord += 10 + ButtonSize;

                SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)9, ins, Style, TextStyle);
                YCoord += 5 + ButtonSize;

                DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DA", _Base, (HMENU)10, ins, Style, TextStyle);

                Width = OldWidth;
                Height = OldHeight;
            }

            YCoord = ResetY;
            XCoord = 10;
            Width = WndRect.right - (10 + XCoord + ButtonSize + 10);
            Height = WndRect.bottom - (10 + YCoord);

            ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
            ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
            ObjectScroll->SetViewer(ObjectView);
            FillObjects();
        }
    }
    void JobPositionsEditor::FillObjects()
    {
        JobPositionList* List = !_System ? nullptr : _System->Positions;
        if (!List)
            return;

        Vector<JobPosition*> Sorted = *List;
        QuickSort(Sorted, [](JobPosition*& One, JobPosition*& Two) { return static_cast<wstring>(One->Title()) < static_cast<wstring>(Two->Title()); });

        CloseControls(Objects);
        Objects = ComponentViewer::GenerateList(Sorted, ObjectView, NULL, true, true, ObjectScroll);
        ObjectCount->SetText(Objects.Size);
    }

    void JobPositionsEditor::Reset()
    {
        FillObjects();
    }

    LRESULT JobPositionsEditor::Command(WPARAM wp, LPARAM lp)
    {
        switch (wp)
        {
        case 4: //Search
            EditorRegistry::OpenEditor(new Misc::QuickSearchEditor(CT_JobPosition), nullptr);
            break;
        case 5: //Add
            if ((AppState & APS_HasAdminUser))
                EditorRegistry::OpenEditor(new AddJobPositionEditor(), nullptr);
            break;
        case 6: //Remove
        {
            if ((AppState & APS_HasAdminUser))
            {
                MessageBoxW(GetAncestor(_Base, GA_ROOT), L"You do not have the proper user level to remove inventory items.", L"Remove:", MB_OK | MB_ICONERROR);
                break;
            }

            Vector<ComponentViewer*> Selected;
            Vector<Component*> RawTargets = ComponentViewer::RetriveFromList(Objects, Selected);

            if (RawTargets.Size == 0)
            {
                MessageBox(GetAncestor(_Base, GA_ROOT), L"Please select at least one Job Position for removing.", L"Job Positions:", MB_OK | MB_ICONERROR);
                break;
            }

            int Result = MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The selected objects will be removed permanantly.\n\nDo you want to continue?", L"Remove Positions:", MB_YESNOCANCEL | MB_ICONWARNING);
            if (Result != IDYES)
                break;

            for (uint i = 0; i < RawTargets.Size; i++)
            {
                JobPosition* Temp = dynamic_cast<JobPosition*>(RawTargets[i]);
                if (!Temp)
                    continue;

                delete Temp;
                ComponentViewer* Viewer = Selected[i];
                Objects.Remove(Viewer);
                delete Viewer;
            }

            AppState |= APS_HasEdit;
            ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
            break;
        }
        case 7: //View        
        case 8: //Edit
            ComponentViewer::OpenSelectedForEditView(Objects, wp == 8 && (AppState & APS_HasAdminUser));
        case 9: //Select All
            for (ComponentViewer* Obj : Objects)
                Obj->CheckState(true);
            break;
        case 10: //DeSelectAll
            for (ComponentViewer* Obj : Objects)
                Obj->CheckState(false);
            break;
        }
        return 0;
    }
    LRESULT JobPositionsEditor::KeyDown(WPARAM Key)
    {
        switch (Key)
        {
        case VK_RETURN:
            FillObjects();
            break;
        case VK_DELETE:
            Command(6, 0);
            break;
        case 'N':
            if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
                Command(5, 0);
            break;
        case 'V':
        case 'E':
            if (GetKeyState(VK_CONTROL) & 0x8000)
                ComponentViewer::OpenSelectedForEditView(Objects, Key == 'E' && (AppState & APS_HasAdminUser));
            break;
        default:
            return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
        }
        return LRESULT();
    }
    LRESULT JobPositionsEditor::Size()
    {
        if (!_Loaded)
            return 0;

        RECT WndRect;
        GetClientRect(_Base, &WndRect);
        int BaseYCoord = 110;

        MoveUpperButtons(WndRect);

        {
            int XCoord = 10;
            int YCoord = BaseYCoord;
            int Width = 130;
            int Height = 27;

            int ResetY = 0;

            {
                MiscControls[0]->Move(XCoord, YCoord, Width, Height);

                ObjectCount->Move(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height);

                YCoord += 10 + Height;
                Width = (WndRect.right - (10 + XCoord + 10)) / 2;
                XCoord += Width / 2;

                Search->Move(XCoord, YCoord, Width, Height);
                XCoord -= Width / 2;
                ResetY = YCoord += 10 + Height;
            }

            int ButtonSize = 40;

            {
                XCoord = WndRect.right - (ButtonSize + 10);

                int OldWidth = Width, OldHeight = Height;
                Width = Height = ButtonSize;

                Add->Move(XCoord, YCoord, Width, Height);
                YCoord += 5 + ButtonSize;

                Remove->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + ButtonSize;

                View->Move(XCoord, YCoord, Width, Height);
                YCoord += 5 + ButtonSize;

                Edit->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + ButtonSize;

                SelectAll->Move(XCoord, YCoord, Width, Height);
                YCoord += 5 + ButtonSize;

                DeSelectAll->Move(XCoord, YCoord, Width, Height);

                Width = OldWidth;
                Height = OldHeight;
            }

            YCoord = ResetY;
            XCoord = 10;
            Width = WndRect.right - (10 + XCoord + ButtonSize + 10);
            Height = WndRect.bottom - (10 + YCoord);

            ObjectScroll->Move(XCoord, YCoord, Width, Height);
            ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
        }

        return 0;
    }
}