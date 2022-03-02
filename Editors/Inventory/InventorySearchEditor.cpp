#include "..\EditorFrame.h"

#include "UI\StyleButton.h"
#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\Tool\GroupSelector.h"

namespace Armin::Editors::Inventory
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Tool;

	InventorySearchEditor::InventorySearchEditor(InventorySystem* System, bool Mode)
	{
		if (!System)
			_System = dynamic_cast<InventorySystem*>(LoadedProject);
		else
			_System = System;
		_Mode = Mode;
	}
	InventorySearchEditor::~InventorySearchEditor()
	{
		delete Objects;
	}

	LRESULT __stdcall InventorySearchEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		InventorySearchEditor* This = reinterpret_cast<InventorySearchEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void InventorySearchEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = this->BaseYCoord;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			{
				Width = ((WndRect.right / 2) - (10 + XCoord + 10)) / 2;

				Run = new StyleButton(XCoord, YCoord, Width, Height, L"Run Search", _Base, (HMENU)4, ins, Style, TextStyle, RECT{ 0, 0, 0, 5 });
				XCoord += 10 + Width;

				SaveResult = new StyleButton(XCoord, YCoord, Width, Height, L"Save Result", _Base, (HMENU)5, ins, Style, TextStyle, RECT{ 0, 0, 0, 5 });
				XCoord = 10;
				YCoord += 10 + Height;

				StandardMode = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)6, _Mode == true, L"Inventory", CBT_Radio, Style, TextStyle);
				XCoord += 10 + Width;

				OperationMode = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)7, _Mode == false, L"Operation Inventory", CBT_Radio, Style, TextStyle);
			}

			XCoord = 10;
			Width = 130;
			int ResetY = YCoord += 20 + Height;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Bold = true;

			int DescY = 0; //YCoord jumping to description area
			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			if (!_Mode)
			{
				int PreX = XCoord;
				int PreWidth = Width;
				Width = ((WndRect.right / 2) - (10 + XCoord + 10)) / 2;
				TextStyle.Bold = false;

				WorkingOrder = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Working Order", CBT_CheckBox, Style, TextStyle);
				XCoord += 10 + Width;

				PartialWorkingOrder = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Partial Working Order", CBT_CheckBox, Style, TextStyle);
				XCoord -= 10 + Width;
				YCoord += 10 + Height;

				Broken = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Broken", CBT_CheckBox, Style, TextStyle);
				XCoord += 10 + Width;

				UnderRepair = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Under Repair", CBT_CheckBox, Style, TextStyle);
				XCoord -= 10 + Width;
				YCoord += 10 + Height;

				XCoord = (((WndRect.right / 2) - (10 + XCoord)) / 2) - (Width / 2);

				NotInPossession = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Not In Possession", CBT_CheckBox, Style, TextStyle);
				YCoord += 10 + Height;

				XCoord = PreX;
				Width = PreWidth;
				TextStyle.Bold = true;
			}
			else
			{
				int PreWidth = Width;
				int PreX = XCoord;
				XCoord += Width + 5;
				Width = (WndRect.right / 2) - (10 + XCoord);
				TextStyle.Bold = false;

				NotInPossession = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Is In Possession", CBT_CheckBox, Style, TextStyle);
				YCoord += 10 + Height;

				XCoord = PreX;
				Width = PreWidth;
				TextStyle.Bold = true;
			}

			YCoord = ResetY;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;

			SerialNumber = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			Width -= Height + 5;
			Group = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			XCoord += Width + 5;
			SelectGroups = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)11, ins, Style, TextStyle);
			XCoord -= Width + 5;
			YCoord = DescY;
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
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

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Results:", BaseBk, TextStyle, false));
			int ResetY = YCoord += 10 + Height;

			int ButtonSize = 40;
			YCoord += (WndRect.bottom - (10 + YCoord)) / 2 - int((ButtonSize + 5) * 1.5f);
			TextStyle.Bold = false;

			View = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)8, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			Edit = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"ED", _Base, (HMENU)9, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			DuplicateResult = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"DR", _Base, (HMENU)10, ins, Style, TextStyle);

			YCoord = ResetY;
			XCoord += 5 + ButtonSize;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);

			if (Objects)
				delete Objects;
			Objects = new ComponentViewerList(ObjectView, ObjectScroll);
		}
	}
	void InventorySearchEditor::RunSearch()
	{
		bool IsInPossession = !_Mode ? false : this->NotInPossession->GetCheckState();
		int State = _Mode ? 0 :
			((WorkingOrder->GetCheckState() ? OperationInventoryItem::IS_WorkingOrder : 0) |
				(PartialWorkingOrder->GetCheckState() ? OperationInventoryItem::IS_PartialWorkingOrder : 0) |
				(Broken->GetCheckState() ? OperationInventoryItem::IS_Broken : 0) |
				(UnderRepair->GetCheckState() ? OperationInventoryItem::IS_UnderRepair : 0) |
				(NotInPossession->GetCheckState() ? OperationInventoryItem::IS_NotInPossession : 0));

		String SerialNumber = this->SerialNumber->GetText(), Group = this->Group->GetText();
		String SerialPart = (SerialNumber != String() ? (L"serialnumber=\"" + SerialNumber + L"\"") : String());
		String GroupPart = (Group != String() ? (L"group=\"" + Group + L"\"") : String());
		String Combined = SerialPart;
		if (SerialPart == String())
			Combined = GroupPart;
		else
			Combined += L',' + GroupPart;

		SearchCriteria Criteria;
		Criteria.AllowedTypes = _Mode ? CT_InventoryItem : CT_OperationInventoryItem;
		Criteria.Arguments = Combined;

		Vector<Component*> ResultR = Criteria.GetComponents(_System);
		Vector<InventoryItem*> InvItems;
		Vector<OperationInventoryItem*> OperInvItems;
		for (Component* Obj : ResultR)
		{
			InventoryBase* First = dynamic_cast<InventoryBase*>(Obj);
			if (!First)
				continue;

			InventoryItem* Inv = dynamic_cast<InventoryItem*>(First);
			OperationInventoryItem* OperInv = dynamic_cast<OperationInventoryItem*>(First);
			if (Inv)
				InvItems.Add(Inv);
			if (OperInv)
				OperInvItems.Add(OperInv);
		}

		Vector<InventoryBase*> Filtered;

		for (InventoryItem* Item : InvItems)
		{
			if (Item->IsInPossession == IsInPossession)
				Filtered.Add(Item);
		}
		for (OperationInventoryItem* Item : OperInvItems)
		{
			if (State & Item->CurrentState)
				Filtered.Add(Item);
		}

		Objects->Clear();
		Objects->GenerateList(Filtered, nullptr, true, true);
	}
	void InventorySearchEditor::SwitchModes()
	{
		bool LastMode = WorkingOrder == nullptr;
		if (LastMode == _Mode)
			return;

		if (_Mode) //From Operational to Standard
		{
			delete WorkingOrder;
			delete PartialWorkingOrder;
			delete Broken;
			delete UnderRepair;

			WorkingOrder = PartialWorkingOrder = Broken = UnderRepair = nullptr;

			NotInPossession->Text(L"Is In Possession");;
			NotInPossession->SetCheckState(true);
		}
		else //From Standard to Operational
		{
			HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = EditorGrey;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int XCoord = 0, YCoord = 0, Width = 0, Height = 0;

			WorkingOrder = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Working Order", CBT_CheckBox, Style, TextStyle);
			PartialWorkingOrder = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Partial Working Order", CBT_CheckBox, Style, TextStyle);
			Broken = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Broken", CBT_CheckBox, Style, TextStyle);
			UnderRepair = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Under Repair", CBT_CheckBox, Style, TextStyle);

			NotInPossession->Text(L"Not In Possession");
			NotInPossession->SetCheckState(true);
		}

		Objects->Clear();
		Size();
	}

	LRESULT InventorySearchEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //RunSearch
			RunSearch();
			break;
		case 5: //SaveSearch
			Objects->SaveSelectedAsGroup();
			break;
		case 6: //StandardToggle
		case 7: //OperationToggle
			_Mode = wp == 6;
			SwitchModes();
			break;
		case 8: //View
		case 9: //Edit
			Objects->OpenSelectedForEditView(wp == 9);
		case 10: //Duplicate Result
			Objects->PopoutObjects(L"Inventory Search Result", reinterpret_cast<HINSTANCE>(GetWindowLongPtr(_Base, GWLP_HINSTANCE)));
			break;
		case 11:			
		{
			Vector<String> Groups = GroupSelector::Execute(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), _System, _Mode ? GSS_Inventory : GSS_OperationInventory, true);
			String Return = Groups.Size == 0 ? String() : Groups[0];
			for (uint i = 1; i < Groups.Size; i++)
				Return += L',' + Groups[i];

			this->Group->SetText(Return);
			break;
		}
		}
		return 0;
	}
	LRESULT InventorySearchEditor::KeyDown(WPARAM wp)
	{
		switch (wp)
		{
		case L'D':
			if (GetKeyState(VK_CONTROL) & 0x8000)
				Command(10, 0);
			return 0;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}		
	}
	LRESULT InventorySearchEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		int BaseYCoord = this->BaseYCoord;

		MoveUpperButtons(WndRect);

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			{
				Width = ((WndRect.right / 2) - (10 + XCoord + 10)) / 2;

				Run->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				SaveResult->Move(XCoord, YCoord, Width, Height);
				XCoord = 10;
				YCoord += 10 + Height;

				StandardMode->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				OperationMode->Move(XCoord, YCoord, Width, Height);
			}

			XCoord = 10;
			Width = 130;
			int ResetY = YCoord += 20 + Height;

			int DescY = 0; //YCoord jumping to description area
			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			if (!_Mode)
			{
				int PreX = XCoord;
				int PreWidth = Width;
				Width = ((WndRect.right / 2) - (10 + XCoord + 10)) / 2;

				WorkingOrder->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				PartialWorkingOrder->Move(XCoord, YCoord, Width, Height);
				XCoord -= 10 + Width;
				YCoord += 10 + Height;

				Broken->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				UnderRepair->Move(XCoord, YCoord, Width, Height);
				XCoord -= 10 + Width;
				YCoord += 10 + Height;

				XCoord = (((WndRect.right / 2) - (10 + XCoord)) / 2) - (Width / 2);

				NotInPossession->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				XCoord = PreX;
				Width = PreWidth;
			}
			else
			{
				int PreWidth = Width;
				int PreX = XCoord;
				XCoord += Width + 5;
				Width = (WndRect.right / 2) - (10 + XCoord);

				NotInPossession->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				XCoord = PreX;
				Width = PreWidth;
			}

			YCoord = ResetY;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);
			
			SerialNumber->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Width -= Height + 5;
			Group->Move(XCoord, YCoord, Width, Height);
			XCoord += Width + 5;
			SelectGroups->Move(XCoord, YCoord, Height, Height);
			XCoord -= Width + 5;
			YCoord = DescY;
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			int ResetY = YCoord += 10 + Height;

			int ButtonSize = 40;
			YCoord += (WndRect.bottom - (10 + YCoord)) / 2 - int((ButtonSize + 5) * 1.5f);

			View->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			Edit->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			DuplicateResult->Move(XCoord, YCoord, ButtonSize, ButtonSize);

			YCoord = ResetY;
			XCoord += 5 + ButtonSize;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
			Objects->ReSizeList();
		}

		return 0;
	}

	bool InventorySearchEditor::TestOnCondition(Vector<void*> Args) const
	{
		return Args.Size != 0 && (!Args[0] == _Mode);
	}
	void InventorySearchEditor::Reset()
	{
		_Mode = true;
		SwitchModes();
	}
}