#pragma once

#include <Windows.h>

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
		class ArminSessionBase;
	}
	namespace UI
	{
		class Main;
		class FooterHost;
		class RibbonHost;
	}

	extern UI::FooterHost* FooterOutput; //Use to display text out.
	extern UI::RibbonHost* MasterRibbon; //Use to set the state of the MasterRibbon

	extern const AaColor& Background1;
	extern const AaColor& Background2;

	extern const AaColor& Accent1;
	extern const AaColor& Accent2;
	extern const AaColor& Accent3;
	extern const AaColor& Accent4;

	extern const AaColor& Grey1;
	extern const AaColor& Grey2;
	extern const AaColor& Grey3;
	extern const AaColor& FontColor;
	extern const AaColor& EditorGrey;

	void LoadAllColors();

	extern HICON ArminIcon;
	extern HCURSOR ArminCursor;

	extern Config::Ins* InsInstance;
	extern Config::Recent* RecentInstance;

	extern Files::ArminSessionBase* LoadedSession;
	extern String LoadedSessionPath;
	extern bool HasEdit;

	extern const String Version;
	extern const String TitleFontName;
	extern const String StandardFontName;

	/*
	bool HasEdit();
	void HasEdit(bool New);
	*/
}