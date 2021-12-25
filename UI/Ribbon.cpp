#include "Main.h"

#include "Files\Stream.h"
#include "About.h"
#include "SideHost.h"
#include "Tool\CreateImage.h"
#include "..\UserRegistry.h"
#include "..\Editors\EditorFrame.h"
#include "..\Editors\EditorRegistry.h"
#include "..\Files\ArminSessions.h"
#include "..\Files\SessionControl.h"

namespace Armin::UI
{
	using namespace Files;
	using namespace Editors;

	void Main::SetRibbonStatus()
	{
		ProjectBase* Current = LoadedProject;
		InventorySystem* RInvFile = dynamic_cast<InventorySystem*>(Current);
		UserSystem* RUsrFile = dynamic_cast<UserSystem*>(Current);
		TaskSystem* RTskFile = dynamic_cast<TaskSystem*>(Current);
		ResourceSystem* RRcFile = dynamic_cast<ResourceSystem*>(Current);

		bool InventoryFile = RInvFile != nullptr,
			UserFile = RUsrFile != nullptr,
			TaskFile = RTskFile != nullptr,
			ResourceFile = RRcFile != nullptr;

		bool File = AppState & APS_FileLoaded;
		bool User = !UserFile ? true : (AppState & APS_HasUser);
		bool AdminUser = !UserFile ? true : (AppState & APS_HasAdminUser);

		bool SignedIn = (!UserFile && File) ? true : File && User;
		bool AdminSignedIn = (!UserFile && File) ? true : File && AdminUser;

		int AllowedForView = !File ? 0 :
			(InventoryFile && AdminSignedIn ? CT_InventoryItem : 0) |
			(InventoryFile && SignedIn ? CT_OperationInventoryItem : 0) |
			(TaskFile && SignedIn ? CT_Task | CT_CompletedTask : 0) |
			(UserFile && SignedIn ? CT_User | CT_JobPosition : 0) | 
			(File && SignedIn ? CT_RefrenceGroup : 0) |
			(ResourceFile && SignedIn ? CT_Image : 0);
		int AllowedForEdit = !File ? 0 :
			(InventoryFile && AdminSignedIn ? CT_InventoryItem : 0) |
			(InventoryFile && SignedIn ? CT_OperationInventoryItem : 0) |
			(TaskFile && AdminSignedIn ? CT_Task : 0) |
			(UserFile && AdminSignedIn ? CT_User : 0) |
			(UserFile && SignedIn ? CT_JobPosition : 0) |
			(File && SignedIn ? CT_RefrenceGroup : 0) | 
			(ResourceFile && SignedIn ? CT_Image : 0);

		HideRibbonGrids();
		ShowWindow(*FileGrid, SW_SHOW);
		
		EnableWindow(*UserMsCmd, File && UserFile);
		EnableWindow(*InventoryMsCmd, (AllowedForView & CT_InventoryItem) || (AllowedForView & CT_OperationInventoryItem));
		EnableWindow(*TasksMsCmd, (AllowedForView & CT_Task) || (AllowedForView & CT_CompletedTask));

		//File
		{
			EnableWindow(*CloseCmd, File);
			EnableWindow(*SaveCmd, File);
			EnableWindow(*SaveAsCmd, File);
			EnableWindow(*ApplyCmd, File);
			EnableWindow(*ApplyAllCmd, File);
		}

		//Edit
		{
			EnableWindow(*ProjectSettingsCmd, AdminSignedIn);
			EnableWindow(*RefrenceGroupsCmd, SignedIn);
			EnableWindow(*QuickSearchCmd, SignedIn);

			EnableWindow(*ViewImages, AllowedForEdit & CT_Image);
			EnableWindow(*CreateImage, AllowedForEdit & CT_Image);
		}

		//Tasks
		{
			EnableWindow(*TasksCmd, AllowedForView & CT_Task);
			EnableWindow(*AddTaskCmd, AllowedForEdit & CT_Task);
			EnableWindow(*CompletedTasksCmd, AllowedForView & CT_CompletedTask);
		}

		//Inventory
		{
			EnableWindow(*InventoryCmd, AllowedForView & CT_InventoryItem);
			EnableWindow(*AddInvItemCmd, AllowedForEdit & CT_InventoryItem);

			EnableWindow(*OperInventoryCmd, AllowedForView & CT_OperationInventoryItem);
			EnableWindow(*AddOperInvItemCmd, AllowedForEdit & CT_OperationInventoryItem);
		}

		//User
		{
			EnableWindow(*SignInCmd, File && UserFile && !User);
			EnableWindow(*SignOutCmd, SignedIn);
			EnableWindow(*LockCmd, SignedIn);
			EnableWindow(*CurrentUserCmd, SignedIn);
			EnableWindow(*UsersCmd, AllowedForView & CT_User);
			EnableWindow(*AddUserCmd, AllowedForEdit & CT_User);
			EnableWindow(*UserSearch, AdminSignedIn);
			EnableWindow(*JobPositions, SignedIn);

			//(*TimecardCmd, SignedIn);
			//EnableWindow(*TimeclockCmd, AdminSignedIn);
		}
	}

	void Main::HideRibbonGrids()
	{
		if (FileGrid)
			ShowWindow(*FileGrid, SW_HIDE);
		if (EditGrid)
			ShowWindow(*EditGrid, SW_HIDE);
		if (UserGrid)
			ShowWindow(*UserGrid, SW_HIDE);
		if (TasksGrid)
			ShowWindow(*TasksGrid, SW_HIDE);
		if (InventoryGrid)
			ShowWindow(*InventoryGrid, SW_HIDE);
		if (WindowGrid)
			ShowWindow(*WindowGrid, SW_HIDE);
		if (HelpGrid)
			ShowWindow(*HelpGrid, SW_HIDE);
	}

	void Main::LoadRibbon()
	{
		RECT WndRect;
		GetClientRect(RibbonBase, &WndRect);
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		{
			int XCoord = 5;
			int YCoord = 3;
			int Width = 140;
			int Height = 33;

			TextStyleSheet TextStyle;
			TextStyle.FontSize = 16;
			TextStyle.FontFamily = TitleFontName;
			TextStyle.Foreground = FontColor;
			TextStyle.Bold = false;
			TextStyle.CharSpacing = 1;

			StyleSheet Style;
			Style.Background = Grey2;
			Style.BorderBrush = Accent4;
			Style.BorderThickness = 4;
			Style.Radius = 0;

			int OrigX = XCoord;

			FileMsCmd = new Button(XCoord, YCoord, Width, Height, L"File", RibbonBase, (HMENU)1, ins, Style, TextStyle);
			XCoord += 5 + Width;

			EditMsCmd = new Button(XCoord, YCoord, Width, Height, L"Edit", RibbonBase, (HMENU)3, ins, Style, TextStyle);
			XCoord += 5 + Width;

			TasksMsCmd = new Button(XCoord, YCoord, Width, Height, L"Tasks", RibbonBase, (HMENU)4, ins, Style, TextStyle);
			XCoord += 5 + Width;

			InventoryMsCmd = new Button(XCoord, YCoord, Width, Height, L"Inventory", RibbonBase, (HMENU)5, ins, Style, TextStyle);
			XCoord += 5 + Width;

			UserMsCmd = new Button(XCoord, YCoord, Width, Height, L"User", RibbonBase, (HMENU)6, ins, Style, TextStyle);
			XCoord += 5 + Width;

			WindowMsCmd = new Button(XCoord, YCoord, Width, Height, L"Window", RibbonBase, (HMENU)7, ins, Style, TextStyle);
			XCoord += 5 + Width;

			HelpMsCmd = new Button(XCoord, YCoord, Width, Height, L"Help", RibbonBase, (HMENU)8, ins, Style, TextStyle);

			XCoord = OrigX;
			YCoord += 5 + Height;
			Style.Background = Grey2;
			Style.BorderThickness = 0;

			Width = WndRect.right - (XCoord * 2);
			Height = WndRect.bottom - 3 - YCoord;

			FileGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);
			EditGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);
			TasksGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);
			InventoryGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);
			UserGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);
			WindowGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);
			HelpGrid = new Grid(XCoord, YCoord, Width, Height, RibbonBase, ins, Style);

			HideRibbonGrids();
			ShowWindow(*FileGrid, SW_SHOW);
		}

		TextStyleSheet TextStyle;
		TextStyle.FontSize = 13;
		TextStyle.FontFamily = StandardFontName;
		TextStyle.Foreground = FontColor;
		TextStyle.Bold = false;

		StyleSheet Style;
		Style.BorderBrush = Accent3;
		Style.BorderThickness = 3;
		Style.Background = Grey2;
		Style.Radius = 20;

		{
			RECT ThisRect;
			GetClientRect(FileGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			OpenCmd = new Button(XCoord, YCoord, Width, Height, L"Open", FileGrid, (HMENU)10, ins, Style, TextStyle);
			XCoord += 3 + Width;

			NewCmd = new Button(XCoord, YCoord, Width, Height, L"New", FileGrid, (HMENU)11, ins, Style, TextStyle);
			XCoord += 3 + Width;

			CloseCmd = new Button(XCoord, YCoord, Width, Height, L"Close", FileGrid, (HMENU)12, ins, Style, TextStyle);
			XCoord += 12 + Width;

			SaveCmd = new Button(XCoord, YCoord, Width, Height, L"Save", FileGrid, (HMENU)13, ins, Style, TextStyle);
			XCoord += 3 + Width;

			SaveAsCmd = new Button(XCoord, YCoord, Width, Height, L"Save As", FileGrid, (HMENU)14, ins, Style, TextStyle);
			XCoord += 3 + Width;

			ApplyCmd = new Button(XCoord, YCoord, Width, Height, L"Apply", FileGrid, (HMENU)15, ins, Style, TextStyle);
			XCoord += 3 + Width;

			ApplyAllCmd = new Button(XCoord, YCoord, Width, Height, L"Apply All", FileGrid, (HMENU)16, ins, Style, TextStyle);
		}

		{
			RECT ThisRect;
			GetClientRect(EditGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			SettingsCmd = new Button(XCoord, YCoord, Width, Height, L"Settings", EditGrid, (HMENU)30, ins, Style, TextStyle);
			XCoord += 3 + Width;
			Width += 50;

			ProjectSettingsCmd = new Button(XCoord, YCoord, Width, Height, L"Project Settings", EditGrid, (HMENU)31, ins, Style, TextStyle);
			XCoord += 12 + Width;
			Width -= 20;

			RefrenceGroupsCmd = new Button(XCoord, YCoord, Width, Height, L"Refrence Groups", EditGrid, (HMENU)32, ins, Style, TextStyle);
			XCoord += 12 + Width;

			QuickSearchCmd = new Button(XCoord, YCoord, Width, Height, L"Quick Search", EditGrid, (HMENU)33, ins, Style, TextStyle);
			XCoord += 12 + Width;

			ViewImages = new Button(XCoord, YCoord, Width, Height, L"Images", EditGrid, (HMENU)38, ins, Style, TextStyle);
			XCoord += 3 + Width;

			CreateImage = new Button(XCoord, YCoord, Width, Height, L"Create Image", EditGrid, (HMENU)39, ins, Style, TextStyle);
		}

		{
			RECT ThisRect;
			GetClientRect(TasksGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			TasksCmd = new Button(XCoord, YCoord, Width, Height, L"Tasks", TasksGrid, (HMENU)40, ins, Style, TextStyle);
			XCoord += 3 + Width;

			AddTaskCmd = new Button(XCoord, YCoord, Width, Height, L"Add Task", TasksGrid, (HMENU)41, ins, Style, TextStyle);
			XCoord += 12 + Width;
			Width += 70;

			CompletedTasksCmd = new Button(XCoord, YCoord, Width, Height, L"Completed Tasks", TasksGrid, (HMENU)42, ins, Style, TextStyle);
		}

		{
			RECT ThisRect;
			GetClientRect(InventoryGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			InventoryCmd = new Button(XCoord, YCoord, Width, Height, L"Inventory", InventoryGrid, (HMENU)50, ins, Style, TextStyle);
			XCoord += 3 + Width;
			Width += 50;

			AddInvItemCmd = new Button(XCoord, YCoord, Width, Height, L"Add Inv. Item", InventoryGrid, (HMENU)51, ins, Style, TextStyle);
			XCoord += 12 + Width;

			OperInventoryCmd = new Button(XCoord, YCoord, Width, Height, L"Oper. Inventory", InventoryGrid, (HMENU)52, ins, Style, TextStyle);
			XCoord += 3 + Width;
			Width += 20;

			AddOperInvItemCmd = new Button(XCoord, YCoord, Width, Height, L"Add Oper. Inv. Item", InventoryGrid, (HMENU)53, ins, Style, TextStyle);
			XCoord += 12 + Width;

			InvSearch = new Button(XCoord, YCoord, Width, Height, L"Inv. Search", InventoryGrid, (HMENU)54, ins, Style, TextStyle);
		}

		{
			RECT ThisRect;
			GetClientRect(UserGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			SignInCmd = new Button(XCoord, YCoord, Width, Height, L"Sign In", UserGrid, (HMENU)60, ins, Style, TextStyle);
			XCoord += 3 + Width;

			SignOutCmd = new Button(XCoord, YCoord, Width, Height, L"Sign Out", UserGrid, (HMENU)61, ins, Style, TextStyle);
			XCoord += 12 + Width;

			LockCmd = new Button(XCoord, YCoord, Width, Height, L"Lock", UserGrid, (HMENU)62, ins, Style, TextStyle);
			XCoord += 3 + Width;

			CurrentUserCmd = new Button(XCoord, YCoord, Width + 20, Height, L"Current User", UserGrid, (HMENU)63, ins, Style, TextStyle);
			XCoord += 12 + (Width + 20);

			UsersCmd = new Button(XCoord, YCoord, Width, Height, L"Users", UserGrid, (HMENU)64, ins, Style, TextStyle);
			XCoord += 3 + Width;

			AddUserCmd = new Button(XCoord, YCoord, Width, Height, L"Add User", UserGrid, (HMENU)65, ins, Style, TextStyle);
			XCoord += 12 + Width;

			UserSearch = new Button(XCoord, YCoord, Width, Height, L"User Search", UserGrid, (HMENU)66, ins, Style, TextStyle);
			XCoord += 12 + Width;

			JobPositions = new Button(XCoord, YCoord, Width, Height, L"Positions", UserGrid, (HMENU)67, ins, Style, TextStyle);
			XCoord += 12 + Width;

			//TimecardCmd = new Button(XCoord, YCoord, Width, Height, L"Timecard", UserGrid, (HMENU)68, ins, Style, TextStyle);
			//XCoord += 3 + Width;

			//TimeclockCmd = new Button(XCoord, YCoord, Width, Height, L"Timeclock", UserGrid, (HMENU)69, ins, Style, TextStyle);
		}

		{
			RECT ThisRect;
			GetClientRect(WindowGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			MaximizeCmd = new Button(XCoord, YCoord, Width, Height, L"Maximize", WindowGrid, (HMENU)70, ins, Style, TextStyle);
			XCoord += 3 + Width;

			MinimizeCmd = new Button(XCoord, YCoord, Width, Height, L"Minimize", WindowGrid, (HMENU)71, ins, Style, TextStyle);
			XCoord += 3 + Width;

			NormalSizeCmd = new Button(XCoord, YCoord, Width, Height, L"Restore", WindowGrid, (HMENU)72, ins, Style, TextStyle);
			XCoord += 12 + Width;

			SideHost = new Button(XCoord, YCoord, Width, Height, L"Side Host", WindowGrid, (HMENU)73, ins, Style, TextStyle);
			XCoord += 12 + Width;

			Quit = new Button(XCoord, YCoord, Width, Height, L"Quit", WindowGrid, (HMENU)74, ins, Style, TextStyle);
		}

		{
			RECT ThisRect;
			GetClientRect(HelpGrid, &ThisRect);

			int XCoord = 5;
			int YCoord = 3;
			int Width = 100;
			int Height = ThisRect.bottom - (YCoord * 2);

			AboutCmd = new Button(XCoord, YCoord, Width, Height, L"About", HelpGrid, (HMENU)80, ins, Style, TextStyle);
		}
	}
	void Main::ResizeRibbon()
	{
		RECT WndRect;
		GetClientRect(RibbonBase, &WndRect);

		{
			int XCoord = 5;
			int YCoord = 3;
			int Width = 140;
			int Height = 33;

			int OrigX = XCoord;

			FileMsCmd->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			EditMsCmd->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			TasksMsCmd->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			InventoryMsCmd->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			UserMsCmd->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			WindowMsCmd->Move(XCoord, YCoord, Width, Height);
			XCoord += 5 + Width;

			HelpMsCmd->Move(XCoord, YCoord, Width, Height);

			XCoord = OrigX;
			YCoord += 5 + Height;

			Width = WndRect.right - (XCoord * 2);
			Height = WndRect.bottom - 3 - YCoord;

			FileGrid->Move(XCoord, YCoord, Width, Height);
			EditGrid->Move(XCoord, YCoord, Width, Height);
			TasksGrid->Move(XCoord, YCoord, Width, Height);
			InventoryGrid->Move(XCoord, YCoord, Width, Height);
			UserGrid->Move(XCoord, YCoord, Width, Height);
			WindowGrid->Move(XCoord, YCoord, Width, Height);
			HelpGrid->Move(XCoord, YCoord, Width, Height);
		}
	}
	void Main::ProcessRibbon(WPARAM wp, LPARAM lp)
	{
		HINSTANCE ins = reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE));

		switch (wp)
		{
		case 1: //File		
			HideRibbonGrids();
			ShowWindow(*FileGrid, SW_SHOW);
			break;
		case 2: //Unused
			break;
		case 3: //Edit
			HideRibbonGrids();
			ShowWindow(*EditGrid, SW_SHOW);
			break;
		case 4: //Task
			HideRibbonGrids();
			ShowWindow(*TasksGrid, SW_SHOW);
			break;
		case 5: //Inventory
			HideRibbonGrids();
			ShowWindow(*InventoryGrid, SW_SHOW);
			break;
		case 6: //User
			HideRibbonGrids();
			ShowWindow(*UserGrid, SW_SHOW);
			break;
		case 7: //Window
			HideRibbonGrids();
			ShowWindow(*WindowGrid, SW_SHOW);
			break;
		case 8: //Help
			HideRibbonGrids();
			ShowWindow(*HelpGrid, SW_SHOW);
			break;
		case 9: //Unused
			break;

			//File
		case 10: //Open
			if (SessionControl::Open(ins))
				SetWindowTextW(_Base, static_cast<LPCWSTR>(L"Armin " + String(Version) + L" - " + FileFullName(LoadedProjectPath)));
			break;
		case 11: //New
			if (SessionControl::NewFile(ins))
				SetWindowTextW(_Base, static_cast<LPCWSTR>(L"Armin " + String(Version) + L" - " + FileFullName(LoadedProjectPath)));
			break;
		case 12: //Close
			if (SessionControl::Close(ins))
			{
				SetWindowTextW(_Base, static_cast<LPCWSTR>(L"Armin " + String(Version)));
				EditorRegistry::OpenEditor(new Misc::WelcomeEditor(), nullptr);
				CurrentEdtBttnY = 5;
			}
			break;
		case 13: //Save
			SessionControl::Save(ins);
			break;
		case 14: //SaveAs
			SessionControl::SaveAs(ins);
			break;
		case 15: //Apply
			SessionControl::Apply(Current);
			break;
		case 16: //Apply All
			SessionControl::ApplyAll();
			break;
		case 17: //Unused
		case 18: //Unused	
		case 19: //Unused
			break;

			//Unused (OLD: Project)
		case 20: //Import			
		case 21: //Unused
		case 22: //Unused
		case 23: //Unused
		case 24: //Unused		
		case 25: //Unused
		case 26: //Unused
		case 27: //Unused
		case 28: //Unused
		case 29: //Unused
			break;

			//Edit
		case 30: //Settings
			EditorRegistry::OpenEditor(new Misc::SettingsEditor(), nullptr);
			break;
		case 31: //ProjectSettings
			EditorRegistry::OpenEditor(new Misc::ProjectSettingsEditor(), nullptr);
			break;
		case 32: //RefrenceGroups
			EditorRegistry::OpenEditor(new RefGroups::ReferenceGroupsEditor(nullptr), nullptr);
			break;
		case 33: //QuickSearch
			EditorRegistry::OpenEditor(new Misc::QuickSearchEditor(), nullptr);
			break;
		case 34: //Unused
		case 35: //Unused
		case 36: //Unused
		case 37: //Unused
		case 38: //Images
			EditorRegistry::OpenEditor(new Resources::ImagesEditor(), nullptr);
			break;
		case 39: //CreateImage			
		{
			Image* Target = Tool::CreateImage::Execute(ins, false);
			break;
		}

			//Tasks
		case 40: //Tasks
			EditorRegistry::OpenEditor(new Tasks::TasksEditor(nullptr), nullptr);
			break;
		case 41: //AddTask
			if (!(AppState & APS_HasAdminUser))
				MessageBoxW(_Base, L"You must be an admin to add tasks.", L"Armin:", MB_OK | MB_ICONERROR);
			else
				EditorRegistry::OpenEditor(new Tasks::AddEditTaskEditor(nullptr), nullptr);
			break;
		case 42: //Completed Tasks
			EditorRegistry::OpenEditor(new Tasks::CompletedTasksEditor(nullptr), nullptr);
			break;
		case 43: //Unused
		case 44: //Unused
		case 45: //Unused
		case 46: //Unused
		case 47: //Unused
		case 48: //Unused
		case 49: //Unused
			break;

			//Inventory
		case 50: //Inventory
			EditorRegistry::OpenEditor(new Inventory::InventoryEditor(nullptr), nullptr);
			break;
		case 51: //AddInvItem
			EditorRegistry::OpenEditor(new Inventory::AddInventoryItemEditor(), nullptr);
			break;
		case 52: //Oper inventory
			EditorRegistry::OpenEditor(new Inventory::OperationInventoryEditor(nullptr), nullptr);
			break;
		case 53: //Add oper inventory item
			EditorRegistry::OpenEditor(new Inventory::AddOperationInventoryItemEditor(), nullptr);
			break;
		case 54: //Inventory Search
			EditorRegistry::OpenEditor(new Inventory::InventorySearchEditor(nullptr, true), nullptr);
			break;
		case 55: //Unused
		case 56: //Unused
		case 57: //Unused
		case 58: //Unused
		case 59: //Unused
			break;

			//User
		case 60: //Sign In
			SignIn();
			break;
		case 61: //Sign Out
			UserSignOut();
			break;
			//case 42:
		case 62: //Lock
			UserLock();
			break;
		case 63: //CurrentUser
			if (!CurrentUser)
				MessageBoxW(_Base, L"There is no user currently signed in.\n\nPlease sign in and then try again.", L"Current User:", MB_OK | MB_ICONERROR);
			else
				EditorRegistry::OpenEditor(new Users::UserHomepageEditor(CurrentUser), nullptr);
			break;
		case 64: //Users
			EditorRegistry::OpenEditor(new Users::UsersEditor(nullptr), nullptr);
			break;
		case 65: //Add User
			if (!(AppState & APS_HasAdminUser))
				MessageBoxW(_Base, L"You must be an admin to create users.", L"Armin:", MB_OK | MB_ICONERROR);
			else
				EditorRegistry::OpenEditor(new Users::CreateEditUserEditor(nullptr), nullptr);
			break;
		case 66: //UserSearch
			EditorRegistry::OpenEditor(new Users::UserSearch(nullptr), nullptr);
			break;
		case 67: //JobPositions
			EditorRegistry::OpenEditor(new Users::JobPositionsEditor(nullptr), nullptr);
			break;
		case 68: //Timecard
			EditorRegistry::OpenEditor(new Users::TimecardsEditor(CurrentUser), nullptr);
			break;
		case 69: //Timeclock
			//EditorRegistry::OpenEditor(new Timecards::TimeclockEditor(), nullptr);
			break;

			//Window
		case 70: //Maxmimize
			ShowWindow(_Base, SW_MAXIMIZE);
			break;
		case 71: //Minimize
			ShowWindow(_Base, SW_MINIMIZE);
			break;
		case 72: //Restore
			ShowWindow(_Base, SW_NORMAL);
			break;
		case 73:
			new UI::SideHost(ins);
			break;
		case 74: //Quit
			OnClose();
			break;
		case 75: //Unused
		case 76: //Unused
		case 77: //Unused
		case 78: //Unused
		case 79: //Unused
			break;

			//Help
		case 80: //About
			new About(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));
			break;
		case 81: //Unused
		case 82: //Unused
		case 83: //Unused
		case 84: //Unused
		case 85: //Unused
		case 86: //Unused
		case 87: //Unused
		case 88: //Unused
		case 89: //Unused
			break; 
		}
	}
}