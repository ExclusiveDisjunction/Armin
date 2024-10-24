#pragma once

#include "..\..\Common.h"
#include "..\..\UICommon.h"

//TODO: Complete FileVersionControl & implement in SessionControl

namespace Armin
{
	namespace Files
	{
		namespace V2021
		{
			class ArminSessionBase;
		}

		class ProjectBase;
	}
	namespace UI::Tool
	{
		class FileVersionControl
		{
		private:
			HWND _Base;
			bool _Loaded;

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

		public:
			FileVersionControl(HINSTANCE ins);

			static bool Execute(HINSTANCE ins, Files::ProjectBase* ToRender);
			static bool Execute(HINSTANCE ins, Files::V2021::ArminSessionBase* ToConvert);
			static LRESULT RunMessageLoop(FileVersionControl* Obj, HINSTANCE ins, WindowState Running);
		};
	}
}