#include "..\EditorFrame.h"

#include "Sort.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"

namespace Armin::Editors::Users
{
	using namespace Files;
	using namespace UI;

	ViewUserEditor::ViewUserEditor(User* Target)
	{	
		Current = !Target ? UserRegistry::CurrentUser() : Target;
	}
	ViewUserEditor::~ViewUserEditor()
	{
		MiscControls.Clear();
		Positions.Clear();
	}

	LRESULT __stdcall ViewUserEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ViewUserEditor* This = reinterpret_cast<ViewUserEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ViewUserEditor::LoadControls()
	{
		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		int BaseXCoord = 10;
		int BaseYCoord = 110;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);

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

			int TWidth = (WndRect.right - 20 - XCoord - 10) / 2;

			Change = new Button(XCoord, YCoord, TWidth, Height, L"Change Information", _Base, (HMENU)4, ins, Style, TextStyle);
			EnableWindow(*Change, UserRegistry::CurrentUserType() == UT_Admin);
			XCoord += 10 + TWidth;

			//Timecards = new Button(XCoord, YCoord, TWidth, Height, L"Timecards", _Base, (HMENU)5, ins, Style, TextStyle);

			Homepage = new Button(XCoord, YCoord, TWidth, Height, L"Homepage", _Base, (HMENU)6, ins, Style, TextStyle);
			BaseYCoord = (YCoord += 10 + Height);
			XCoord = BaseXCoord;

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
				YCoord += 5 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Positions:", EditorGrey, TextStyle, false));

				YCoord = BaseYCoord;
				XCoord += 5 + Width;
			}

			TextStyle.Bold = false;
			TextStyle.Alignment = TA_LeftAlignment;
			Width = WndRect.right - 10 - XCoord;

			{
				Username = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->Username, EditorGrey, TextStyle, false);
				YCoord += 5 + Height;

				Password = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->Password.ReplaceAll(L'*'), EditorGrey, TextStyle, false);
				YCoord += 5 + Height;

				FirstName = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->FirstName, EditorGrey, TextStyle, false);
				YCoord += 5 + Height;

				MiddleName = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->MiddleName, EditorGrey, TextStyle, false);
				YCoord += 5 + Height;

				LastName = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : Current->LastName, EditorGrey, TextStyle, false);
				YCoord += 5 + Height;

				UserType = new Label(XCoord, YCoord, Width, Height, _Base, ins, !Current ? L"[ERROR]" : (Current->IsAdmin ? L"Admin" : Current->IsAssurance ? L"Assurance" : L"Standard"), EditorGrey, TextStyle, false);
				YCoord += 5 + Height;
			}

			Width = WndRect.right - 10 - XCoord;
			Height = WndRect.bottom - 10 - YCoord;

			PositionScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			PositionView = new Grid(0, 0, Width, Height, PositionScroll, ins, Style);
			PositionScroll->SetViewer(PositionView);

			if (Current != nullptr)
				Positions = ComponentViewer::GenerateListRef(Current->Positions, PositionView, NULL, true, true, PositionScroll);
		}
	}

	LRESULT ViewUserEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Change Info
			EditorRegistry::OpenEditor(new EditUserEditor(Current), nullptr);
			break;
		case 5:
			//EditorRegistry::OpenEditor(new TimecardsEditor(Current), nullptr);
			break;
		case 6:
			EditorRegistry::OpenEditor(new UserHomepageEditor(Current), nullptr);
			break;
		}
		return 0;
	}
	LRESULT ViewUserEditor::SpecialCommand(HMENU ID, uint Command, LPARAM Sender)
	{
		return 0;
	}
	LRESULT ViewUserEditor::KeyDown(WPARAM Key)
	{
		return SendMessageW(GetAncestor(_Base, GA_ROOT), WM_KEYDOWN, Key, 0);
	}
	LRESULT ViewUserEditor::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (!_Loaded)
			return 0;

		MoveUpperButtons(WndRect);

		int BaseXCoord = 10;
		int BaseYCoord = 110;

		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			int TWidth = (WndRect.right - 20 - XCoord - 10) / 2;

			Change->Move(XCoord, YCoord, TWidth, Height);
			XCoord += 10 + TWidth;

			//Timecards->Move(XCoord, YCoord, TWidth, Height);
			//XCoord += 10 + TWidth;

			Homepage->Move(XCoord, YCoord, TWidth, Height);
			BaseYCoord = (YCoord += 10 + Height);
			XCoord = BaseXCoord;

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
				YCoord += 5 + Height;

				MiscControls[6]->Move(XCoord, YCoord, Width, Height);

				YCoord = BaseYCoord;
				XCoord += 5 + Width;
			}

			Width = WndRect.right - 10 - XCoord;

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

				UserType->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;
			}

			Width = WndRect.right - 10 - XCoord;
			Height = WndRect.bottom - 10 - YCoord;

			PositionScroll->Move(XCoord, YCoord, Width, Height);
			ComponentViewer::ReSizeList(Positions, PositionView, PositionScroll);
		}

		return 0;
	}
	LRESULT ViewUserEditor::Destroy()
	{
		Current = nullptr;

		return 0;
	}

	void ViewUserEditor::Reset()
	{
		Username->SetText(Current->Username);
		Password->SetText(Current->Password.ReplaceAll(L'*'));
		FirstName->SetText(Current->FirstName);
		MiddleName->SetText(Current->MiddleName);
		LastName->SetText(Current->LastName);
		UserType->SetText(Current->IsAdmin ? L"Admin" : Current->IsAssurance ? L"Assurance" : L"Standard");

		{
			CloseControls(Positions);
			Positions = ComponentViewer::GenerateListRef(Current->Positions, PositionView, NULL, true, true, PositionScroll);
		}
	}
	bool ViewUserEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Args.Size == 0)
			return false;

		return Args[0] == static_cast<void*>(this->Current);
	}
	bool ViewUserEditor::EquatableTo(EditorFrame* Other) const
	{
		ViewUserEditor* Target = dynamic_cast<ViewUserEditor*>(Other);
		if (!Target)
			return false;

		return Target->Current == Current;
	}
}