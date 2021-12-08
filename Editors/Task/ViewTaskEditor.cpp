#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\ComponentViewer.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
    using namespace UI;

	ViewTaskEditor::ViewTaskEditor(Files::Task* Target, bool EditMode)
	{
        Source = Target;		
		_EditMode = EditMode;
	}

	LRESULT __stdcall ViewTaskEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ViewTaskEditor* This = reinterpret_cast<ViewTaskEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ViewTaskEditor::LoadControls()
	{
        if (_Loaded)
            return;

        HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
        RECT WndRect;
        GetClientRect(_Base, &WndRect);
        LoadUpperButtons(WndRect, ins);

        _Loaded = true;

        int BaseYCoord = 110;
        AaColor BaseBk = EditorGrey;

        {
            int XCoord = 10;
            int YCoord = BaseYCoord;
            int Width = (WndRect.right / 2) - (XCoord + 10);
            int Height = 27;

            TextStyleSheet TextStyle;
            TextStyle.FontFamily = StandardFontName;
            TextStyle.FontSize = 13;
            TextStyle.Foreground = FontColor;

            StyleSheet Style;
            Style.Background = Grey3;
            Style.BaseBackground = BaseBk;
            Style.BorderBrush = Accent4;
            Style.BorderThickness = 3;
            Style.Radius = 20;

            {
                CompleteTask = new Button(XCoord, YCoord, Width, Height, L"Complete Task", _Base, (HMENU)5, ins, Style, TextStyle);
                XCoord += 5 + Width;

                EnableWindow(*CompleteTask, _EditMode);
            }

            XCoord = 10;
            int ResetYCoord = YCoord += 10 + Height;
            Style.BorderBrush = Accent4;
            TextStyle.Bold = true;
            TextStyle.Alignment = TA_RightAlignment;

            Width = 150;

            {
                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
                YCoord += 10 + Height;

                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Due Date:", BaseBk, TextStyle, false));
                YCoord += 10 + Height;

                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Needs Assurance?", BaseBk, TextStyle, false));
                YCoord += 10 + Height;

                TextStyle.Alignment = TA_LeftAlignment;

                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assigned To:", BaseBk, TextStyle, false));

                XCoord += Width + 5;
                YCoord = ResetYCoord;
                Width = (WndRect.right / 2) - 10 - XCoord;
                Style.Background = Grey3;
                TextStyle.Bold = false;

                Title = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Source ? L"[ERROR]" : Source->Title(), BaseBk, TextStyle, false);
                YCoord += 10 + Height;

                DueBy = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Source ? L"[ERROR]" : Source->DueBy.ToString(DateStringFormat::ShortDate), BaseBk, TextStyle, false);
                YCoord += 10 + Height;

                RequiresAssurance = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Source ? L"[ERROR]" : (Source->RequiresAssurance ? L"Yes" : L"No"), BaseBk, TextStyle, false);
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
            YCoord = ResetYCoord;
            Width = 120;
            Height = 27;

            {
                MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Instructions:", BaseBk, TextStyle, false));
                YCoord += 10 + Height;
                int OldWidth = Width;
                int OldHeight = Height;

                Style.Background = Grey3;

                Width = WndRect.right - 10 - XCoord;
                Height = WndRect.bottom - 10 - YCoord;

                TextStyle.Bold = false;
                TextStyle.Alignment = TA_LeftAlignment;

                Instructions = new Label(XCoord, YCoord, Width, Height, _Base, ins, (Source ? Source->Instructions : L"[ERROR]"), Style.Background, TextStyle, true);
            }
        }
	}
	void ViewTaskEditor::FillAssigned()
	{
        if (!Source || !_Loaded)
            return;

        Vector<Files::User*> Assigned = ComponentReference::Convert<User>(Source->AssignedTo);
        CloseControls(AssignedObjects);

        AssignedObjects = ComponentViewer::GenerateList(Assigned, AssignedView, NULL, _DummySelect, true, AssignedScroll);
	}

	LRESULT ViewTaskEditor::Command(WPARAM wp, LPARAM lp)
	{
        switch (wp)
        {
        case 5:
            EditorRegistry::OpenEditor(new CompleteTaskEditor(Source), nullptr);
            EditorRegistry::CloseEditor(this, false);
            break;
        case 6:
        {
            if (!UserRegistry::CurrentUserType() == UT_Admin)
            {
                MessageBoxW(GetAncestor(_Base, GA_ROOT), L"You do not have the proper user privileges to preform this task.", L"Edit Task:", MB_OK | MB_ICONERROR);
                EnableWindow(*EditThis, false);
                break;
            }

            EditorRegistry::OpenEditor(new EditTaskEditor(Source), nullptr);
            EditorRegistry::CloseEditor(this, false);
        }
        }
        return 0;
	}
	LRESULT ViewTaskEditor::KeyDown(WPARAM wp)
	{
        return 0;
	}
	LRESULT ViewTaskEditor::Size()
	{
        if (!_Loaded)
            return 0;

        RECT WndRect;
        GetClientRect(_Base, &WndRect);
        MoveUpperButtons(WndRect);

        _Loaded = true;

        int BaseYCoord = 110;

        {
            int XCoord = 10;
            int YCoord = BaseYCoord;
            int Width = (WndRect.right / 2) - (XCoord + 10);
            int Height = 27;

            {
                CompleteTask->Move(XCoord, YCoord, Width, Height);
                XCoord += 5 + Width;
            }

            XCoord = 10;
            int ResetYCoord = YCoord += 10 + Height;
            Width = 150;

            {
                MiscControls[0]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                MiscControls[1]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                MiscControls[2]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;

                MiscControls[3]->Move(XCoord, YCoord, Width, Height);

                XCoord += Width + 5;
                YCoord = ResetYCoord;
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
                ComponentViewer::ReSizeList(AssignedObjects, AssignedView, AssignedScroll);
            }

            XCoord = (WndRect.right / 2) + 10;
            YCoord = BaseYCoord;
            Width = 120;
            Height = 27;

            {
                MiscControls[4]->Move(XCoord, YCoord, Width, Height);
                YCoord += 10 + Height;
                int OldWidth = Width;
                int OldHeight = Height;

                Width = WndRect.right - 10 - XCoord;
                Height = WndRect.bottom - 10 - YCoord;

                Instructions->Move(XCoord, YCoord, Width, Height);

                YCoord += Height + 10;
                Height = OldHeight;
                Width = OldWidth;

                XCoord -= Width + 5;
            }
        }

        return 0;
	}

	void ViewTaskEditor::Reset()
	{
        Title->SetText(Source->Title());
        Instructions->SetText(Source->Instructions);
        DueBy->SetText(Source->DueBy.ToString(DateStringFormat::ShortDate));
        RequiresAssurance->SetText(Source->RequiresAssurance ? L"Yes" : L"No");

        FillAssigned();
	}
    bool ViewTaskEditor::TestOnCondition(Vector<void*> Args) const
    {
        return Args.Size != 0 && Args[0] == static_cast<void*>(Source);
    }
    bool ViewTaskEditor::EquatableTo(EditorFrame* Other) const
    {
        ViewTaskEditor* Temp = dynamic_cast<ViewTaskEditor*>(Other);
        return Temp != nullptr && Temp->Source == Source;
    }
}