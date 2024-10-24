#include "..\EditorFrame.h"

#include "UI\StyleButton.h"
#include "..\..\Files\ArminSessions.h"

namespace Armin::Editors::Users
{
	using namespace Files;
	using namespace UI;

	UserSearchEditor::UserSearchEditor(UserSystem* System)
	{
		if (!System)
			_System = dynamic_cast<UserSystem*>(LoadedProject);
		else
			_System = System;
	}
	UserSearchEditor::~UserSearchEditor()
	{
		if (Objects)
			delete Objects;
	}

	LRESULT __stdcall UserSearchEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		UserSearchEditor* Item = reinterpret_cast<UserSearchEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);
		
		return EditorProc(Item, Window, Message, wp, lp);
	}

	void UserSearchEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = this->BaseYCoord;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = EditorGrey;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 4;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;

			{
				Width = (WndRect.right - (10 + XCoord + 10 + 10)) / 3;

				Search = new StyleButton(XCoord, YCoord, Width, Height, L"Run Search", _Base, (HMENU)4, ins, Style, TextStyle, RECT{ 0, 0, 0, 5 });
				XCoord += 10 + Width;

				SaveResult = new StyleButton(XCoord, YCoord, Width, Height, L"Save Result", _Base, (HMENU)5, ins, Style, TextStyle, RECT{ 0, 0, 0, 5 });
				XCoord += 10 + Width;

				DuplicateResult = new StyleButton(XCoord, YCoord, Width, Height, L"Duplicate Result", _Base, (HMENU)6, ins, Style, TextStyle, RECT{ 0, 0, 0, 5 });

				XCoord = 10;
				Width = 130;
				BaseYCoord = (YCoord += 10 + Height);
			}

			int ResetLabelY = 0;
			{
				TextStyle.Bold = true;
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Criteria:", BaseBk, TextStyle, false));

				ResetLabelY = (YCoord += 10 + Height);
				TextStyle.Alignment = TA_RightAlignment;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Username:", BaseBk, TextStyle, false));
				YCoord += 10 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"First Name:", BaseBk, TextStyle, false));
				YCoord += 10 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Last Name:", BaseBk, TextStyle, false));
				YCoord += 10 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"User Level:", BaseBk, TextStyle, false));
			
				YCoord = BaseYCoord;
				XCoord += 5 + Width;
				TextStyle.Bold = false;
				TextStyle.Alignment = TA_LeftAlignment;
			}

			{
				YCoord = ResetLabelY;
				Width = (WndRect.right / 2) - (10 + XCoord);

				Username = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
				YCoord += 10 + Height;

				FirstName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
				YCoord += 10 + Height;

				LastName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
				YCoord += 10 + Height;

				Admin = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, nullptr, true, L"Admins", CBT_CheckBox, Style, TextStyle);
				YCoord += 10 + Height; 
				
				Assurance = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, nullptr, true, L"Assurance", CBT_CheckBox, Style, TextStyle);
				YCoord += 10 + Height;

				Standard = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, nullptr, true, L"Standard", CBT_CheckBox, Style, TextStyle);
			}

			int ButtonSize = 40;
			{
				YCoord = BaseYCoord;
				XCoord = WndRect.right - (10 + ButtonSize);

				Height = Width = ButtonSize;

				View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
				YCoord += 5 + Height;

				Edit = new Button(XCoord, YCoord, Width, Height, L"ED", _Base, (HMENU)8, ins, Style, TextStyle);
				YCoord += 10 + Height;

				SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)9, ins, Style, TextStyle);
				YCoord += 5 + Height;

				DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DS", _Base, (HMENU)10, ins, Style, TextStyle);

				Width = 130;
				Height = 27;
			}

			{
				XCoord = (WndRect.right / 2) + 10;
				YCoord = BaseYCoord;
				Width = WndRect.right - (10 + XCoord + 10 + ButtonSize);
				Height = WndRect.bottom - (10 + YCoord);

				ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
				ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
				ObjectScroll->SetViewer(ObjectView);

				if (Objects)
					delete Objects;
				Objects = new ComponentViewerList(ObjectView, ObjectScroll);
			}
		}
	}
	void UserSearchEditor::RunSearch()
	{
		UserSet* Users = !_System ? nullptr : _System->Users;
		if (!Users || Users->Count == 0)
			return;

		StringList Usernames, FirstNames, LastNames;
		bool UT, FT, LT;
		{
			String Username = this->Username->GetText(), FirstName = this->FirstName->GetText(), LastName = this->LastName->GetText();

			UT = Username.Length() != 0;
			FT = FirstName.Length() != 0;
			LT = LastName.Length() != 0;

			Usernames = Username.Split(L',');
			FirstNames = FirstName.Split(L',');
			LastNames = LastName.Split(L',');

			for (String& Obj : Usernames)
			{
				Obj = Obj.ToLower();
				Obj = Obj.RemoveFromFront(L' ');
				Obj = Obj.RemoveFromEnd(L' ');
			}
			for (String& Obj : FirstNames)
			{
				Obj = Obj.ToLower();
				Obj = Obj.RemoveFromFront(L' ');
				Obj = Obj.RemoveFromEnd(L' ');
			}
			for (String& Obj : FirstNames)
			{
				Obj = Obj.ToLower();
				Obj = Obj.RemoveFromFront(L' ');
				Obj = Obj.RemoveFromEnd(L' ');
			}
		}

		bool Admins = Admin->GetCheckState(), Assurance = this->Assurance->GetCheckState(), Standard = this->Standard->GetCheckState();
		
		Vector<User*> Revised;
		for (uint i = 0; i < Users->Count; i++)
		{
			User* User = Users->Item(i);

			bool Result = true;
			Result &= (User->IsAdmin && Admins) || (User->IsAssurance && Assurance) || (Standard && (!User->IsAdmin && !User->IsAssurance));
			Result &= !UT ? true : Usernames.Contains(User->Username.ToLower());
			Result &= !FT ? true : FirstNames.Contains(User->FirstName.ToLower());
			Result &= !LT ? true : LastNames.Contains(User->LastName.ToLower());

			if (Result)
				Revised.Add(User);
		}

		Objects->GenerateList(Revised, nullptr, true, true);
	}

	LRESULT UserSearchEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = this->BaseYCoord;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			{
				Width = (WndRect.right - (10 + XCoord + 10 + 10)) / 3;

				Search->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				SaveResult->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				DuplicateResult->Move(XCoord, YCoord, Width, Height);

				XCoord = 10;
				Width = 130;
				BaseYCoord = (YCoord += 10 + Height);
			}

			int ResetLabelY = 0;
			{
				MiscControls[0]->Move(XCoord, YCoord, Width, Height);

				ResetLabelY = (YCoord += 10 + Height);

				MiscControls[1]->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				MiscControls[2]->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				MiscControls[3]->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				MiscControls[4]->Move(XCoord, YCoord, Width, Height);

				YCoord = BaseYCoord;
				XCoord += 5 + Width;
			}

			{
				YCoord = ResetLabelY;
				Width = (WndRect.right / 2) - (10 + XCoord);

				Username->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				FirstName->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				LastName->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				Admin->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				Assurance->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				Standard->Move(XCoord, YCoord, Width, Height);
			}

			int ButtonSize = 40;
			{
				YCoord = BaseYCoord;
				XCoord = WndRect.right - (10 + ButtonSize);

				Height = Width = ButtonSize;

				View->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				Edit->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				SelectAll->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				DeSelectAll->Move(XCoord, YCoord, Width, Height);

				Width = 130;
				Height = 27;
			}

			{
				XCoord = (WndRect.right / 2) + 10;
				YCoord = BaseYCoord;
				Width = WndRect.right - (10 + XCoord + 10 + ButtonSize);
				Height = WndRect.bottom - (10 + YCoord);

				ObjectScroll->Move(XCoord, YCoord, Width, Height);
				Objects->ReSizeList();
			}
		}

		return 0;
	}
	LRESULT UserSearchEditor::KeyDown(WPARAM wp)
	{
		bool Shift = GetKeyState(VK_SHIFT) & 0x8000;
		bool Control = GetKeyState(VK_CONTROL) & 0x8000;

		switch (wp)
		{
		case VK_RETURN:
			RunSearch();
			break;
		case 'V':
		case 'E':
			if (Shift && Control)
				Objects->OpenSelectedForEditView(wp == 'E');
			break;
		case 'D':
			if (Control)
				Command(6, 0);
			break;
		case 'R':
			if (Control && Shift)
				Command(5, 0);
			break;
		default:
			return SendMessage(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}

		return 0;
	}
	LRESULT UserSearchEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //RunSearch
			RunSearch();
			break;
		case 5: //Save Result
			Objects->SaveSelectedAsGroup();
			break;
		case 6: //Duplicate Result
			Objects->PopoutObjects(L"User Search Result", reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			break;
		case 7: //View
		case 8: //Edit
			Objects->OpenSelectedForEditView(wp == 8);
			break;
		case 9: //SelectAll
		case 10: //DeSelectAll
			for (ComponentViewer* Obj = Objects->Item(0); Obj != nullptr; Obj = Obj->Next)
				Obj->CheckState(wp == 9);
			break;
		}

		return 0;
	}

	void UserSearchEditor::Reset()
	{
		Username->SetText(String());
		FirstName->SetText(String());
		LastName->SetText(String());
		Admin->SetCheckState(true);
		Assurance->SetCheckState(true);
		Standard->SetCheckState(true);

		Objects->Clear();
	}
}