#pragma once

#include <Windows.h>

#include "UI\Controls.h"

namespace Armin
{
	namespace Files
	{
		class Image;
	}

	namespace UI::Tool
	{
		class CreateImage
		{
		private:
			CreateImage();
			void Construct(HINSTANCE ins);

			Files::Image* _Created;

			HWND _Base;
			bool _Loaded, _Mondal;
			
			Label* Text,* TitleTxt;
			TextBox* FilePath, *Title;
			Button* SelectPath;
			Button* BeginImport, * Cancel;

			void LoadControls();

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

			LRESULT Paint();
			LRESULT Size();
			LRESULT Command(WPARAM wp, LPARAM lp);

			Files::Image* GenerateImage();
		public:
			CreateImage(const CreateImage& Obj) = delete;
			CreateImage(const CreateImage&& Obj) = delete;
			~CreateImage();

			CreateImage& operator=(const CreateImage& Obj) = delete;
			CreateImage& operator=(const CreateImage&& Obj) = delete;

			static Files::Image* Execute(HINSTANCE ins, bool Mondal);
			static LRESULT RunMessageLoop(CreateImage* Obj, HINSTANCE ins, bool* Running);
		};
	}
}