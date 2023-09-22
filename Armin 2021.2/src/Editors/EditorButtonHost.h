#pragma once

#include "Container.h"
#include "EditorHost.h"
#include "UI\Controls.h"
#include "..\Common.h"
#include "..\UI\ArminControls.h"

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

		class EditorButtonHost : public EditorHost
		{
		protected:
			Grid* EditorButtonsBase;
			ScrollViewer* EditorButtonsScroll;
			Vector<UI::EditorButton*> EditorButtons;

			void ProcessOpenEditor(EditorFrame* New) override;
			void ProcessCloseCurrent() override
			{
				Current = nullptr;

				OpenLastEditor();
			}

			EditorFrame* LastEditor;

			int CurrentEdtBttnY = 10;
			virtual void AddEditorButton(EditorFrame* Editor);
			virtual void RemoveEditorButton(EditorFrame* Editor);
			virtual void ResizeEditorButtons();

			virtual void LoadEditorButtonsBase(HWND Parent, HINSTANCE ins, int XCoord, int YCoord, int Width, int Height, AaColor BaseBk);
			virtual void MoveEditorButtonsBase(int XCoord, int YCoord, int Width, int Height);
		public:
			EditorButtonHost() {}
			EditorButtonHost(const EditorButtonHost& Obj) = delete;
			EditorButtonHost(const EditorButtonHost&& Obj) = delete;
			virtual ~EditorButtonHost();
			friend EditorRegistry;
			friend UI::EditorButton;

			EditorButtonHost& operator=(const EditorButtonHost& Obj) = delete;
			EditorButtonHost& operator=(const EditorButtonHost&& Obj) = delete;

			virtual bool SingleHostEditor() const override { return false; }
			void OpenLastEditor();
		};
	}\
}