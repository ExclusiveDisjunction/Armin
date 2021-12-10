#pragma once

#include "Components.h"

/*
* This header contains the 2021.2 version of the Component Model with slight tweaks. This data is stored to keep support for previous types.
*/

namespace Armin::Files::V2021
{
	class InventorySystem : virtual public ArminSessionBase, public InventoryItemParent, public OperationInventoryItemParent
	{
	public:
		InventorySystem() : ArminSessionBase(), InventoryItemParent(this), OperationInventoryItemParent(this) { }
	};
	class UserSystem : virtual public ArminSessionBase, public UserParent, public JobPositionParent
	{
	public:
		UserSystem() : ArminSessionBase(), UserParent(this), JobPositionParent(this) { }
	};
	class TaskSystem : virtual public ArminSessionBase, public TaskParent, public CompletedTaskParent
	{
	public:
		TaskSystem() : ArminSessionBase(), TaskParent(this), CompletedTaskParent(this) { }
	};
	class ResourceSystem : virtual public ArminSessionBase, public ImageParent, public DirectoryParent
	{
	public:
		ResourceSystem() : ArminSessionBase(), ImageParent(this), DirectoryParent(this)	{ }

		bool CheckResources(Vector<AString>& MissingPaths);
	};

	class Project : public UserSystem, public TaskSystem, public InventorySystem
	{
	public:
		Project();
		Project(String Path);

		void Save() override;
		void Load() override;

		ArminSessionBase* ParentFile() override { return this; }

		/**
		* Retrives a Component* From the current project. If the object does not exist, or the object is not of the type specified in Filter, it returns nullptr.
		*
		* @param The ID of the object to retrive. Must be non-zero.
		* @param The type of object to retreive. This may be of diffrent types. By default, this is set to CT_All
		*/
		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};

	class ProjectRc : public UserSystem, public TaskSystem, public InventorySystem, public ResourceSystem
	{
	public:
		ProjectRc() : ArminSessionBase(), UserSystem(), TaskSystem(), InventorySystem(), ResourceSystem() {}
		ProjectRc(String Path) : ArminSessionBase(Path), UserSystem(), TaskSystem(), InventorySystem(), ResourceSystem() { Load(); }

		void Save() override;
		void Load() override;

		ArminSessionBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};

	class InventoryProject : public InventorySystem
	{
	public:
		InventoryProject(String Path) : ArminSessionBase(Path), InventorySystem() { Load(); }
		InventoryProject() : ArminSessionBase(), InventorySystem() { }

		void Save() override;
		void Load() override;

		Component* GetFromID(unsigned long long ID, int Filter) const override;

		ArminSessionBase* ParentFile() override { return this; }
	};
	
	class InventoryProjectRc : public InventorySystem, public ResourceSystem
	{
	public:
		InventoryProjectRc() : ArminSessionBase(), InventorySystem(), ResourceSystem() {}
		InventoryProjectRc(String Path) : ArminSessionBase(Path), InventorySystem(), ResourceSystem() { Load(); }

		void Save() override;
		void Load() override;

		ArminSessionBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};

	class TeamProject : public UserSystem, public TaskSystem
	{
	public:
		TeamProject() : ArminSessionBase(), UserSystem(), TaskSystem() {}
		TeamProject(String Path) : ArminSessionBase(Path), UserSystem(), TaskSystem() { Load(); }

		void Save() override;
		void Load() override;

		ArminSessionBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};
	
	class TeamProjectRc : public UserSystem, public TaskSystem, public ResourceSystem
	{
	public:
		TeamProjectRc() : ArminSessionBase(), TaskSystem(), ResourceSystem() {}
		TeamProjectRc(String Path) : ArminSessionBase(Path), TaskSystem(), ResourceSystem() { Load(); }

		void Save() override;
		void Load() override;

		ArminSessionBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};
}