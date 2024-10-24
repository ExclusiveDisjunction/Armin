#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\ComponentViewer.h"
#include "..\..\UI\Tool\DatePicker.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;
	using namespace UI::Tool;

	EditTaskEditor::EditTaskEditor(Task* Target)
	{
		this->Target = Target;	
	}

	LRESULT __stdcall EditTaskEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		EditTaskEditor* This = reinterpret_cast<EditTaskEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProc(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void EditTaskEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);
		const int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_RightAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Due By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			YCoord += 10 + Height; // Space for requires assurance

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Instructions", BaseBk, TextStyle, false));

			TextStyle.Alignment = TA_LeftAlignment;
			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Title = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, Target->Title(), Style, TextStyle);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			int TempWidth = Width - (5 + ButtonSize);
			DueBy = new Label(XCoord, YCoord, TempWidth, Height, _Base, ins, Target->DueBy.ToString(DateStringFormat::ShortDate), BaseBk, TextStyle, false);
			DueByD = Target->DueBy;
			XCoord += 5 + TempWidth;

			DueBySelect = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)4, ins, Style, TextStyle);
			XCoord -= 5 + TempWidth;
			YCoord += 10 + Height;

			RequiresAssurance = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, Target->RequiresAssurance, L"Needs Assurance?", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			Height = WndRect.bottom - (10 + YCoord);
			Instructions = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, Target->Instructions, Style, TextStyle, true);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_LeftAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assigned To:", BaseBk, TextStyle, false));

			int PreXCoord = XCoord;
			XCoord += 10 + Width;
			int ButtonWidth = (WndRect.right - (10 + XCoord + 10)) / 2;

			ModifyAssigned = new Button(XCoord, YCoord, ButtonWidth, Height, L"Modify", _Base, (HMENU)5, ins, Style, TextStyle);
			XCoord += 10 + ButtonWidth;

			ViewAssigned = new Button(XCoord, YCoord, ButtonWidth, Height, L"View", _Base, (HMENU)6, ins, Style, TextStyle);

			XCoord = PreXCoord;
			YCoord += 10 + Height;

			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			Style.Background = Grey3;

			AssignedUsersScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			AssignedUsersView = new Grid(0, 0, 910, 32, AssignedUsersScroll, ins, Style);
			AssignedUsersScroll->SetViewer(AssignedUsersView);
		}		

		FillAssigned();
	}
	void EditTaskEditor::FillAssigned()
	{
		if (!Target)
			return;

		CloseControls(AssignedUsers);
		AssignedUsers = ComponentViewer::GenerateListRef(Target->AssignedTo, AssignedUsersView, NULL, _Multiselect, true, AssignedUsersScroll);
	}

	LRESULT EditTaskEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			DueByD = DatePicker::Execute(DueByD, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			this->DueBy->SetText(DueByD.ToString(DateStringFormat::ShortDate));
			break;
		}
		case 5:
		{
			Vector<Component*> PrevItems = ComponentViewer::RetriveFromList(AssignedUsers);

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_User;
			Criteria.Multiselect = true;
			Criteria.PreSelected = PrevItems;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			CloseControls(AssignedUsers);

			AssignedUsers = ComponentViewer::GenerateList(New, AssignedUsersView, NULL, _Multiselect, true, AssignedUsersScroll);
			break;
		}
		case 6:
		{
			Vector<Component*> Items = ComponentViewer::RetriveFromList(AssignedUsers);
			if (Items.Size == 0)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please select at least one user to view it.", L"Edit Task:", MB_OK | MB_ICONERROR);
				break;
			}

			for (uint i = 0; i < Items.Size; i++)
				EditorRegistry::OpenEditor(new Users::ViewUserEditor(dynamic_cast<User*>(Items[i])), nullptr);
			break;
		}
		}

		return 0;
	}
	LRESULT EditTaskEditor::KeyDown(WPARAM Key)
	{
		return 0;
	}
	LRESULT EditTaskEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);
		const int BaseYCoord = 110;
		
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			YCoord += 10 + Height; // Space for requires assurance

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Title->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			int TempWidth = Width - (5 + ButtonSize);
			DueBy->Move(XCoord, YCoord, TempWidth, Height);
			XCoord += 5 + TempWidth;

			DueBySelect->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord -= 5 + TempWidth;
			YCoord += 10 + Height;

			RequiresAssurance->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Height = WndRect.bottom - (10 + YCoord);
			Instructions->Move(XCoord, YCoord, Width, Height);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);

			int PreXCoord = XCoord;
			XCoord += 10 + Width;
			int ButtonWidth = (WndRect.right - (10 + XCoord + 10)) / 2;

			ModifyAssigned->Move(XCoord, YCoord, ButtonWidth, Height);
			XCoord += 10 + ButtonWidth;

			ViewAssigned->Move(XCoord, YCoord, ButtonWidth, Height);

			XCoord = PreXCoord;
			YCoord += 10 + Height;

			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			AssignedUsersScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			ComponentViewer::ReSizeList(AssignedUsers, AssignedUsersView, AssignedUsersScroll);
		}

		return 0;
	}

	bool EditTaskEditor::TestOnCondition(Vector<void*> Args) const
	{
		return Args.Size != 0 && Args[0] == static_cast<void*>(Target);
	}
	void EditTaskEditor::Reset()
	{
		Title->SetText(Target->Title());
		Instructions->SetText(Target->Instructions);
		DueBy->SetText(Target->DueBy.ToString(DateStringFormat::ShortDate));
		DueByD = Target->DueBy;
		RequiresAssurance->SetCheckState(Target->RequiresAssurance);

		FillAssigned();
	}
	bool EditTaskEditor::Apply(ArminSessionBase* File, bool ProptErrors)
	{
		String Title = this->Title->GetText();
		String Instructions = this->Instructions->GetText();
		DateTime TargetTime = DueByD;
		bool RequiresAssurance = this->RequiresAssurance->GetCheckState();
		Vector<Files::User*> AssignedTo;

		{
			for (uint i = 0; i < AssignedUsers.Size; i++)
			{
				Component* Item = AssignedUsers[i]->Target();
				Files::User* Conv = dynamic_cast<Files::User*>(Item);
				if (Conv)
					AssignedTo.Add(Conv);
			}
		}

		if (!Target)
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"There was an unexpected error. Please report this error.", L"Edit Task:", MB_OK | MB_ICONERROR);
			return false;
		}

		Target->Title(Title);
		Target->Instructions = Instructions;
		Target->DueBy = TargetTime;
		Target->AssignedTo = ComponentReference::Generate(AssignedTo);
		Target->RequiresAssurance = RequiresAssurance;

		HasEdit = true;
		EditorRegistry::ResetEditorOfType(EDT_ViewTask, { Target });
		EditorRegistry::ResetEditorOfType(EDT_Tasks | EDT_UserHomepage);
		return true;
	}
}