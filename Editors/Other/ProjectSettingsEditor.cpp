#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"

namespace Armin::Editors::Misc
{
	using namespace Files;

	ProjectSettingsEditor::ProjectSettingsEditor(ProjectBase* File)
	{
		Target = !File ? LoadedProject: File;
	}

	LRESULT __stdcall ProjectSettingsEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ProjectSettingsEditor* This = reinterpret_cast<ProjectSettingsEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ProjectSettingsEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 170;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = true;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;
			TextStyle.FontSize = 13;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Passwords & Security:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			XCoord += 20;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Bold = false;

			bool AdminCondition = dynamic_cast<UserSystem*>(Target) == nullptr ? true : (AppState & APS_HasAdminUser);

			ControlBase* Temp;
			MiscControls.Add(Temp = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)4, Target->RequiresPassword(), L"Requires Password", CBT_CheckBox, Style, TextStyle));
			EnableWindow(*Temp, AdminCondition);
			YCoord += 10 + Height;
			
			MiscControls.Add(Temp = new Label(XCoord, YCoord, 130, Height, _Base, ins, L"Password:", BaseBk, TextStyle, false));			
			XCoord += 135;

			MiscControls.Add(Temp = new TextBox(XCoord, YCoord, WndRect.right - (XCoord + 10), Height, _Base, ins, Target->Password(), Style, TextStyle, false, true));
			EnableWindow(*Temp, Target->RequiresPassword() && AdminCondition);
			XCoord -= 135;
			YCoord += 10 + Height;

			XCoord = 10;
			Width = 130;
		}
	}

	LRESULT ProjectSettingsEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);
		int BaseYCoord = 110;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 170;
			int Height = 27;
			
			int i = 0;

			MiscControls[i++]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			XCoord += 20;
			Width = WndRect.right - (10 + XCoord);

			MiscControls[i++]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[i++]->Move(XCoord, YCoord, 130, Height);
			XCoord += 135;
			MiscControls[i++]->Move(XCoord, YCoord, WndRect.right - (XCoord + 10), Height);
			XCoord -= 135;
			YCoord += 10 + Height;

			XCoord = 10;
			Width = 130;
		}

		return 0;
	}
	LRESULT ProjectSettingsEditor::KeyDown(WPARAM wp)
	{
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
	}
	LRESULT ProjectSettingsEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			bool New = (bool)lp;

			EnableWindow(*MiscControls[3], New);
			break;
		}
		}
		return 0;
	}

	bool ProjectSettingsEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		File = Target;

		bool RequiresPassword = dynamic_cast<CheckableButton*>(MiscControls[1])->GetCheckState();
		String Password = dynamic_cast<TextBox*>(MiscControls[3])->GetText();
		if (Password.Contains(L"~") || (RequiresPassword && Password == L""))
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please ensure that the password contains no tildie ('~') and that if a password is required, provide a password.", L"Project Settings:", MB_OK | MB_ICONERROR);
			return false;
		}

		File->RequiresPassword(RequiresPassword);
		File->Password(Password);

		AppState |= APS_HasEdit;
		return true;
	}
	bool ProjectSettingsEditor::EquatableTo(EditorFrame* Other) const
	{
		ProjectSettingsEditor* Conv = dynamic_cast<ProjectSettingsEditor*>(Other);
		return Conv != nullptr && Conv->Target == Target;
	}
	void ProjectSettingsEditor::Reset()
	{
		if (!_Loaded)
			return;

		dynamic_cast<CheckableButton*>(MiscControls[1])->SetCheckState(Target->RequiresPassword());
		dynamic_cast<TextBox*>(MiscControls[3])->SetText(Target->Password());

		bool AdminCondition = dynamic_cast<UserSystem*>(Target) == nullptr ? true : (AppState & APS_HasAdminUser);
		EnableWindow(*MiscControls[3], Target->RequiresPassword() && AdminCondition);
		EnableWindow(*MiscControls[1], AdminCondition);
	}
}