#include "ComponentSidebar.h"

#include "..\UICommon.h"
#include "..\Files\Components.h"
#include "..\Files\ComponentRefrence.h"

namespace Armin::UI
{
	using namespace Files;

	ComponentSidebar::ComponentSidebar(int X, int Y, int Width, int Height, HWND Parent, HINSTANCE ins, AaColor Background, Files::Component* Target)
	{

	}
	ComponentSidebar::~ComponentSidebar()
	{

	}

	ATOM ComponentSidebar::_ThisAtom = ATOM();
	void ComponentSidebar::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = ComponentSidebar::WndProc;
		wn.lpszClassName = L"ComponentSidebarClass";
		wn.hCursor = ArminCursor;
		wn.hInstance = ins;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(ComponentSidebar*);
		wn.hIcon = NULL;
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT __stdcall ComponentSidebar::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ComponentSidebar* Item = reinterpret_cast<ComponentSidebar*>(GetWindowLongPtr(Window, GWLP_USERDATA));
		if (!Item)
			return DefWindowProcW(Window, Message, wp, lp);

		return CommonWndProc(Window, Message, wp, lp);
	}

	void ComponentSidebar::LoadControls()
	{

	}
	void ComponentSidebar::DestroyControls()
	{

	}

	Component* ComponentSidebar::TargetData() const
	{
		return !BTarget ? nullptr : BTarget->Target();
	}
	void ComponentSidebar::TargetData(Component* New)
	{

	}

	LRESULT ComponentSidebar::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		AaColor BkColor = this->BkColor;
		if (!IsWindowEnabled(_Base))
			BkColor.Reduce(0.7);

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(BkColor);
		FillRect(Dc, &WndRect, Bk);

		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT ComponentSidebar::Size()
	{
		return 0;
	}
	LRESULT ComponentSidebar::Command(WPARAM wp, LPARAM lp)
	{
		return 0;
	}
}