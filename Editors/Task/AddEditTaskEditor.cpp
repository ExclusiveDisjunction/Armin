#include "..\EditorFrame.h"

#include "UI\StyleButton.h"
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

	AddEditTaskEditor::AddEditTaskEditor(Task* ToEdit)
	{
		if (ToEdit)
			Target = ToEdit;
		else
			Target = nullptr;
	}

	LRESULT __stdcall AddEditTaskEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		AddEditTaskEditor* This = reinterpret_cast<AddEditTaskEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(This, Window, Message, wp, lp);
	}

	void AddEditTaskEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		int BaseYCoord = this->BaseYCoord;

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
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Instructions:", BaseBk, TextStyle, false));

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);
			TextStyle.Alignment = TA_LeftAlignment;

			Title = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, Target ? Target->Title() : String(), Style, TextStyle, false, false);
			YCoord += 10 + Height;

			Width -= 5 + Height;

			DueBy = new Label(XCoord, YCoord, Width, Height, _Base, ins, Target ? Target->DueBy.ToString(DateStringFormat::ShortDate) : DateTime().ToString(DateStringFormat::ShortDate), BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			DueBySelect = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)4, ins, Style, TextStyle);

			XCoord -= 5 + Width;
			Width += 5 + Height;
			YCoord += 10 + Height;

			RequiresAssurance = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, Target ? Target->RequiresAssurance : true, L"Requires Assurance", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			Width -= 5 + Height;

			AssignedLabel = new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Collection {Count = " + String(Target ? Target->AssignedTo.Size : 0) + L"}", BaseBk, TextStyle, false);
			XCoord += 5 + Width;
			ModifyAssigned = new Button(XCoord, YCoord, Height, Height, L"...", _Base, (HMENU)5, ins, Style, TextStyle);

			if (Target)
				AssignedTo = ComponentReference::Convert<User>(Target->AssignedTo);

			XCoord -= 5 + Width;
			Width += 5 + Height;
			YCoord += 10 + Height;

			Height = WndRect.bottom - (YCoord + 10);
			Width = WndRect.right - (XCoord + 10);

			Instructions = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, Target ? Target->Instructions : String(), Style, TextStyle, true);
		}
	}

	LRESULT AddEditTaskEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = this->BaseYCoord;

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
			YCoord += 10 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

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

			Width -= 5 + Height;

			AssignedLabel->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			ModifyAssigned->Move(XCoord, YCoord, Height, Height);

			XCoord -= 5 + Width;
			Width += 5 + Height;
			YCoord += 10 + Height;

			Height = WndRect.bottom - (YCoord + 10);
			Width = WndRect.right - (XCoord + 10);

			Instructions->Move(XCoord, YCoord, Width, Height);
		}

		return 0;
	}
	LRESULT AddEditTaskEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT AddEditTaskEditor::Command(WPARAM wp, LPARAM lp)
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
			Vector<Component*> OldItems;
			for (User* Obj : AssignedTo)
				OldItems.Add(Obj);

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_User;
			Criteria.Multiselect = true;
			Criteria.PreSelected = OldItems;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

			AssignedTo.Clear();
			for (Component* Obj : New)
			{
				User* Conv = dynamic_cast<User*>(Obj);
				if (Conv)
					AssignedTo.Add(Conv);
			}

			AssignedLabel->SetText(L"Collection {Count = " + String(AssignedTo.Size) + L"}");
			break;
		}
		}
		return 0;
	}

	void AddEditTaskEditor::Reset()
	{
		Title->SetText(Target ? Target->Title() : String());
		Instructions->SetText(Target ? Target->Instructions : String());
		RequiresAssurance->SetCheckState(Target ? Target->RequiresAssurance : true);
		DueBy->SetText(DateTime().ToString(DateStringFormat::ShortDate));
		
		AssignedTo.Clear();
		if (Target)
			AssignedTo = ComponentReference::Convert<User>(Target->AssignedTo);
		AssignedLabel->SetText(L"Collection {Count = " + String(AssignedTo.Size) + L"}");
	}
	bool AddEditTaskEditor::Apply(ProjectBase* File, bool PromptErrors)
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
		Vector<Files::User*> AssignedTo = this->AssignedTo;

		if (Title.Contains(L'~') || Instructions.Contains(L'~'))
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The title and instructions cannot contain a tilde (\"~\").", L"Add Task:", MB_OK | MB_ICONERROR);
			return false;
		}

		Files::Task* New = Target ? Target : new Task(File, Tasks);
		New->Title(Title);
		New->AssignedTo = ComponentReference::Generate(AssignedTo);
		New->DueBy = DueBy;
		New->Instructions = Instructions;
		New->RequiresAssurance = RequiresAssurance;

		AppState |= APS_HasEdit;
		if (!Target)
			EditorRegistry::ResetEditorOfType(EDT_Tasks);
		EditorRegistry::ResetEditorOfType(EDT_UserHomepage);

		return true;
	}
	bool AddEditTaskEditor::TestOnCondition(Vector<void*> Args) const
	{
		if (Target)
			return Args.Size != 0 && Args[0] == Target;
		else
			return false;
	}
}