#pragma once

#include "Container.h"
#include "Controls.h"
#include "..\..\..\Common.h"
#include "..\..\..\Controls\ArminControls.h"
#include "..\..\..\Files\SearchCriteria.h"

namespace Armin
{
	namespace Files
	{
		class Component;
	}

	namespace Windows::Tool
	{
		class ObjectSelector
		{
		private:
			ObjectSelector(Files::SearchCriteria& Criteria, HINSTANCE ins);

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

			Files::SearchCriteria _Criteria;
			Vector<Files::Component*> Return;
			bool ProperReturn = false;

			HWND _Base;
			bool _Loaded, _FirstRefresh = true;

			Button* SearchByName, * DeselectAll, * SelectAll, * Submit, * Cancel;
			Grid* G1;
			Grid* ObjectView;
			ScrollViewer* ObjectScroll;
			Vector<ComponentViewer*> Objects;

			LRESULT KeyDown(WPARAM);
			LRESULT Command(WPARAM ID);

			void Refresh();
			void GetReturn();

			void LoadControls();
			void MoveControls();
			void Destroy();
			void Paint();
		public:
			static Vector<Files::Component*> Execute(Files::SearchCriteria& Criteria, HINSTANCE ins);

		};
	}
}