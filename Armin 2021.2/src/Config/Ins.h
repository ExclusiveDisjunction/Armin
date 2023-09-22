#pragma once

#include <filesystem>
#include <Windows.h>

#include "..\Common.h"

namespace Armin
{
	namespace Files
	{
		class ArminSessionBase;
	}

	namespace Config
	{
		class Ins
		{
		public:
			Ins()
			{
				LastWindowState = WS_Normal;
				DevMode = LightMode = LoadLast = false;
			}
			Ins(const Ins& Other) = delete;
			Ins(const Ins&& Other) = delete;

			static void InitiateBase(HINSTANCE Ins, LPWSTR CmdLine);
			static void Shutdown();

			static String BaseDir();
			static bool IsLoaded();

			void LoadData();
			void Save();

			WindowState LastWindowState;
			bool LoadLast;
			String LastLoaded;
			bool DevMode;
			bool LightMode;
			bool FirstTime;

			Ins& operator=(const Ins& Other) = delete;
			Ins& operator=(const Ins&& Other) = delete;
		};
	}
}