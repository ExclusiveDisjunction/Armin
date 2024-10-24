#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\..\Common.h"

namespace Armin
{
	namespace Files
	{
		class User;
	}

	namespace UI::Import
	{
		class ImportFromCSV
		{
		private:
			enum Page
			{
				Home = 1,
				SelectColumns = 2,
				FinalPage = 3,
				None = 4
			};

			HWND _Base;
			bool _Loaded;

			Vector<ControlBase*> MiscControls;

			Label* Title, * Instructions;
			TextBox* HOMPath;
			Button* HOMSelect;

			Grid* IDColumnsView, * DesColumnsView;
			ScrollViewer* IDColumnsScroll, * DesColumnsScroll;
			Vector<CheckableButton*> IDColumns, DesColumns;
			TextBox* SeperateBy, * TXTGroupName;

			Button* Next, * Back, * Cancel; //ID: 1, 2, 3
			Page LastPage = None, CurrentPage = Home;
			void LoadPage();
			bool ProcessPage();
			void DestroyControls(bool Full);

			String ToPath;
			String FromPath;
			int IDColumn = 0;
			Vector<int> DescriptionColumns;
			String SepBy;
			String GroupName;

			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);

			void BeginImport();

			LRESULT Paint();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT Size();
			LRESULT Destroy();

		public:
			ImportFromCSV(HINSTANCE ins);
			~ImportFromCSV();
		};
	}
}