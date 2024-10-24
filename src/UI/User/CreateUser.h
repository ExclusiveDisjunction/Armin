#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\..\Common.h"
#include "..\..\Config\Ins.h"

#include <Windows.h>

namespace Armin
{
	namespace Files
	{
		class ArminSessionBase;
		class User;
		class UserParent;

		template<typename T, typename TParent>
		class ComponentList;
	}

	namespace UI::Users
	{
		class CreateUser
		{
		private:
			CreateUser(bool AdminOnly, HINSTANCE ins);

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

			Files::ArminSessionBase* File;
			Files::ComponentList<Files::User, Files::UserParent>* Users;
			bool Return = false;

			bool _Loaded = false;
			bool _Admin = false;
			Vector<ControlBase*> MiscControls;
			Button* CreateCmd, * CancelCmd; //ID: 1, 2
			TextBox* Username, * Password, * PasswordConfirm, * First, * Middle, * Last;
			ComboBox* UserType;
			HWND _Base;

			LRESULT KeyDown(WPARAM Key)
			{
				if (Key == VK_RETURN)
					Command(1);
				if (Key == VK_ESCAPE)
					Command(2);

				return 0;
			}
			LRESULT Paint();
			LRESULT Destroy();
			LRESULT Command(WPARAM ID);

			void Open();
			void LoadControls();
		public:
			static bool Execute(bool AdminOnly, HINSTANCE ins);
		};
	}
}