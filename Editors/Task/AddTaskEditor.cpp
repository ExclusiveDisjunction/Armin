#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\ComponentViewer.h"
#include "..\..\UI\Tool\DatePicker.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;
	using namespace UI::Tool;

	AddTaskEditor::AddTaskEditor()
	{

	}

	LRESULT __stdcall AddTaskEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		AddTaskEditor* This = reinterpret_cast<AddTaskEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(This, Window, Message, wp, lp);
	}

	void AddTaskEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		int BaseYCoord = 110;

		LoadUpperButtons(WndRect, ins);

		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 5;
			int YCoord = BaseYCoord;
			int Width = 120;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_RightAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Due By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			YCoord += 10 + Height; //Space for requires assurance

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assigned To:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;

			Title = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, false, false);
			YCoord += 10 + Height;

			Width -= 5 + Height;

			DueBy = new Label(XCoord, YCoord, Width, Height, _Base, ins, DateTime().ToString(DateStringFormat::ShortDate), BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			DueBySelect = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);

			XCoord -= 5 + Width;
			Width += 5 + Height;
			YCoord += 10 + Height;

			RequiresAssurance = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, true, L"Requires Assurance", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			ModifyAssigned = new Button(XCoord, YCoord, Width, Height, L"Assign Users", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 10 + Height;

			Height = WndRect.bottom - 10 - YCoord;
			AssignedScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			AssignedView = new Grid(0, 0, Width, Height, AssignedScroll, ins, Style);
			AssignedScroll->SetViewer(AssignedView);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 120;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_LeftAlignment;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Instructions:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			Height = WndRect.bottom - (YCoord + 10);
			Width = WndRect.right - (XCoord + 10);

			Instructions = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle, true);
		}
	}

	LRESULT AddTaskEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = 110;

		{
			int XCoord = 5;
			int YCoord = BaseYCoord;
			int Width = 120;
			int Height = 27;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			YCoord += 10 + Height; //Space for requires assurance

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Title->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Width -= 5 + Height;

			DueBy->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			DueBySelect->Move(XCoord, YCoord, Height, Height);

			XCoord -= 5 + Width;
			Width += 5 + Height;
			YCoord += 10 + Height;

			RequiresAssurance->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			ModifyAssigned->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Height = WndRect.bottom - 10 - YCoord;
			AssignedScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 120;
			int Height = 27;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);
			YCoord += 5 + Height;

			Height = WndRect.bottom - (YCoord + 10);
			Width = WndRect.right - (XCoord + 10);

			Instructions->Move(XCoord, YCoord, Width, Height);
		}

		{
			int XCoord = 5;
			int YCoord = 5;
			int Width = 900;
			int Height = 27;

			RECT Client;
			GetClientRect(AssignedView, &Client);

			AssignedView->Move(0, 0, Width + 10, static_cast<int>((Height + 5) * (AssignedTo.Size + 1)));
			AssignedScroll->Reset();

			for (uint i = 0; i < AssignedTo.Size; i++)
			{
				AssignedTo[i]->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;
			}
		}

		return 0;
	}
	LRESULT AddTaskEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT AddTaskEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4:
		{
			DueByD = DatePicker::Execute(DueByD, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			this->DueBy->SetText(DueByD.ToString(DateStringFormat::ShortDate));
			break;
		}
		case 5:
		{
			Vector<Component*> OldItems = ComponentViewer::GetAllComponents(AssignedTo);

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_User;
			Criteria.Multiselect = true;
			Criteria.PreSelected = OldItems;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

			CloseControls(AssignedTo);
			AssignedTo = ComponentViewer::GenerateList(New, AssignedView, NULL, _IsMultiselect, true, AssignedScroll);
			break;
		}
		}
		return 0;
	}

	void AddTaskEditor::Reset()
	{
		Title->SetText(String());
		Instructions->SetText(String());
		CloseControls(AssignedTo);
		RequiresAssurance->SetCheckState(true);
		DueBy->SetText(DateTime().ToString(DateStringFormat::ShortDate));
		_IsMultiselect = false;
	}
	bool AddTaskEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		TaskSystem* TaskFile = dynamic_cast<class TaskSystem*>(LoadedProject);
		TaskList* Tasks = !TaskFile ? nullptr : TaskFile->Tasks;
		if (!TaskFile || !Tasks)
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The current project does not support a task system.", L"Add Task:", MB_OK | MB_ICONERROR);
			return false;
		}

		String Title = this->Title->GetText();
		String Instructions = this->Instructions->GetText();
		DateTime DueBy = DueByD;
		bool RequiresAssurance = this->RequiresAssurance->GetCheckState();

		Vector<Component*> RawAssignedTo = ComponentViewer::RetriveFromList(AssignedTo);
		Vector<Files::User*> AssignedTo;
		for (uint i = 0; i < RawAssignedTo.Size; i++)
		{
			Files::User* Conv = dynamic_cast<Files::User*>(RawAssignedTo[i]);
			if (Conv)
				AssignedTo.Add(Conv);
		}

		if (Title.Contains(L'~') || Instructions.Contains(L'~'))
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The title and instructions cannot contain a tilde (\"~\").", L"Add Task:", MB_OK | MB_ICONERROR);
			return false;
		}

		Files::Task* New = new Task(File, Tasks);
		New->Title(Title);
		New->AssignedTo = ComponentReference::Generate(AssignedTo);
		New->DueBy = DueBy;
		New->Instructions = Instructions;
		New->RequiresAssurance = RequiresAssurance;

		HasEdit = true;
		EditorRegistry::ResetEditorOfType(EDT_Tasks);
		EditorRegistry::ResetEditorOfType(EDT_UserHomepage);

		return true;
	}
	bool AddTaskEditor::TestOnCondition(Vector<void*> Args) const
	{
		return true;
	}
}