#include "Recent.h"

#include "Files\Settings Parsing.h"
#include "Ins.h"

#include <filesystem>

namespace Armin::Config
{
	namespace RecentData
	{
		String _Path;
	}
	
	using namespace RecentData;

	void Recent::Initalize()
	{
		_Path = Ins::BaseDir() + L"config\\recent.armin";

		if (!std::filesystem::exists(static_cast<std::filesystem::path>(Ins::BaseDir() + L"config")))
			std::filesystem::create_directory(static_cast<std::filesystem::path>(Ins::BaseDir() + L"config"));
	}

	void Recent::LoadData()
	{
		SettingsParser* Parser = SettingsParser::Load(_Path);
		if (!Parser)
			return;

		RecentPaths = Parser->GetAllValues();
		RecentPaths.Reverse();
		
		delete Parser;
	}
	void Recent::Save()
	{
		StringList Temp = RecentPaths;
		Temp.Reverse();

		SettingsParser* Parser = SettingsParser::Create(_Path);
		if (!Parser)
			return;

		SettingsItemList* Items = Parser->SettingsItems();
		if (!Items)
			return;

		for (String Item : Temp)
			Items->Add(Setting, L"", Item);

		Parser->Save();
		delete Parser;
	}

	StringList Recent::GetRecentsForUI()
	{
		StringList& Raw = RecentPaths;
		StringList Return;

		for (uint i = 0u; i < Raw.Size; i++)
			Return.Add(GetRecentName(Raw[i]));

		return Return;
	}
	String Recent::operator[](uint Index)
	{
		if (Index >= RecentPaths.Size || Index < 0)
			return String();

		return RecentPaths[Index];
	}
	String Recent::GetRecentName(String Path)
	{
		String Fileext = FileExt(Path);

		if (Fileext == L"arminproj" || Fileext == L"arminrcproj" || Fileext == L"armininvproj" || Fileext == L"arminrcinvproj" || Fileext == L"arminteamproj" || Fileext == L"arminrcteamproj")
			return FileName(Path) + L" - 2021.2 Armin Project";
		else if (Fileext == L"armin")
			return FileName(Path) + L" - 2021.1 Armin Project";
		else if (Fileext == L"arminuni")
			return FileName(Path) + L" - Armin Uni-Project";

		return FileName(Path) + L" - Unknown File Type";
	}

	void Recent::AddRecent(String Value)
	{
		if (!FileExists(Value))
			return;

		if (RecentPaths.Contains(Value))
		{
			uint Index = RecentPaths.IndexOf(Value);
			RecentPaths.InsertAt(0, Value);
			RecentPaths.RemoveAt(Index);
		}
		else
			RecentPaths.Add(Value);
	}
	void Recent::DeleteRecent(uint Index)
	{
		RecentPaths.RemoveAt(Index);
	}
	void Recent::DeleteAllRecents()
	{
		RecentPaths.Clear();
	}
}