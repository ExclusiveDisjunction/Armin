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
		APS_Saving = 64,
		APS_Appending = 128,
		APS_AppendableEditorOpen = 256,
		APS_EditorOpen = 512,
		APS_UserRegInit = 1024
	};

	enum EditorTypes
	{
		EDT_Tasks = 1,
		EDT_AddTask = 2,
		EDT_ViewTask = 4,
		EDT_EditTask = 8,
		EDT_CompleteTask = 16,
		EDT_CompletedTasks = 32,
		EDT_ViewCompletedTask = 64,

		EDT_JobPositions = 128,
		EDT_AddJobPosition = 256,

		EDT_Users = 512,
		EDT_ViewUser = 1'024,
		EDT_EditUser = 2'048,
		EDT_CreateUser = 4'096,
		EDT_UserSearch = 8'192,

		EDT_Inventory = 16'384,
		EDT_OperationInventory = 32'768,
		EDT_AddInventoryItem = 65'536,
		EDT_AddOperationInventoryItem = 131'072,
		EDT_InventorySearch = 262'144,

		EDT_AddReferenceGroup = 524'288,
		EDT_ViewEditReferenceGroup = 1'048'576,
		EDT_ReferenceGroups = 2'097'152,

		EDT_BasicEditor = 4'194'304,
		EDT_BasicViewer = 8'388'608,
		EDT_QuickSearch = 16'777'216,
		EDT_ProjectSettings = 33'554'432,
		EDT_Settings = 67'108'864,

		EDT_UserHomepage = 134'217'728,
		EDT_Timecards = 268'435'456,

		EDT_ViewImage = 536'870'912,
		EDT_Images = 1'073'741'824,

		EDT_Welcome = 2'147'483'648,

		EDT_None = 0,
		EDT_All = EDT_Tasks | EDT_AddTask | EDT_ViewTask | EDT_EditTask | EDT_CompleteTask | EDT_CompletedTasks | EDT_ViewCompletedTask | EDT_JobPositions | EDT_AddJobPosition | EDT_Users | EDT_ViewUser | EDT_EditUser | EDT_CreateUser | EDT_UserSearch | EDT_Inventory | EDT_OperationInventory | EDT_AddInventoryItem | EDT_AddOperationInventoryItem | EDT_InventorySearch |EDT_AddReferenceGroup | EDT_ViewEditReferenceGroup | EDT_ReferenceGroups | EDT_BasicEditor | EDT_BasicViewer | EDT_QuickSearch | EDT_ProjectSettings | EDT_Settings |				EDT_UserHomepage | EDT_Timecards | EDT_ViewImage | EDT_Images | EDT_Welcome
	};
}