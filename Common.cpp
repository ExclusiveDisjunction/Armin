#include "Common.h"
#include "UICommon.h"

#include "Ins.h"
#include "Recent.h"
#include "Files\ArminSessions.h"

namespace Armin
{
	namespace Colors
	{
		//This group is for the base values. It is done in here so that the data will not be modifyed by anyone outside of this namespace.

		AaColor Background1 = 0xFF22272C;
		AaColor Background2 = 0xFF484E53;

		AaColor Accent1 = 0xFF2A4158;
		AaColor Accent2 = 0xFF597387;
		AaColor Accent3 = 0xFF8C9EA3;
		AaColor Accent4 = 0xFF958A56;

		AaColor Grey1 = 0xFF292929;
		AaColor Grey2 = 0xFF383838;
		AaColor Grey3 = 0xFF202020;
		AaColor FontColor = 0xFFFFFFFF;
		AaColor EditorGrey = 0xFF313131;
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

	UI::FooterHost* FooterOutput;
	UI::RibbonHost* MasterRibbon;

	HICON ArminIcon = NULL;
	HCURSOR ArminCursor = LoadCursorW(NULL, IDC_ARROW);

	Ins* InsInstance = nullptr;
	Recent* RecentInstance = nullptr;

	Files::ProjectBase* LoadedProject;
	String LoadedProjectPath;
	int AppState = 0;

	const String Version = L"2022.1";
	const String TitleFontName = L"Baskerville Old Face";
	const String StandardFontName = L"Candara";
}