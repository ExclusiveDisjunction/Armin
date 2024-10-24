#pragma once

#include "Container.h"
#include "UI\Controls.h"

namespace Armin::Files
{
	class ComponentReference;
	class Component;
}

namespace Armin::UI
{
	class ComponentSidebar : public ControlBase
	{
	private:
		Files::ComponentReference* BTarget;
		Files::Component* Target;

		AaColor BkColor;
		bool _Loaded;

		ControlList MiscControls;
		Vector<Label*> Data;
		Vector<Button*> ExtraButtons;
		Button* OpenFullView;

		static ATOM _ThisAtom;
		static void InitBase(HINSTANCE ins);
		static LRESULT __stdcall WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp);

		void LoadControls();
		void DestroyControls();
	public:
		ComponentSidebar(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, AaColor Background, Files::Component* Target);
		ComponentSidebar(int X, int Y, int Width, int Height, ControlBase* Parent, HINSTANCE ins, AaColor Background, Files::Component* Target) : ComponentSidebar(X, Y, Width, Height, (!Parent ? nullptr : *Parent), ins, Background, Target) {}
		~ComponentSidebar();

		Files::Component* TargetData() const;
		void TargetData(Files::Component* New);

		LRESULT Paint() override;
		LRESULT Size() override;
		LRESULT Command(WPARAM wp, LPARAM lp) override;
	};
}