#include "ComponentViewer.h"

#include "UI\Controls.h"
#include "..\UserRegistry.h"
#include "..\Editors\EditorFrame.h"
#include "..\Editors\EditorRegistry.h"
#include "..\Files\ArminSessions.h"
#include "..\UI\EditorPopout.h"
#include "..\UI\Tool\ComponentListRenderer.h"

namespace Armin::UI
{
	using namespace Files;
	using namespace Editors;
	using namespace UI::Tool;

	ComponentViewer::ComponentViewer(int XCoord, int YCoord, int Width, int Height, HWND Parent, HINSTANCE ins, HMENU ID, Component* Target, bool Multi, ComponentViewerList* List, bool CanEdit, bool DisableCommands) : _IsMultiselect(Multi)
	{
		if (!_ThisAtom)
			InitBase(ins);

		_Source = Target;
		_BSource = new ComponentReference(Target);
		_ParentList = List;
		Flags = (CanEdit ? FlagStates::CanEdit : 0) | (DisableCommands ? FlagStates::DisableCommands : 0);

		_Base = CreateWindowExW(0l, MAKEINTATOM(_ThisAtom), L"", WS_CHILD | WS_VISIBLE, XCoord, YCoord, Width, Height, Parent, ID, ins, NULL);
		SetWindowLongPtrW(_Base, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		if (_ParentList)
			_ParentList->Append(this);
	}
	ComponentViewer::~ComponentViewer()
	{
		if (_ParentList && !_ParentList->IsDeleting)
			_ParentList->Pop(this);
		delete _BSource;
	}

	ATOM ComponentViewer::_ThisAtom = ATOM();
	void ComponentViewer::InitBase(HINSTANCE ins)
	{
		WNDCLASSW wn = { 0 };
		wn.lpfnWndProc = ComponentViewer::WndProc;
		wn.hInstance = ins;
		wn.hCursor = LoadCursorW(NULL, IDC_ARROW);
		wn.hbrBackground = NULL;
		wn.lpszMenuName = NULL;
		wn.lpszClassName = L"ComponentViewerClass";
		wn.hIcon = NULL;
		wn.cbClsExtra = 0;
		wn.cbWndExtra = sizeof(ComponentViewer*);
		wn.style = CS_HREDRAW | CS_VREDRAW;

		_ThisAtom = RegisterClassW(&wn);
	}
	LRESULT ComponentViewer::WndProc(HWND Window, UINT Message, WPARAM wp, LPARAM lp)
	{
		ComponentViewer* This = reinterpret_cast<ComponentViewer*>(GetWindowLongPtrW(Window, GWLP_USERDATA));
		if (!This)
			return DefWindowProcW(Window, Message, wp, lp);

		switch (Message)
		{
		case ComponentViewer_Check:
			This->Check();
			return 0;
		case ComponentViewer_Uncheck:
			This->Uncheck();
			return 0;
		case WM_MOUSEWHEEL:
			return This->MouseWheel(wp);
		case WM_MOUSEMOVE:
			return This->MouseMove();
		case WM_MOUSELEAVE:
			return This->MouseLeave();
		default:
			return CommonWndProc(Window, Message, wp, lp);
		}
	}

	void ComponentViewer::Uncheck()
	{
		Flags &= ~Checked;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
	}
	void ComponentViewer::Check()
	{
		Flags |= Checked;
		RedrawWindow(_Base, NULL, NULL, RDW_INVALIDATE | RDW_ERASENOW);
	}
	bool ComponentViewer::CheckState() const
	{ 
		return Flags & Checked;
	}
	void ComponentViewer::CheckState(bool New)
	{
		if (!_IsMultiselect && New)
		{
			HWND Parent = GetParent(_Base);

			for (HWND Child = GetTopWindow(Parent); Child != nullptr; Child = GetNextWindow(Child, GW_HWNDNEXT))
				SendMessageW(Child, ComponentViewer_Uncheck, 0, 0);
		}

		if (New)
			Check();
		else
			Uncheck();

		SendMessageW(GetParent(_Base), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(_Base)), static_cast<LPARAM>(Flags & Checked));
	}

	LRESULT ComponentViewer::Paint()
	{
		PAINTSTRUCT ps;
		HDC Dc = BeginPaint(_Base, &ps);

		AaColor HoverColor = Accent4;
		HoverColor.Reduce(0.7f);

		AaColor BkColor = (Flags & Checked) ? Accent4 : (Flags & HasMouse) ? HoverColor : Grey3, ForegroundColor = AaColor(0xFFFFFFFF);
		if (!IsEnabled)
		{
			BkColor.Reduce(0.7f);
			ForegroundColor.Reduce(0.7f);
		}

		RECT WndRect;
		GetClientRect(_Base, &WndRect);

		HBRUSH Bk = CreateSolidBrush(BkColor), Foreground = CreateSolidBrush(ForegroundColor);
		FillRect(Dc, &WndRect, Bk);

		SetTextColor(Dc, ForegroundColor);
		SetBkMode(Dc, TRANSPARENT);

		String RawText;
		if (_BSource && _BSource->Target() != _Source)
			RawText = L"Deleted Component";
		else
			RawText = _Source->ToString();

		uint FirstIndex;
		{
			if (RawText == L"No Data" || RawText == L"Deleted Component")
				FirstIndex = RawText.Length();
			else
			{
				StringList Parts = RawText.Split(L'|');
				FirstIndex = Parts[0].Length();
			}

		}
		const wchar_t* Text = RawText.ConstWchar();
		int TextLen = static_cast<int>(RawText.Length());
		if (TextPos > TextLen)
			TextPos = TextLen;
		uint TTextPos = static_cast<uint>(TextPos);

		SIZE TextSize;
		GetTextExtentPoint32W(Dc, Text, TextLen, &TextSize);

		int TextX = WndRect.left + 7;
		int TextY = (WndRect.bottom / 2) - (TextSize.cy / 2);
		RECT TextArea = { TextX, TextY, WndRect.right, WndRect.bottom };

		if (FirstIndex > TTextPos)
		{
			HFONT Font = CreateFontW(-MulDiv(13, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, FW_BOLD, true, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(StandardFontName));
			SelectObject(Dc, Font);

			DrawTextW(Dc, &Text[TTextPos], FirstIndex, &TextArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);

			SIZE FirstSize;
			GetTextExtentPoint32W(Dc, Text, FirstIndex, &FirstSize);

			TextArea.left += FirstSize.cx;
			DeleteObject(Font);
		}

		HFONT SFont = CreateFontW(-MulDiv(13, GetDeviceCaps(Dc, LOGPIXELSY), 72), 0, 0, 0, 0, false, false, false, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE, static_cast<LPCWSTR>(StandardFontName));
		SelectObject(Dc, SFont);

		DrawTextW(Dc, &Text[max(TTextPos, FirstIndex)], TextLen - max(TTextPos, FirstIndex), &TextArea, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS | DT_NOPREFIX);
		DeleteObject(SFont);

		
		DeleteObject(Bk);
		EndPaint(_Base, &ps);
		return 0;
	}
	LRESULT ComponentViewer::Click()
	{
		CheckState(!(Flags & Checked));

		return 0;// SendMessageW(GetParent(_Base), WM_COMMAND, reinterpret_cast<WPARAM>(GetMenu(_Base)), reinterpret_cast<LPARAM>(this));
	}
	LRESULT ComponentViewer::MouseMove()
	{
		if (!(Flags & HasMouse))
		{
			Flags |= HasMouse;
			TRACKMOUSEEVENT tme = { sizeof(tme) };
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = _Base;
			TrackMouseEvent(&tme);

			Redraw();
		}

		return 0;
	}
	LRESULT ComponentViewer::MouseLeave()
	{
		Flags &= ~HasMouse; //HasMouse is false
		Redraw();
		return 0;
	}
	LRESULT ComponentViewer::RightClick()
	{
		if (!_Source)
		{
			MessageBoxW(GetAncestor(_Base, GA_ROOT), L"There is no data to edit or view.", L"Error:", MB_OK | MB_ICONERROR);
			return 0;
		}

		if (_BSource && _BSource->Target() != _Source)
		{
			MessageBox(GetAncestor(_Base, GA_ROOT), L"The targeted component has been deleted an can not be viewed or edited.", L"Error:", MB_OK | MB_ICONERROR);
			return 0;
		}

		if (Flags & DisableCommands)
			return 0;

		bool Shift = GetKeyState(VK_SHIFT) & 0x8000;
		EditorHost* Parent = !Shift ? nullptr : new UI::EditorPopout(reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(_Base, GWLP_HINSTANCE)));

		if ((GetKeyState(VK_CONTROL) & 0x8000) && (Flags & CanEdit))
		{
			switch (_Source->ObjectType())
			{
			case CT_Task:
				if ((AppState & APS_HasAdminUser))
					EditorRegistry::OpenEditor(new Tasks::AddEditTaskEditor(dynamic_cast<Task*>(_Source)), Parent);
				break;
			case CT_InventoryItem:
				if ((AppState & APS_HasUser))
					EditorRegistry::OpenEditor(new Inventory::AddEditInventoryItemEditor(dynamic_cast<InventoryItem*>(_Source)), Parent);
				break;
			case CT_OperationInventoryItem:
				if ((AppState & APS_HasUser))
					EditorRegistry::OpenEditor(new Inventory::AddEditOperationInventoryItemEditor(dynamic_cast<OperationInventoryItem*>(_Source)), Parent);
				break;
			case CT_JobPosition:
				if ((AppState & APS_HasUser))
					EditorRegistry::OpenEditor(new Users::AddEditJobPositionEditor(dynamic_cast<JobPosition*>(_Source)), Parent);
				break;
			case CT_User:
				if ((AppState & APS_HasAdminUser) || CurrentUser == dynamic_cast<Files::User*>(_Source))
					EditorRegistry::OpenEditor(new Users::CreateEditUserEditor(dynamic_cast<User*>(_Source)), Parent);
				break;
			}
		}
		else
		{
			switch (_Source->ObjectType())
			{
			case CT_CompletedTask:
				EditorRegistry::OpenEditor(new Tasks::ViewCompletedTaskEditor(dynamic_cast<CompletedTask*>(_Source)), Parent);
				break;
			case CT_Task:
				if (Shift && (Flags & CanEdit))
					EditorRegistry::OpenEditor(new Tasks::CompleteTaskEditor(dynamic_cast<Task*>(_Source)), nullptr);
				else
					EditorRegistry::OpenEditor(new Tasks::ViewTaskEditor(dynamic_cast<Task*>(_Source), bool(Flags & CanEdit)), nullptr);
				break;
			case CT_InventoryItem:
			case CT_OperationInventoryItem:
			case CT_JobPosition:
				if ((AppState & APS_HasUser))
					EditorRegistry::OpenEditor(new Misc::BasicViewerEditor(_Source), Parent);
				break;
			case CT_User:
				if ((AppState & APS_HasUser))
					EditorRegistry::OpenEditor(new Users::ViewUserEditor(dynamic_cast<User*>(_Source)), Parent);
				break;
			}
		}

		return 0;
	}
	LRESULT ComponentViewer::MouseWheel(WPARAM wp)
	{
		if (GetKeyState(VK_CONTROL) & 0x8000)
		{
			int Change = GET_WHEEL_DELTA_WPARAM(wp);
			if (Change < 0)
				TextPos++;
			else
				TextPos--;

			if (TextPos < 0)
				TextPos = 0;

			Redraw();
		}

		return 0;
	}
	LRESULT ComponentViewer::KeyDown(WPARAM wp)
	{
		if (wp == VK_RETURN)
			return SendMessageW(_Base, WM_LBUTTONDOWN, 0, 0);
		else if (wp == VK_TAB)
		{
			if (GetKeyState(VK_SHIFT) & 0x8000)
				SetFocus(GetWindow(_Base, GW_HWNDPREV));
			else
				SetFocus(GetWindow(_Base, GW_HWNDNEXT));
			return 0;
		}
		else
			return SendMessageW(GetParent(_Base), WM_KEYDOWN, wp, 0);
	}
}