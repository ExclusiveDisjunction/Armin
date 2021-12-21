#pragma once

#ifndef Armin_Project
#define Armin_Project

#include "Components.h"

namespace Armin::Files
{
	/// <summary>
	/// Represents a collection system of Inventory tools.
	/// </summary>
	class InventorySystem : virtual public ProjectBase, public InventoryItemParent, public OperationInventoryItemParent
	{
	public:
		InventorySystem() : ProjectBase(), InventoryItemParent(this), OperationInventoryItemParent(this) { }
	};
	/// <summary>
	/// Represents a collection system of User tools.
	/// </summary>
	class UserSystem : virtual public ProjectBase, public UserParent, public JobPositionParent, public ChecklistParent
	{
	public:
		UserSystem() : ProjectBase(), UserParent(this), JobPositionParent(this), ChecklistParent(this) { }
	};
	/// <summary>
	/// Represents a collection system of Taslk tools.
	/// </summary>
	class TaskSystem : virtual public ProjectBase, public TaskParent, public CompletedTaskParent, public RequestParent
	{
	public:
		TaskSystem() : ProjectBase(), TaskParent(this), CompletedTaskParent(this), RequestParent(this) { }
	};
	/// <summary>
	/// Represents a collection system of Resource tools.
	/// </summary>
	class ResourceSystem : virtual public ProjectBase, public ImageParent
	{
	public:
		ResourceSystem() : ProjectBase(), ImageParent(this)	{ }

		AString ResourcePath;
		bool CheckResources(Vector<AString>& MissingPaths);
	};

	/// <summary>
	/// Used to configure a UniProject's systems and what data it can store.
	/// </summary>
	enum UniProjConfig
	{
		UPC_Users = 1,
		UPC_Tasks = 2,
		UPC_Inventory = 4,
		UPC_Resource = 8,
		UPC_All = UPC_Users | UPC_Tasks | UPC_Inventory | UPC_Resource
	};

	/// <summary>
	/// Represents the root node of the Component Model tree. This class is dynamic and can support different types of systems.
	/// </summary>
	class UniProject : public UserSystem, public TaskSystem, public InventorySystem, public ResourceSystem
	{
	public:
		UniProject();
		UniProject(String Path);

		int Config = UPC_All;
		void ConfigureMemory();

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

		Component* GetFromID(unsigned long long ID, int Filter) const override;
	};

	/*
	class Project : public UserSystem, public TaskSystem, public InventorySystem
	{
	public:
		Project();
		Project(String Path);

		void Save() override;
		void Load() override;

		ProjectBase* ParentFile() override { return this; }

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
	*/
}

#endif