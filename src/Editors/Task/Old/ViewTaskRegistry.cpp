#include "ViewTaskRegistry.h"

#include "ViewTask.h"
#include "EditTask.h"
#include "ViewCompletedTask.h"

namespace Armin::Editors::Task
{
	void ViewTaskRegistry::CloseAll()
	{
		for (uint i = 0; i < ViewTasks.Size(); i++)
			delete ViewTasks[i];
		ViewTasks.Clear();

		for (uint i = 0; i < EditTasks.Size(); i++)
			delete EditTasks[i];
		EditTasks.Clear();

		for (uint i = 0; i < ViewCompletedTasks.Size(); i++)
			delete ViewCompletedTasks[i];
		ViewCompletedTasks.Clear();
	}

	Vector<ViewTask*> ViewTaskRegistry::ViewTasks;
	Vector<EditTask*> ViewTaskRegistry::EditTasks;
	Vector<ViewCompletedTask*> ViewTaskRegistry::ViewCompletedTasks;
}