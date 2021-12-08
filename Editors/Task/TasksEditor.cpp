#include "..\EditorFrame.h"

#include "Sort.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\ComponentViewer.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
	using namespace UI;

	TasksEditor::TasksEditor(TaskSystem* System)
	{
		if (!System)
			_System = dynamic_cast<TaskSystem*>(LoadedProject);
		else
			_System = System;
	}

	LRESULT __stdcall TasksEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		TasksEditor* This = reinterpret_cast<TasksEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(This, Window, Message, wp, lp);
	}

	void TasksEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);
		int BaseYCoord = 110;

		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.Bold = true;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int ResetY = 0;

			{
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Task Count:", BaseBk, TextStyle, false));

				TextStyle.Bold = false;
				TextStyle.Alignment = TA_LeftAlignment;
				ObjectCount = new Label(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height, _Base, ins, L"0", BaseBk, TextStyle, false);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;
				XCoord += Width / 2;

				Search = new Button(XCoord, YCoord, Width, Height, L"Task Search", _Base, (HMENU)4, ins, Style, TextStyle);
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				Add = new Button(XCoord, YCoord, Width, Height, L"+", _Base, (HMENU)5, ins, Style, TextStyle);
				EnableWindow(*Add, UserRegistry::CurrentUserType() == UT_Admin);
				YCoord += 5 + ButtonSize;

				Remove = new Button(XCoord, YCoord, Width, Height, L"-", _Base, (HMENU)6, ins, Style, TextStyle);
				EnableWindow(*Remove, UserRegistry::CurrentUserType() == UT_Admin);
				YCoord += 10 + ButtonSize;

				View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				Edit = new Button(XCoord, YCoord, Width, Height, L"ED", _Base, (HMENU)8, ins, Style, TextStyle);
				EnableWindow(*Edit, UserRegistry::CurrentUserType() == UT_Admin);
				YCoord += 10 + ButtonSize;

				SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)9, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DA", _Base, (HMENU)10, ins, Style, TextStyle);
				YCoord += 10 + ButtonSize;

				CompleteTask = new Button(XCoord, YCoord, Width, Height, L"CT", _Base, (HMENU)11, ins, Style, TextStyle);

				Width = OldWidth;
				Height = OldHeight;
			}

			YCoord = ResetY;
			XCoord = 10;
			Width = WndRect.right - (10 + XCoord + ButtonSize + 10);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);
			FillObjects();
		}
	}
	void TasksEditor::FillObjects()
	{
		TaskList* Tasks = !_System ? nullptr : _System->Tasks;
		if (!Tasks)
			return;

		Vector<Task*> SortedTasks = *Tasks;

		QuickSort(SortedTasks, [](Task*& One, Task*& Two) { return One->DueBy < Two->DueBy; });

		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(SortedTasks, ObjectView, NULL, true, true, ObjectScroll);
		this->ObjectCount->SetText(SortedTasks.Size);
	}

	LRESULT TasksEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		MoveUpperButtons(WndRect);
		int BaseYCoord = 110;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			int ResetY = 0;

			{
				MiscControls[0]->Move(XCoord, YCoord, Width, Height);

				ObjectCount->Move(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;
				XCoord += Width / 2;

				Search->Move(XCoord, YCoord, Width, Height);
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				Add->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + ButtonSize;

				Remove->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + ButtonSize;

				View->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + ButtonSize;

				Edit->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + ButtonSize;

				SelectAll->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + ButtonSize;

				DeSelectAll->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + ButtonSize;

				CompleteTask->Move(XCoord, YCoord, Width, Height);

				Width = OldWidth;
				Height = OldHeight;
			}

			YCoord = ResetY;
			XCoord = 10;
			Width = WndRect.right - (10 + XCoord + ButtonSize + 10);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}

		return 0;
	}
	LRESULT TasksEditor::KeyDown(WPARAM wp)
	{
		switch (wp)
		{
		case VK_RETURN:
			FillObjects();
			break;
		case VK_DELETE:
			Command(6, 0);
			break;
		case 'N':
			if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
				Command(5, 0);
			break;
		case 'V':
		case 'E':
			if (GetKeyState(VK_CONTROL) & 0x8000)
				ComponentViewer::OpenSelectedForEditView(Objects, wp == 'E' && UserRegistry::CurrentUserType() == UT_Admin);
			break;
		case 'C':
			if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_SHIFT) & 0x8000))
				Command(11, 0);
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}
		return 0;
	}
	LRESULT TasksEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //TaskSearch
			EditorRegistry::OpenEditor(new Misc::QuickSearchEditor(CT_Task), nullptr);
			break;
		case 5: //Add
			if (UserRegistry::CurrentUserType() == UT_Admin)
				EditorRegistry::OpenEditor(new AddTaskEditor(), nullptr);
			break;
		case 6: //Remove
		{
			if (UserRegistry::CurrentUserType() != UT_Admin)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"You do not have the proper user level to remove tasks.", L"Remove:", MB_OK | MB_ICONERROR);
				break;
			}

			Vector<ComponentViewer*> Viewers;
			Vector<Component*> Targets = ComponentViewer::RetriveFromList(Objects, Viewers);

			if (Targets.Size == 0)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please select at least one object to remove.", L"Remove Task:", MB_OK | MB_ICONERROR);
				break;
			}

			int Result = MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The following data will be remove permanantly.\n\nDo you want to continue?", L"Remove Tasks:", MB_YESNOCANCEL | MB_ICONWARNING);
			if (Result != IDYES)
				break;

			for (uint i = 0; i < Targets.Size; i++)
			{
				Task* Conv = dynamic_cast<Task*>(Targets[i]);
				if (!Conv)
					continue;

				delete Conv;

				ComponentViewer* Viewer = Viewers[i];
				Objects.Remove(Viewer);
				delete Viewer;
			}

			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);

			AppState |= APS_HasEdit;
			break;
		}
		case 7: //View
		case 8: //Edit
			ComponentViewer::OpenSelectedForEditView(Objects, wp == 8 && UserRegistry::CurrentUserType() == UT_Admin);
			break;
		case 9: //SelectAll
		case 10: //DeSelectAll
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(wp == 9);
			break;
		case 11: //CompleteTask
		{
			Vector<Component*> Raw = ComponentViewer::RetriveFromList(Objects);

			for (uint i = 0; i < Raw.Size; i++)
			{
				Files::Task* Conv = dynamic_cast<Files::Task*>(Raw[i]);
				if (Conv)
					EditorRegistry::OpenEditor(new CompleteTaskEditor(Conv), nullptr);
			}
			break;
		}
		}
		return 0;
	}

	void TasksEditor::Reset()
	{
		FillObjects();
	}
}