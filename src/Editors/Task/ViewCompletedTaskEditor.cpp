#include "..\EditorFrame.h"

#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\ComponentViewer.h"
#include "..\..\UI\Tool\ComponentListRenderer.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Tool;

	ViewCompletedTaskEditor::ViewCompletedTaskEditor(CompletedTask* Target)
	{		
		this->Target = Target;
	}

	LRESULT __stdcall ViewCompletedTaskEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ViewCompletedTaskEditor* This = reinterpret_cast<ViewCompletedTaskEditor*>(GetWindowLongPtr(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ViewCompletedTaskEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);
		const int BaseYCoord = this->BaseYCoord;
		AaColor BaseBk = EditorGrey;

		TextStyleSheet LabelStyle;
		TextStyleSheet TextStyle;
		StyleSheet Style;

		{
			LabelStyle.FontFamily = TextStyle.FontFamily = StandardFontName;
			LabelStyle.FontSize = TextStyle.FontSize = 13;
			LabelStyle.Foreground = TextStyle.Foreground = FontColor;
			LabelStyle.Bold = true;
			TextStyle.Bold = false;

			TextStyle.Alignment = TA_LeftAlignment;
			LabelStyle.Alignment = TA_RightAlignment;

			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;
		}

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Task:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Date Completed:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assured By:", BaseBk, LabelStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Completed By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"References:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Actions Done:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			LabelStyle.Alignment = TA_LeftAlignment;

			Task = new ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, NULL, Target->Tasks->Item(0), false, nullptr, false);
			YCoord += 10 + Height;

			DateCompleted = new Label(XCoord, YCoord, Width, Height, _Base, ins, Target->DateCompleted.ToString(DateStringFormat::ShortDate), BaseBk, TextStyle, false);
			YCoord += 10 + Height;

			AssuredBy = new ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, NULL, Target->AssuredBy->Target(), false, nullptr);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			int NWidth = Width - 5 - ButtonSize;

			CompletedBy = new Label(XCoord, YCoord, NWidth, Height, _Base, ins, L"Collection {Count = " + String(Target->CompletedBy.Size) + L"}", BaseBk, TextStyle, false);
			XCoord += 5 + NWidth;
			ViewCompleted = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)4, ins, Style, TextStyle);
			XCoord -= 5 + NWidth;
			YCoord += 10 + Height;

			References = new Label(XCoord, YCoord, NWidth, Height, _Base, ins, L"Collection {Count = " + String(Target->OperationInventoryItems ? Target->OperationInventoryItems->Count : 0) + L"}", BaseBk, TextStyle, false);
			XCoord += 5 + NWidth;
			ViewReferences = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)5, ins, Style, TextStyle);
			XCoord -= 5 + NWidth;
			YCoord += 10 + Height;

			Width = (WndRect.right / 2) - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ActionsDone = new Label(XCoord, YCoord, Width, Height, _Base, ins, Target->ActionsDone, BaseBk, TextStyle, true);
		}
	}

	LRESULT ViewCompletedTaskEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			new ComponentListRenderer(Target->CompletedBy, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"Completed By:", false);
			break;
		}
		case 5:
		{
			if (!Target->OperationInventoryItems)
				break;
			
			Vector<Component*> Obj;
			for (uint i = 0; i < Target->OperationInventoryItems->Count; i++)
				Obj.Add(Target->OperationInventoryItems->Item(i));

			new ComponentListRenderer(Obj, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"References:", false);
			break;
		}
		}
		return 0;
	}
	LRESULT ViewCompletedTaskEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT ViewCompletedTaskEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);
		const int BaseYCoord = this->BaseYCoord;
		
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[5]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			Task->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			DateCompleted->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			AssuredBy->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			int NWidth = Width - 5 - ButtonSize;

			CompletedBy->Move(XCoord, YCoord, NWidth, Height);
			XCoord += 5 + NWidth;
			ViewCompleted->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord -= 5 + NWidth;
			YCoord += 10 + Height;

			References->Move(XCoord, YCoord, NWidth, Height);
			XCoord += 5 + NWidth;
			ViewReferences->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord -= 5 + NWidth;
			YCoord += 10 + Height;

			Width = (WndRect.right / 2) - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ActionsDone->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}

	bool ViewCompletedTaskEditor::TestOnCondition(Vector<void*> Args) const
	{
		return Args.Size != 0 && Args[0] == static_cast<void*>(Target);
	}
	bool ViewCompletedTaskEditor::EquatableTo(EditorFrame* Other) const
	{
		ViewCompletedTaskEditor* Temp = dynamic_cast<ViewCompletedTaskEditor*>(Other);
		return Temp != nullptr && Temp->Target == Target;
	}
	void ViewCompletedTaskEditor::Reset()
	{

	}
}