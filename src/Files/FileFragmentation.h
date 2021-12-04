#pragma once

#include <iostream>
#include "AString.h"

namespace Armin::Files
{
	class FileSegment
	{
	public:
		AString TargetPath;
		std::streampos BeginPos, EndPos;
	};
}