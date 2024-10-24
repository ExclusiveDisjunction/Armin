#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Config\Ins.h"
#include "..\..\Config\Recent.h"
#include "..\..\Files\ArminSessions.h"

namespace Armin::Editors::Misc
{
	using namespace Config;
	using namespace Files;

	SettingsEditor::SettingsEditor()
	{

	}

	LRESULT __stdcall SettingsEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		SettingsEditor* This = reinterpret_cast<SettingsEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void SettingsEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = 110;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Files", BaseBk, TextStyle, false));
			XCoord += 20;
			YCoord += 10 + Height;
			TextStyle.Bold = false;

			LoadLast = new CheckableButton(XCoord, YCoord, WndRect.right - (10 + XCoord), Height, _Base, ins, (HMENU)4, InsInstance->LoadLast, L"Load Last Opened", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;
			
			MiscControls.Add(new Label(XCoord, YCoord, 130, Height, _Base, ins, L"Last Loaded:", BaseBk, TextStyle, false));
			LastLoaded = new TextBox(XCoord + 135, YCoord, WndRect.right - (10 + XCoord + 135), Height, _Base, ins, InsInstance->LastLoaded, Style, TextStyle);
			EnableWindow(*LastLoaded, false);
			
			YCoord += 20 + Height;
			XCoord -= 10;
			TextStyle.Bold = true;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Recents:", BaseBk, TextStyle, false));

			YCoord += 10 + Height;
			TextStyle.Bold = false;

			Width = (WndRect.right - (10 + XCoord)) / 3;
			XCoord = (WndRect.right / 2) - (Width / 2);
			
			Remove = new Button(XCoord, YCoord, Width, Height, L"Remove All", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 10 + Height;

			XCoord = 10;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			RecentScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			RecentView = new Grid(0, 0, 910, 32, RecentScroll, ins, Style);
			RecentScroll->SetViewer(RecentView);
		}

		{
			RECT ClientRect;
			GetClientRect(*RecentScroll, &ClientRect);

			int XCoord = 5;
			int YCoord = 5;
			int Width = max(ClientRect.right - (XCoord * 2), 900);
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			StringList Recents = RecentInstance->GetRecentsForUI();
			RecentView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Recents.Size + 1)));
			RecentScroll->Reset();
			
			for (uint i = 0; i < Recents.Size; i++, YCoord += 5 + Height)
				this->Recents.Add(new CheckableButton(XCoord, YCoord, Width, Height, RecentView, ins, NULL, false, Recents[i], CBT_CheckBox, Style, TextStyle));
		}
	}

	LRESULT SettingsEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = 110;
		int i = 0;

		{
			int XCoord = 10;
			int YCoord = 110;
			int Width = 130;
			int Height = 27;

			MiscControls[i++]->Move(XCoord, YCoord, Width, Height);
			XCoord += 20;
			YCoord += 10 + Height;

			LoadLast->Move(XCoord, YCoord, WndRect.right - (10 + XCoord), Height);
			YCoord += 10 + Height;

			MiscControls[i++]->Move(XCoord, YCoord, 130, Height);
			LastLoaded->Move(XCoord + 135, YCoord, WndRect.right - (10 + XCoord + 135), Height);

			YCoord += 20 + Height;
			XCoord -= 10;

			MiscControls[i++]->Move(XCoord, YCoord, Width, Height);

			YCoord += 10 + Height;

			Width = (WndRect.right - (10 + XCoord)) / 3;
			XCoord = (WndRect.right / 2) - (Width / 2);

			Remove->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			XCoord = 10;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			RecentScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			RECT ClientRect;
			GetClientRect(*RecentScroll, &ClientRect);

			int XCoord = 5;
			int YCoord = 5;
			int Width = max(ClientRect.right - (XCoord * 2), 900);
			int Height = 27;

			RecentView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Recents.Size + 1)));
			RecentScroll->Reset();

			for (uint i = 0; i < Recents.Size; i++, YCoord += 5 + Height)
				Recents[i]->Move(XCoord, YCoord, Width, Height);
		}
		return 0;
	}
	LRESULT SettingsEditor::KeyDown(WPARAM wp)
	{
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
	}
	LRESULT SettingsEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //LoadLast
		{
			InsInstance->LoadLast = LoadLast->GetCheckState();
			break;
		}
		case 5: //Remove
		{
			RecentInstance->DeleteAllRecents();
			CloseControls(Recents);
		}
		}
		return 0;
	}

	void SettingsEditor::Reset()
	{
	}
}