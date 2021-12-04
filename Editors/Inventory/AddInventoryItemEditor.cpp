#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\GroupSelector.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Inventory
{
	using namespace Files;
	using namespace UI::Tool;

	AddInventoryItemEditor::AddInventoryItemEditor()
	{
	}

	LRESULT __stdcall AddInventoryItemEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		AddInventoryItemEditor* This = reinterpret_cast<AddInventoryItemEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void AddInventoryItemEditor::LoadControls()
	{
		int BaseXCoord = 10;
		int BaseYCoord = 110;

		_Loaded = true;

		InventorySystem* File = dynamic_cast<InventorySystem*>(LoadedProject);
		InventoryItemGroup* Inventory = !File ? nullptr : File->InventoryItems;
		if (!File || !Inventory)
			return;

		StringList Groups;
		for (uint i = 0; i < Inventory->Count; i++)
		{
			String Current = Inventory->Item(i)->Group;
			if (!Groups.Contains(Current))
				Groups.Add(Current);
		}

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

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", EditorGrey, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", EditorGrey, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Image:", EditorGrey, TextStyle, false));
			YCoord += 10 + Height;
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", EditorGrey, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 10 + Width;

			Width = WndRect.right - 10 - XCoord;
			TextStyle.Alignment = TA_CenterAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.BaseBackground = EditorGrey;
			Style.Radius = 15;

			SerialNumber = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);

			YCoord += Height + 10;

			Group = new TextBox(XCoord, YCoord, Width - Height - 5, Height, _Base, ins, String(), Style, TextStyle);
			XCoord += (Width - Height);
			SelectGroup = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);

			YCoord += Height + 10;
			XCoord -= (Width - Height);
			
			TargetImage = new UI::ComponentViewer(XCoord, YCoord, Width - Height - 5, Height, _Base, ins, nullptr, nullptr, false);
			XCoord += (Width - Height);
			SelectImage = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)5, ins, Style, TextStyle);

			YCoord += Height + 10;
			XCoord -= (Width - Height);

			IsInPossession = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Is In Possession", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;
			Height = WndRect.bottom - 10 - Height;

			Description = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, true, false);
		}
	}

	LRESULT AddInventoryItemEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			StringList List = GroupSelector::Execute(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), nullptr, GSS_Inventory, false);
			Group->SetText(List.Size == 0 ? String() : List[0]);
			break;
		}
		case 5:
		{
			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_Image;
			Criteria.Multiselect = false;
			Criteria.PreSelected = TargetImage->Target();

			Vector<Component*> Selected = UI::Search::SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

			if (Selected.Size == 0)
				break;

			TargetImage->Target(Selected[0]);
			break;
		}
		}
		return 0;
	}
	LRESULT AddInventoryItemEditor::KeyDown(WPARAM Key)
	{
		if (Key == VK_ESCAPE)
			Command(1, 0);
		else if (Key == 0x50 && (GetKeyState(VK_CONTROL) & 0x8000))
			Command(2, 0);

		return SendMessageW(GetParent(_Base), WM_KEYDOWN, Key, 0);
	}
	LRESULT AddInventoryItemEditor::Size()
	{
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		if (!_Loaded)
			return 0;

		int BaseXCoord = 10;
		int BaseYCoord = 110;

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

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;
			YCoord += 10 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);

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

			TargetImage->Move(XCoord, YCoord, Width - Height - 5, Height);
			XCoord += (Width - Height);
			SelectImage->Move(XCoord, YCoord, Height, Height);

			YCoord += 10 + Height;
			XCoord -= (Width - Height);

			IsInPossession->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;
			Height = WndRect.bottom - 10 - Height;

			Description->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}
	LRESULT AddInventoryItemEditor::Destroy()
	{
		return 0;
	}

	void AddInventoryItemEditor::Reset()
	{
		SerialNumber->SetText(String());
		Description->SetText(String());
		Group->SetText(String());
		IsInPossession->SetCheckState(true);
	}
	bool AddInventoryItemEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		String SerialNumber = this->SerialNumber->GetText();
		String Description = this->Description->GetText();
		String Group = this->Group->GetText();
		bool IsInPossession = this->IsInPossession->GetCheckState();
		Image* TargetImage = dynamic_cast<Image*>(this->TargetImage->Target());

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
				MessageBoxW(NULL, L"The current Project does not contain an Inventory System.", L"Add Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}

		if (SerialNumber == L"")
		{
			if (PromptErrors)
				MessageBoxW(NULL, L"The Serial Number must have a value.", L"Add Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}
		if (SerialNumber.Contains(L'~') || Description.Contains(L'~') || Group.Contains(L'~'))
		{
			if (PromptErrors)
				MessageBoxW(NULL, L"There cannot be a Tilde ('~') character in any feild.", L"Add Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}
		if (Inv->Contains(SerialNumber))
		{
			if (PromptErrors)
				MessageBoxW(NULL, L"The Serial Number provided is non-unique.\n\nPlease provide a unique Serial Number and try again.", L"Add Inventory Item:", MB_OK | MB_ICONERROR);
			return false;
		}

		InventoryItem* New = new InventoryItem(_System, Inv);
		New->SerialNumber = SerialNumber;
		New->Description = Description;
		New->Group = Group;
		New->IsInPossession = IsInPossession;
		New->TargetImage = new ComponentReference(TargetImage);

		HasEdit = true;
		EditorRegistry::ResetEditorOfType(EDT_Inventory);

		return true;
	}
	bool AddInventoryItemEditor::TestOnCondition(Vector<void*> Args) const
	{
		return true;
	}
}