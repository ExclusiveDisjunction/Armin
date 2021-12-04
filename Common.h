#pragma once

#include "Arithmetic.h"
#include "Color.h"
#include "Container.h"
#include "Enums.h"
#include "Str.h"

namespace Armin
{
	namespace Config
	{
		class Ins;
		class Recent;
	}
	namespace Files
	{
		class ProjectBase;
	}
	namespace UI
	{
		class FooterHost;
		class RibbonHost;
	}

	extern UI::FooterHost* FooterOutput; //Use to display text out.
	extern UI::RibbonHost* MasterRibbon; //Use to set the state of the MasterRibbon

	extern Config::Ins* InsInstance;
	extern Config::Recent* RecentInstance;

	extern Files::ProjectBase* LoadedProject;
	extern String LoadedProjectPath;
	extern int AppState;

	extern const String Version;
	extern const String TitleFontName;
	extern const String StandardFontName;

	/*
	bool HasEdit();
	void HasEdit(bool New);
	*/
}