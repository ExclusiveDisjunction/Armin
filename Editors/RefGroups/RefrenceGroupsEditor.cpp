#include "..\EditorFrame.h"

#include "UI\StyleButton.h"
#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\ComponentViewer.h"

namespace Armin::Editors::RefGroups
{
	using namespace Files;
	using namespace UI;

	ReferenceGroupsEditor::ReferenceGroupsEditor(ProjectBase* Source)
	{
		if (!Source)
			_Source = LoadedProject;
		else
			_Source = Source;
	}

	LRESULT __stdcall ReferenceGroupsEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ReferenceGroupsEditor* This = reinterpret_cast<ReferenceGroupsEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ReferenceGroupsEditor::LoadControls()
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
			int Width = 150;
			int Height = 27;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = EditorGrey;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			int ResetY = 0;

			{
				TextStyle.Alignment = TA_RightAlignment;
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Group Count:", BaseBk, TextStyle, false));
				TextStyle.Alignment = TA_LeftAlignment;

				TextStyle.Bold = false;
				TextStyle.Alignment = TA_LeftAlignment;
				ObjectCount = new Label(XCoord + 5 + Width, YCoord, (WndRect.right / 2) - (10 + (XCoord + 5 + Width)), Height, _Base, ins, L"0", BaseBk, TextStyle, false);

				YCoord += 10 + Height;
				Width = (WndRect.right - (10 + XCoord + 10)) / 2;
				XCoord += Width / 2;

				Search = new StyleButton(XCoord, YCoord, Width, Height, L"Groups Search", _Base, (HMENU)4, ins, Style, TextStyle, RECT{0, 0, 0, 5});
				XCoord -= Width / 2;
				ResetY = YCoord += 10 + Height;
			}

			int ButtonSize = 40;

			{
				XCoord = WndRect.right - (ButtonSize + 10);

				int OldWidth = Width, OldHeight = Height;
				Width = Height = ButtonSize;

				Add = new Button(XCoord, YCoord, Width, Height, L"+", _Base, (HMENU)5, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				Remove = new Button(XCoord, YCoord, Width, Height, L"-", _Base, (HMENU)6, ins, Style, TextStyle);
				YCoord += 10 + ButtonSize;

				View = new Button(XCoord, YCoord, Width, Height, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				Edit = new Button(XCoord, YCoord, Width, Height, L"ED", _Base, (HMENU)8, ins, Style, TextStyle);
				YCoord += 10 + ButtonSize;

				SelectAll = new Button(XCoord, YCoord, Width, Height, L"SA", _Base, (HMENU)9, ins, Style, TextStyle);
				YCoord += 5 + ButtonSize;

				DeSelectAll = new Button(XCoord, YCoord, Width, Height, L"DA", _Base, (HMENU)10, ins, Style, TextStyle);

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
	void ReferenceGroupsEditor::FillObjects()
	{
		RefrenceGroupList* Groups = !LoadedProject ? nullptr : LoadedProject->RefrenceGroups;
		if (!Groups)
			return;

		Vector<RefrenceGroup*> New = *Groups;

		CloseControls(Objects);
		Objects = ComponentViewer::GenerateList(New, ObjectView, NULL, true, true, ObjectScroll);
		ObjectCount->SetText(Objects.Size);
	}

	LRESULT ReferenceGroupsEditor::Size()
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
			int Width = 150;
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
	LRESULT ReferenceGroupsEditor::KeyDown(WPARAM wp)
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
			if ((GetKeyState(VK_SHIFT) & 0x8000) && (GetKeyState(VK_CONTROL) & 0x8000))
				Command(5, 0);
			break;
		case 'V':
		case 'E':
			if (GetKeyState(VK_CONTROL) & 0x8000)
				ComponentViewer::OpenSelectedForEditView(Objects, wp == 'E');
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}
		return 0;
	}
	LRESULT ReferenceGroupsEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Search
			EditorRegistry::OpenEditor(new Misc::QuickSearchEditor(CT_RefrenceGroup), nullptr);
			break;
		case 5: //Add
			EditorRegistry::OpenEditor(new AddReferenceGroupEditor(), nullptr);
			break;
		case 6: //Remove
		{
			Vector<ComponentViewer*> Selected;
			Vector<Component*> RTargets = ComponentViewer::RetriveFromList(Objects, Selected);
			Vector<RefrenceGroup*> Targets;
			for (uint i = 0; i < RTargets.Size; i++)
			{
				RefrenceGroup* Conv = dynamic_cast<RefrenceGroup*>(RTargets[i]);
				if (Conv)
					Targets.Add(Conv);
			}

			RefrenceGroupList* Groups = LoadedProject->RefrenceGroups;
			if (!Groups)
				break;

			if (Targets.Size > 0)
			{
				int Result = MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The selected objects will be deleted permanantly.\n\nDo you wish to coninue?", L"Warning:", MB_YESNOCANCEL | MB_ICONWARNING);
				if (Result == IDYES)
				{
					for (uint i = 0; i < Targets.Size; i++)
						delete Targets[i];

					for (uint i = 0; i < Selected.Size; i++)
						Objects.Remove(Selected[i]);
					CloseControls(Selected);
					ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
				}
			}
			else
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please select at least one object to remove.", L"Remove Items:", MB_OK | MB_ICONERROR);
			break;
		}
		case 7: //View
		case 8: //Edit
			ComponentViewer::OpenSelectedForEditView(Objects, wp == 8);
			break;
		case 9: //Select All
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(false);
			break;
		case 10: //DeSelect All
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(false);
			break;
		}
		return 0;
	}

	void ReferenceGroupsEditor::Reset()
	{
		FillObjects();
	}
}