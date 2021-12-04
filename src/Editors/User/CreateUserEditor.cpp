#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Users
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;

	CreateUserEditor::CreateUserEditor()
	{
		
	}

	LRESULT __stdcall CreateUserEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		CreateUserEditor* This = reinterpret_cast<CreateUserEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(This, Window, Message, wp, lp);
	}

	void CreateUserEditor::LoadControls()
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
			int Width = 120;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Alignment = TA_RightAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Username:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Password:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"First Name:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Middle Name:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Last Name:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"User Type:", BaseBk, TextStyle, false));

			XCoord += 5 + Width;
			YCoord = BaseYCoord;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Username = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			Password = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, false, true);
			YCoord += 10 + Height;

			FirstName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			MiddleName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			LastName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			UserType = new ComboBox(XCoord, YCoord, Width, Height, _Base, NULL, ins, { L"Standard", L"Assurance", L"Admin" }, Style, TextStyle);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Alignment = TA_RightAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Job Positions:", BaseBk, TextStyle, false));
			int ResetYCoord = (YCoord += Height + 5);

			int ButtonSize = 40;
			YCoord += ((WndRect.bottom - YCoord - 10) / 2) - (ButtonSize + 5);

			ModifyPositions = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"MP", _Base, (HMENU)4, ins, Style, TextStyle);
			YCoord += ButtonSize + 5;

			ViewPosition = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)5, ins, Style, TextStyle);

			YCoord = ResetYCoord;
			XCoord += 5 + ButtonSize;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			PositionsScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			PositionsView = new Grid(0, 0, 910, 37, PositionsScroll, ins, Style);
			PositionsScroll->SetViewer(PositionsView);
		}
	}

	LRESULT CreateUserEditor::Size()
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
			int Width = 120;
			int Height = 27;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[5]->Move(XCoord, YCoord, Width, Height);

			XCoord += 5 + Width;
			YCoord = BaseYCoord;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Username->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Password->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			FirstName->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiddleName->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			LastName->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			UserType->Move(XCoord, YCoord, Width, Height);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls[6]->Move(XCoord, YCoord, Width, Height);
			int ResetYCoord = (YCoord += Height + 5);

			int ButtonSize = 40;
			YCoord += ((WndRect.bottom - YCoord - 10) / 2) - (ButtonSize + 5);

			ModifyPositions->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += ButtonSize + 5;

			ViewPosition->Move(XCoord, YCoord, ButtonSize, ButtonSize);

			YCoord = ResetYCoord;
			XCoord += 5 + ButtonSize;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			PositionsScroll->Move(XCoord, YCoord, Width, Height);
			ComponentViewer::ReSizeList(Positions, PositionsView, PositionsScroll);
		}

		return 0;
	}
	LRESULT CreateUserEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT CreateUserEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			Vector<Component*> OldItems = ComponentViewer::GetAllComponents(Positions);

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_JobPosition;
			Criteria.Multiselect = true;
			Criteria.PreSelected = OldItems;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

			CloseControls(Positions);
			Positions = ComponentViewer::GenerateList(New, PositionsView, NULL, true, true, PositionsScroll);
			break;
		}
		case 5:
			ComponentViewer::OpenSelectedForEditView(Positions, false);
			break;
		}
		return 0;
	}

	void CreateUserEditor::Reset()
	{
		Username->SetText(String());
		Password->SetText(String());
		FirstName->SetText(String());
		MiddleName->SetText(String());
		LastName->SetText(String());
		UserType->Seek(L"Standard");
		
		CloseControls(Positions);
		PositionsView->Move(0, 0, 910, 32);
		PositionsScroll->Reset();
	}
	bool CreateUserEditor::Apply(ArminSessionBase* File, bool PromptErrors)
	{
		if (!File)
			return false;

		UserSystem* UFile = dynamic_cast<UserSystem*>(File);
		UserSet* Users = !UFile ? nullptr : UFile->Users;
		if (!Users)
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The current file does not support user files.", L"Create User:", MB_OK);
			return false;
		}

		String Username = this->Username->GetText();
		String Password = this->Password->GetText();
		String FirstName = this->FirstName->GetText();
		String MiddleName = this->MiddleName->GetText();
		String LastName = this->LastName->GetText();
		bool IsAdmin = this->UserType->Current() == L"Admin";
		bool IsAssurance = this->UserType->Current() == L"Assurance";
		
		Vector<Component*> TempPositions = ComponentViewer::GetAllComponents(Positions);
		Vector<JobPosition*> Positions;
		for (uint i = 0; i < TempPositions.Size; i++)
		{
			JobPosition* Temp = dynamic_cast<JobPosition*>(TempPositions[i]);
			if (Temp)
				Positions.Add(Temp);
		}

		if (Username == L"" || Password == L"")
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please supply a Username and/or Password and try again.", L"Create User:", MB_OK | MB_ICONERROR);
			return false;
		}

		if (Username.Contains(L'~') || Password.Contains(L'~') || FirstName.Contains(L'~') || MiddleName.Contains(L'~') || LastName.Contains(L'~'))
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please ensure that there are no tilde (\"~\") in any fields.", L"Create User:", MB_OK | MB_ICONERROR);
			return false;
		}

		Files::User* New = new User(File, Users);
		New->Username = Username;
		New->Password = Password;
		New->FirstName = FirstName;
		New->MiddleName = MiddleName;
		New->LastName = LastName;
		New->IsAdmin = IsAdmin;
		New->IsAssurance = IsAssurance;
		New->Positions = ComponentReference::Generate(Positions);

		HasEdit = true;
		EditorRegistry::ResetEditorOfType(EDT_Users);
		return true;
	}
}