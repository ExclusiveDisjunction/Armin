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

	void UserRegInit();
	void UserRegInit(Files::UserSystem* File, HINSTANCE ins);

	extern Files::User*& CurrentUser;

	void SignIn();
	Files::User* SignInForAssurance();
	bool SignInForConfirmation(Files::User* Target);
	void UserLock();
	bool SignInForAdmin();
	void UserSignOut();

	/*class UserRegistry
	{
	private:
		UserRegistry() {}

		static Files::User* _Current;
		static Files::UserSystem* _File;
		static Files::ComponentList<Files::User, Files::UserParent>* _Users;
		static HINSTANCE ThisInstance;
	public:
		static void Initalize();
		static void Initalize(Files::UserSystem* File, HINSTANCE Instance);

		static bool IsSignedIn();
		static Files::User* CurrentUser();

		static void SignIn();
		static Files::User* SignInForAssurance();
		static bool SignInForConfirmation(Files::User* Target);
		static void Lock();
		static bool SignInForAdmin();
		static void SignOut();
	};*/
}