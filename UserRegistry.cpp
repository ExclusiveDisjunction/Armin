#include "UserRegistry.h"

#include "Ins.h"
#include "Editors\EditorFrame.h"
#include "Editors\EditorRegistry.h"
#include "Files\ArminSessions.h"
#include "UI\RibbonHost.h"
#include "UI\SignIn.h"

namespace Armin
{
	using namespace Files;
	using namespace Editors;
	using namespace UI;
	using namespace UI::Users;

	namespace UserReg
	{
		Files::User* _Current;
		Files::UserSystem* _File;
		Files::ComponentList<Files::User, Files::UserParent>* _Users;
		HINSTANCE ThisInstance;
	}

	void UserRegInit()
	{
		UserReg::_File = nullptr;
		UserReg::_Current = nullptr;
		UserReg::_Users = nullptr;

		AppState &= ~APS_HasUser;
		AppState &= ~APS_HasAdminUser;
		AppState &= ~APS_HasAssuranceUser;
		AppState &= ~APS_UserRegInit;
	}
	void UserRegInit(UserSystem* Target, HINSTANCE ins)
	{
		if (!Target || !Target->Users)
		{
			UserRegInit();
			return;
		}

		UserReg::_File = Target;
		UserReg::_Users = Target->Users;

		AppState &= ~APS_HasUser;
		AppState &= ~APS_HasAdminUser;
		AppState &= ~APS_HasAssuranceUser;
		AppState |= APS_UserRegInit;
	}

	Files::User*& CurrentUser = UserReg::_Current;

	void SignIn()
	{
		if (!(AppState & APS_UserRegInit))
			return;

		if (AppState & APS_HasUser)
			UserSignOut();

		if (FooterOutput)
			FooterOutput->SetFooterTextTillNext(L"Signing In...");

		AppState &= ~APS_HasUser;
		AppState &= ~APS_HasAdminUser;
		AppState &= ~APS_HasAssuranceUser;

		UserReg::_Current = SignIn::Execute(nullptr, false, false, false, UserReg::ThisInstance);

		if (UserReg::_Current)
		{
			AppState |= APS_HasUser;
			if (UserReg::_Current->IsAdmin)
			{
				AppState |= APS_HasAdminUser;
				AppState |= APS_HasAssuranceUser;
			}
			else if (UserReg::_Current->IsAssurance)
				AppState |= APS_HasAssuranceUser;

			if (MasterRibbon)
				MasterRibbon->SetRibbonStatusDef();

			if (FooterOutput)
				FooterOutput->SetFooterText(L"Sign In Sucessful");

			EditorRegistry::OpenEditor(new Editors::Users::UserHomepageEditor(UserReg::_Current), nullptr);
		}
		else
		{
			if (MasterRibbon)
				MasterRibbon->SetRibbonStatusDef();

			if (FooterOutput)
				FooterOutput->SetFooterText(L"Sign In Failed");
		}
	}
	Files::User* SignInForAssurance()
	{
		if (!(AppState & APS_UserRegInit))
			return nullptr;

		MessageBoxW(nullptr, L"Please sign in as an Assurance or Admin user to complete this task.", L"Sign in for Admin:", MB_OK | MB_ICONINFORMATION);
		return SignIn::Execute(nullptr, true, false, false, UserReg::ThisInstance);
	}
	bool SignInForConfirmation(Files::User* Target)
	{
		if (!Target)
			return false;

		Files::User* Returned = SignIn::Execute(Target, false, false, false, UserReg::ThisInstance);
		return Returned != nullptr;
	}
	void UserLock()
	{
		if (!(AppState & APS_UserRegInit) || !(AppState & APS_HasUser))
			return;

		Files::User* Target = UserReg::_Current;
		if (!Target)
			return;

		Files::User* Returned = SignIn::Execute(Target, false, false, true, UserReg::ThisInstance);
		if (Returned == nullptr)
			UserSignOut();
	}
	bool SignInForAdmin()
	{
		MessageBoxW(nullptr, L"Please sign in as an Admin user to complete this task.", L"Sign in for Admin:", MB_OK | MB_ICONINFORMATION);

		Files::User* Return = SignIn::Execute(nullptr, false, true, false, UserReg::ThisInstance);
		return Return != nullptr;
	}
	void UserSignOut()
	{
		if (!(AppState & APS_UserRegInit) || !(AppState & APS_HasUser))
			return;

		if (!EditorRegistry::CloseAllEditors())
			return;

		UserReg::_Current = nullptr;
	
		AppState &= ~APS_HasUser;
		AppState &= ~APS_HasAdminUser;
		AppState &= ~APS_HasAssuranceUser;

		if (MasterRibbon)
			MasterRibbon->SetRibbonStatusDef();

		if (FooterOutput)
			FooterOutput->SetFooterText(L"Signed Out.");
	}
}