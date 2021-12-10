#pragma once

#include "UI\Button.h"
#include "UI\CheckableButton.h"
#include "UI\TextBox.h"
#include "UI\Grid.h"
#include "..\..\UICommon.h"

namespace Armin::UI
{
	class NewFile
	{
	private:
		HWND _Base;
		bool _Loaded;

		String _Return, _Username, _Password;
		int _Config;

		Vector<ControlBase*> MiscControls; //Labels and such
		TextBox* Name = nullptr, * Directory = nullptr; //Location & File Name
		Button* SelectDirectory = nullptr, * Submit = nullptr, * Cancel = nullptr; //Buttons for Commands
		CheckableButton* InventorySys = nullptr, * TaskSys = nullptr, * UserSys = nullptr, * ResourceSys = nullptr; //Options for different systems in armin.
		TextBox* Username = nullptr, * Password = nullptr; //Username and Password for Root User.
		Button* StdProj = nullptr, * InvProj = nullptr, * TaskProj = nullptr;

		void CreateBase(HINSTANCE ins);

		LRESULT Paint();
		LRESULT Size();
		LRESULT KeyDown(WPARAM Key);
		LRESULT Command(WPARAM wp, LPARAM lp);

		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
	public:
		NewFile();
		NewFile(const NewFile& Obj) = delete;
		NewFile(const NewFile&& Obj) = delete;
		~NewFile()
		{
			SetWindowLongPtr(_Base, GWLP_USERDATA, 0);
			DestroyWindow(_Base);
		}

		NewFile& operator=(const NewFile& Obj) = delete;
		NewFile& operator=(const NewFile&& Obj) = delete;

		static LRESULT RunMessageLoop(NewFile* Object, HINSTANCE ins, bool* Running);
	
		String const& Return = _Return;
		static String Execute(HINSTANCE ins, String& Username, String& Password, int& Config);
	};
}