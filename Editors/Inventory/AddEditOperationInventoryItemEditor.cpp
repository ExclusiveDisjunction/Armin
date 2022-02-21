#include "..\EditorFrame.h"

#include "..\..\Editors\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\GroupSelector.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Inventory
{
	using namespace Files;
	using namespace UI::Tool;

	AddEditOperationInventoryItemEditor::AddEditOperationInventoryItemEditor(OperationInventoryItem* ToEdit)
	{
		if (ToEdit)
			Target = new ComponentReference(ToEdit);
		else
			Target = nullptr;
	}

	LRESULT __stdcall AddEditOperationInventoryItemEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		AddEditOperationInventoryItemEditor* This = reinterpret_cast<AddEditOperationInventoryItemEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void AddEditOperationInventoryItemEditor::LoadControls()
	{
		int BaseXCoord = 10;
		int BaseYCoord = this->BaseYCoord;

		if (Target && !Target->Target())
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The target Inventory Item has been deleted or moved. The editor will now add a new inventory item.", L"Error:", MB_OK | MB_ICONERROR);
			delete Target;
			Target = nullptr;
		}

		OperationInventoryItem* TrueTarget = !Target ? nullptr : dynamic_cast<OperationInventoryItem*>(Target->Target());

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);

		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 30;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", EditorGrey, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", EditorGrey, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Status:", EditorGrey, TextStyle, false));
			YCoord += 10 + Height;
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", EditorGrey, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 10 + Width;

			TextStyle.Alignment = TA_CenterAlignment;
			TextStyle.Bold = false;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.BaseBackground = EditorGrey;
			Style.Radius = 15;

			Width = WndRect.right - 10 - XCoord;
			SerialNumber = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, TrueTarget ? TrueTarget->SerialNumber : String(), Style, TextStyle);

			YCoord += Height + 10;

			Group = new TextBox(XCoord, YCoord, Width - Height - 5, Height, _Base, ins, TrueTarget ? TrueTarget->Group : String(), Style, TextStyle);
			XCoord += (Width - Height);
			SelectGroup = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);
			
			YCoord += Height + 10;
			XCoord -= (Width - Height);

			{
				int OldWidth = Width;
				int OldX = XCoord;

				Width = (Width - 10) / 3;

				WorkingOrder = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, TrueTarget ? TrueTarget->CurrentState == OperationInventoryItem::IS_WorkingOrder : true, L"Working Order", CBT_Radio, Style, TextStyle);
				XCoord += 5 + Width;

				PartialWorkingOrder = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, TrueTarget ? TrueTarget->CurrentState == OperationInventoryItem::IS_PartialWorkingOrder : false, L"Partial Working Order", CBT_Radio, Style, TextStyle);
				XCoord += 5 + Width;

				Broken = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, TrueTarget ? TrueTarget->CurrentState == OperationInventoryItem::IS_Broken : false, L"Broken", CBT_Radio, Style, TextStyle);
				YCoord += 10 + Height;
				XCoord = OldX;

				UnderRepair = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, TrueTarget ? TrueTarget->CurrentState == OperationInventoryItem::IS_UnderRepair : false, L"Under Repair", CBT_Radio, Style, TextStyle);
				XCoord += 5 + Width;

				NotInPossession = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, TrueTarget ? TrueTarget->CurrentState == OperationInventoryItem::IS_NotInPossession : false, L"Not In Possession", CBT_Radio, Style, TextStyle);
				XCoord += 5 + Width;

				XCoord = OldX;
				Width = OldWidth;
			}

			YCoord += 10 + Height;
			Height = WndRect.bottom - (YCoord + 10);

			Description = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, TrueTarget ? TrueTarget->Description : String(), Style, TextStyle, true);
		}
	}

	LRESULT AddEditOperationInventoryItemEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			StringList List = GroupSelector::Execute(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), nullptr, GSS_OperationInventory, false);
			this->Group->SetText(List.Size == 0 ? String() : List[0]);
			break;
		}
		}
		return 0;
	}
	LRESULT AddEditOperationInventoryItemEditor::KeyDown(WPARAM Key)
	{
		if (Key == VK_ESCAPE)
			Command(1, 0);
		else if (Key == 0x50 && (GetKeyState(VK_CONTROL) & 0x8000))
			Command(2, 0);

		return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
	}
	LRESULT AddEditOperationInventoryItemEditor::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (!_Loaded)
			return 0;

		int BaseXCoord = 10;
		int BaseYCoord = this->BaseYCoord;

		MoveUpperButtons(WndRect);

		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 30;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;
			YCoord += 10 + Height;
			
			MiscControls[2]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 10 + Width;

			Width = WndRect.right - 10 - XCoord;
			SerialNumber->Move(XCoord, YCoord, Width, Height);

			YCoord += Height + 10;

			Group->Move(XCoord, YCoord, Width - Height - 5, Height);
			XCoord += (Width - Height);
			SelectGroup->Move(XCoord, YCoord, Height, Height);

			YCoord += Height + 10;
			XCoord -= (Width - Height);

			{
				int OldWidth = Width;
				int OldX = XCoord;

				Width = (Width - 10) / 3;

				WorkingOrder->Move(XCoord, YCoord, Width, Height);
				XCoord += 5 + Width;

				PartialWorkingOrder->Move(XCoord, YCoord, Width, Height);
				XCoord += 5 + Width;

				Broken->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;
				XCoord = OldX;

				UnderRepair->Move(XCoord, YCoord, Width, Height);
				XCoord += 5 + Width;

				NotInPossession->Move(XCoord, YCoord, Width, Height);
				XCoord += 5 + Width;

				XCoord = OldX;
				Width = OldWidth;
			}

			YCoord += 10 + Height;
			Height = WndRect.bottom - (YCoord + 10);

			Description->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}
	LRESULT AddEditOperationInventoryItemEditor::Destroy()
	{
		return 0;
	}

	void AddEditOperationInventoryItemEditor::Reset()
	{
		SerialNumber->SetText(String());
		Description->SetText(String());
		Group->SetText(String());
		WorkingOrder->SetCheckState(true);
	}
	bool AddEditOperationInventoryItemEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		if (Target && !Target->Target())
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"This Operation Inventory Item has already been deleted, and cannot be edited anymore.", L"Error:", MB_OK | MB_ICONERROR);
			EditorRegistry::CloseEditor(this, false);
			return false;
		}


		String SerialNumber = this->SerialNumber->GetText();
		String Description = this->Description->GetText();
		String Group = this->Group->GetText();
		OperationInventoryItem::ItemState State = OperationInventoryItem::IS_NotInPossession;

		if (WorkingOrder->GetCheckState())
			State = OperationInventoryItem::IS_WorkingOrder;
		else if (PartialWorkingOrder->GetCheckState())
			State = OperationInventoryItem::IS_PartialWorkingOrder;
		else if (Broken->GetCheckState())
			State = OperationInventoryItem::IS_Broken;
		else if (UnderRepair->GetCheckState())
			State = OperationInventoryItem::IS_UnderRepair;
		else
			State = OperationInventoryItem::IS_NotInPossession;

		if (!File)
		{
			if (PromptErrors)
				MessageBoxW(NULL, L"There is no file loaded.", L"Add Operation Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}

		InventorySystem* _System = dynamic_cast<InventorySystem*>(File);
		OperationInventoryItemGroup* Inv = !_System ? nullptr : _System->OperationInventoryItems;
		if (!Inv)
		{
			if (PromptErrors)
				MessageBoxW(NULL, L"The current Project does not contain an Inventory System.", L"Add Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}

		if (SerialNumber.RemoveWhiteSpace() == String())
		{
			if (PromptErrors)
				MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The Serial Number must have a value."), TEXT("Add Operation Inventory Item:"), MB_OK | MB_ICONERROR);
			return false;
		}
		if (SerialNumber.Contains(L'~') || Description.Contains(L'~') || Group.Contains(L'~'))
		{
			if (PromptErrors)
				MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("There cannot be a Tilde ('~') character in any feild."), TEXT("Add Operation Inventory Item:"), MB_OK | MB_ICONERROR);
			return false;
		}
		if (Inv->Contains(SerialNumber))
		{
			if (PromptErrors)
				MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The Serial Number provided is non-unique.\n\nPlease provide a unique Serial Number and try again."), TEXT("Add Operation Inventory Item:"), MB_OK | MB_ICONERROR);
			return false;
		}

		if (Group.RemoveWhiteSpace() == String())
		{
			int Result = MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The group was left blank. Was this intentional?"), TEXT("Add Operation Inventory Item:"), MB_YESNOCANCEL | MB_ICONWARNING);
			if (Result != IDYES)
				return false;
		}

		OperationInventoryItem* New = Target ? dynamic_cast<OperationInventoryItem*>(Target->Target()) : new OperationInventoryItem(_System, Inv);
		New->SerialNumber = SerialNumber;
		New->Description = Description;
		New->Group = Group;
		New->CurrentState = State;

		AppState |= APS_HasEdit;
		EditorRegistry::ResetEditorOfType(EDT_OperationInventory);

		return true;
	}
	Vector<void*> AddEditOperationInventoryItemEditor::CondenseArgs() const
	{
		return Target ? Target->Target() : Vector<void*>();
	}
	bool AddEditOperationInventoryItemEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Target)
			return Args.Size != 0 && Args[0] == Target->Target();
		return false;
	}
	bool AddEditOperationInventoryItemEditor::EquatableTo(EditorFrame* Other) const
	{
		if (Target)
			return dynamic_cast<AddEditOperationInventoryItemEditor*>(Other) != nullptr && Other->TestOnCondition(CondenseArgs());
		return false;
	}
}