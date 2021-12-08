#pragma once

namespace Armin::Files
{
	enum ComponentTypes
	{
		CT_None = 0,
		CT_User = 1,
		CT_Task = 2,
		CT_CompletedTask = 4,
		CT_InventoryItem = 16,
		CT_JobPosition = 32,
		CT_ConfigItem = 128,
		CT_OperationInventoryItem = 256,
		CT_RefrenceGroup = 512,
		CT_TimecardEntry = 1024,
		CT_Image = 2048,
		CT_All = CT_User | CT_Task | CT_CompletedTask | CT_InventoryItem | CT_JobPosition | CT_ConfigItem | CT_OperationInventoryItem | CT_RefrenceGroup | CT_TimecardEntry | CT_Image
	};
}