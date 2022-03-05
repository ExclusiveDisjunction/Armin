#include "..\EditorFrame.h"

#include "..\EditorRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\ComponentViewer.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::RefGroups
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;

	ViewEditReferenceGroupEditor::ViewEditReferenceGroupEditor(ReferenceGroup* Target, bool EditMode)
	{
		_EditMode = EditMode;
		_Target = Target;		
	}
	ViewEditReferenceGroupEditor::~ViewEditReferenceGroupEditor()
	{
		if (Objects)
			delete Objects;
	}

	LRESULT __stdcall ViewEditReferenceGroupEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ViewEditReferenceGroupEditor* This = reinterpret_cast<ViewEditReferenceGroupEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void ViewEditReferenceGroupEditor::LoadControls()
	{
		if (_Loaded)
			return;

		if (_Target && _BTarget && !_BTarget->Target())
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
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;
			TextStyle.FontFamily = StandardFontName;

			StyleSheet Style;
			Style.Background = Grey3;
			Style.BaseBackground = BaseBk;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			{
				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Title:", BaseBk, TextStyle, false));
				YCoord += 10 + Height;

				MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Selected Objects:", BaseBk, TextStyle, false));
			}
			
			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			{
				if (_EditMode)
					TitleEd = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, _Target ? _Target->Title() : String(), Style, TextStyle);
				else
					TitleVi = new Label(XCoord, YCoord, Width, Height, _Base, ins, _Target->Title(), Grey3, TextStyle, false);
			}

			YCoord += 10 + Height;
			Width = _EditMode ? ((WndRect.right - (10 + XCoord) - 15) / 4) : ((WndRect.right - (10 + XCoord) - 5) / 2);
			int ResetX = XCoord;

			{
				if (_EditMode)
				{
					Modify = new Button(XCoord, YCoord, Width, Height, L"Modify", _Base, (HMENU)4, ins, Style, TextStyle);
					XCoord += 5 + Width;

					Remove = new Button(XCoord, YCoord, Width, Height, L"Remove", _Base, (HMENU)5, ins, Style, TextStyle);
					XCoord += 5 + Width;
				}

				View = new Button(XCoord, YCoord, Width, Height, L"View", _Base, (HMENU)6, ins, Style, TextStyle);
				XCoord += 5 + Width;

				Edit = new Button(XCoord, YCoord, Width, Height, L"Edit", _Base, (HMENU)7, ins, Style, TextStyle);
				XCoord += 5 + Width;
			}

			XCoord = ResetX;
			YCoord += 10 + Height;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			{
				ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
				ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
				ObjectScroll->SetViewer(ObjectView);

				Objects->GenerateListRef(!_Target ? ReferenceList() : _Target->Targets, NULL, _Multiselect, true);
			}
		}
	}

	LRESULT ViewEditReferenceGroupEditor::Size()
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

			{
				MiscControls[0]->Move(XCoord, YCoord, Width, Height);
				YCoord += 10 + Height;

				MiscControls[1]->Move(XCoord, YCoord, Width, Height);
			}

			YCoord = BaseYCoord;
			XCoord += 5 + Width;
			Width = WndRect.right - (10 + XCoord);

			{
				if (_EditMode)
					TitleEd->Move(XCoord, YCoord, Width, Height);
				else
					TitleVi->Move(XCoord, YCoord, Width, Height);
			}

			YCoord += 10 + Height;
			Width = _EditMode ? ((WndRect.right - (10 + XCoord) - 15) / 4) : ((WndRect.right - (10 + XCoord) - 5) / 2);
			int ResetX = XCoord;

			{
				if (_EditMode)
				{
					Modify->Move(XCoord, YCoord, Width, Height);
					XCoord += 10 + Width;

					Remove->Move(XCoord, YCoord, Width, Height);
					XCoord += 10 + Width;
				}

				View->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;

				Edit->Move(XCoord, YCoord, Width, Height);
				XCoord += 10 + Width;
			}

			XCoord = ResetX;
			YCoord += 10 + Height;
			Width = WndRect.right - (10 + XCoord);
			Height = WndRect.bottom - (10 + YCoord);

			{
				ObjectScroll->Move(XCoord, YCoord, Width, Height);
				Objects->ReSizeList();
			}
		}
		return 0;
	}
	LRESULT ViewEditReferenceGroupEditor::KeyDown(WPARAM wp)
	{
		return 0;
	}
	LRESULT ViewEditReferenceGroupEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //Add
		{
			Vector<Component*> Old = Objects->GetAllComponents();

			SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_All;
			Criteria.Multiselect = true;
			Criteria.PreSelected = Old;

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

	void ViewEditReferenceGroupEditor::Reset()
	{
		if (_EditMode)
			TitleEd->SetText(_Target->Title());
		else
			TitleVi->SetText(_Target->Title());

		Objects->GenerateListRef(_Target->Targets, NULL, _Multiselect, true);
	}
	bool ViewEditReferenceGroupEditor::EquatableTo(EditorFrame* Other) const
	{
		ViewEditReferenceGroupEditor* Conv = dynamic_cast<ViewEditReferenceGroupEditor*>(Other);
		return Conv && Conv->_EditMode == _EditMode && Conv->_Target == _Target;
	}
	bool ViewEditReferenceGroupEditor::Apply(ProjectBase* File, bool PromptErrors)
	{
		if (!_EditMode)
			return true;

		String Title = TitleEd->GetText();
		Vector<Component*> Targets = Objects->GetAllComponents();

		if (Title.Contains(L'~') || Title == L"")
		{
			if (PromptErrors)
				MessageBoxW(GetAncestor(_Base, GA_ROOT), L"Please supply a title for the Refrence Group, and ensure that there are no tilde (\'`\') in it.", L"Edit Refrence Group:", MB_OK | MB_ICONERROR);
			return false;
		}

		if (Targets.Size == 0)
		{
			int Result = MessageBoxW(GetAncestor(_Base, GA_ROOT), L"There are no items selected for this group. Do you want to continue?", L"Edit Refrence Group:", MB_ICONWARNING | MB_YESNOCANCEL);
			if (Result != IDYES)
				return false;
		}

		_Target->Title(Title);
		_Target->Targets = ComponentReference::Generate(Targets);
		return true;
	}
}