#pragma once

namespace Armin::Files::V2021
{
	enum ComponentTypes
	{
		CT_None = 0,
		CT_User = 1,
		CT_Task = 2,
		CT_CompletedTask = 4,
		CT_InventoryItem = 8,
		CT_JobPosition = 16,
		CT_ConfigItem = 32,
		CT_OperationInventoryItem = 64,
		CT_RefrenceGroup = 128,
		CT_Image = 256,
		CT_Sector = 512,
		CT_Directory = 1024,
		CT_All = CT_User | CT_Task | CT_CompletedTask | CT_InventoryItem | CT_JobPosition | CT_ConfigItem | CT_OperationInventoryItem | CT_RefrenceGroup | CT_Image | CT_Sector | CT_Directory
	};
}