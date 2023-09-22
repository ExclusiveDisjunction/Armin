#pragma once

#include "UI\Controls.h"
#include "..\Common.h"
#include "..\Editors\EditorHost.h"

namespace Armin::UI
{
	class EditorPopout : public Editors::EditorHost
	{
	private:
		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND, UINT, WPARAM, LPARAM);

		HWND _Base;

	protected:
		void ProcessOpenEditor(Editors::EditorFrame* Editor) override;
	private:

		LRESULT Paint();
		LRESULT Command(WPARAM wp, LPARAM lp);
		LRESULT KeyDown(WPARAM Key);
		LRESULT OnClose();
		LRESULT Destroy();
		LRESULT Size();
		LRESULT GetMinMaxInfo(LPARAM lp);
	public:
		EditorPopout(HINSTANCE ins);
		~EditorPopout();
		friend Armin::Editors::EditorRegistry;

		HWND EditorParent() const override { return _Base; }
		bool SingleHostEditor() const override { return true; }

		void Close();
		void RunMsgLoop();
		operator HWND()
		{
			return _Base;
		}
	};
}