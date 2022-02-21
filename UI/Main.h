#pragma once

#include "ArminControls.h"
#include "RibbonHost.h"
#include "UI\Controls.h"
#include "..\Common.h"
#include "..\Editors\EditorHost.h"
#include "..\Editors\EditorButtonHost.h"

namespace Armin
{
	namespace Editors
	{
		class EditorRegistry;
	}

	namespace UI
	{
		class Main : public Editors::EditorButtonHost, public FooterHost, public RibbonHost
		{
		private:
			static ATOM _ThisAtom;
			static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);
			static void InitBase(HINSTANCE ins);

			HWND _Base;
			bool _Loaded = false;

		private:
			//Controls
			void LoadControls();

			//Messages
			LRESULT Paint();
			LRESULT OnClose();
			LRESULT Destroy();
			LRESULT Size();
			LRESULT KeyDown(WPARAM wp);
			LRESULT Timer(WPARAM wp);
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT GetMinMaxInfo(LPARAM lp);

			//Ribbon
			Grid* RibbonBase;

			Button* FileMsCmd, * EditMsCmd, * TasksMsCmd, * InventoryMsCmd, * UserMsCmd, * WindowMsCmd, * HelpMsCmd; //MsCmd 1-9
			Grid* FileGrid, * EditGrid, * TasksGrid, * InventoryGrid, * UserGrid, * WindowGrid, * HelpGrid;

			Button* OpenCmd, * NewCmd, * CloseCmd, * SaveCmd, * SaveAsCmd, * ApplyCmd, * ApplyAllCmd; //File 10-19
			Button* SettingsCmd, *ProjectSettingsCmd, *RefrenceGroupsCmd, *QuickSearchCmd; //Edit 30 - 37
			Button* TasksCmd, * AddTaskCmd, * CompletedTasksCmd; //Tasks 40-49
			Button* InventoryCmd, * AddInvItemCmd, * OperInventoryCmd, * AddOperInvItemCmd, *InvSearch; //Inventory 50-59
			Button* SignInCmd, * SignOutCmd, * LockCmd, * CurrentUserCmd, * UsersCmd, * AddUserCmd, *UserSearch, *JobPositions; //User 60-67
			Button* MaximizeCmd, * MinimizeCmd, * NormalSizeCmd, *SideHost, * Quit; //Window 70 - 79
			Button* AboutCmd; //Help 80-89

			void HideRibbonGrids();
			void LoadRibbon();
			void ResizeRibbon();
			void ProcessRibbon(WPARAM wp, LPARAM lp);
		public:
			Main(HINSTANCE ins);

			void Open();
			void Close();

			HWND EditorParent() const override { return _Base; }
			HWND FooterParent() const override { return _Base; }

			void SetRibbonStatus() override;

			operator HWND() const
			{
				return _Base;
			}
		};
	}
}