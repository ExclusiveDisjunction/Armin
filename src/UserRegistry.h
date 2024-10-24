#pragma once

#include <Windows.h>

namespace Armin
{
	namespace Files
	{
		class UserSystem;
		class User;
		class UserParent;

		template<typename T,typename TParent>
		class ComponentList;
	}

	enum UserRegStatus
	{
		URS_SignedIn,
		URS_NotSignedIn,
		URS_NotActive
	};

	enum UserType
	{
		UT_Admin,
		UT_Assurance,
		UT_Standard,
		UT_Undefined
	};

	class UserRegistry
	{
	private:
		UserRegistry() {}

		static Files::User* _Current;
		static Files::UserSystem* _File;
		static Files::ComponentList<Files::User, Files::UserParent>* _Users;
		static UserRegStatus Status;
		static HINSTANCE ThisInstance;
	public:
		static void Initalize();
		static void Initalize(Files::UserSystem* File, HINSTANCE Instance);

		static bool IsSignedIn();
		static Files::User* CurrentUser();
		static UserType CurrentUserType();

		static void SignIn();
		static Files::User* SignInForAssurance();
		static bool SignInForConfirmation(Files::User* Target);
		static void Lock();
		static bool SignInForAdmin();
		static void SignOut();
	};
}