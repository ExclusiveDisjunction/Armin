#pragma once

#ifndef Armin_Project
#define Armin_Project

#include "Components.h"

namespace Armin::Files
{
	class InventorySystem : virtual public ProjectBase, public InventoryItemParent, public OperationInventoryItemParent
	{
	public:
		InventorySystem() : ProjectBase(), InventoryItemParent(this), OperationInventoryItemParent(this) { }
	};
	class UserSystem : virtual public ProjectBase, public UserParent, public JobPositionParent
	{
	public:
		UserSystem() : ProjectBase(), UserParent(this), JobPositionParent(this) { }
	};
	class TaskSystem : virtual public ProjectBase, public TaskParent, public CompletedTaskParent
	{
	public:
		TaskSystem() : ProjectBase(), TaskParent(this), CompletedTaskParent(this) { }
	};
	class ResourceSystem : virtual public ProjectBase, public ImageParent
	{
	public:
		ResourceSystem() : ProjectBase(), ImageParent(this)	{ }

		String ResourcePath;
		bool CheckResources(Vector<AString>& MissingPaths);
	};

	class Project : public UserSystem, public TaskSystem, public InventorySystem
	{
	public:
		Project();
		Project(String Path);

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

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
		ProjectRc() : ProjectBase(), UserSystem(), TaskSystem(), InventorySystem(), ResourceSystem() {}
		ProjectRc(String Path) : ProjectBase(Path), UserSystem(), TaskSystem(), InventorySystem(), ResourceSystem() { Load(); }

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};

	class InventoryProject : public InventorySystem
	{
	public:
		InventoryProject(String Path) : ProjectBase(Path), InventorySystem() { Load(); }
		InventoryProject() : ProjectBase(), InventorySystem() { }

		void Save() override;
		void Load() override;

		Component* GetFromID(unsigned long long ID, int Filter) const override;

		ProjectBase* ParentFile() override { return this; }
	};
	
	class InventoryProjectRc : public InventorySystem, public ResourceSystem
	{
	public:
		InventoryProjectRc() : ProjectBase(), InventorySystem(), ResourceSystem() {}
		InventoryProjectRc(String Path) : ProjectBase(Path), InventorySystem(), ResourceSystem() { Load(); }

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};

	class TeamProject : public UserSystem, public TaskSystem
	{
	public:
		TeamProject() : ProjectBase(), UserSystem(), TaskSystem() {}
		TeamProject(String Path) : ProjectBase(Path), UserSystem(), TaskSystem() { Load(); }

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};
	
	class TeamProjectRc : public UserSystem, public TaskSystem, public ResourceSystem
	{
	public:
		TeamProjectRc() : ProjectBase(), TaskSystem(), ResourceSystem() {}
		TeamProjectRc(String Path) : ProjectBase(Path), TaskSystem(), ResourceSystem() { Load(); }

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};
}

#endif