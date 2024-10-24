#include <iostream>
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment(lib, "Gdiplus.lib")

#include "Files\Stream.h"
#include "src\Common.h"
#include "src\Config\Ins.h"
#include "src\Config\Recent.h"
#include "src\Editors\EditorRegistry.h"
#include "src\Files\SessionControl.h"
#include "src\UI\Main.h"
#include "src\UI\SideHost.h"
#include "src\UI\Startup.h"
#include "src\UI\Welcome.h"

using namespace std;
using namespace Gdiplus;

using namespace Armin;
using namespace Armin::Config;
using namespace Armin::Files;
using namespace Armin::Editors;
using namespace Armin::UI;

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	ULONG_PTR Handle;
	GdiplusStartupInput In;
	GdiplusStartup(&Handle, &In, NULL);

	{
		Ins::InitiateBase(hInstance, lpCmdLine);
		Recent::Initalize();

		InsInstance = new Ins();
		InsInstance->LoadData();

		RecentInstance = new Recent();
		RecentInstance->LoadData();
	}

	LoadAllColors();

	if (!InsInstance->DevMode)
	{
		Startup* startup = new Startup(hInstance);
		startup->Open();

		if (startup->DidFail)
			return 1;

		delete startup;
	}	

	{
		String CommandPath(lpCmdLine);
		CommandPath.RemoveAllOf(L'\"');
		if (FileExists(CommandPath))
			SessionControl::Open(CommandPath, hInstance);
		else if (FileExists(InsInstance->LastLoaded) && InsInstance->LoadLast)
			SessionControl::Open(InsInstance->LastLoaded, hInstance);
	}

	if (!Ins::IsLoaded())
	{
		Welcome* Wel = new Welcome(hInstance);
		Wel->Open();

		if (Welcome::DoQuit)
		{
			delete Wel;
			return 0;
		}

		delete Wel;
	}

	Main* main = new Main(hInstance);
	MasterRibbon = main;
	FooterOutput = main;

	main->Open();
	delete main;

	{
		InsInstance->Save();
		RecentInstance->Save();

		delete InsInstance;
		InsInstance = nullptr;
		delete RecentInstance;
		RecentInstance = nullptr;

		Ins::Shutdown();
		EditorRegistry::ExitRegistry();
	}

	GdiplusShutdown(Handle);
	
	DestroyIcon(ArminIcon);
	DestroyCursor(ArminCursor);

	return 0;
}