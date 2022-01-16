#pragma once

#include <Windows.h>
#include "Container.h"
#include "UI\ControlBase.h"

namespace Armin
{
	namespace Editors
	{
		class EditorFrame;
		class EditorHost;
	}

	namespace UI
	{
		class EditorButton : public ControlBase
		{
		private:
			Editors::EditorFrame* Source;
			Editors::EditorHost* Host;
			bool _Clicked = false, HasMouse;

			StyleSheet Style;
			TextStyleSheet TextStyle;

			static ATOM _ThisAtom;
			static void InitBase(HINSTANCE ins);
			static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

		public:
			EditorButton(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, Editors::EditorFrame* Source, Editors::EditorHost* Host, StyleSheet Style, TextStyleSheet TextStyle);
			EditorButton(int XCoord, int YCoord, int Width, int Height, ControlBase* Parent, HINSTANCE ins, Editors::EditorFrame* Source, Editors::EditorHost* Host, StyleSheet Style, TextStyleSheet TextStyle) : EditorButton(XCoord, YCoord, Width, Height, (!Parent ? NULL : *Parent), ins, Source, Host, Style, TextStyle) {}
			~EditorButton();

			LRESULT Paint() override;
			LRESULT Click() override;
			LRESULT MouseLeave();
			LRESULT MouseMove();
			LRESULT RightClick() override;
			LRESULT TimerTick(LONG wp) override;

			Editors::EditorFrame* Current() { return Source; }
		};
	}
}