#include "..\EditorFrame.h"

#include "Sort.h"
#include "UI\StyleButton.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\Import\ImportFromCSV.h"

namespace Armin::Editors::Inventory
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Import;

	InventoryEditor::InventoryEditor(InventorySystem* System)
	{		
		if (!System)
			_System = dynamic_cast<InventorySystem*>(LoadedProject);
		else
			_System = System;
	}

	void InventoryEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseYCoord = this->BaseYCoord;
		AaColor BaseBk = EditorGrey;

		LoadUpperButtons(WndRect, ins);

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Bold = true;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int ResetY = 0;

			{
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Inventory Size:", BaseBk, TextStyle, false));

				TextStyle.Bold = false;
				TextStyle.Alignment = TA_LeftAlignment;
				InventorySize = new Label(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height, _Base, ins, L"0", BaseBk, TextStyle, false);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;

				ImportCSV = new StyleButton(XCoord, YCoord, Width, Height, L"Import from CSV/Excel", _Base, (HMENU)4, ins, Style, TextStyle, RECT{0, 0, 0, 5});
				XCoord += 10 + Width;

				InvSearch = new StyleButton(XCoord, YCoord, Width, Height, L"Inventory Search", _Base, (HMENU)5, ins, Style, TextStyle, RECT{ 0, 0, 0, 5 });

				XCoord -= 10 + Width;
				Width = 130;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				Add = new Button(XCoord, YCoord, Width, Height, L"+", _Base, (HMENU)6, ins, Style, TextStyle);
				EnableWindow(*Add, (AppState & APS_HasAdminUser));
				YCoord += 5 + ButtonSize;

				Remove = new Button(XCoord, YCoord, Width, Height, L"-", _Base, (HMENU)7, ins, Style, TextStyle);
				EnableWindow(*Add, (AppState & APS_HasAdminUser));
				YCoord += 10 + ButtonSize;

				View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)8, ins, Style, TextStyle);
				EnableWindow(*View, (AppState & APS_HasAdminUser));
				YCoord += 5 + ButtonSize;

				Edit = new Button(XCoord, YCoord, Width, Height, L"ED", _Base, (HMENU)9, ins, Style, TextStyle);
				EnableWindow(*Edit, (AppState & APS_HasAdminUser));
				YCoord += 10 + ButtonSize;

				SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)10, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DA", _Base, (HMENU)11, ins, Style, TextStyle);

				Width = OldWidth;
				Height = OldHeight;
			}

			YCoord = ResetY;
			XCoord = 10;
			Width = WndRect.right - (10 + XCoord + ButtonSize + 10);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);
			FillObjects();
		}
	}
	void InventoryEditor::FillObjects()
	{
		InventoryItemGroup* InvItems = _System->InventoryItems;
		if (!InvItems)
			return;

		Vector<InventoryItem*> Items = *InvItems;
		InventorySize->SetText(Items.Size);

		QuickSort(Items, [](InventoryItem*& One, InventoryItem*& Two) { return static_cast<wstring>(One->SerialNumber) < static_cast<wstring>(Two->SerialNumber); });

		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(Items, ObjectView, NULL, true, true, ObjectScroll);
	}

	LRESULT InventoryEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Import
			new ImportFromCSV(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			break;
		case 5: //Search
			EditorRegistry::OpenEditor(new InventorySearchEditor(_System, true), nullptr);
			break;
		case 6: //Add
			EditorRegistry::OpenEditor(new AddInventoryItemEditor(), nullptr);
			break;
		case 7: //Remove
		{
			Vector<ComponentViewer*> Selected;
			Vector<Component*> Targets = ComponentViewer::RetriveFromList(Objects, Selected);

			if (Targets.Size == 0)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please select at least one inventory item to remove.", L"Remove From Inventory", MB_OK | MB_ICONERROR);
				break;
			}

			int Result = MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The selected data will be removed permanantly.\n\nDo you want to continue?", L"Remove from Inventory:", MB_YESNOCANCEL | MB_ICONWARNING);
			if (Result != IDYES)
				break;

			for (uint i = 0; i < Targets.Size; i++)
			{
				InventoryItem* Item = dynamic_cast<InventoryItem*>(Targets[i]);
				if (!Item)
					continue;

				delete Item;

				ComponentViewer* Viewer = Selected[i];
				Objects.Remove(Viewer);
				delete Viewer;
			}

			AppState |= APS_HasEdit;
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
			break;
		}
		case 9: //Edit		
		case 8: //View
			ComponentViewer::OpenSelectedForEditView(Objects, wp == 9);
			break;
		case 10: //SelectAll
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(true);
			break;
		case 11: //DelectAll
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(false);
			break;
		}
		return 0;
	}
	LRESULT InventoryEditor::KeyDown(WPARAM Key)
	{
		switch (Key)
		{
		case VK_RETURN: //Refresh
			FillObjects();
			break;
		case VK_DELETE: //Delete
			Command(7, 0);
			break;
		case 'N': //New Item
			if ((GetKeyState(VK_SHIFT) & 0x8000) && (GetKeyState(VK_CONTROL) & 0x8000))
				Command(6, 0);
			break;
		case 'V':
		case 'E':
			if (!(AppState & APS_HasAdminUser))
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"You do not have the proper user level to view or edit inventory items.", L"View/Edit:", MB_ICONERROR | MB_OK);
				break;
			}

			if ((GetKeyState(VK_CONTROL) & 0x8000))
				ComponentViewer::OpenSelectedForEditView(Objects, Key == 'E');
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
		}
		return 0;
	}
	LRESULT InventoryEditor::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseYCoord = this->BaseYCoord;

		if (!_Loaded)
			return 0;

		MoveUpperButtons(WndRect);

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			int ResetY = 0;

			{
				MiscControls[0]->Move(XCoord, YCoord, Width, Height);

				InventorySize->Move(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;

				ImportCSV->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				InvSearch->Move(XCoord, YCoord, Width, Height);

				XCoord -= 10 + Width;
				Width = 130;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				int ButtonSize = 40;
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				Add->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + ButtonSize;

				Remove->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + ButtonSize;

				View->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + ButtonSize;

				Edit->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + ButtonSize;

				SelectAll->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + ButtonSize;

				DeSelectAll->Move(XCoord, YCoord, Width, Height);

				Width = OldWidth;
				Height = OldHeight;
			}

			YCoord = ResetY;
			XCoord = 10;
			Width = WndRect.right - (10 + XCoord + ButtonSize + 10);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}

		return 0;
	}

	LRESULT __stdcall InventoryEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		InventoryEditor* This = reinterpret_cast<InventoryEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void InventoryEditor::Reset()
	{
		FillObjects();
	}
	bool InventoryEditor::EquatableTo(EditorFrame* Other) const
	{ 
		return dynamic_cast<InventoryEditor*>(Other) != nullptr && dynamic_cast<InventoryEditor*>(Other)->_System == _System; 
	}
}