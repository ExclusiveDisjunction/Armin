#include "RibbonHost.h"

#include "..\UserRegistry.h"
#include "..\Config\Ins.h"

#include <thread>
#include <chrono>

namespace Armin::UI
{
	using namespace Config;

	void FooterHost::SetFooterText(String Text, int Delay)
	{
		if (!FooterText)
			return;

		FooterText->SetText(Text);
		SetTimer(FooterParent(), 2, Delay, NULL);
	}
	void FooterHost::SetFooterTextTillNext(String Text)
	{
		if (FooterText)
			FooterText->SetText(Text);
	}
	void FooterHost::ResetFooterText()
	{
		if (FooterText)
			FooterText->SetText(L"Ready");
	}
	void RibbonHost::SetRibbonStatusDef()
	{
		SetRibbonStatus((Ins::IsLoaded() ? 0 : RibbonStatus::MRS_NoFile) | (UserRegistry::CurrentUser() ? 0 : MRS_NoUser) | (!UserRegistry::CurrentUser() ? 0 : (UserRegistry::CurrentUserType() == UT_Admin ? 0 : MRS_StandardUser)));
	}
}