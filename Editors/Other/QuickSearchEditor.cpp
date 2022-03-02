#include "..\EditorFrame.h"

#include "Sort.h"
#include "..\EditorRegistry.h"
#include "..\..\UserRegistry.h"
#include "..\..\Files\ArminSessions.h"
#include "..\..\Files\SearchCriteria.h"
#include "..\..\UI\ComponentViewer.h"
#include "..\..\UI\Tool\ComponentListRenderer.h"
#include "..\..\UI\Tool\SearchByName.h"

namespace Armin::Editors::Misc
{
	using namespace Files;
	using namespace UI;
	using namespace UI::Search;
	using namespace UI::Tool;

	QuickSearchEditor::QuickSearchEditor(int Filter)
	{
		this->Filter = Filter;
	}
	QuickSearchEditor::~QuickSearchEditor()
	{
		delete Objects;
	}

	LRESULT __stdcall QuickSearchEditor::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		QuickSearchEditor* This = reinterpret_cast<QuickSearchEditor*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		return EditorProc(This, Window, Message, wp, lp);
	}

	void QuickSearchEditor::LoadControls()
	{
		if (_Loaded)
			return;

		_Loaded = true;

		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));
		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		
		AaColor BaseBk = EditorGrey;
		int BaseYCoord = this->BaseYCoord;
		int BaseXCoord = 10;

		LoadUpperButtons(WndRect, ins);

		//Search and Buttons
		{
			int ButtonSize = 80;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = WndRect.right - (10 + XCoord + ButtonSize + 5);
			int Height = 27;

			StyleSheet Style;
			Style.Background = Style.BaseBackground = Grey3;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 3;
			Style.Radius = 20;

			TextStyleSheet TextStyle;
			TextStyle.FontFamily = StandardFontName;
			TextStyle.FontSize = 13;
			TextStyle.Foreground = FontColor;

			SearchCriteria = new TextBox(XCoord, YCoord, Width, Height, _Base, ins, String(), Style, TextStyle);
			XCoord += 5 + Width;

			_RunSearch = new Button(XCoord, YCoord, ButtonSize, Height, L"Search", _Base, (HMENU)4, ins, Style, TextStyle);
			BaseYCoord = (YCoord + 10 + Height);
		}

		//Filter Types
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 230;
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

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Allowed Types:", BaseBk, TextStyle, false));

			YCoord += 5 + Height;
			Height = WndRect.bottom - (10 + YCoord);

			TypesScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			TypesView = new Grid(0, 0, 600, (13 * (5 + 27)), TypesScroll, ins, Style);
			TypesScroll->SetViewer(TypesView);

			RECT Client;
			GetClientRect(*TypesView, &Client);

			BaseXCoord = XCoord + 10 + Width;

			Style.BaseBackground = Style.Background;
			XCoord = 3;
			YCoord = 3;
			Width = Client.right - (XCoord * 2);
			Height = 27;

			Users = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_User, L"Users", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			JobPositions = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_JobPosition, L"Job Positions", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			Tasks = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_Task, L"Tasks", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			CompletedTasks = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_CompletedTask, L"Completed Tasks", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			InventoryItems = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_InventoryItem, L"Inventory Items", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			OperationInventoryItems = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_OperationInventoryItem, L"Operation Inventory Items", CBT_CheckBox, Style, TextStyle);
			YCoord += 5 + Height;

			RefrenceGroups = new CheckableButton(XCoord, YCoord, Width, Height, TypesView, ins, NULL, Filter & CT_RefrenceGroup, L"Refrence Groups", CBT_CheckBox, Style, TextStyle);
		}

		//Grids
		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = WndRect.right - (10 + XCoord);
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

			MiscControls.Add(new Label(XCoord, YCoord, Width, Height, _Base, ins, L"Search Results:", BaseBk, TextStyle, false));

			int ResetYCoord = YCoord += 5 + Height;
			int ButtonSize = 40;
			XCoord = WndRect.right - (10 + ButtonSize);

			Add = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"+", _Base, (HMENU)5, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			Remove = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"-", _Base, (HMENU)6, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			View = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"VI", _Base, (HMENU)7, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			Edit = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"ED", _Base, (HMENU)8, ins, Style, TextStyle);
			YCoord += 15 + ButtonSize;

			SelectAll = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"SA", _Base, (HMENU)9, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			ClearSelection = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"CS", _Base, (HMENU)10, ins, Style, TextStyle);
			YCoord += 15 + ButtonSize;

			SaveResults = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"SR", _Base, (HMENU)11, ins, Style, TextStyle);
			YCoord += 5 + ButtonSize;

			DuplicateResults = new Button(XCoord, YCoord, ButtonSize, ButtonSize, L"DR", _Base, (HMENU)12, ins, Style, TextStyle);

			YCoord = ResetYCoord;
			XCoord = BaseXCoord;
			Width = WndRect.right - (10 + XCoord + 5 + ButtonSize);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll = new ScrollViewer(XCoord, YCoord, Width, Height, _Base, ins, Style);
			ObjectView = new Grid(0, 0, 910, 32, ObjectScroll, ins, Style);
			ObjectScroll->SetViewer(ObjectView);
		
			if (Objects)
				delete Objects;
			Objects = new ComponentViewerList(ObjectView, ObjectScroll);
		}
	}

	LRESULT QuickSearchEditor::Command(WPARAM wp, LPARAM lp)
	{
		switch (wp)
		{
		case 4: //_Run Search
			RunSearch();
			break;
		case 5: //Add
		{
			class SearchCriteria Criteria;
			Criteria.AllowedTypes = CT_All;
			Criteria.Multiselect = true;

			Vector<Component*> New = SearchByName::Execute(Criteria, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			Vector<Component*> Old = Objects->GetAllComponents();
			Old.Add(New);

			QuickSort(Old, [](Component* One, Component* Two) -> bool { return One->ObjectType() < Two->ObjectType(); });

			Objects->GenerateList(Old, NULL, _Multiselect, true);			
			break;
		}
		case 6: //Remove
		{
			Vector<ComponentViewer*> Select;
			Objects->RetriveFromList(Select);

			for (uint i = 0; i < Select.Size; i++)
			{
				ComponentViewer* Current = Select[i];
				delete Select[i];
			}

			Objects->ReSizeList();
			break;
		}
		case 7: //View
		case 8: //Edit
			Objects->OpenSelectedForEditView(wp == 8);
		case 9: //Select All
		case 10: //Clear Selection
			for (ComponentViewer* Obj = Objects->Item(0); Obj != nullptr; Obj = Obj->Next)
				Obj->CheckState(wp == 9);
			break;
		case 11: //Save Selection
		{
			RefrenceGroupList* RefGroups = LoadedProject->RefrenceGroups;
			if (!RefGroups)
				break;

			RefrenceGroup* New = new RefrenceGroup(LoadedProject, RefGroups);
			New->Targets = ComponentReference::Generate(Objects->GetAllComponents());

			AppState |= APS_HasEdit;
			EditorRegistry::OpenEditor(new RefGroups::ViewEditReferenceGroupEditor(New, true), nullptr);
			break;
		}
		case 12: //Duplicate Selection
		{
			Vector<Component*> Selection = Objects->GetAllComponents();
			String Query = SearchCriteria->GetText();

			new ComponentListRenderer(Selection, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)), L"Search Results for \"" + Query + L"\"");
		}
		}
		return 0;
	}
	LRESULT QuickSearchEditor::KeyDown(WPARAM Key)
	{
		switch (Key)
		{
		case L'D':
			if (GetKeyState(VK_CONTROL) & 0x8000)
				Command(12, 0); //Duplicate
			else if (GetKeyState(VK_SHIFT) & 0x8000)
				Command(10, 0); //Deselect
			break;
		case L'A':
			if (GetKeyState(VK_SHIFT) & 0x8000)
				Command(9, 0); //Select All
			break;
		case VK_RETURN:
			RunSearch();
			break;
		}
		return 0;
	}
	LRESULT QuickSearchEditor::Size()
	{
		if (!_Loaded)
			return 0;

		RECT WndRect;
		GetClientRect(_Base, &WndRect);
		MoveUpperButtons(WndRect);

		int BaseYCoord = this->BaseYCoord;
		int BaseXCoord = 10;

		//Search and Buttons
		{
			int ButtonSize = 80;

			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = WndRect.right - (10 + XCoord + ButtonSize + 5);
			int Height = 27;

			SearchCriteria->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			_RunSearch->Move(XCoord, YCoord, ButtonSize, Height);
			BaseYCoord = (YCoord + 10 + Height);
		}

		//Filter Types
		{
			int XCoord = 10;
			int YCoord = BaseYCoord;
			int Width = 230;
			int Height = 27;

			MiscControls[0]->Move(XCoord, YCoord, Width, Height);

			YCoord += 5 + Height;
			Height = WndRect.bottom - (10 + YCoord);

			TypesScroll->Move(XCoord, YCoord, Width, Height);
			TypesScroll->Reset();	
			BaseXCoord = XCoord += Width + 10;
		}

		//Grids
		{
			int XCoord = BaseXCoord;
			int YCoord = BaseYCoord;
			int Width = WndRect.right - (10 + XCoord);
			int Height = 27;

			MiscControls[1]->Move(XCoord, YCoord, Width, Height);

			int ResetYCoord = YCoord += 5 + Height;
			int ButtonSize = 40;
			XCoord = WndRect.right - (10 + ButtonSize);

			Add->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			Remove->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			View->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			Edit->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 15 + ButtonSize;

			SelectAll->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			ClearSelection->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 15 + ButtonSize;

			SaveResults->Move(XCoord, YCoord, ButtonSize, ButtonSize);
			YCoord += 5 + ButtonSize;

			DuplicateResults->Move(XCoord, YCoord, ButtonSize, ButtonSize);

			YCoord = ResetYCoord;
			XCoord = BaseXCoord;
			Width = WndRect.right - (10 + XCoord + 5 + ButtonSize);
			Height = WndRect.bottom - (10 + YCoord);

			ObjectScroll->Move(XCoord, YCoord, Width, Height);
			Objects->ReSizeList();
		}

		return 0;
	}

	void QuickSearchEditor::Reset()
	{
		Objects->Clear();
		Objects->ReSizeList();
		SearchCriteria->SetText(String());

		Users->SetCheckState(Filter & CT_User);
		Tasks->SetCheckState(Filter & CT_Task);
		CompletedTasks->SetCheckState(Filter & CT_CompletedTask);
		InventoryItems->SetCheckState(Filter & CT_InventoryItem);
		JobPositions->SetCheckState(Filter & CT_JobPosition);
		OperationInventoryItems->SetCheckState(Filter & CT_OperationInventoryItem);
		RefrenceGroups->SetCheckState(Filter & CT_RefrenceGroup);
	}
	void QuickSearchEditor::RunSearch()
	{
		String Criteria = SearchCriteria->GetText();
		int AllowedTypes = (Users->GetCheckState() ? CT_User : 0) | (Tasks->GetCheckState() ? CT_Task : 0) | (CompletedTasks->GetCheckState() ? CT_CompletedTask : 0) | (InventoryItems->GetCheckState() ? CT_InventoryItem : 0) | (JobPositions->GetCheckState() ? CT_JobPosition : 0) | (OperationInventoryItems->GetCheckState() ? CT_OperationInventoryItem : 0) | (RefrenceGroups->GetCheckState() ? CT_RefrenceGroup : 0);

		class SearchCriteria CriteriaObj;
		CriteriaObj.AllowedTypes = AllowedTypes;
		CriteriaObj.Arguments = Criteria;

		Vector<Component*> Components = CriteriaObj.GetComponents(LoadedProject);
		if (Components.Size == 0)
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"No results were found for your query.", L"Quick Search:", MB_OK | MB_ICONWARNING);
		else
		{
			QuickSort(Components, [](Component* One, Component* Two) -> bool { return One->ObjectType() < Two->ObjectType(); });

			Objects->GenerateList(Components, NULL, _Multiselect, true);
		}
	}
}