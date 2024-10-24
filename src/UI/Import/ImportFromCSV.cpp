#include "ImportFromCSV.h"

#include "Misc.h"
#include "Files\Stream.h"
#include "..\..\Editors\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace Armin::UI::Import
{
	using namespace Files;
	using namespace Editors;

	ImportFromCSV::ImportFromCSV(HINSTANCE ins)
	{
		if (!_ThisAtom)
			InitBase(ins);

		ToPath = LoadedSessionPath;

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Import From CSV", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 750, 550, NULL, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);

		LoadPage();
	}
	ImportFromCSV::~ImportFromCSV()
	{
		if (_Base)
			DestroyWindow(_Base);
	}

	LRESULT __stdcall ImportFromCSV::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ImportFromCSV* Item = reinterpret_cast<ImportFromCSV*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_PAINT:
			return Item->Paint();
		case WM_SIZE:
			return Item->Size();
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_DESTROY:
		{
			LRESULT Return = Item->Destroy();
			Item->_Base = nullptr;
			delete Item;
			return Return;
		}
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}
	ATOM ImportFromCSV::_ThisAtom = ATOM();
	void ImportFromCSV::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = ImportFromCSV::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"ImportFromCSVClass";
		wn.hIcon = ArminIcon;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(ImportFromCSV*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}

	void ImportFromCSV::LoadPage()
	{
		if (_Loaded)
			DestroyControls(false);

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (!Next && !Back && !Cancel)
		{
			TextStyleSheet TextStyle;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = Background1;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int Width = 150;
			int Height = 27;
			int XCoord = (WndRect.right / 2) - (Width / 2) - (5 + Width);
			int YCoord = WndRect.bottom - (10 + Height);

			Cancel = new Button(XCoord, YCoord, Width, Height, L"Cancel", _Base, (HMENU)3, ins, Style, TextStyle);
			XCoord += 5 + Width;

			Back = new Button(XCoord, YCoord, Width, Height, L"Back", _Base, (HMENU)2, ins, Style, TextStyle);
			XCoord += 5 + Width;

			Next = new Button(XCoord, YCoord, Width, Height, L"Next", _Base, (HMENU)1, ins, Style, TextStyle);
			XCoord += 5 + Width;
		}

		EnableWindow(*Back, CurrentPage != Home && CurrentPage != None);
		//EnableWindow(*Next, CurrentPage != FinalPage && CurrentPage != None);

		AaColor BaseBk = Background1;

		if (CurrentPage == Home)
		{
			TextStyleSheet TextStyle;
			TextStyle.FontFamily = TitleFontName;
			TextStyle.FontSize = 20;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_CenterAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int Width = 300;
			int Height = 40;
			int XCoord = (WndRect.right / 2) - (Width / 2);
			int YCoord = 20;

			Title = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Import From CSV", BaseBk, TextStyle, false);
			YCoord += 20 + Height;

			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;

			Instructions = new Label(10, YCoord, WndRect.right - 20, 27, _Base, ins, L"Please select a CSV File to continue.", BaseBk, TextStyle, false);

			XCoord = 40;
			YCoord += 30 + Height;
			Height = 27;
			Width = (WndRect.right) - (XCoord * 2) - ((Height * 2) + 5);

			HOMPath = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, FromPath, Style, TextStyle);
			XCoord += 5 + Width;
			HOMSelect = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);
		}
		if (CurrentPage == SelectColumns)
		{
			TextStyleSheet TextStyle;
			TextStyle.Alignment = TA_CenterAlignment;
			TextStyle.FontFamily = TitleFontName;
			TextStyle.FontSize = 16;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.BaseBackground = BaseBk;
			Style.Background = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int Width = 300;
			int Height = 40;
			int XCoord = (WndRect.right / 2) - (Width / 2);
			int YCoord = 20;

			Title = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Extra Data", BaseBk, TextStyle, false);
			YCoord += 20 + Height;

			TextStyle.FontFamily = StandardFontName;
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.FontSize = 13;
			XCoord = 10;
			Width = (WndRect.right / 2) - (10 + XCoord);

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"ID:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			int OldHeight = Height;
			Height = WndRect.bottom - 45 - YCoord - (32 * 2);

			IDColumnsScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			IDColumnsView = new Grid(0, 0, Width, Height, IDColumnsScroll, ins, Style);
			IDColumnsScroll->SetViewer(IDColumnsView);

			YCoord -= 5 + OldHeight;
			Height = OldHeight;
			XCoord += 20 + Width;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			OldHeight = Height;
			Height = (WndRect.bottom - 45 - YCoord) - (32 * 2);

			DesColumnsScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			DesColumnsView = new Grid(0, 0, Width, Height, DesColumnsScroll, ins, Style);
			DesColumnsScroll->SetViewer(DesColumnsView);

			YCoord += 5 + Height;
			XCoord -= 20 + Width;
			Width = 120;
			Height = 27;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Separate By:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));

			YCoord -= 5 + Height;
			XCoord += 5 + Width;
			Width = (WndRect.right - 10 - XCoord);

			SeperateBy = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, SepBy, Style, TextStyle);
			YCoord += 5 + Height;

			TXTGroupName = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, GroupName, Style, TextStyle);

			{
				wifstream InFile(FromPath);
				if (!InFile)
					return;

				Style.BaseBackground = Style.Background;

				String FirstLine;
				getline(InFile, FirstLine);
				if (FirstLine == L"")
					return;

				StringList Parts = FirstLine.Split(L',');

				XCoord = 5;
				YCoord = 5;
				Width = 400;
				Height = 27;

				IDColumnsView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Parts.Size + 1)));
				DesColumnsView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (Parts.Size + 1)));
				IDColumnsScroll->Reset();
				DesColumnsScroll->Reset();

				for (uint i = 0; i < Parts.Size; i++)
				{
					IDColumns.Add(new CheckableButton(XCoord, YCoord, Width, Height, IDColumnsView, ins, NULL, i == IDColumn, Parts[i], CBT_Radio, Style, TextStyle));
					DesColumns.Add(new CheckableButton(XCoord, YCoord, Width, Height, DesColumnsView, ins, NULL, DescriptionColumns.Contains(i), Parts[i], CBT_CheckBox, Style, TextStyle));

					YCoord += 5 + Height;
				}
			}
		}

		if (CurrentPage == FinalPage)
		{
			TextStyleSheet TextStyle;
			TextStyle.Alignment = TA_CenterAlignment;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 16;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.BaseBackground = BaseBk;
			Style.Background = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int Width = WndRect.right - 20;
			int Height = 40;
			int XCoord = (WndRect.right / 2) - (Width / 2);
			int YCoord = 20;

			Instructions = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"The data is complete. Press 'Next' to begin importing.", BaseBk, TextStyle, false);
		}
	}
	void ImportFromCSV::DestroyControls(bool Full)
	{
		if (!_Loaded)
			return;

		CloseControls(MiscControls);
		delete Title;
		delete Instructions;
		delete HOMPath;
		delete HOMSelect;

		delete IDColumnsScroll;
		delete DesColumnsScroll;
		delete SeperateBy;
		delete TXTGroupName;

		Title = Instructions = nullptr;
		HOMPath = nullptr;
		HOMSelect = nullptr;
		IDColumnsView = DesColumnsView = nullptr;
		IDColumnsScroll = DesColumnsScroll = nullptr;
		SeperateBy = TXTGroupName = nullptr;

		if (Full)
		{
			delete Next;
			delete Back;
			delete Cancel;
			Next = Back = Cancel = nullptr;

			_Loaded = false;
		}
	}
	bool ImportFromCSV::ProcessPage()
	{
		switch (CurrentPage)
		{
		case Home:
		{
			String Path = HOMPath->GetText();
			if (!FileExists(Path) || FileExt(Path) != L"csv")
			{
				MessageBoxW(_Base, L"Please supply a path that exists, and has a .csv file type.", L"Import:", MB_OK | MB_ICONERROR);
				return false;
			}

			this->FromPath = Path;
			return true;
		}
		case SelectColumns:
		{
			this->SepBy = SeperateBy->GetText();
			this->GroupName = TXTGroupName->GetText();

			if (IDColumns.Size == 0u && DesColumns.Size == 0u)
			{
				MessageBoxW(_Base, L"The file that you selected does not have any columns for retriving data, or any data.\n\nPlease select a diffrent file, or fill this file with data and try again.", L"Import:", MB_OK | MB_ICONERROR);
				return false;
			}

			for (uint i = 0; i < IDColumns.Size; i++)
			{
				if (IDColumns[i]->GetCheckState())
					IDColumn = static_cast<int>(i);
			}

			DescriptionColumns.Clear();

			for (uint i = 0; i < DesColumns.Size; i++)
			{
				if (DesColumns[i]->GetCheckState())
					DescriptionColumns.Add(static_cast<int>(i));
			}
			return true;
		}
		case FinalPage:
		{
			BeginImport();
			return true;
		}
		default:
			return true;
		}
	}

	LRESULT ImportFromCSV::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background1);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT ImportFromCSV::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1:
		{
			if (CurrentPage == FinalPage)
			{
				BeginImport();
				break;
			}
			if (!ProcessPage())
				break;

			LastPage = CurrentPage;
			int Temp = CurrentPage;
			Temp++;
			CurrentPage = static_cast<ImportFromCSV::Page>(Temp);
			LoadPage();
			break;
		}
		case 2:
		{
			if (CurrentPage == Home)
			{
				EnableWindow(*Back, false);
				break;
			}

			if (CurrentPage != FinalPage && !ProcessPage())
				break;

			Page Temp = CurrentPage;
			CurrentPage = LastPage;
			LastPage = Temp;

			LoadPage();
			break;
		}
		case 3:
			DestroyWindow(_Base);
			break;
		case 4: //Home Select
		{
			String Text = OpenFileEx(_Base, L"CSV Files\0*.csv\0", L".csv");
			HOMPath->SetText(Text);
		}
		}
		return 0;
	}
	LRESULT ImportFromCSV::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (!Next && !Back && !Cancel)
		{
			int Width = 150;
			int Height = 27;
			int XCoord = (WndRect.right / 2) - (Width / 2) - (5 + Width);
			int YCoord = WndRect.bottom - (10 + Height);

			Cancel->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			Back->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			Next->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
		}

		if (CurrentPage == Home)
		{
			int Width = 300;
			int Height = 40;
			int XCoord = (WndRect.right / 2) - (Width / 2);
			int YCoord = 20;

			Title->Move(XCoord, YCoord, Width, Height);
			YCoord += 20 + Height;

			Instructions->Move(10, YCoord, WndRect.right - 20, 27);

			XCoord = 40;
			YCoord += 30 + Height;
			Height = 27;
			Width = (WndRect.right) - (XCoord * 2) - ((Height * 2) + 5);

			HOMPath->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			HOMSelect->Move(XCoord, YCoord, Height, Height);
		}
		if (CurrentPage == SelectColumns)
		{
			int Width = 300;
			int Height = 40;
			int XCoord = (WndRect.right / 2) - (Width / 2);
			int YCoord = 20;

			Title->Move(XCoord, YCoord, Width, Height);
			YCoord += 20 + Height;

			XCoord = 10;
			Width = (WndRect.right / 2) - (10 + XCoord);

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			int OldHeight = Height;
			Height = WndRect.bottom - 45 - YCoord - (32 * 2);

			IDColumnsScroll->Move(XCoord, YCoord, Width, Height);

			YCoord -= 5 + OldHeight;
			Height = OldHeight;
			XCoord += 20 + Width;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			OldHeight = Height;
			Height = (WndRect.bottom - 45 - YCoord) - (32 * 2);

			DesColumnsScroll->Move(XCoord, YCoord, Width, Height);

			YCoord += 5 + Height;
			XCoord -= 20 + Width;
			Width = 120;
			Height = 27;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);

			YCoord -= 5 + Height;
			XCoord += 5 + Width;
			Width = (WndRect.right - 10 - XCoord);

			SeperateBy->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			TXTGroupName->Move(XCoord, YCoord, Width, Height);

			{
				XCoord = 5;
				YCoord = 5;
				Width = 900;
				Height = 27;

				IDColumnsView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (IDColumns.Size + 1)));
				DesColumnsView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (DesColumns.Size + 1)));
				IDColumnsScroll->Reset();
				DesColumnsScroll->Reset();

				for (uint i = 0; i < IDColumns.Size; i++, YCoord += 5 + Height)
				{
					IDColumns[i]->Move(XCoord, YCoord, Width, Height);
					DesColumns[i]->Move(XCoord, YCoord, Width, Height);
				}
			}
		}

		if (CurrentPage == FinalPage)
		{
			int Width = WndRect.right - 20;
			int Height = 40;
			int XCoord = (WndRect.right / 2) - (Width / 2);
			int YCoord = 20;

			Instructions->Move(XCoord, YCoord, Width, Height);
		}
		return 0;
	}
	LRESULT ImportFromCSV::Destroy()
	{
		if (!_Loaded)
			return 0;

		DestroyControls(true);

		_Loaded = false;
		return 0;
	}

	void ImportFromCSV::BeginImport()
	{
		InventorySystem* File = dynamic_cast<InventorySystem*>(LoadedSession);
		InventoryItemGroup* Inventory = !File ? nullptr : File->InventoryItems;
		if (!File || !Inventory)
		{
			MessageBoxW(_Base, L"The current file does not support an inventory system. Please load another project and try again.", L"Import:", MB_OK | MB_ICONERROR);
			return;
		}

		wifstream InFile(FromPath);

		int& IDCol = IDColumn;
		IntList& DesCol = DescriptionColumns;
		String& AddBy = this->SepBy;
		String& Group = this->GroupName;

		int ObjectCount = 0;

		String FirstLine;
		getline(InFile, FirstLine);

		while (!InFile.eof())
		{
			String CurrentLine;
			getline(InFile, CurrentLine);

			StringList OrigParts = CurrentLine.Split(L',');

			if (IDCol > static_cast<int>(OrigParts.Size))
				continue;

			String SerialNumber = OrigParts[IDCol];
			String Description;

			if (DesCol.Size > 0u)
			{
				if (DesCol[0u] >= static_cast<int>(OrigParts.Size))
					continue;

				int Index = DesCol[0u];
				Description += OrigParts[Index];
				for (uint i = 1; i < DesCol.Size; i++)
				{
					int Current = DesCol[i];
					if (Current >= static_cast<int>(OrigParts.Size))
						continue;

					Description += AddBy + OrigParts[Current];
				}
			}

			InventoryItem* New = new InventoryItem(File, Inventory);
			New->SerialNumber = SerialNumber;
			New->Description = Description;
			New->Group = Group;
			New->IsInPossession = true;

			ObjectCount++;
		}

		String Prompt = String(ObjectCount) + L" Objects have been proccessed successfuly.";
		MessageBoxW(_Base, static_cast<LPCWSTR>(Prompt), L"Import:", MB_OK | MB_ICONINFORMATION);

		DestroyWindow(_Base);

		File->Save();
		HasEdit = false;

		EditorRegistry::ResetEditorOfType(EDT_Inventory);
		return;
	}
}