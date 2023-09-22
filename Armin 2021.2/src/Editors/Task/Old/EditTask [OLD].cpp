#include "EditTask.h"

#include "ArminSessions.h"
#include "Components.h"
#include "ViewTaskRegistry.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Config\Ins.h"
#include "..\..\Windows\Tool\Information\DatePicker.h"
#include "..\..\Windows\Tool\Object\ObjectSelector.h"

namespace Armin::Editors::Task
{
	using namespace Config;
	using namespace Files;
	using namespace Windows::Tool;

	EditTask::EditTask(Files::Task* Target)
	{
		HINSTANCE ins = Ins::Instance();
		if (!_ThisAtom)
			InitBase(ins);

		this->Target = Target;

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"Edit Task", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 850, 600, _Base, NULL, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		ViewTaskRegistry::EditTasks.Add(this);

		LoadControls();

		ShowWindow(_Base, SW_NORMAL);
		UpdateWindow(_Base);
	}
	EditTask::~EditTask()
	{
		SetWindowLongPtrW(_Base, GWLP_USERDATA, 0);
		DestroyWindow(_Base);

		ViewTaskRegistry::EditTasks.Remove(this);
		Destroy();
	}

	ATOM EditTask::_ThisAtom = ATOM();
	void EditTask::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(EditTask*);
		wn.hInstance = ins;
		wn.hIcon = ArminIcon;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"EditTaskClass";
		wn.lpfnWndProc = EditTask::WndProc;
		wn.style = CS_VREDRAW | CS_HREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall EditTask::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		EditTask* Item = reinterpret_cast<EditTask*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case WM_ERASEBKGND:
			return 1;
		case WM_SIZE:
			return Item->Size();
		case WM_DESTROY:
		{
			LRESULT Return = Item->Destroy();

			ViewTaskRegistry::EditTasks.Remove(Item);
			SetWindowLongPtrW(Window, GWLP_USERDATA, 0);
			delete Item;
			return Return;
		}
		case WM_COMMAND:
			return Item->Command(wp, lp);
		case WM_KEYDOWN:
			return Item->KeyDown(wp);
		case WM_PAINT:
			return Item->Paint();
		case WM_SPECIALCOMMAND:
			return Item->SpecialCommand(reinterpret_cast<HMENU>(HIWORD(wp)), static_cast<uint>(LOWORD(wp)), lp);
		default:
			return DefWindowProcW(Window, Message, wp, lp);
		}
	}

	void EditTask::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		AaColor BaseBk = Background1;
		int BaseYCoord = 0;

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 150;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = TitleFontName;
			TextStyle.FontSize = 15;
			TextStyle.Foreground = 0xFFFFFFFF;
			TextStyle.Alignment = TA_LeftAlignment;

			StyleSheet Style;
			Style.Background = Grey1;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyle.FontFamily = StandardFontName;

			ApplyBttn = new Button(WndRect.right - 130, 10, 120, 27, L"Apply", _Base, (HMENU)5, ins, Style, TextStyle);

			TextStyle.FontFamily = TitleFontName;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Edit Task", BaseBk, TextStyle, false));
			YCoord += 20 + Height;
			BaseYCoord = YCoord;

			int BaseYCoord = YCoord;

			TextStyle.Alignment = TA_RightAlignment;
			TextStyle.FontSize = 13;
			TextStyle.FontFamily = StandardFontName;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Due By:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			YCoord += 10 + Height; // Space for requires assurance

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Instructions", BaseBk, TextStyle, false));

			TextStyle.Alignment = TA_LeftAlignment;

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Title = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, Target->Title(), Style, TextStyle);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			int TempWidth = Width - (5 + ButtonSize);
			DueBy = new Label(XCoord, YCoord, TempWidth, Height, _Base, ins, Target->DueBy.ToString(), BaseBk, TextStyle, false);
			XCoord += 5 + TempWidth;

			DueBySelect = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"...", _Base, (HMENU)1, ins, Style, TextStyle);
			XCoord -= 5 + TempWidth;
			YCoord += 10 + Height;

			RequiresAssurance = new CheckableButton(XCoord, YCoord, Width, Height, _Base, ins, NULL, Target->RequiresAssurance, L"Needs Assurance?", CBT_CheckBox, Style, TextStyle);
			YCoord += 10 + Height;

			Height = WndRect.bottom - (10 + YCoord);
			Instructions = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, Target->Instructions, Style, TextStyle, true);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = 0xFFFFFFFF;
			TextStyle.Alignment = TA_LeftAlignment;

			StyleSheet Style;
			Style.Background = Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Assigned To:", BaseBk, TextStyle, false));

			int ButtonWidth = 40;
			int PreXCoord = XCoord;
			XCoord += 10 + Width;

			ModifyAssigned = new Button(XCoord, YCoord, ButtonWidth, Height, L"MA", _Base, (HMENU)2, ins, Style, TextStyle);
			XCoord += 5 + ButtonWidth;

			ViewAssigned = new Button(XCoord, YCoord, ButtonWidth, Height, L"VI", _Base, (HMENU)3, ins, Style, TextStyle);
			XCoord += 5 + ButtonWidth;

			Multiselect = new Button(XCoord, YCoord, ButtonWidth, Height, L"ML", _Base, (HMENU)4, ins, Style, TextStyle);

			XCoord = PreXCoord;
			YCoord += 10 + Height;

			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			Style.Background = Grey3;

			AssignedUsersScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			AssignedUsersView = new Grid(0, 0, 910, 32, AssignedUsersScroll, ins, Style);
			AssignedUsersScroll->SetViewer(AssignedUsersView);
		}

		{
			if (!Target)
				return;

			int XCoord = 5;
			int YCoord = 5;
			int Width = 910;
			int Height = 27;

			Vector<Files::User*>& AssignedTo = Target->AssignedTo;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = 0xFFFFFFFF;

			StyleSheet Style;
			Style.Background = Grey1;
			Style.BorderBrush = Accent4;
			Style.BaseBackground = Grey3;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			AssignedUsers = ComponentViewer::GenerateList(AssignedTo, AssignedUsersView, NULL, _IsMultiselect, true, Style, TextStyle, AssignedUsersScroll);
		}
	}

	LRESULT EditTask::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		AaColor BaseBk = Background1;
		int BaseYCoord = 0;

		{
			int XCoord = 10;
			int YCoord = 10;
			int Width = 150;
			int Height = 27;

			ApplyBttn->Move(WndRect.right - 130, 10, 120, 27);

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			YCoord += 20 + Height;
			BaseYCoord = YCoord;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			MiscControls[2]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			YCoord += 10 + Height;

			MiscControls[3]->Move(XCoord, YCoord, Width, Height);

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = (WndRect.right / 2) - (10 + XCoord);

			Title->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			int ButtonSize = Height;
			int TempWidth = Width - (5 + ButtonSize);
			DueBy->Move(XCoord, YCoord, TempWidth, Height);
			XCoord += 5 + TempWidth;

			DueBySelect->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			XCoord -= 5 + TempWidth;
			YCoord += 10 + Height;

			RequiresAssurance->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			Height = WndRect.bottom - (10 + YCoord);
			Instructions->Move(XCoord, YCoord, Width, Height);
		}

		{
			int XCoord = (WndRect.right / 2) + 10;
			int YCoord = BaseYCoord;
			int Width = 150;
			int Height = 27;

			MiscControls[4]->Move(XCoord, YCoord, Width, Height);

			int ButtonWidth = 40;
			int PreXCoord = XCoord;
			XCoord += 10 + Width;

			ModifyAssigned->Move(XCoord, YCoord, ButtonWidth, Height);
			XCoord += 5 + ButtonWidth;

			ViewAssigned->Move(XCoord, YCoord, ButtonWidth, Height);
			XCoord += 5 + ButtonWidth;

			Multiselect->Move(XCoord, YCoord, ButtonWidth, Height);

			XCoord = PreXCoord;
			YCoord += 10 + Height;

			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			AssignedUsersScroll->Move(XCoord, YCoord, Width, Height);
		}

		{
			ComponentViewer::ReSizeList(AssignedUsers, AssignedUsersView, AssignedUsersScroll);
		}

		return 0;
	}
	LRESULT EditTask::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(Background1);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT EditTask::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 1:
		{
			String Text = this->DueBy->GetText();
			DateTime Time = DateTime::FromString(Text);

			DateTime New = DatePicker::Execute(&Time, GetAncestor(_Base, GA_ROOT));
			this->DueBy->SetText(New.ToString());
			break;
		}
		case 2:
		{
			Vector<Component*> PrevItems = ComponentViewer::RetriveFromList(AssignedUsers);

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_User;
			Criteria.Multiselect = true;
			Criteria.PreSelected = PrevItems;
			Criteria.TypeLock = true;

			Vector<Component*> New = ObjectSelector::Execute(Criteria, GetAncestor(_Base, GA_ROOT));
			CloseControls(AssignedUsers);

			HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

			StyleSheet Style;
			Style.Background = Grey1;
			Style.BaseBackground = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = 0xFFFFFFFF;

			AssignedUsers = ComponentViewer::GenerateList(New, AssignedUsersView, NULL, _IsMultiselect, true, Style, TextStyle, AssignedUsersScroll);
			break;
		}
		case 3:
		{
			Vector<Component*> Items = ComponentViewer::RetriveFromList(AssignedUsers);
			if (Items.Size() == 0)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please select at least one user to view it.", L"Edit Task:", MB_OK | MB_ICONERROR);
				break;
			}

			for (uint i = 0; i < Items.Size(); i++)
				EditorRegistry::OpenEditorArgs(EDT_ViewUser, { Items[i] });
		}
		case 4:
		{
			_IsMultiselect = !_IsMultiselect;

			for (uint i = 0; i < AssignedUsers.Size(); i++)
				AssignedUsers[i]->CheckState(false);

			Multiselect->Text(_IsMultiselect ? L"SI" : L"ML");
			break;
		}
		case 5:
		{
			if (Apply(LoadedSession))
				DestroyWindow(_Base);
			break;
		}
		}

		return 0;
	}
	LRESULT EditTask::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT EditTask::Destroy()
	{
		return 0;
	}
	LRESULT EditTask::SpecialCommand(HMENU ID, uint Command, LPARAM Sender)
	{
		return 0;
	}

	bool EditTask::Apply(ArminSessionBase* File)
	{
		if (!File)
			return false;

		String Title = this->Title->GetText();
		String Instructions = this->Instructions->GetText();
		DateTime TargetTime = DateTime::FromString(DueBy->GetText());
		bool RequiresAssurance = this->RequiresAssurance->GetCheckState();
		Vector<Files::User*> AssignedTo;

		{
			for (uint i = 0; i < AssignedUsers.Size(); i++)
			{
				Component* Item = AssignedUsers[i]->Target();
				Files::User* Conv = dynamic_cast<Files::User*>(Item);
				if (Conv)
					AssignedTo.Add(Conv);
			}
		}

		if (!Target)
		{
			MessageBoxW(_Base, L"There was an unexpected error. Please report this error.", L"Edit Task:", MB_OK | MB_ICONERROR);
			return false;
		}

		Target->Title(Title);
		Target->Instructions = Instructions;
		Target->DueBy = TargetTime;
		Target->AssignedTo = AssignedTo;
		Target->RequiresAssurance = RequiresAssurance;

		MessageBoxW(_Base, L"The Task was modified sucessfully.", L"Edit Task:", MB_ICONINFORMATION | MB_OK);

		Ins::HasEdit = true;
		EditorRegistry::ResetEditorOfType(EDT_TaskOverview);
		EditorRegistry::ResetEditorOfType(EDT_ViewUser);
		return true;
	}
}