#include "EditorButtonHost.h"

#include "EditorFrame.h"
#include "EditorRegistry.h"
#include "..\UI\EditorButton.h"

namespace Armin::Editors
{
	using namespace UI;

	EditorButtonHost::~EditorButtonHost()
	{
		CurrentEdtBttnY = 10;
	}

	void EditorButtonHost::ProcessOpenEditor(EditorFrame* Editor)
	{
		EditorFrame* OldCurrent = Current;		
		EditorHost::ProcessOpenEditor(Editor);

		LastEditor = OldCurrent;
	}

	void EditorButtonHost::AddEditorButton(EditorFrame* Editor)
	{
		RECT WndRect;
		GetClientRect(this->EditorButtonsBase, &WndRect);

		int XCoord = 10;
		int YCoord = CurrentEdtBttnY;
		int Height = 30;
		int Width = WndRect.right - 10 - XCoord;

		this->EditorButtonsBase->Move(0, 0, WndRect.right, CurrentEdtBttnY + Height + 10);
		EditorButtonsScroll->Reset();

		EditorButtons.Add(new EditorButton(XCoord, YCoord, Width, Height, EditorButtonsBase, reinterpret_cast<HINSTANCE>(GetWindowLongPtrW(*EditorButtonsBase, GWLP_HINSTANCE)), Editor, this, Accent1));
		CurrentEdtBttnY += Height + 10;
	}
	void EditorButtonHost::RemoveEditorButton(EditorFrame* Editor)
	{
		for (uint i = 0; i < EditorButtons.Size; i++)
		{
			EditorButton* Current = EditorButtons[i];
			if (Current->Current() == Editor)
			{
				EditorButtons.Remove(Current);
				delete Current;
				break;
			}
		}

		ResizeEditorButtons();
	}
	void EditorButtonHost::ResizeEditorButtons()
	{
		RECT WndRect;
		GetClientRect(EditorButtonsBase, &WndRect);

		int XCoord = 10;
		int& YCoord = (CurrentEdtBttnY = 10);
		int Width = WndRect.right - XCoord - 10;
		int Height = 30;

		EditorButtonsBase->Move(0, 0, WndRect.right, (Height + 10) * (EditorButtons.Size + 1));
		EditorButtonsScroll->Reset();

		for (uint i = 0; i < EditorButtons.Size; i++)
		{
			EditorButtons[i]->Move(XCoord, YCoord, Width, Height);
			YCoord += 10 + Height;
		}
	}
	void EditorButtonHost::LoadEditorButtonsBase(HWND Parent, HINSTANCE ins, int XCoord, int YCoord, int Width, int Height, AaColor BaseBk)
	{
		StyleSheet Style;
		Style.Background = BaseBk;
		Style.BorderBrush = 0x00FFFFFF;
		Style.BorderThickness = 0;
		Style.Radius = 20;

		EditorButtonsScroll = new ScrollViewer(XCoord, YCoord, Width, Height, Parent, ins, Style);
		EditorButtonsBase = new Grid(0, 0, Width, 40, EditorButtonsScroll, ins, Style);
		EditorButtonsScroll->SetViewer(EditorButtonsBase);
	}
	void EditorButtonHost::MoveEditorButtonsBase(int XCoord, int YCoord, int Width, int Height)
	{
		MoveWindow(*EditorButtonsScroll, XCoord, YCoord, Width, Height, true);
		ResizeEditorButtons();
	}

	void EditorButtonHost::OpenLastEditor()
	{
		Vector<EditorFrame*> AllEditors = EditorRegistry::GetEditors(EDT_All, this);
		for (EditorFrame*& Obj : AllEditors)
			ShowWindow(*Obj, SW_HIDE);

		if (LastEditor && LastEditor->Host == this)
		{
			ShowWindow(*LastEditor, SW_SHOW);
			Current = LastEditor;
			LastEditor = nullptr;
		}
		else if (EditorButtons.Size != 0)
		{
			Vector<EditorButton*> Temp = EditorButtons;
			Temp.Reverse();

			uint Index = 0;
			EditorFrame* Target = nullptr;

			for (uint i = 0; i < Temp.Size; i++)
			{
				EditorFrame* ThisRun = Temp[i]->Current();
				if (ThisRun->Host == this)
				{
					Target = ThisRun;
					break;
				}
			}

			Current = Target;
			if (Current)
				ShowWindow(*Current, SW_SHOW);
			Temp.Clear();
		}
	}
}