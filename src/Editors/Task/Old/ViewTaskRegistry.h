#pragma once

#include "Container.h"

namespace Armin
{
	namespace Files
	{
		class SessionControl;
	}

	namespace Editors::Task
	{
		class ViewTask;
		class EditTask;
		class ViewCompletedTask;


		class ViewTaskRegistry
		{
		private:
			static Vector<ViewTask*> ViewTasks;
			static Vector<EditTask*> EditTasks;
			static Vector<ViewCompletedTask*> ViewCompletedTasks;

		public:
			static void CloseAll();
			inline static bool HasEdits() { return EditTasks.Size() != 0; }

			friend ViewTask;
			friend EditTask;
			friend ViewCompletedTask;
			friend Files::SessionControl;
		};
	}
}
