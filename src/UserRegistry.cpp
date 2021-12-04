#include "UserRegistry.h"

#include "Config\Ins.h"
#include "Editors\EditorFrame.h"
#include "Editors\EditorRegistry.h"
#include "Files\ArminSessions.h"
#include "UI\RibbonHost.h"
#include "UI\User\SignIn.h"

namespace Armin
{
	using namespace Files;
	using namespace Config;
	using namespace Editors;
	using namespace UI;
	using namespace UI::Users;

	Files::User* UserRegistry::_Current = nullptr;
	UserSystem* UserRegistry::_File = nullptr;
	UserSet* UserRegistry::_Users = nullptr;
	UserRegStatus UserRegistry::Status = URS_NotActive;
	HINSTANCE UserRegistry::ThisInstance = nullptr;

	void UserRegistry::Initalize()
	{
		_File = nullptr;
		_Current = nullptr;
		_Users = nullptr;

		Status = URS_NotActive;
	}
	void UserRegistry::Initalize(UserSystem* Target, HINSTANCE ins)
	{
		if (!Target || !Target->Users)
		{
			Initalize();
			return;
		}

		_File = Target;
		_Users = Target->Users;

		Status = URS_NotSignedIn;
		ThisInstance = ins;
	}

	bool UserRegistry::IsSignedIn()
	{
		return _Current != nullptr;
	}
	Files::User* UserRegistry::CurrentUser()
	{
		return _Current;
	}
	UserType UserRegistry::CurrentUserType()
	{
		if (!_Current)
			return UT_Undefined;

		if (_Current->IsAdmin)
			return UT_Admin;
		else if (_Current->IsAssurance)
			return UT_Assurance;
		else
			return UT_Standard;
	}

	void UserRegistry::SignIn()
	{
		if (Status == URS_NotActive)
			return;

		if (IsSignedIn())
			SignOut();

		if (FooterOutput)
			FooterOutput->SetFooterTextTillNext(L"Signing In...");

		_Current = SignIn::Execute(nullptr, false, false, false, ThisInstance);

		if (_Current)
		{
			Status = URS_SignedIn;

			if (MasterRibbon)
				MasterRibbon->SetRibbonStatusDef();

			if (FooterOutput)
				FooterOutput->SetFooterText(L"Sign In Sucessful");

			EditorRegistry::OpenEditor(new Editors::Users::UserHomepageEditor(_Current), nullptr);
		}
		else
		{
			Status = URS_NotSignedIn;

			if (MasterRibbon)
				MasterRibbon->SetRibbonStatusDef();

			if (FooterOutput)
				FooterOutput->SetFooterText(L"Sign In Failed");
		}
	}
	Files::User* UserRegistry::SignInForAssurance()
	{
		if (Status == URS_NotActive)
			return nullptr;

		MessageBoxW(nullptr, L"Please sign in as an Assurance or Admin user to complete this task.", L"Sign in for Admin:", MB_OK | MB_ICONINFORMATION);
		return SignIn::Execute(nullptr, true, false, false, ThisInstance);
	}
	bool UserRegistry::SignInForConfirmation(Files::User* Target)
	{
		if (!Target)
			return false;

		Files::User* Returned = SignIn::Execute(Target, false, false, false, ThisInstance);
		return Returned != nullptr;
	}
	void UserRegistry::Lock()
	{
		Files::User* Target = CurrentUser();
		if (!Target)
			return;

		Files::User* Returned = SignIn::Execute(Target, false, false, true, ThisInstance);
		if (Returned == nullptr)
			SignOut();
	}
	bool UserRegistry::SignInForAdmin()
	{
		MessageBoxW(nullptr, L"Please sign in as an Admin user to complete this task.", L"Sign in for Admin:", MB_OK | MB_ICONINFORMATION);

		Files::User* Return = SignIn::Execute(nullptr, false, true, false, ThisInstance);
		return Return != nullptr;
	}
	void UserRegistry::SignOut()
	{
		if (Status == URS_NotActive)
			return;

		if (!IsSignedIn())
			return;

		if (!EditorRegistry::CloseAllEditors())
			return;

		_Current = nullptr;
		Status = URS_NotSignedIn;
		if (MasterRibbon)
			MasterRibbon->SetRibbonStatusDef();

		if (FooterOutput)
			FooterOutput->SetFooterText(L"Signed Out.");
	}
}