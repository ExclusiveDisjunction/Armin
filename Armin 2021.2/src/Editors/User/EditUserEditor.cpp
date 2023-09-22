#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Users
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;

	EditUserEditor::EditUserEditor(User* Target)
	{		
		Current = Target;		
	}

	LRESULT __stdcall EditUserEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		EditUserEditor* This = reinterpret_cast<EditUserEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void EditUserEditor::LoadControls()
	{
		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		const int BaseXCoord = 10;
		const int BaseYCoord = 110;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Bold = true;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.BaseBackground = EditorGrey;
			Style.Background = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 25;

			{
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Username:", EditorGrey, TextStyle, false));
				YCoord += 5 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Password:", EditorGrey, TextStyle, false));
				YCoord += 5 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"First Name:", EditorGrey, TextStyle, false));
				YCoord += 5 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Middle Name:", EditorGrey, TextStyle, false));
				YCoord += 5 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Last Name:", EditorGrey, TextStyle, false));
				YCoord += 5 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"User Type:", EditorGrey, TextStyle, false));

				YCoord = BaseYCoord;
				XCoord += 5 + Width;
			}

			TextStyle.Bold = false;
			TextStyle.Alignment = TA_LeftAlignment;
			Width = (WndRect.right / 2) - 10 - XCoord;

			{
				Username = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->Username, Style, TextStyle);
				YCoord += 5 + Height;

				Password = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->Password, Style, TextStyle, false, true);
				YCoord += 5 + Height;

				FirstName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->FirstName, Style, TextStyle);
				YCoord += 5 + Height;

				MiddleName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->MiddleName, Style, TextStyle);
				YCoord += 5 + Height;

				LastName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->LastName, Style, TextStyle);
				YCoord += 5 + Height;

				int OldWidth = Width;
				Width = (WndRect.right / 2) - 10 - XCoord;
				AaColor Old = Style.Background;
				Style.Background = Grey3;

				UserType = new ComboBox(XCoord, YCoord, Width, Height, _Base, NULL, ins, { L"Admin", L"Standard", L"Assurance" }, Style, TextStyle);
				UserType->Seek(!Current ? L"Standard" : (Current->IsAdmin ? L"Admin" : Current->IsAssurance ? L"Assurance" : L"Standard"));
			}
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;

			int ButtonSize = 40;

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

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Job Positions:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			YCoord += (WndRect.bottom - (YCoord + 10)) / 2 - ((ButtonSize + 5) * 2);

			ModifyPositions = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"MP", _Base, (HMENU)4, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			ViewPosition = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)5, ins, Style, TextStyle);

			XCoord += 5 + ButtonSize;
			YCoord = BaseYCoord + Height + 5;
			Height = WndRect.bottom - (10 + YCoord);
			Width = WndRect.right - (10 + XCoord);

			PositionScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			PositionView = new Grid(0, 0, 910, 32, PositionScroll, ins, Style);
			PositionScroll->SetViewer(PositionView);
		}

		{
			Positions = ComponentViewer::GenerateListRef(Current->Positions, PositionView, NULL, _Multiselect, true, PositionScroll);
		}
	}

	LRESULT EditUserEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Modify
		{
			ArminSessionBase* File = LoadedSession;
			if (!File || !Current)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"An unexpected error occured.", L"Error:", MB_OK | MB_ICONERROR);
				break;
			}

			Vector<Component*> Last = ComponentViewer::GetAllComponents(Positions);

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_JobPosition;
			Criteria.Multiselect = true;
			Criteria.PreSelected = Last;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			Vector<JobPosition*> Conv;
			for (uint i = 0; i < New.Size; i++)
			{
				JobPosition* Target = dynamic_cast<JobPosition*>(New[i]);
				if (Target)
					Conv.Add(Target);
			}

			CloseControls(Positions);
			Positions = ComponentViewer::GenerateList(Conv, PositionView, NULL, _Multiselect, true, PositionScroll);
			break;
		}
		case 6: //View
		{
			Vector<Component*> Selected = ComponentViewer::RetriveFromList(Positions);

			for (uint i = 0; i < Selected.Size; i++)
				EditorRegistry::OpenEditor(new Misc::BasicViewerEditor(Selected[i]), nullptr);
			break;
		}
		}

		return 0;
	}
	LRESULT EditUserEditor::KeyDown(WPARAM Key)
	{
		return SendMessageW(GetAncestor(_Base, GA_ROOT), WM_KEYDOWN, Key, 0);
	}
	LRESULT EditUserEditor::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (!_Loaded)
			return 0;

		MoveUpperButtons(WndRect);

		const int BaseXCoord = 10;
		const int BaseYCoord = 110;

		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			{
				MiscControls[0]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				MiscControls[1]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				MiscControls[2]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				MiscControls[3]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				MiscControls[4]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				MiscControls[5]->Move(XCoord, YCoord, Width, Height);

				YCoord = BaseYCoord;
				XCoord += 5 + Width;
			}

			Width = (WndRect.right / 2) - 10 - XCoord;

			{
				Username->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				Password->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				FirstName->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				MiddleName->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				LastName->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				int OldWidth = Width;
				Width = (WndRect.right / 2) - 10 - XCoord;

				UserType->Move(XCoord, YCoord, Width, Height);
			}
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;
			int ButtonSize = 40;

			MiscControls[6]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			YCoord += (((WndRect.bottom - (YCoord + 10))) / 2) - ((ButtonSize + 5) * 2);

			ModifyPositions->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			ViewPosition->Move(XCoord, YCoord, ButtonSize, ButtonSize);

			XCoord += 5 + ButtonSize;
			YCoord = BaseYCoord + Height + 5;
			Height = WndRect.bottom - (10 + YCoord);
			Width = WndRect.right - (10 + XCoord);

			PositionScroll->Move(XCoord, YCoord, Width, Height);
		}


		{
			ComponentViewer::ReSizeList(Positions, PositionView, PositionScroll);
		}
		return 0;
	}

	void EditUserEditor::Reset()
	{
		Username->SetText(Current->Username);
		Password->SetText(Current->Password);;
		FirstName->SetText(Current->FirstName);
		MiddleName->SetText(Current->MiddleName);
		LastName->SetText(Current->LastName);
		UserType->Seek(Current->IsAdmin ? L"Admin" : Current->IsAssurance ? L"Assurance" : L"Standard");

		CloseControls(Positions);
		Positions = ComponentViewer::GenerateListRef(Current->Positions, PositionView, NULL, _Multiselect, true, PositionScroll);
	}
	bool EditUserEditor::Apply(ArminSessionBase* File, bool PromptErrors)
	{
		String Username = this->Username->GetText();
		String Password = this->Password->GetText();
		String First = this->FirstName->GetText();
		String Last = this->LastName->GetText();
		String Middle = this->MiddleName->GetText();
		enum UserType Type = UserType->Current() == L"Admin" ? UT_Admin : UserType->Current() == L"Assurance" ? UT_Assurance : UT_Standard;
		Vector<JobPosition*> Positions;

		for (uint i = 0; i < this->Positions.Size; i++)
		{
			JobPosition* Temp = dynamic_cast<JobPosition*>(this->Positions[i]->Target());
			if (Temp)
				Positions.Add(Temp);
		}

		if (Username == L"" || Password == L"")
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please provide a Username and\\or a password.", L"Missing Data:", MB_OK | MB_ICONERROR);
			return 0;
		}

		if (!Current)
			return false;

		if (!UserRegistry::SignInForAdmin())
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"You do not have access to modify this user.", L"Error:", MB_OK | MB_ICONERROR);
			return false;
		}

		if (Current->ID == 1 && Type != UT_Admin)
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The Apex User cannot change the user type from an Admin.\n\nThe user type will be kept as an admin.", L"Edit User:", MB_OK | MB_ICONWARNING);
			Type = UT_Admin;
		}

		Current->Username = Username;
		Current->Password = Password;
		Current->FirstName = First;
		Current->MiddleName = Middle;
		Current->LastName = Last;
		Current->IsAdmin = Type == UT_Admin;
		Current->IsAssurance = Type == UT_Assurance;
		Current->Positions = ComponentReference::Generate(Positions);

		EditorRegistry::ResetEditorOfType(EDT_ViewUser);
		EditorRegistry::ResetEditorOfType(EDT_Users);

		HasEdit = true;
		return true;
	}
	bool EditUserEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Args.Size == 0)
			return false;

		return Args[0] == static_cast<void*>(Current);
	}
	bool EditUserEditor::EquatableTo(EditorFrame* Object) const
	{
		EditUserEditor* Item = dynamic_cast<EditUserEditor*>(Object);
		if (!Item)
			return false;

		return Item->Current == Current;
	}
}