#include "..\EditorFrame.h"

#include "Sort.h"
#include "UI\StyleButton.h"
#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"

namespace Armin::Editors::Inventory
{
	using namespace Files;
	using namespace UI;

	OperationInventoryEditor::OperationInventoryEditor(InventorySystem* System)
	{
		if (!System)
			_System = dynamic_cast<InventorySystem*>(LoadedProject);
		else
			_System = System;
	}

	LRESULT __stdcall OperationInventoryEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		InventoryEditor* This = reinterpret_cast<InventoryEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void OperationInventoryEditor::LoadControls()
	{
		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		AaColor BaseBk = EditorGrey;
		int BaseYCoord = this->BaseYCoord;

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
				XCoord += Width / 2;

				Search = new StyleButton(XCoord, YCoord, Width, Height, L"Inventory Search", _Base, (HMENU)4, ins, Style, TextStyle, RECT{0, 0, 0, 5});
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				Add = new Button(XCoord, YCoord, Width, Height, L"+", _Base, (HMENU)5, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				Remove = new Button(XCoord, YCoord, Width, Height, L"-", _Base, (HMENU)6, ins, Style, TextStyle);
				YCoord += 10 + ButtonSize;

				View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				Edit = new Button(XCoord, YCoord, Width, Height, L"ED", _Base, (HMENU)8, ins, Style, TextStyle);
				YCoord += 10 + ButtonSize;

				SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)9, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DA", _Base, (HMENU)10, ins, Style, TextStyle);

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
	void OperationInventoryEditor::FillObjects()
	{
		OperationInventoryItemGroup* InvItems = !_System ? nullptr : _System->OperationInventoryItems;
		if (!InvItems)
			return;

		Vector<OperationInventoryItem*> Items = *InvItems;
		InventorySize->SetText(Items.Size);

		QuickSort(Items, [](OperationInventoryItem*& One, OperationInventoryItem*& Two) { return static_cast<wstring>(One->SerialNumber) < static_cast<wstring>(Two->SerialNumber); });

		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(Items, ObjectView, NULL, true, true, ObjectScroll);
	}

	LRESULT OperationInventoryEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Search
			EditorRegistry::OpenEditor(new InventorySearchEditor(_System, false), nullptr);
			break;
		case 5: //Add
			EditorRegistry::OpenEditor(new AddEditOperationInventoryItemEditor(), nullptr);
			break;
		case 6: //Remove
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

				ComponentViewer* Current = Selected[i];
				Objects.Remove(Current);
				delete Current;
			}

			AppState |= APS_HasEdit;

			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
			break;
		}
		case 7: //View
		case 9: //Edit	
			ComponentViewer::OpenSelectedForEditView(Objects, wp == 9);
			break;
		case 8: //SelectAll
			for (ComponentViewer* Item : Objects)
				Item->CheckState(true);
			break;
		case 10: //DeSelectAll
			for (ComponentViewer* Item : Objects)
				Item->CheckState(false);
			break;
		}
		return 0;
	}
	LRESULT OperationInventoryEditor::KeyDown(WPARAM Key)
	{
		switch (Key)
		{
		case VK_RETURN:
			FillObjects();
			break;
		case VK_DELETE:
			Command(6, 0);
			break;
		case 'N':
			if ((GetKeyState(VK_SHIFT) & 0x8000) && (GetKeyState(VK_CONTROL) & 0x8000))
				Command(5, 0);
			break;
		case 'V':
		case 'E':
			if ((GetKeyState(VK_CONTROL) & 0x8000))
				ComponentViewer::OpenSelectedForEditView(Objects, Key == 'E');
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
		}
		return 0;
	}
	LRESULT OperationInventoryEditor::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseXCoord = 10;
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
				XCoord += Width / 2;

				Search->Move(XCoord, YCoord, Width, Height);
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
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
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}

		return 0;
	}

	void OperationInventoryEditor::Reset()
	{
		FillObjects();
	}
}