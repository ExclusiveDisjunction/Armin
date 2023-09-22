#include "ArminSessions.h"

#include "Files\Stream.h"

namespace Armin::Files
{
	bool ResourceSystem::CheckResources(Vector<AString>& MissingPaths)
	{
		MissingPaths.Clear();

		for (uint i = 0; i < Directories->Count; i++)
		{
			AString Path = Directories->Item(i)->TargetPath;
			if (!FileExists((String)Path))
				MissingPaths.Add(Path);
		}

		return MissingPaths.Size == 0;
	}
}