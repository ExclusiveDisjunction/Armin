#include "..\EditorFrame.h"

#include "Sort.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"

namespace Armin::Editors::Users
{
	using namespace Files;
	using namespace UI;

	UserHomepageEditor::UserHomepageEditor(User* Target)
	{
		_Target = Target;
	}

	LRESULT __stdcall UserHomepageEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		UserHomepageEditor* This = reinterpret_cast<UserHomepageEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void UserHomepageEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = 110;
			int Width = 130;
			int Height = 27;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.Alignment = TA_RightAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Current User:", BaseBk, TextStyle, false));
			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;

			CurrentUser = new ComponentViewer(XCoord, YCoord, Width, Height, _Base, ins, nullptr, _Target, true);
			YCoord += 20 + Height;
			XCoord = 10;
			Width = (WndRect.right - 10 - XCoord - 20) / 3;

			SignOut = new Button(XCoord, YCoord, Width, Height, L"Sign Out", _Base, (HMENU)4, ins, Style, TextStyle);
			XCoord += 10 + Width;

			Lock = new Button(XCoord, YCoord, Width, Height, L"Lock", _Base, (HMENU)5, ins, Style, TextStyle);
			XCoord += 10 + Width;

			GoToTasks = new Button(XCoord, YCoord, Width, Height, L"Go To Tasks", _Base, (HMENU)6, ins, Style, TextStyle);
			XCoord += 10 + Width;

			//Timecard = new Button(XCoord, YCoord, Width, Height, L"Timecard", _Base, (HMENU)7, ins, Style, TextStyle);

			XCoord = 10;
			Width = 130;
			YCoord += 10 + Height;
			TextStyle.Alignment = TA_LeftAlignment;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assigned Tasks:", BaseBk, TextStyle, false));
			int ResetY = (YCoord += 10 + Height);
			
			int ButtonSize = 40;
			XCoord = WndRect.right - 10 - ButtonSize;

			CompleteTask = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"CT", _Base, (HMENU)8, ins, Style, TextStyle);
			YCoord += 10 + ButtonSize;

			View = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)9, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			Edit = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"ED", _Base, (HMENU)10, ins, Style, TextStyle);
			EnableWindow(*Edit, (AppState & APS_HasAdminUser));
			YCoord += 10 + ButtonSize;

			SelectAll = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"SA", _Base, (HMENU)11, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			DeSelectAll = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"DS", _Base, (HMENU)12, ins, Style, TextStyle);

			YCoord = ResetY;
			XCoord = 10;
			Width = WndRect.right - 10 - ButtonSize - 10 - XCoord;
			Height = WndRect.bottom - 10 - YCoord;

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);

			FillObjects();
		}
	}
	void UserHomepageEditor::FillObjects()
	{
		TaskSystem* TaskSystem = dynamic_cast<class TaskSystem*>(LoadedProject);
		TaskList* Tasks = !TaskSystem ? nullptr : TaskSystem->Tasks;
		if (!Tasks)
			return;

		Vector<Task*> Assigned;
		for (uint i = 0; i < Tasks->Count; i++)
		{
			Task* Current = Tasks->Item(i);
			if (Current->AssignedTo.Contains(_Target))
				Assigned.Add(Current);
		}

		QuickSort(Assigned, [](Task* One, Task* Two) { return One->DueBy < Two->DueBy; });

		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(Assigned, ObjectView, nullptr, true, true, ObjectScroll);
	}

	LRESULT UserHomepageEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);

		int BaseYCoord = 110;

		{
			int XCoord = 10;
			int YCoord = 110;
			int Width = 130;
			int Height = 27;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;
			Width = WndRect.right - 10 - XCoord;

			CurrentUser->Move(XCoord, YCoord, Width, Height);
			YCoord += 20 + Height;
			XCoord = 10;
			Width = (WndRect.right - 10 - XCoord - 20) / 3;

			SignOut->Move(XCoord, YCoord, Width, Height);
			XCoord += 10 + Width;

			Lock->Move(XCoord, YCoord, Width, Height);
			XCoord += 10 + Width;

			GoToTasks->Move(XCoord, YCoord, Width, Height);
			XCoord += 10 + Width;

			//Timecard->Move(XCoord, YCoord, Width, Height);

			XCoord = 10;
			Width = 130;
			YCoord += 10 + Height;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			int ResetY = (YCoord += 10 + Height);

			int ButtonSize = 40;
			XCoord = WndRect.right - 10 - ButtonSize;

			CompleteTask->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 10 + ButtonSize;

			View->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			Edit->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 10 + ButtonSize;

			SelectAll->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			DeSelectAll->Move(XCoord, YCoord, ButtonSize, ButtonSize);

			YCoord = ResetY;
			XCoord = 10;
			Width = WndRect.right - 10 - ButtonSize - 10 - XCoord;
			Height = WndRect.bottom - 10 - YCoord;

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}

		return 0;
	}
	LRESULT UserHomepageEditor::KeyDown(WPARAM wp)
	{
		switch (wp)
		{
		case 'C':
			Command(8, 0);
			break;
		case 'V':
		case 'E':
			if (GetKeyState(VK_CONTROL) & 0x8000)
				ComponentViewer::OpenSelectedForEditView(Objects, wp == 'E' && (AppState & APS_HasAdminUser));
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}
		return 0;
	}
	LRESULT UserHomepageEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Sign Out
			UserSignOut();
			break;
		case 5: //Lock
			UserLock();
			break;
		case 6: //Tasks
			EditorRegistry::OpenEditor(new Tasks::TasksEditor(nullptr), nullptr);
			break;
		case 7: //Timecard
			//EditorRegistry::OpenEditor(new TimecardsEditor(_Target), nullptr);
			break;
		case 8: //Complete Task
		{
			if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
			{
				Vector<Task*> Tasks;
				for (ComponentViewer* Obj : Objects)
				{
					Task* Conv = dynamic_cast<Task*>(Obj->Target());
					if (Conv)
						Tasks.Add(Conv);
				}

				for (Task* Obj : Tasks)
					EditorRegistry::OpenEditor(new Tasks::CompleteTaskEditor(Obj), nullptr);
			}
			break;
		}
		case 9: //View
		case 10: //Edit
			ComponentViewer::OpenSelectedForEditView(Objects, wp == 9);
			break;
		case 11: //SelectAll
		case 12: //DeSelectAll
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(wp == 11);
			break;
		}
		return 0;
	}

	bool UserHomepageEditor::TestOnCondition(Vector<void*> Args) const
	{
		return Args == CondenseArgs();
	}
	bool UserHomepageEditor::EquatableTo(EditorFrame* Other) const
	{
		return dynamic_cast<UserHomepageEditor*>(Other) != nullptr && Other->CondenseArgs() == CondenseArgs();
	}
	void UserHomepageEditor::Reset()
	{

	}
}