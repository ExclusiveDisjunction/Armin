#include "ArminSessions.h"

#include "Files\Stream.h"

namespace Armin::Files
{
	bool ResourceSystem::CheckResources(Vector<AString>& MissingPaths)
	{
		MissingPaths.Clear();

		return MissingPaths.Size == 0;
	}
}