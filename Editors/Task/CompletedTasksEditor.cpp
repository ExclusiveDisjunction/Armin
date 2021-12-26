#include "..\EditorFrame.h"

#include "Sort.h"
#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\ComponentViewer.h"

namespace Armin::Editors::Tasks
{
	using namespace Files;
	using namespace UI;

	CompletedTasksEditor::CompletedTasksEditor(TaskSystem* System)
	{
		if (!System)
			_System = dynamic_cast<TaskSystem*>(LoadedProject);
		else
			_System = System;
	}

	LRESULT __stdcall CompletedTasksEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		CompletedTasksEditor* This = reinterpret_cast<CompletedTasksEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorFrame::EditorProc(This, Window, Message, wp, lp);
	}

	void CompletedTasksEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = this->BaseYCoord;
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
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Object Count:", BaseBk, TextStyle, false));

				TextStyle.Bold = false;
				TextStyle.Alignment = TA_LeftAlignment;
				ObjectCount = new Label(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height, _Base, ins, L"0", BaseBk, TextStyle, false);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;
				XCoord += Width / 2;

				GoToTasks = new Button(XCoord, YCoord, Width, Height, L"Go to Tasks", _Base, (HMENU)4, ins, Style, TextStyle);
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)5, ins, Style, TextStyle);
				YCoord += 10 + Height;

				SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)6, ins, Style, TextStyle);
				YCoord += 5 + Height;

				DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DA", _Base, (HMENU)7, ins, Style, TextStyle);

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
	void CompletedTasksEditor::FillObjects()
	{
		CompletedTaskList* List = !_System ? nullptr : _System->CompletedTasks;
		if (!List)
			return;

		Vector<CompletedTask*> Tasks = *List;

		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(Tasks, ObjectView, NULL, true, false, ObjectScroll);
		ObjectCount->SetText(Objects.Size);
	}

	LRESULT CompletedTasksEditor::Size()
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

			int ResetY = 0;

			{
				MiscControls[0]->Move(XCoord, YCoord, Width, Height);

				ObjectCount->Move(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;
				XCoord += Width / 2;

				GoToTasks->Move(XCoord, YCoord, Width, Height);
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				View->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				SelectAll->Move(XCoord, YCoord, Width, Height);
				YCoord += 5 + Height;

				DeSelectAll->Move(XCoord, YCoord, Width, Height);

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
	LRESULT CompletedTasksEditor::KeyDown(WPARAM wp)
	{
		switch (wp)
		{
		case VK_RETURN:
			FillObjects();
			break;
		case 'V':
			if (GetKeyState(VK_CONTROL) & 0x8000)
				ComponentViewer::OpenSelectedForEditView(Objects, false);
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}
		return 0;
	}
	LRESULT CompletedTasksEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Go To Tasks;
			EditorRegistry::OpenEditor(new TasksEditor(nullptr), nullptr);
			break;
		case 5: //View
			ComponentViewer::OpenSelectedForEditView(Objects, false);
			break;
		case 6: //Select All
		case 7: //DeSelectAll
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(wp == 7);
		}
		return 0;
	}

	void CompletedTasksEditor::Reset()
	{
		FillObjects();
	}
}