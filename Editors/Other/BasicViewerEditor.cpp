#include "..\EditorFrame.h"

#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\ComponentListRenderer.h"

namespace Armin::Editors::Misc
{
	using namespace Files;	

	BasicViewerEditor::BasicViewerEditor(Component* Target)
	{
		ThisTarget = Target;		
	}

	LRESULT __stdcall BasicViewerEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		BasicViewerEditor* This = reinterpret_cast<BasicViewerEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void BasicViewerEditor::LoadControls()
	{
		if (_Loaded || !ThisTarget)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = this->BaseYCoord;
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

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Target Component:", 0xFF313131, TextStyle, false));

			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;

			Target = new UI::ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, NULL, ThisTarget, _DummySelect, false, true);
			BaseYCoord = YCoord += 40 + Height;
		}

		if (ThisTarget->ObjectType() == CT_InventoryItem)
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
			Style.Background = Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Is In Possession:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
			YCoord += (5 + Height);

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;
			Style.Background = Grey3;
			BaseBk = Grey3;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->SerialNumber, BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, (ConvTarget->IsInPossession ? L"Yes" : L"No"), BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Group, BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Height = WndRect.bottom - 10 - YCoord;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Description, BaseBk, TextStyle, true));
		}
		else if (ThisTarget->ObjectType() == CT_OperationInventoryItem)
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
			Style.Background = Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Serial Number:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Status:", BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group:", BaseBk, TextStyle, false));
			YCoord += (5 + Height);

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Description:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;
			TextStyle.Bold = false;
			Style.Background = Grey3;
			BaseBk = Grey3;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->SerialNumber, BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, OperationInventoryItem::ISToString(ConvTarget->CurrentState), BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Group, BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Height = WndRect.bottom - 10 - YCoord;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Description, BaseBk, TextStyle, true));
		}
		else if (ThisTarget->ObjectType() == CT_JobPosition)
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
			Style.Background = Grey1;
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
			Style.Background = Grey3;
			BaseBk = Grey3;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Title(), BaseBk, TextStyle, false));
			YCoord += 5 + Height;

			Height = WndRect.bottom - 10 - YCoord;

			Data.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, ConvTarget->Description, BaseBk, TextStyle, true));
		}
	}

	LRESULT BasicViewerEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
			break;
		}
		return 0;
	}
	LRESULT BasicViewerEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT BasicViewerEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = this->BaseYCoord;
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
			BaseYCoord = YCoord += 40 + Height;
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
			YCoord += (5 + Height);

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			Data[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Data[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Data[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			CV1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - 10 - YCoord;

			Data[3]->Move(XCoord, YCoord, Width, Height);
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
			YCoord += (5 + Height);

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			Data[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Data[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Data[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			CV1->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - 10 - YCoord;

			Data[3]->Move(XCoord, YCoord, Width, Height);
		}
		else if (ThisTarget->ObjectType() == CT_JobPosition)
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = BaseWidth;
			int Height = BaseHeight;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			Data[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - 10 - YCoord;

			Data[1]->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}
	LRESULT BasicViewerEditor::Destroy()
	{
		return 0;
	}

	void BasicViewerEditor::Reset()
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

			Data[0]->SetText(ConvTarget->SerialNumber);
			Data[1]->SetText(ConvTarget->Group);
			Data[2]->SetText(ConvTarget->IsInPossession ? L"Yes" : L"No");
			Data[3]->SetText(ConvTarget->Description);
			break;
		}
		case CT_OperationInventoryItem:
		{
			OperationInventoryItem* ConvTarget = dynamic_cast<OperationInventoryItem*>(ThisTarget);
			if (!ConvTarget)
				break;

			Data[0]->SetText(ConvTarget->SerialNumber);
			Data[1]->SetText(OperationInventoryItem::ISToString(ConvTarget->CurrentState));
			Data[1]->SetText(ConvTarget->Group);
			Data[2]->SetText(ConvTarget->Description);
			break;
		}
		case CT_JobPosition:
		{
			JobPosition* ConvTarget = dynamic_cast<JobPosition*>(ThisTarget);
			if (!ConvTarget)
				break;

			Data[0]->SetText(ConvTarget->Title());
			Data[1]->SetText(ConvTarget->Description);
			break;
		}
		}
	}
	bool BasicViewerEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Args.Size == 0)
			return false;

		return Args[0] == static_cast<void*>(this->ThisTarget);
	}
	bool BasicViewerEditor::EquatableTo(EditorFrame* Obj) const
	{
		BasicViewerEditor* Item = dynamic_cast<BasicViewerEditor*>(Obj);
		if (!Item)
			return false;

		return Item->ThisTarget = ThisTarget;
	}
	String BasicViewerEditor::GetName() const
	{
		switch (ThisTarget->ObjectType())
		{
		case CT_JobPosition:
			return L"View Job Position";
		case CT_InventoryItem:
			return L"View Inventory Item";
		case CT_OperationInventoryItem:
			return L"View Operation Inventory Item";
		default:
			return L"Basic Viewer";
		}
	}
}