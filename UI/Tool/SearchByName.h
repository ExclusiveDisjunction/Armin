#pragma once

#include <Windows.h>
#include "Container.h"
#include "UI\ControlDef.h"
#include "..\..\Files\SearchCriteria.h"

namespace Armin
{
	namespace Files
	{
		class Component;
		class ProjectBase;
	}

	namespace UI
	{
		class ComponentViewer;

		namespace Search
		{
			class SearchByName
			{
			private:
				SearchByName(Files::SearchCriteria& Criteria, Files::ProjectBase* Target);

				void Construct(HINSTANCE ins);

				HWND _Base;
				bool _Loaded, _FirstRefresh = true;

				Files::SearchCriteria Criteria;
				Files::ProjectBase* File;
				Vector<Files::Component*> Return;

				void LoadControls();

				void GetReturn();
				void Refresh();

				Vector<ControlBase*> MiscControls;
				TextBox* NameToSearch;
				ScrollViewer* ObjectScroll;
				Grid* ObjectView, * ButtonHost;
				Vector<ComponentViewer*> Objects;
				Button* Submit, * Cancel, * SelectAll, * ClearSelection, * _RunSearch;

				static ATOM _ThisAtom;
				static void InitBase(HINSTANCE ins);
				static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

				LRESULT Paint();
				LRESULT Size();
				LRESULT KeyDown(WPARAM wp);
				LRESULT Command(WPARAM wp, LPARAM lp);

			public:
				SearchByName(const SearchByName& Obj) = delete;
				SearchByName(const SearchByName&& Obj) = delete;
				~SearchByName()
				{
					SetWindowLongPtr(_Base, GWLP_USERDATA, 0);
					DestroyWindow(_Base);
				}

				SearchByName& operator=(const SearchByName& Obj) = delete;
				SearchByName& operator=(const SearchByName&& Obj) = delete;

				/// <summary>
				/// Opens a window that allows searching and selection of components.
				/// </summary>
				/// <param name="Criteria">The SearchCriteria object to set the filter and mask for the searching.</param>
				/// <param name="Instance">The HINSTANCE to use.</param>
				/// <param name="SourceFile">The source file to retrive all data from the SearchCriteria with. If this parameter is nullptr (default), it uses the currently loaded project.</param>
				/// <returns>The list of selected components from the window, fitting the bounds of the Criteria.</returns>
				static Vector<Files::Component*> Execute(Files::SearchCriteria& Criteria, HINSTANCE Instance, Files::ProjectBase* SourceFile = nullptr);

				static LRESULT RunMessageLoop(SearchByName* Obj, HINSTANCE ins, bool* Running);
			};
		}
	}
}