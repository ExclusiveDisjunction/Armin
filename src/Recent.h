#pragma once

#include "Common.h"

namespace Armin
{
	class Recent
	{
	public:
		Recent() {}
		Recent(const Recent& Other) = delete;
		Recent(const Recent&& Other) = delete;

		static void Initalize();
		
		void LoadData();
		void Save();

		StringList RecentPaths;
		String operator[](uint Index);
		StringList GetRecentsForUI();	
		String GetRecentName(String Path);

		void AddRecent(String Value);
		void DeleteRecent(uint Index);
		void DeleteAllRecents();

		Recent& operator=(const Recent& Other) = delete;
		Recent& operator=(const Recent&& Other) = delete;
	};
}