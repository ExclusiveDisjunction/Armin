#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\ComponentViewer.h"
#include "..\..\UI\Tool\ComponentListRenderer.h"
#include "..\..\UI\Tool\DatePicker.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
	using namespace Editors::Misc;
	using namespace UI;
	using namespace UI::Search;
	using namespace UI::Tool;

	CompleteTaskEditor::CompleteTaskEditor(Files::Task* Target)
	{		
		_Target = Target;
	}

	LRESULT __stdcall CompleteTaskEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		CompleteTaskEditor* This = reinterpret_cast<CompleteTaskEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(This, Window, Message, wp, lp);
	}

	void CompleteTaskEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);
		AaColor BaseBk = EditorGrey;
		int BaseYCoord = this->BaseYCoord;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BorderThickness = 3;
			Style.BorderBrush = Accent4;
			Style.BaseBackground = BaseBk;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_RightAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assured By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Date Completed:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Completed By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Refers To:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Actions Done:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;
			TextStyle.Alignment = TA_LeftAlignment;

			AssuredBy = new ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, (HMENU)4, nullptr, false);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			Width -= 5 + ButtonSize;

			DateCompletedD = DateTime::Now();
			DateCompleted = new Label(XCoord, YCoord, Width, Height, _Base, ins, DateCompletedD.ToString(DateStringFormat::ShortDate), BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			DateCompletedSelect = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)5, ins, Style, TextStyle);
			XCoord -= 5 + Width;
			YCoord += 10 + Height;

			Width -= 5 + ButtonSize;

			CompletedBy = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Collection {Count = 0}", BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			CompletedBySelect = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)6, ins, Style, TextStyle);
			XCoord += 5 + ButtonSize;
			ViewCompletedBy = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
			XCoord -= (5 + ButtonSize) + (5 + Width);
			YCoord += 10 + Height;

			OperRef = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Collection {Count = 0}", BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			OperRefSelect = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)8, ins, Style, TextStyle);
			XCoord += 5 + ButtonSize;
			ViewRefs = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)9, ins, Style, TextStyle);
			XCoord -= (5 + ButtonSize) + (5 + Width);
			YCoord += 10 + Height;

			Width = WndRect.right - 10 - XCoord;
			Height = WndRect.bottom - 10 - YCoord;

			ActionReport = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, true);
		}
	}

	LRESULT CompleteTaskEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);
		int BaseYCoord = this->BaseYCoord;

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

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;

			AssuredBy->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			Width -= 5 + ButtonSize;

			DateCompleted->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			DateCompletedSelect->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord -= 5 + Width;
			YCoord += 10 + Height;

			Width -= 5 + ButtonSize;

			CompletedBy->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			CompletedBySelect->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord += 5 + ButtonSize;
			ViewCompletedBy->Move(XCoord, YCoord, Width, Height);
			XCoord -= (5 + ButtonSize) + (5 + Width);
			YCoord += 10 + Height;

			OperRef->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			OperRefSelect->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord += 5 + ButtonSize;
			ViewRefs->Move(XCoord, YCoord, Width, Height);
			XCoord -= (5 + ButtonSize) + (5 + Width);
			YCoord += 10 + Height;

			Width = WndRect.right - 10 - XCoord;
			Height = WndRect.bottom - 10 - YCoord;

			ActionReport->Move(XCoord, YCoord, Width, Height);
		}
		return 0;
	}
	LRESULT CompleteTaskEditor::KeyDown(WPARAM wp)
	{
		return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
	}
	LRESULT CompleteTaskEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Assured By
			AssuredBy->Target(SignInForAssurance());
			break;
		case 5: //Select Date
			DateCompletedD = DatePicker::Execute(DateCompletedD, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			break;
		case 6: //CompletedBySelect
		{
			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_User;
			Criteria.Multiselect = true;
			Criteria.PreSelected = CompletedByD;

			CompletedByD = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			CompletedBy->SetText(L"Collection {Count = " + String(CompletedByD.Size) + L"}");
			break;
		}
		case 7: //ViewCompleted
		{
			new Tool::ComponentListRenderer(CompletedByD, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"Completed By");
			break;
		}
		case 8: //OperRefSelect
		{
			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_OperationInventoryItem;
			Criteria.Multiselect = true;
			Criteria.PreSelected = OperRefD;

			OperRefD = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			OperRef->SetText(L"Collection {Count = " + String(OperRefD.Size) + L"}");
			break;
		}
		case 9: //ViewRefs
		{
			new Tool::ComponentListRenderer(OperRefD, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"References");
			break;
		}
		}

		return 0;
	}

	void CompleteTaskEditor::Reset()
	{
		DateCompleted->SetText(DateTime().ToString(DateStringFormat::ShortDate));
		DateCompletedD = DateTime();
		AssuredBy->Target(nullptr);
		TargetTask->Target(_Target);
	}
	bool CompleteTaskEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		if (!File)
			return false;

		DateTime DateCompleted = DateCompletedD;
		Vector<Component*> CompletedBy = CompletedByD;
		Vector<Component*> References = OperRefD;
		String ActionsDone = this->ActionReport->GetText();

		TaskSystem* Parent = dynamic_cast<TaskSystem*>(File);
		CompletedTaskList* ParentList = !Parent ? nullptr : Parent->CompletedTasks;
		if (!ParentList)
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The current file does not support Completed Tasks.", L"Error:", MB_OK | MB_ICONERROR);
			return false;
		}

		CompletedTask* Task = new CompletedTask(File, ParentList);
		Task->DateCompleted = DateCompleted;
		Task->CompletedBy = CompletedBy;
		Task->ActionsDone = ActionsDone;

		OperationInventoryItemGroup* List = Task->OperationInventoryItems;
		for (Component* Obj : References)
			new OperationInventoryItem(File, List, dynamic_cast<OperationInventoryItem*>(Obj));

		if (Task->Tasks)
			new class Task(File, Task->Tasks, _Target);
		return true;
	}
	bool CompleteTaskEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Args.Size == 0)
			return false;

		return Args[0] == static_cast<void*>(_Target);
	}
}