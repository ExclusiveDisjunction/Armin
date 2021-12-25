#include "..\EditorFrame.h"

#include "Files\Stream.h"
#include "..\EditorRegistry.h"
#include "..\..\Ins.h"
#include "..\..\Recent.h"
#include "..\..\Files\SessionControl.h"

namespace Armin::Editors::Misc
{
	using namespace Files;

	WelcomeEditor::WelcomeEditor()
	{

	}

	LRESULT __stdcall WelcomeEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		WelcomeEditor* This = reinterpret_cast<WelcomeEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProc(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void WelcomeEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 220;
			int Height = 70;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 17;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;

			Open = new Button(XCoord, YCoord, Width, Height, L"Open Project", _Base, (HMENU)4, ins, Style, TextStyle);
			YCoord += 7 + Height;

			New = new Button(XCoord, YCoord, Width, Height, L"New Project", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 15 + Height;

			TextStyle.FontSize = 15;
			Height = 40;

			OpenLast = new Button(XCoord, YCoord, Width, Height, L"Open Last Project", _Base, (HMENU)6, ins, Style, TextStyle);
			YCoord += 7 + Height;

			Settings = new Button(XCoord, YCoord, Width, Height, L"Settings", _Base, (HMENU)7, ins, Style, TextStyle);
			YCoord += 7 + Height;

			Quit = new Button(XCoord, YCoord, Width, Height, L"Quit", _Base, (HMENU)8, ins, Style, TextStyle);

			YCoord = BaseYCoord;
			XCoord += 10 + Width;
			Height = WndRect.bottom - (10 + YCoord);
			Width = WndRect.right - (10 + XCoord);

			RecentsScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			RecentsViewer = new Grid(0, 0, 910, 32, RecentsScroll, ins, Style);
			RecentsScroll->SetViewer(RecentsViewer);

			RefreshList();
		}
	}
	void WelcomeEditor::RefreshList()
	{
		RECT WndRect;
		GetClientRect(RecentsScroll, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		int XCoord = 10;
		int YCoord = 10;
		int Height = 27;
		int Width = WndRect.right - 10 > 900 ? WndRect.right - 10 : 900;

		TextStyleSheet TextStyle;
		TextStyle.FontSize = 13;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.Foreground = FontColor;

		StyleSheet Style;
		Style.BaseBackground = Style.Background = Grey3;
		Style.BorderBrush = Accent4;
		Style.BorderThickness = 0;
		Style.Radius = 4;

		StringList Paths = RecentInstance->GetRecentsForUI();
		this->Paths = RecentInstance->RecentPaths;

		RecentsViewer->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Paths.Size + 1)));

		LONG_PTR Item = 19;
		for (uint i = 0; i < Paths.Size; i++)
		{
			Item++;
			Recents.Add(new Button(XCoord, YCoord, Width, Height, Paths[i], RecentsViewer, (HMENU)Item, ins, Style, TextStyle, TA_LeftAlignment));
			YCoord += 5 + Height;
		}
	}

	LRESULT WelcomeEditor::Size()
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
			int Width = 220;
			int Height = 70;

			Open->Move(XCoord, YCoord, Width, Height);
			YCoord += 7 + Height;

			New->Move(XCoord, YCoord, Width, Height);
			YCoord += 15 + Height;

			Height = 40;

			OpenLast->Move(XCoord, YCoord, Width, Height);
			YCoord += 7 + Height;

			Settings->Move(XCoord, YCoord, Width, Height);
			YCoord += 7 + Height;

			Quit->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 10 + Width;
			Height = WndRect.bottom - (10 + YCoord);
			Width = WndRect.right - (10 + XCoord);

			RecentsScroll->Move(XCoord, YCoord, Width, Height);
		
			RECT ClientRect;
			GetClientRect(*RecentsScroll, &ClientRect);

			XCoord = 10;
			YCoord = 10;
			Width = ClientRect.right - 10 > 900 ? ClientRect.right - 10 : 900;
			Height = 27;

			RecentsViewer->Move(0, 0, Width + 10, (Height + 5) * static_cast<int>((Recents.Size + 1)));
			RecentsScroll->Reset();

			for (uint i = 0; i < Recents.Size; i++, YCoord += 5 + Height)
				Recents[i]->Move(XCoord, YCoord, Width, Height);
		}
		return 0;
	}
	LRESULT WelcomeEditor::KeyDown(WPARAM wp)
	{
		return SendMessage(GetParent(_Base), WM_KEYDOWN, wp, 0);
	}
	LRESULT WelcomeEditor::Command(WPARAM wp, LPARAM lp)
	{
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtr(_Base, GWLP_HINSTANCE));

		switch (wp)
		{
		case 4: //Open
			if (SessionControl::Open(ins))
				EditorRegistry::CloseEditor(this);
			break;
		case 5: //New
			if (SessionControl::NewFile(ins))
				EditorRegistry::CloseEditor(this);
			break;
		case 6: //Load Last
		{
			String Path = InsInstance->LastLoaded;
			if (SessionControl::Open(Path, ins))
				DestroyWindow(_Base);
			else
				MessageBoxW(_Base, L"The last loaded file does not exist.\n\nPlease try again later.", L"Welcome:", MB_OK | MB_ICONERROR);

			break;
		}
		case 7: //Settings
			EditorRegistry::OpenEditor(new SettingsEditor(), nullptr);
			break;
		case 8: //Quit
			PostQuitMessage(0);
			break;
		}

		static WPARAM LastWp = 0;

		if (wp >= 20)
		{
			if (wp == LastWp)
				return 0;

			LastWp = wp;

			uint Index = static_cast<uint>(wp) - 20;

			if (Index >= Paths.Size || Index < 0)
				return 0;

			String Current = Paths[Index];
			if (!FileExists(Current))
			{
				MessageBoxW(_Base, L"The path could not be opened.\n\nIt will be removed from recents.", L"Welcome:", MB_OK | MB_ICONERROR);

				RecentInstance->DeleteRecent(Index);
				Button* Item = reinterpret_cast<Button*>(lp);
				if (!Item)
					return 0;

				Item->Text(L"[FILE NOT FOUND]");
			}
			else
			{
				SessionControl::Open(Current, ins);
				EditorRegistry::CloseEditor(this);
			}
		}
		return 0;
	}

	void WelcomeEditor::Reset()
	{

	}
}