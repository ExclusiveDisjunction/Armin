#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\GroupSelector.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Inventory
{
	using namespace Files;
	using namespace UI::Tool;

	AddEditInventoryItemEditor::AddEditInventoryItemEditor(InventoryItem* ToEdit)
	{
		if (ToEdit)
			Target = new ComponentReference(ToEdit);
		else
			Target = nullptr;
	}

	LRESULT __stdcall AddEditInventoryItemEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		AddEditInventoryItemEditor* This = reinterpret_cast<AddEditInventoryItemEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void AddEditInventoryItemEditor::LoadControls()
	{
		int BaseXCoord = 10;
		int BaseYCoord = this->BaseYCoord;

		if (Target && !Target->Target())
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The target Inventory Item has been deleted or moved. The editor will now add a new inventory item.", L"Error:", MB_OK | MB_ICONERROR);
			delete Target;
			Target = nullptr;
		}

		InventoryItem* TrueTarget = !Target ? nullptr : dynamic_cast<InventoryItem*>(Target->Target());

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

			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", EditorGrey, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 10 + Width;

			Width = WndRect.right - 10 - XCoord;
			TextStyle.Alignment = TA_CenterAlignment;
			TextStyle.Bold = false;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.BaseBackground = EditorGrey;
			Style.Radius = 15;

			SerialNumber = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, TrueTarget ? TrueTarget->SerialNumber : String(), Style, TextStyle);

			YCoord += Height + 10;

			Group = new TextBox(XCoord, YCoord, Width - Height - 5, Height, _Base, ins, TrueTarget ? TrueTarget->Group : String(), Style, TextStyle);
			XCoord += (Width - Height);
			SelectGroup = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);

			YCoord += Height + 10;
			XCoord -= (Width - Height);

			IsInPossession = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, TrueTarget ? TrueTarget->IsInPossession : true, L"Is In Possession", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;
			Height = WndRect.bottom - 10 - Height;

			Description = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, TrueTarget ? TrueTarget->Description : String(), Style, TextStyle, true, false);
		}
	}

	LRESULT AddEditInventoryItemEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			StringList List = GroupSelector::Execute(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), nullptr, GSS_Inventory, false);
			Group->SetText(List.Size == 0 ? String() : List[0]);
			break;
		}
		}
		return 0;
	}
	LRESULT AddEditInventoryItemEditor::KeyDown(WPARAM Key)
	{
		if (Key == VK_ESCAPE)
			Command(1, 0);
		else if (Key == 0x50 && (GetKeyState(VK_CONTROL) & 0x8000))
			Command(2, 0);

		return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
	}
	LRESULT AddEditInventoryItemEditor::Size()
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

			YCoord += 10 + Height;
			XCoord -= (Width - Height);

			IsInPossession->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;
			Height = WndRect.bottom - 10 - Height;

			Description->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}
	LRESULT AddEditInventoryItemEditor::Destroy()
	{
		return 0;
	}

	void AddEditInventoryItemEditor::Reset()
	{
		SerialNumber->SetText(String());
		Description->SetText(String());
		Group->SetText(String());
		IsInPossession->SetCheckState(true);
	}
	bool AddEditInventoryItemEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		if (Target && !Target->Target())
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"This Inventory Item has already been deleted, and cannot be edited anymore.", L"Error:", MB_OK | MB_ICONERROR);
			EditorRegistry::CloseEditor(this, false);
			return false;
		}

		String SerialNumber = this->SerialNumber->GetText();
		String Description = this->Description->GetText();
		String Group = this->Group->GetText();
		bool IsInPossession = this->IsInPossession->GetCheckState();

		if (!File)
		{
			if (PromptErrors)
				MessageBoxW(NULL, L"There is no file loaded.", L"Add Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}

		InventorySystem* _System = dynamic_cast<InventorySystem*>(File);
		InventoryItemGroup* Inv = !_System ? nullptr : _System->InventoryItems;
		if (!Inv)
		{
			if (PromptErrors)
				MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The current Project does not contain an Inventory System."), TEXT("Add Inventory Item:"), MB_OK | MB_ICONERROR);
			return false;
		}

		if (SerialNumber.RemoveWhiteSpace() == String())
		{
			if (PromptErrors)
				MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The Serial Number must have a value."), TEXT("Add Inventory Item:"), MB_OK | MB_ICONERROR);
			return false;
		}
		if (SerialNumber.Contains(L'~') || Description.Contains(L'~') || Group.Contains(L'~'))
		{
			if (PromptErrors)
				MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("There cannot be a Tilde ('~') character in any feild."), TEXT("Add Inventory Item:"), MB_OK | MB_ICONERROR);
			return false;
		}

		if (Inv->Count != 0)
		{
			InventoryItem* TTarget = nullptr;

			if (Target)
				TTarget = dynamic_cast<InventoryItem*>(Target->Target());
			
			for (InventoryItem* Current = Inv->Item(0); Current != nullptr; Current = Current->Next)
			{
				if (Current == TTarget)
					continue;
				
				if (Current->SerialNumber == SerialNumber)
				{
					if (PromptErrors)
						MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The Serial Number provided is non-unique.\n\nPlease provide a unique Serial Number and try again."), TEXT("Add Inventory Item:"), MB_OK | MB_ICONERROR);
					return false;
				}
			}
		}

		if (Group.RemoveWhiteSpace() == String())
		{
			int Result = MessageBox(GetAncestor(_Base, GA_ROOT), TEXT("The group was left blank. Was this intentional?"), TEXT("Add Inventory Item:"), MB_YESNOCANCEL | MB_ICONWARNING);
			if (Result != IDYES)
				return false;
		}

		InventoryItem* New = Target ? dynamic_cast<InventoryItem*>(Target->Target()) : new InventoryItem(_System, Inv);
		New->SerialNumber = SerialNumber;
		New->Description = Description;
		New->Group = Group;
		New->IsInPossession = IsInPossession;

		AppState |= APS_HasEdit;
		EditorRegistry::ResetEditorOfType(EDT_Inventory);

		return true;
	}
	Vector<void*> AddEditInventoryItemEditor::CondenseArgs() const
	{
		return Target ? Target->Target() : Vector<void*>();
	}
	bool AddEditInventoryItemEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Target)
			return Args.Size != 0 && Args[0] == Target->Target();
		return false;
	}
	bool AddEditInventoryItemEditor::EquatableTo(EditorFrame* Other) const
	{
		if (Target)
			return dynamic_cast<AddEditInventoryItemEditor*>(Other) != nullptr && Other->TestOnCondition(CondenseArgs());
		return false;
	}
}