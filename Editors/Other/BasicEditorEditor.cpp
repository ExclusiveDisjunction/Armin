#include "..\EditorFrame.h"

#include "..\..\Editors\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\ComponentListRenderer.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Misc
{
	using namespace Files;
	using namespace UI::Search;

	BasicEditorEditor::BasicEditorEditor(Component* Target)
	{
		ThisTarget = Target;		
	}

	LRESULT __stdcall BasicEditorEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		BasicEditorEditor* Item = reinterpret_cast<BasicEditorEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(Item, Window, Message, wp, lp);
	}

	void BasicEditorEditor::ProcessCommand(WPARAM wp)
	{
		if (ThisTarget->ObjectType() == CT_InventoryItem || ThisTarget->ObjectType() == CT_OperationInventoryItem)
		{
			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_Image;
			Criteria.Multiselect = false;
			Criteria.PreSelected = CV1->Target();

			Vector<Component*> Objects = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			CV1->Target(Objects.Size == 0 ? nullptr : Objects[0]);
		}
	}
	void BasicEditorEditor::LoadControls()
	{
		if (_Loaded || !ThisTarget)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = 110;
		int BaseWidth = 175;
		int BaseHeight = 27;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int& YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.Bold = true;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_RightAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Target Component:", 0xFF313131, TextStyle, false));

			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;

			Target = new UI::ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, NULL, ThisTarget, _DummySelect, false, true);
			BaseYCoord = YCoord += 40 + Height;
		}

		switch (ThisTarget->ObjectType())
		{
		case CT_InventoryItem:
		{
			InventoryItem* ConvTarget = dynamic_cast<InventoryItem*>(ThisTarget);
			if (!ConvTarget)
				return;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Image:", BaseBk, TextStyle, false));
			YCoord += (5 + Height) * 2; //A gap for the IsInPossession

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;
			Style.Background = Grey3;

			TB1 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->SerialNumber, Style, TextStyle);
			YCoord += 5 + Height;

			TB2 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Group, Style, TextStyle);
			YCoord += 5 + Height;

			CV1 = new UI::ComponentViewer(XCoord, YCoord, Width - Height - 5, Height, _Base, ins, nullptr, !ConvTarget->TargetImage ? nullptr : ConvTarget->TargetImage->Target(), false);
			B1 = new Button(XCoord + (Width - Height), YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);
			YCoord += 5 + Height;

			CB1 = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, ConvTarget->IsInPossession, L"Is In Possession", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (10 + YCoord);

			TB3 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Description, Style, TextStyle, true);
			break;
		}
		case CT_OperationInventoryItem:
		{
			OperationInventoryItem* ConvTarget = dynamic_cast<OperationInventoryItem*>(ThisTarget);
			if (!ConvTarget)
				return;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			AaColor BaseBk = EditorGrey;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Status:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Image:", BaseBk, TextStyle, false));
			YCoord += (5 + Height);

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;
			Style.Background = Grey3;

			TB1 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->SerialNumber, Style, TextStyle);
			YCoord += 5 + Height;

			DD1 = new ComboBox(XCoord, YCoord, Width, Height, _Base, NULL, ins, { L"Working Order", L"Partial Working Order", L"Broken", L"Under Repair", L"Not In Possession" }, Style, TextStyle);
			DD1->Seek(OperationInventoryItem::ISToString(ConvTarget->CurrentState));
			YCoord += 5 + Height;

			CV1 = new UI::ComponentViewer(XCoord, YCoord, Width - Height - 5, Height, _Base, ins, nullptr, !ConvTarget->TargetImage ? nullptr : ConvTarget->TargetImage->Target(), false);
			B1 = new Button(XCoord + (Width - Height), YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);
			YCoord += 5 + Height;

			TB2 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Group, Style, TextStyle);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (10 + YCoord);

			TB3 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Description, Style, TextStyle, true);
			break;
		}
		case CT_JobPosition:
		{
			JobPosition* ConvTarget = dynamic_cast<JobPosition*>(ThisTarget);
			if (!ConvTarget)
				return;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			AaColor BaseBk = EditorGrey;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;

			TB1 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Title(), Style, TextStyle);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (10 + YCoord);

			TB2 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Description, Style, TextStyle, true);
			break;
		}
		case CT_Image:
		{
			Image* ConvTarget = dynamic_cast<Image*>(ThisTarget);
			if (!ConvTarget)
				return;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = true;

			AaColor BaseBk = EditorGrey;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));

			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;

			TB1 = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Title(), Style, TextStyle);
			break;
		}
		}
	}

	LRESULT BasicEditorEditor::Command(WPARAM wp, LPARAM lp)
	{
		if (wp >= 4 && wp <= 7)
			ProcessCommand(wp);

		return 0;
	}
	LRESULT BasicEditorEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT BasicEditorEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = 110;
		int BaseWidth = 175;
		int BaseHeight = 27;

		{
			int XCoord = 10;
			int& YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);

			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;

			Target->Move(XCoord, YCoord, Width, Height);
			YCoord += 40 + Height;
		}

		if (ThisTarget->ObjectType() == CT_InventoryItem)
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);
			YCoord += (5 + Height) * 2; //A gap for the IsInPossession

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			TB1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			TB2->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			CV1->Move(XCoord, YCoord, Width - Height - 5, Height);
			B1->Move(XCoord + (Width - Height), YCoord, Height, Height);
			YCoord += 10 + Height;

			CB1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (10 + YCoord);

			TB3->Move(XCoord, YCoord, Width, Height);
		}
		else if (ThisTarget->ObjectType() == CT_OperationInventoryItem)
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);
			YCoord += (5 + Height);

			MiscControls[5]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			TB1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			DD1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			CV1->Move(XCoord, YCoord, Width - Height - 5, Height);
			B1->Move(XCoord + (Width - Height), YCoord, Height, Height);

			TB2->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (10 + YCoord);

			TB3->Move(XCoord, YCoord, Width, Height);
		}
		if (ThisTarget->ObjectType() == CT_JobPosition)
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);

			YCoord -= 5 + Height;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			TB1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (10 + YCoord);

			TB2->Move(XCoord, YCoord, Width, Height);
		}
		if (ThisTarget->ObjectType() == CT_Image)
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);

			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			TB1->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}
	LRESULT BasicEditorEditor::Destroy()
	{
		return 0;
	}

	void BasicEditorEditor::Reset()
	{
		if (Target)
			Target->Target(ThisTarget);
		
		switch (ThisTarget->ObjectType())
		{
		case CT_InventoryItem:
		{
			InventoryItem* ConvTarget = dynamic_cast<InventoryItem*>(ThisTarget);
			if (!ConvTarget)
				break;

			TB1->SetText(ConvTarget->SerialNumber);
			TB2->SetText(ConvTarget->Group);
			CB1->SetCheckState(ConvTarget->IsInPossession);
			TB3->SetText(ConvTarget->Description);
			break;
		}
		case CT_OperationInventoryItem:
		{
			OperationInventoryItem* ConvTarget = dynamic_cast<OperationInventoryItem*>(ThisTarget);
			if (!ConvTarget)
				break;

			TB1->SetText(ConvTarget->SerialNumber);
			DD1->Seek(OperationInventoryItem::ISToString(ConvTarget->CurrentState));
			TB2->SetText(ConvTarget->Group);
			TB3->SetText(ConvTarget->Description);
			break;
		}
		case CT_JobPosition:
		{
			JobPosition* ConvTarget = dynamic_cast<JobPosition*>(ThisTarget);
			if (!ConvTarget)
				break;

			TB1->SetText(ConvTarget->Title());
			TB2->SetText(ConvTarget->Description);
			break;
		}
		case CT_Image:
		{
			break;
		}
		}

	}
	bool BasicEditorEditor::Apply(ArminSessionBase* File, bool PromptErrors)
	{
		if (!ThisTarget)
			return false;

		switch (ThisTarget->ObjectType())
		{
		case CT_InventoryItem:
		{
			InventoryItem* ConvTarget = dynamic_cast<InventoryItem*>(ThisTarget);
			if (!ConvTarget)
			{
				if (PromptErrors)
					MessageBox(GetAncestor(_Base, GA_ROOT), L"There was an unexpected error, this a critical error and you should report it idmediately.", L"Armin:", MB_OK | MB_ICONERROR);
				return false;
			}

			String SerialNumber = TB1->GetText();
			String Group = TB2->GetText();
			bool Possession = CB1->GetCheckState();
			String Description = TB3->GetText();
			Image* TargetImage = dynamic_cast<Image*>(CV1->Target());

			InventorySystem* File = dynamic_cast<InventorySystem*>(LoadedSession);
			InventoryItemGroup* Inv = !File ? nullptr : File->InventoryItems;
			if (!File || !Inv)
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"The current Project does not contain an Inventory System.", L"Edit Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}

			if (SerialNumber == L"")
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"The Serial Number must have a value.", L"Edit Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}
			if (SerialNumber.Contains(L'~') || Description.Contains(L'~') || Group.Contains(L'~'))
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"There cannot be a Tilde ('~') character in any feild.", L"Edit Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}
			if (Inv->Contains(SerialNumber) && SerialNumber != ConvTarget->SerialNumber)
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"The Serial Number provided is non-unique.\n\nPlease provide a unique Serial Number and try again.", L"Edit Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}

			ConvTarget->SerialNumber = SerialNumber;
			ConvTarget->Group = Group;
			ConvTarget->IsInPossession = Possession;
			ConvTarget->Description = Description;
			if (ConvTarget->TargetImage)
				ConvTarget->TargetImage->SetTarget(TargetImage);
			else
				ConvTarget->TargetImage = new ComponentReference(TargetImage);

			HasEdit = true;
			EditorRegistry::ResetEditorOfType(EDT_Inventory);
			break;
		}
		case CT_OperationInventoryItem:
		{
			OperationInventoryItem* ConvTarget = dynamic_cast<OperationInventoryItem*>(ThisTarget);
			if (!ConvTarget)
			{
				if (PromptErrors)
					MessageBox(GetAncestor(_Base, GA_ROOT), L"There was an unexpected error, this a critical error and you should report it idmediately.", L"Edit Operation Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}

			String SerialNumber = TB1->GetText();
			String Group = TB2->GetText();
			String Description = TB3->GetText();
			String RawState = DD1->Current();
			Image* TargetImage = dynamic_cast<Image*>(CV1->Target());

			OperationInventoryItem::ItemState NewState = OperationInventoryItem::IS_NotInPossession;
			if (RawState == L"Working Order")
				NewState = OperationInventoryItem::IS_WorkingOrder;
			else if (RawState == L"Partial Working Order")
				NewState = OperationInventoryItem::IS_PartialWorkingOrder;
			else if (RawState == L"Broken")
				NewState = OperationInventoryItem::IS_Broken;
			else if (RawState == L"Under Repair")
				NewState = OperationInventoryItem::IS_UnderRepair;
			else
				NewState = OperationInventoryItem::IS_NotInPossession;

			InventorySystem* File = dynamic_cast<InventorySystem*>(LoadedSession);
			OperationInventoryItemGroup* Inv = !File ? nullptr : File->OperationInventoryItems;
			if (!File || !Inv)
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"The current Project does not contain an Operational Inventory System.", L"Edit Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}

			if (SerialNumber == L"")
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"The Serial Number must have a value.", L"Armin:", MB_OK | MB_ICONERROR);
				return false;
			}
			if (SerialNumber.Contains(L'~') || Description.Contains(L'~') || Group.Contains(L'~'))
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"There cannot be a Tilde ('~') character in any feild.", L"Edit Operation Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}
			if (Inv->Contains(SerialNumber) && ConvTarget->SerialNumber != SerialNumber)
			{
				if (PromptErrors)
					MessageBoxW(NULL, L"The Serial Number provided is non-unique.\n\nPlease provide a unique Serial Number and try again.", L"Edit Operation Inventory Item:", MB_OK | MB_ICONERROR);
				return false;
			}

			ConvTarget->SerialNumber = SerialNumber;
			ConvTarget->Description = Description;
			ConvTarget->Group = Group;
			ConvTarget->CurrentState = NewState;
			if (ConvTarget->TargetImage)
				ConvTarget->TargetImage->SetTarget(TargetImage);
			else
				ConvTarget->TargetImage = new ComponentReference(TargetImage);

			HasEdit = true;
			EditorRegistry::ResetEditorOfType(EDT_OperationInventory);

			break;
		}
		case CT_JobPosition:
		{
			JobPosition* ConvTarget = dynamic_cast<JobPosition*>(ThisTarget);
			if (!ConvTarget)
			{
				if (PromptErrors)
					MessageBox(GetAncestor(_Base, GA_ROOT), L"There was an unexpected error, this a critical error and you should report it idmediately.", L"Edit Job Position:", MB_OK | MB_ICONERROR);
				return false;
			}

			String Title = TB1->GetText();
			String Description = TB2->GetText();

			if (Title == L"")
			{
				if (PromptErrors)
					MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The Title of a Job Position cannot be empty.\n\nPlease provide a value and try again.", L"Edit Job Position:", MB_OK | MB_ICONERROR);
				return false;
			}
			if (Title.Contains(L'~') || Description.Contains(L'~'))
			{
				if (PromptErrors)
					MessageBoxW(GetAncestor(_Base, GA_ROOT), L"A tilde ('~') is not allowed in any feild. Please remove all tildes and try again.", L"Edit Job Position:", MB_OK | MB_ICONERROR);
				return false;
			}

			ConvTarget->Title(Title);
			ConvTarget->Description = Description;

			HasEdit = true;
			EditorRegistry::ResetEditorOfType(EDT_JobPositions);
			break;
		}
		case CT_Image:
		{
			Image* ConvTarget = dynamic_cast<Image*>(ThisTarget);
			ConvTarget->Title(TB1->GetText());
			break;
		}
		}

		return true;
	}
	bool BasicEditorEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Args.Size == 0)
			return false;

		return static_cast<void*>(ThisTarget) == Args[0];
	}
	String BasicEditorEditor::GetName() const
	{
		switch (ThisTarget->ObjectType())
		{
		case CT_JobPosition:
			return L"Edit Job Position";
		case CT_InventoryItem:
			return L"Edit Inventory Item";
		case CT_OperationInventoryItem:
			return L"Edit Operation Inventory Item";
		default:
			return L"Basic Editor";
		}
	}
}