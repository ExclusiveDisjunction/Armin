#include "Common.h"

#include "Config\Ins.h"
#include "Config\Recent.h"
#include "Files\ArminSessions.h"
#include "UI\Main.h"

using namespace Armin::Config;

namespace Armin
{
	namespace Colors
	{
		namespace Default //This namespace is for all of the default colors
		{
			AaColor Accent1 = 0xFF2A4158;
			AaColor Accent2 = 0xFF597387;
			AaColor Accent3 = 0xFF8C9EA3;
			AaColor Accent4 = 0xFF958A56;

			namespace DarkMode
			{
				AaColor Background1 = 0xFF22272C;
				AaColor Background2 = 0xFF484E53;

				AaColor Grey1 = 0xFF292929;
				AaColor Grey2 = 0xFF383838;
				AaColor Grey3 = 0xFF202020;

				AaColor FontColor = 0xFFFFFFFF;
				AaColor EditorGrey = 0xFF313131;
			}
			namespace LightMode
			{
				AaColor Background1 = 0xFFA8AEB3; //0xFFABA8A4;
				AaColor Background2 = 0xFFC2BDB9;

				AaColor Grey1 = 0xFF9F9F9F; //0xFFCDCDCD;
				AaColor Grey2 = 0xFFA8A8A8; //0xFFE0E0E0;
				AaColor Grey3 = 0xFF808080;

				AaColor FontColor = 0xFF353535;
				AaColor EditorGrey = Accent3; //0xFFA2BBD1; //0xFFCAD8E5;
			}
		}

		//This group is for the base values. It is done in here so that the data will not be modifyed by anyone outside of this namespace.

		AaColor Background1 = Default::DarkMode::Background1;
		AaColor Background2 = Default::DarkMode::Background2;

		AaColor Accent1 = Default::Accent1;
		AaColor Accent2 = Default::Accent2;
		AaColor Accent3 = Default::Accent3;
		AaColor Accent4 = Default::Accent4;

		AaColor Grey1 = Default::DarkMode::Grey1;
		AaColor Grey2 = Default::DarkMode::Grey2;
		AaColor Grey3 = Default::DarkMode::Grey3;
		AaColor FontColor = Default::DarkMode::FontColor;
		AaColor EditorGrey = Default::DarkMode::EditorGrey;
	}

	//The colors in this section are refrenced to the ones in the Colors namespace, but cannot be modifed. This was intentional.

	const AaColor& Background1 = Colors::Background1;
	const AaColor& Background2 = Colors::Background2;

	const AaColor& Grey1 = Colors::Grey1;
	const AaColor& Grey2 = Colors::Grey2;
	const AaColor& Grey3 = Colors::Grey3;
	const AaColor& FontColor = Colors::FontColor;
	const AaColor& EditorGrey = Colors::EditorGrey;
	
	const AaColor& Accent1 = Colors::Accent1;
	const AaColor& Accent2 = Colors::Accent2;
	const AaColor& Accent3 = Colors::Accent3;
	const AaColor& Accent4 = Colors::Accent4;

	void LoadAllColors()
	{
		if (InsInstance->LightMode)
		{
			Colors::Background1 = Colors::Default::LightMode::Background1;
			Colors::Background2 = Colors::Default::LightMode::Background2;
		
			Colors::Grey1 = Colors::Default::LightMode::Grey1;
			Colors::Grey2 = Colors::Default::LightMode::Grey2;
			Colors::Grey3 = Colors::Default::LightMode::Grey3;
			Colors::FontColor = Colors::Default::LightMode::FontColor;
			Colors::EditorGrey = Colors::Default::LightMode::EditorGrey;
		}
		else
		{
			Colors::Background1 = Colors::Default::DarkMode::Background1;
			Colors::Background2 = Colors::Default::DarkMode::Background2;

			Colors::Grey1 = Colors::Default::DarkMode::Grey1;
			Colors::Grey2 = Colors::Default::DarkMode::Grey2;
			Colors::Grey3 = Colors::Default::DarkMode::Grey3;
			Colors::FontColor = Colors::Default::DarkMode::FontColor;
			Colors::EditorGrey = Colors::Default::DarkMode::EditorGrey;
		}
	}

	UI::FooterHost* FooterOutput;
	UI::RibbonHost* MasterRibbon;

	HICON ArminIcon = NULL;
	HCURSOR ArminCursor = LoadCursorW(NULL, IDC_ARROW);

	Config::Ins* InsInstance = nullptr;
	Config::Recent* RecentInstance = nullptr;

	Files::ArminSessionBase* LoadedSession;
	String LoadedSessionPath;
	bool HasEdit;

	const String Version = L"2021.2";
	const String TitleFontName = L"Baskerville Old Face";
	const String StandardFontName = L"Candara";
}