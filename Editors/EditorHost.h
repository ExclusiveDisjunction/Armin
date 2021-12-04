#pragma once

#include <Windows.h>
#include "..\Common.h"

namespace Armin
{
	namespace UI
	{
		class EditorButton;
	}

	namespace Editors
	{
		class EditorFrame;
		class EditorRegistry;

		/**
		* When a class wants to be able to host editors, they must inherit this class.
		*/
		class EditorHost
		{
		protected:
			EditorFrame* Current;
			virtual void ProcessOpenEditor(EditorFrame* Frame);
			virtual void ProcessCloseCurrent()
			{
				Current = nullptr;
				if (SingleHostEditor())
					DestroyWindow(operator HWND());
					
			}
			RECT EditorPlacement;

			friend EditorRegistry;
			friend UI::EditorButton;

			EditorHost() {}
		public:
			EditorHost(const EditorHost& Obj) = delete;
			EditorHost(const EditorHost&& Obj) = delete;
			virtual ~EditorHost() {}

			virtual HWND EditorParent() const = 0;
			operator HWND() const
			{
				return GetAncestor(EditorParent(), GA_ROOT);
			}
			/// <summary>
			/// When true, this editor host is only allowed to have one editor open, and once it closes so does this host.
			/// </summary>
			/// <returns></returns>
			virtual bool SingleHostEditor() const { return true; }
		};
	}

}