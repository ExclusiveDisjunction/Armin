#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::RefGroups
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;

	AddReferenceGroupEditor::AddReferenceGroupEditor()
	{
		
	}
	AddReferenceGroupEditor::~AddReferenceGroupEditor()
	{
		if (Objects)
			delete Objects;
	}

	LRESULT __stdcall AddReferenceGroupEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		AddReferenceGroupEditor* This = reinterpret_cast<AddReferenceGroupEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void AddReferenceGroupEditor::LoadControls()
	{
		if (_Loaded)
			return;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		LoadUpperButtons(WndRect, ins);

		int BaseYCoord = this->BaseYCoord;
		AaColor BaseBk = EditorGrey;

		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
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

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Selected Items:", BaseBk, TextStyle, false));
			YCoord += 10 + Height;

			XCoord += Width + 5;
			Width = WndRect.right - (10 + XCoord);
			YCoord = BaseYCoord;

			Name = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			YCoord += 10 + Height;

			int ResetXCoord = XCoord;
			Width = (WndRect.right - (10 + XCoord) - 15) / 4;
			
			Modify = new Button(XCoord, YCoord, Width, Height, L"Modify", _Base, (HMENU)4, ins, Style, TextStyle);
			XCoord += 5 + Width;

			Remove = new Button(XCoord, YCoord, Width, Height, L"Remove", _Base, (HMENU)5, ins, Style, TextStyle);
			XCoord += 5 + Width;

			View = new Button(XCoord, YCoord, Width, Height, L"View", _Base, (HMENU)6, ins, Style, TextStyle);
			XCoord += 5 + Width;

			Edit = new Button(XCoord, YCoord, Width, Height, L"Edit", _Base, (HMENU)7, ins, Style, TextStyle);
			YCoord += 10 + Height;
			
			XCoord = ResetXCoord;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);

			if (Objects)
				delete Objects;
		}
	}

	LRESULT AddReferenceGroupEditor::Size()
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

			XCoord += Width + 5;
			Width = WndRect.right - (10 + XCoord);
			YCoord = BaseYCoord;

			Name->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			int ResetXCoord = XCoord;
			Width = (WndRect.right - (10 + XCoord) - 15) / 4;

			Modify->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			Remove->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			View->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			Edit->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;

			XCoord = ResetXCoord;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
			Objects->ReSizeList();
		}
		return 0;
	}
	LRESULT AddReferenceGroupEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT AddReferenceGroupEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Modify
		{
			Vector<Component*> Current = Objects->GetAllComponents();

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_All;
			Criteria.PreSelected = Current;
			Criteria.Multiselect = true;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

			Objects->GenerateList(New, NULL, _Multiselect, true);
			break;
		}
		case 5: //Remove
		{
			Vector<ComponentViewer*> Selected;
			Objects->RetriveFromList(Selected);

			for (uint i = 0; i < Selected.Size; i++)
				delete Selected[i];

			Objects->ReSizeList();
			break;
		}
		case 6: //View
		case 7: //Edit
			Objects->OpenSelectedForEditView(wp == 7);
			break;
		}
		return 0;
	}

	bool AddReferenceGroupEditor::Apply(Files::ProjectBase* File, bool PromptErrors)
	{
		String Title = this->Name->GetText();
		Vector<Component*> Targets = Objects->GetAllComponents();

		if (Title.Contains(L'~') || Title == L"")
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please supply a title for the Refrence Group, and ensure that there are no tilde (\'`\') in it.", L"Add Refrence Group:", MB_OK | MB_ICONERROR);
			return false;
		}

		if (Targets.Size == 0)
		{
			int Result = MessageBoxW(GetAncestor(_Base, GA_ROOT), L"There are no items selected for this group. Do you want to continue?", L"Edit Refrence Group:", MB_ICONWARNING | MB_YESNOCANCEL);
			if (Result != IDYES)
				return false;
		}

		RefrenceGroupList* Groups = File->RefrenceGroups;
		if (!Groups)
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"The current project does not support Refrence Groups.", L"Add Refrence Group:", MB_OK | MB_ICONERROR);
			return false;
		}

		RefrenceGroup* New = new RefrenceGroup(File, Groups);
		New->Title(Title);
		New->Targets = ComponentReference::Generate(Targets);

		AppState |= APS_HasEdit;
		EditorRegistry::ResetEditorOfType(EDT_ReferenceGroups);
		return true;
	}
	void AddReferenceGroupEditor::Reset()
	{
		String Blank;
		Name->SetText(Blank);
		Objects->Clear();
	}
}