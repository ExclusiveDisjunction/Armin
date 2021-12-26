#pragma once

namespace Armin
{
	enum WindowStates
	{
		WS_Maximized = 3,
		WS_Normal = 1,
		WS_Minimized = 6
	};

	enum AppStates
	{
		APS_None = 0,
		APS_FileLoaded = 1,
		APS_HasEdit = 2,
		APS_HasUser = 4,
		APS_HasAdminUser = 8,
		APS_HasAssuranceUser = 16,
		APS_ReadOnly = 32,
		APS_EditorOpen = 64,
		APS_AppendableEditorRunning = 128,
		APS_UserRegInit = 256
	};

	enum EditorTypes
	{
		EDT_Tasks = 1,
		EDT_AddEditTask = 2,
		EDT_ViewTask = 4,
		EDT_CompleteTask = 8,
		EDT_CompletedTasks = 16,
		EDT_ViewCompletedTask = 32,

		EDT_JobPositions = 64,
		EDT_AddJobEditPosition = 128,

		EDT_Users = 256,
		EDT_ViewUser = 512,
		EDT_CreateEditUser = 1'028,
		EDT_UserSearch = 2'048,

		EDT_Inventory = 4'096,
		EDT_OperationInventory = 8'192,
		EDT_AddEditInventoryItem = 16'384,
		EDT_AddEditOperationInventoryItem = 32'768,
		EDT_InventorySearch = 65'536,

		EDT_AddReferenceGroup = 131'072,
		EDT_ViewEditReferenceGroup = 262'144,
		EDT_ReferenceGroups = 524'288,

		EDT_BasicEditor = 1'048'576,
		EDT_BasicViewer = 2'097'152,
		EDT_QuickSearch = 4'194'304,
		EDT_ProjectSettings = 8'388'608,
		EDT_Settings = 16'777'216,

		EDT_UserHomepage = 33'554'432,
		EDT_Timecards = 67'108'864,

		EDT_ViewImage = 134'217'728,
		EDT_Images = 268'435'456,

		EDT_Welcome = 536'870'912,

		EDT_None = 0,
		EDT_All = EDT_Tasks | EDT_AddEditTask | EDT_ViewTask | EDT_CompleteTask | EDT_CompletedTasks | EDT_ViewCompletedTask | EDT_JobPositions | EDT_AddJobEditPosition | EDT_Users | EDT_ViewUser | EDT_CreateEditUser | EDT_UserSearch | EDT_Inventory | EDT_OperationInventory | EDT_AddEditInventoryItem | EDT_AddEditOperationInventoryItem | EDT_InventorySearch |EDT_AddReferenceGroup | EDT_ViewEditReferenceGroup | EDT_ReferenceGroups | EDT_BasicEditor | EDT_BasicViewer | EDT_QuickSearch | EDT_ProjectSettings | EDT_Settings | EDT_UserHomepage | EDT_Timecards | EDT_ViewImage | EDT_Images | EDT_Welcome
	};
}