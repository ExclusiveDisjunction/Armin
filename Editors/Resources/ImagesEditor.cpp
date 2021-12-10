#include "..\EditorFrame.h"

#include "..\..\Files\ArminSessions.h"
#include "..\..\UI\Tool\CreateImage.h"

namespace Armin::Editors::Resources
{
	using namespace UI;
	using namespace UI::Tool;
	using namespace Files;

	ImagesEditor::ImagesEditor(ResourceSystem* Target) : Add(nullptr), Remove(nullptr), View(nullptr), ObjectView(nullptr), ObjectScroll(nullptr)
	{
		if (!_System)
			_System = dynamic_cast<ResourceSystem*>(LoadedProject);
		else
			_System = Target;
	}

	LRESULT __stdcall ImagesEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ImagesEditor* This = reinterpret_cast<ImagesEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ImagesEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		int BaseYCoord = 110;
		AaColor BaseBk = EditorGrey;

		LoadUpperButtons(WndRect, ins);

		StyleSheet Style;
		Style.Background = Grey3;
		Style.BaseBackground = EditorGrey;
		Style.BorderBrush = Accent4;
		Style.BorderThickness = 4;
		Style.Radius = 20;

		TextStyleSheet TextStyle;
		TextStyle.Alignment = TA_LeftAlignment;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.FontSize = 13;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 130;
			int Height = 27;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Image List:", BaseBk, TextStyle, false));
			int ResetYCoord = (YCoord += 10 + Height);

			int ButtonSize = 40;
			XCoord = WndRect.right - (10 + ButtonSize);

			Add = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"+", _Base, (HMENU)4, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			Remove = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"-", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 10 + ButtonSize;

			View = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)6, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			Edit = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"ED", _Base, (HMENU)7, ins, Style, TextStyle);
			YCoord += 10 + ButtonSize;

			SelectAll = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"SA", _Base, (HMENU)8, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			DeSelectAll = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"DS", _Base, (HMENU)9, ins, Style, TextStyle);
			YCoord += 10 + ButtonSize;

			Compress = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"CO", _Base, (HMENU)10, ins, Style, TextStyle);

			XCoord = 10;
			YCoord = ResetYCoord;
			Width = WndRect.right - (XCoord + 10 + ButtonSize + 5);
			Height = WndRect.bottom - (YCoord + 10);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);
		}

		FillObjects();
	}
	void ImagesEditor::FillObjects()
	{
		if (!_Loaded || !ObjectScroll || !ObjectView)
			return;

		CloseControls(Objects);

		ResourceSystem* Sys = _System;
		ImageList* Images = !Sys ? nullptr : Sys->Images;
		if (!Sys || !Images)
			return;

		Objects = ComponentViewer::GenerateList(static_cast<Vector<Image*>>(* Images), ObjectView, NULL, true, true, ObjectScroll);
	}

	LRESULT ImagesEditor::Size()
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

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);
			int ResetYCoord = (YCoord += 10 + Height);

			int ButtonSize = 40;
			XCoord = WndRect.right - (10 + ButtonSize);

			Add->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			Remove->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 10 + ButtonSize;

			View->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			Edit->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 10 + ButtonSize;

			SelectAll->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			DeSelectAll->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + ButtonSize;

			Compress->Move(XCoord, YCoord, ButtonSize, ButtonSize);

			XCoord = 10;
			YCoord = ResetYCoord;
			Width = WndRect.right - (XCoord + 10);
			Height = WndRect.bottom - (YCoord + 10);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
		}

		return 0;
	}
	LRESULT ImagesEditor::KeyDown(WPARAM wp)
	{
		bool Shift = GetKeyState(VK_SHIFT) & 0x8000, Control = GetKeyState(VK_CONTROL) & 0x8000;
		switch (wp)
		{
		case VK_RETURN:
			FillObjects();
			break;
		case VK_DELETE:
			Command(5, 0);
			break;
		case 'N':
			if (Shift && Control)
				Command(4, 0);
			break;
		case 'V':
		case 'E':
			if (Control)
				ComponentViewer::OpenSelectedForEditView(Objects, wp == 'E');
			break;
		default:
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
		}
		
		return 0;
	}
	LRESULT ImagesEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Add
			CreateImage::Execute(reinterpret_cast<HINSTANCE>(GetWindowLongPtr(_Base, GWLP_HINSTANCE)), false);
			break;
		case 5: //Remove
		{
			Vector<ComponentViewer*> Selected;
			Vector<Component*> ToRemove = ComponentViewer::RetriveFromList(Objects, Selected);

			if (ToRemove.Size == 0)
			{
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please select at least one object to remove, and try again.", L"Images:", MB_OK | MB_ICONERROR);
				break;
			}

			for (uint i = 0; i < ToRemove.Size; i++)
			{
				Component* Obj = ToRemove[i];
				Image* Conv = dynamic_cast<Image*>(Obj);
				if (!Conv)
					continue;

				delete Conv;
				
				ComponentViewer* Viewer = Selected[i];
				Objects.Remove(Viewer);
				delete Viewer;
			}

			ComponentViewer::ReSizeList(Objects, ObjectView, ObjectScroll);
			break;
		}
		case 6: //View
		case 7: //Edit
			ComponentViewer::OpenSelectedForEditView(Objects, wp == 7);
			break;
		case 8: //Select All 
		case 9: //DeSelectAll
			for (ComponentViewer* Obj : Objects)
				Obj->CheckState(wp == 8);
			break;
		case 10: //Compress
			break;
		}
		return 0;
	}

	bool ImagesEditor::TestOnCondition(Vector<void*> Args) const
	{
		return Args.Size == 1 && Args[0] == this->_System;
	}
	bool ImagesEditor::EquatableTo(EditorFrame* Other) const
	{
		ImagesEditor* Temp;
		return (Temp = dynamic_cast<ImagesEditor*>(Other)) != nullptr && Temp->_System == _System;
	}

	void ImagesEditor::Reset()
	{
		FillObjects();
	}
}