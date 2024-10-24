#pragma once

#include "..\Common.h"

namespace Armin
{
	namespace Editors
	{
		class EditorFrame;
	}

	namespace Files
	{
		class SessionControl
		{
		private:
			SessionControl() {}

		public:
			static bool Open(HINSTANCE ins);
			static bool Open(String Path, HINSTANCE ins);
			static bool Close(HINSTANCE ins);
			static bool NewFile(HINSTANCE ins);

			static bool Apply(Editors::EditorFrame* Host, bool ShowCompleted = true);
			static bool ApplyAll();
			static bool Save(HINSTANCE ins);
			static bool SaveAsCopy(HINSTANCE ins);
			static bool SaveAs(HINSTANCE ins);
		};
	}
}