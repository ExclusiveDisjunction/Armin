#pragma once

#include "Container.h"
#include "UI\Controls.h"
#include "..\Common.h"

namespace Armin
{
	namespace Files
	{
		class User;
	}

	namespace UI::Users
	{
		class SignIn
		{
		private:
			SignIn(Files::User* Target, bool ForAssurance, bool ForAdmin, bool Lock, HINSTANCE ins);

			bool ToClose = false, ForAssurance = false, ForAdmin = false, Lock = false;
			Files::User* Target = nullptr, *Return = nullptr;

			HWND _Base;

			Vector<Label*> Labels;
			TextBox* Username, * Password;
			Button* Login;

			void LoadControls();

			LRESULT Paint();
			LRESULT Destroy();
			LRESULT Command(WPARAM wp, LPARAM lp);
			LRESULT KeyDown(WPARAM wp);

			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);

			void TestCondition();

		public:
			static Files::User* Execute(Files::User* Target, bool ForAssurance, bool ForAdmin, bool Lock, HINSTANCE ins);
		};
	}
}