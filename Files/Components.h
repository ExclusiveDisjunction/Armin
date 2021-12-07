#pragma once

#include <iostream>
#include <fstream>

#include "BaseTypes.h"
#include "ComponentTypes.h"
#include "DateTime.h"
#include "FileFragmentation.h"
#include "Str.h"

/*
* All components, lists, and projects are stored as pointers. Like PHS, if you delete the ProjectBase, all of its objects will be deleted as well.
* NEVER delete the pointers if they do not inherit ProjectBase. They are automaticly deleted. 
*/

namespace Armin::Files
{
	//Pre-Defs

	class ConfigItem;
	class ConfigItemParent;
	using ConfigItemSet = ComponentList<ConfigItem, ConfigItemParent>;

	//ResourceSystem

	class Sector;
	class SectorParent;
	using SectorList = ComponentList<Sector, SectorParent>;

	class Directory;
	class DirectoryParent;
	using DirectoryList = ComponentList<Directory, DirectoryParent>;

	class Image;
	class ImageParent;
	using ImageList = ComponentList<Image, ImageParent>;

	//UserSystem

	class JobPosition;
	class JobPositionParent;
	using JobPositionList = ComponentList<JobPosition, JobPositionParent>;

	class TimecardEntry;
	class TimecardEntryParent;
	using TimecardEntryList = ComponentList<TimecardEntry, TimecardEntryParent>;

	class User;
	class UserParent;
	using UserSet = ComponentList<User, UserParent>;

	//TaskSystem

	class Task;
	class TaskParent;
	using TaskList = ComponentList<Task, TaskParent>;

	class CompletedTask;
	class CompletedTaskParent;
	using CompletedTaskList = ComponentList<CompletedTask, CompletedTaskParent>;

	//InventorySystem
	class InventoryBase;

	class InventoryItem;
	class InventoryItemParent;
	using InventoryItemGroup = ComponentList<InventoryItem, InventoryItemParent>;

	class OperationInventoryItem;
	class OperationInventoryItemParent;
	using OperationInventoryItemGroup = ComponentList<OperationInventoryItem, OperationInventoryItemParent>;

	class RefrenceGroup;
	class RefrenceGroupParent;
	using RefrenceGroupList = ComponentList<RefrenceGroup, RefrenceGroupParent>;

	class ComponentReference;

	//Actual Defs

	class ConfigItem : public Component
	{
	private:
		ConfigItemSet* _ParentList;
		ConfigItemParent* _Parent;
	public:
		ConfigItem() = delete;
		ConfigItem(ProjectBase* File, ConfigItemSet* ParentList);
		ConfigItem(ProjectBase* File, ConfigItemSet* ParentList, ConfigItem* ToClone);
		ConfigItem(ProjectBase* File, ConfigItemSet* ParentList, std::ifstream& InFile);
		ConfigItem(const ConfigItem& Obj) = delete;
		ConfigItem(const ConfigItem&& Obj) = delete;
		~ConfigItem();

		ConfigItem& operator=(const ConfigItem& Obj) = delete;
		ConfigItem& operator=(const ConfigItem&& Obj) = delete;

		ConfigItem* Next, * Last;
		ConfigItemParent* const& Parent = _Parent;
		inline static const String ThisName = L"ConfigItem";

		String Value;

		ComponentTypes ObjectType() const override { return ComponentTypes::CT_ConfigItem; }
		String ToString() const override { return L"Config Item | Title: " + Title(); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;
	};
	class ConfigItemParent : public ComponentParent
	{
	private:
		ConfigItemSet* _ConfigItems;
	protected:
		ConfigItemParent(ProjectBase* File) : _ConfigItems(new ConfigItemSet(File, this)) {}
		ConfigItemParent() : _ConfigItems(nullptr) {}
	public:
		~ConfigItemParent()
		{
			delete _ConfigItems;
			_ConfigItems = nullptr;
		}

		ConfigItemSet* const& ConfigItems = _ConfigItems;
	};

#pragma region Resources

	/*
	class Sector : public Component
	{
	private:
		SectorList* _ParentList;
		SectorParent* _Parent;
	public:
		Sector() = delete;
		Sector(ProjectBase* File, SectorList* ParentList);
		Sector(ProjectBase* File, SectorList* ParentList, std::ifstream& InFile);
		Sector(ProjectBase* File, SectorList* ParentList, Sector* ToClone);
		Sector(const Sector& Obj) = delete;
		Sector(const Sector&& Obj) = delete;
		~Sector();

		Sector& operator=(const Sector& Obj) = delete;
		Sector& operator=(const Sector&& Obj) = delete;

		Sector* Next, * Last;
		SectorParent* const& Parent = _Parent;
		inline static const String ThisName = L"Sector";

		String Title() const override { return L"<,.>"; }
		void Title(String New) override { }

		std::streampos BeginPos;
		std::streampos EndPos;

		ComponentTypes ObjectType() const override { return CT_Sector; }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint TabIndex) const override;
	};
	class SectorParent : public ComponentParent
	{
	private:
		SectorList* _Sectors;
	protected:
		SectorParent(ProjectBase* File) : _Sectors(new SectorList(File, this)) {}
		SectorParent() : _Sectors(nullptr) {}
	public:
		~SectorParent()
		{
			delete _Sectors;
			_Sectors = nullptr;
		}

		SectorList* const& Sectors = _Sectors;
	};
	*/

	/*class Directory : public Component, public SectorParent
	{
	private:
		DirectoryList* _ParentList;
		DirectoryParent* _Parent;
	public:
		Directory() = delete;
		Directory(ProjectBase* File, DirectoryList* ParentList);
		Directory(ProjectBase* File, DirectoryList* ParentList, std::ifstream& InFile);
		Directory(ProjectBase* File, DirectoryList* ParentList, Directory* ToClone);
		Directory(const Directory& Obj) = delete;
		Directory(const Directory&& Obj) = delete;
		~Directory();

		Directory& operator=(const Directory& Obj) = delete;
		Directory& operator=(const Directory&& Obj) = delete;

		Directory* Next, * Last;
		DirectoryParent* const& Parent = _Parent;
		inline static const String ThisName = L"Directory";
		ProjectBase* ParentFile() override { return _ParentFile; }

		String Title() const override { return L"<,.>"; }
		void Title(String New) override {}

		AString TargetPath;

		HGLOBAL ReadSector(Sector* ToRead);
		void WriteToSector(Sector* ToWrite, HGLOBAL Data, bool ForceSize = false); //If ForceSize is false, then the function will return if there is not enough space in the sector. Otherwise, a new sector is made if there is not enough space.
		void WriteToSector(Sector* ToWrite, char* Data, unsigned long long Size, bool ForceSize = false);
		void CompressSectors();	
		Sector* CreateSector(HGLOBAL Data);
		Sector* CreateSector(unsigned long long Size);
		Sector* CreateSector(char* Data, unsigned long long Size);

		ComponentTypes ObjectType() const override { return CT_Directory; }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint TabIndex) const override;
	};
	class DirectoryParent : public ComponentParent
	{
	private:
		DirectoryList* _Directories;
	protected:
		DirectoryParent(ProjectBase* File) : _Directories(new DirectoryList(File, this)) {}
		DirectoryParent() : _Directories(nullptr) {}
	public:
		~DirectoryParent()
		{
			delete _Directories;
			_Directories = nullptr;
		}

		DirectoryList* const& Directories = _Directories;
	};
	*/

	class Image : public Component
	{
	private:
		ImageList* _ParentList;
		ImageParent* _Parent;
	public:
		Image() = delete;
		Image(ProjectBase* File, ImageList* ParentList);
		Image(ProjectBase* File, ImageList* ParentList, std::ifstream& InFile);
		Image(ProjectBase* File, ImageList* ParentList, Image* ToClone);
		Image(const Image& Obj) = delete;
		Image(const Image&& Obj) = delete;
		~Image();

		Image* Next, * Last;
		ImageParent* const& Parent = _Parent;
		inline static const String ThisName = L"Image";

		Image& operator=(const Image& Obj) = delete;
		Image& operator=(const Image&& Obj) = delete;

		std::streampos Begin, End;

		HGLOBAL GetHandle();
		void SetHandle(HGLOBAL Data);
		void SetHandle(void* Data, unsigned long long Size);

		ComponentTypes ObjectType() const override { return CT_Image; }
		String ToString() const override { return L"Image | Title: " + Title(); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint TabIndex) const override;
	};
	class ImageParent : public ComponentParent
	{
	private:
		ImageList* _Images;
	protected:
		ImageParent() : _Images(nullptr) {}
		ImageParent(ProjectBase* File) : _Images(new ImageList(File, this)) {}
	public:
		~ImageParent()
		{
			delete _Images;
			_Images = nullptr;
		}

		ImageList* const& Images = _Images;
	};

#pragma endregion

#pragma region Users

	class JobPosition : public Component
	{
	private:
		JobPositionList* _ParentList;
		JobPositionParent* _Parent;
	public:
		JobPosition() = delete;
		JobPosition(ProjectBase* File, JobPositionList* ParentList);
		JobPosition(ProjectBase* File, JobPositionList* ParentList, JobPosition* ToClone);
		JobPosition(ProjectBase* File, JobPositionList* ParentList, std::ifstream& InFile);
		JobPosition(const JobPosition& Obj) = delete;
		JobPosition(const JobPosition&& Obj) = delete;
		~JobPosition();

		JobPosition& operator=(const JobPosition& Obj) = delete;
		JobPosition& operator=(const JobPosition&& Obj) = delete;

		JobPosition* Next, * Last;
		JobPositionParent* const& Parent = _Parent;
		inline static const String ThisName = L"JobPosition";

		String Description;

		ComponentTypes ObjectType() const override
		{
			return ComponentTypes::CT_JobPosition;
		}
		String ToString() const override { return L"Job Position | Title: " + Title(); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;
	};
	class JobPositionParent : public ComponentParent
	{
	private:
		JobPositionList* _Positions;
	protected:
		JobPositionParent(ProjectBase* File) : _Positions(new JobPositionList(File, this)) {}
		JobPositionParent() : _Positions(nullptr) {}
	public:
		~JobPositionParent()
		{
			delete _Positions;
			_Positions = nullptr;
		}

		JobPositionList* const& Positions = _Positions;
	};

	class TimecardEntry : public Component
	{
	private:
		TimecardEntryList* _ParentList;
		TimecardEntryParent* _Parent;
	public:
		TimecardEntry() = delete;
		TimecardEntry(ProjectBase* File, TimecardEntryList* ParentList);
		TimecardEntry(ProjectBase* File, TimecardEntryList* ParentList, TimecardEntry* ToClone);
		TimecardEntry(ProjectBase* File, TimecardEntryList* ParentList, std::ifstream& InFile);
		TimecardEntry(const TimecardEntry& Obj) = delete;
		TimecardEntry(const TimecardEntry&& Obj) = delete;
		~TimecardEntry();

		TimecardEntry& operator=(const TimecardEntry& Obj) = delete;
		TimecardEntry& operator=(const TimecardEntry&& Obj) = delete;

		TimecardEntry* Next, * Last;
		TimecardEntryParent* const& Parent = _Parent;
		inline static const String ThisName = L"TimecardEntry";

		String Title() const override { return L"<,.>"; }
		void Title(String Value) override { }

		DateTime TimeIn, TimeOut, Date;
		String Description;

		ComponentTypes ObjectType() const override { return CT_TimecardEntry; }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint TabIndex) const override;
	};
	class TimecardEntryParent : public ComponentParent
	{
	private:
		TimecardEntryList* _TimecardEntries;
	protected:
		TimecardEntryParent(ProjectBase* File) : _TimecardEntries(new TimecardEntryList(File, this)) {}
		TimecardEntryParent() : _TimecardEntries(nullptr) {}
	public:
		~TimecardEntryParent()
		{
			delete _TimecardEntries;
			_TimecardEntries = nullptr;
		}

		TimecardEntryList* const& TimecardEntries = _TimecardEntries;
	};

	class User : public Component, public TimecardEntryParent
	{
	private:
		UserSet* _ParentList;
		UserParent* _Parent;
	public:
		User() = delete;
		User(ProjectBase* File, UserSet* Parent);
		User(ProjectBase* File, UserSet* Parent, User* ToClone);
		User(ProjectBase* File, UserSet* Parent, std::ifstream& InFile);
		User(const User& Obj) = delete;
		User(const User&& Obj) = delete;
		~User();

		User& operator=(const User& Obj) = delete;
		User& operator=(const User&& Obj) = delete;

		User* Next, * Last;
		UserParent* const& Parent = _Parent;
		inline static const String ThisName = L"User";
		ProjectBase* ParentFile() override { return _ParentFile; }

		String Title() const override {	return Username; }
		void Title(String New) override { Username = New; }

		String Username, Password, FirstName, MiddleName, LastName;
		bool IsAdmin = false, IsAssurance = false;
		ComponentReference* TargetImage = nullptr;
		ReferenceList Positions;

		ComponentTypes ObjectType() const override { return ComponentTypes::CT_User; }
		String ToString() const override { return L"User | Username: " + Username + L" | Name: " + FirstName + + L" " + MiddleName + L" " + LastName; }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;
	};
	class UserParent : public ComponentParent
	{
	private:
		UserSet* _Users;
	protected:		
		UserParent(ProjectBase* File) : _Users(new UserSet(File, this)) {}
		UserParent() : _Users(nullptr) {}
	public:
		~UserParent()
		{
			delete _Users;
			_Users = nullptr;
		}

		UserSet* const& Users = _Users;
	};

#pragma endregion 

#pragma region Inventory

	class InventoryBase : public Component
	{
	protected:
		InventoryBase(ProjectBase* File, bool GetID) : Component(File, GetID) {}
	public:
		virtual ~InventoryBase()
		{
			if (TargetImage)
				delete TargetImage;
			TargetImage = nullptr;
		}

		String Title() const override { return SerialNumber; }
		void Title(String New) override { SerialNumber = New; }

		String SerialNumber, Description, Group;
		ComponentReference* TargetImage = nullptr;
	};

	class InventoryItem : public InventoryBase
	{
	private:
		InventoryItemGroup* _ParentList;
		InventoryItemParent* _Parent;
	public:
		InventoryItem() = delete;
		InventoryItem(ProjectBase* File, InventoryItemGroup* ParentList);
		InventoryItem(ProjectBase* File, InventoryItemGroup* ParentList, InventoryItem* ToClone);
		InventoryItem(ProjectBase* File, InventoryItemGroup* ParentList, std::ifstream& InFile);
		InventoryItem(const InventoryItem& Obj) = delete;
		InventoryItem(const InventoryItem&& Obj) = delete;
		~InventoryItem();

		InventoryItem& operator=(const InventoryItem& Obj) = delete;
		InventoryItem& operator=(const InventoryItem&& Obj) = delete;

		InventoryItem* Next, * Last;
		InventoryItemParent* const& Parent = _Parent;
		inline static const String ThisName = L"InventoryItem";

		bool IsInPossession;

		ComponentTypes ObjectType() const override { return ComponentTypes::CT_InventoryItem; }
		String ToString() const override { return L"Inventory Item | Serial Number: " + SerialNumber + L" | Group: " + Group; }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;

	};
	class InventoryItemParent : public ComponentParent
	{
	private:
		InventoryItemGroup* _InventoryItems;
	protected:
		InventoryItemParent(ProjectBase* File) : _InventoryItems(new InventoryItemGroup(File, this)) {}
		InventoryItemParent() : _InventoryItems(nullptr) {}
	public:
		~InventoryItemParent()
		{
			delete _InventoryItems;
			_InventoryItems = nullptr;
		}

		InventoryItemGroup* const& InventoryItems = _InventoryItems;
	};

	class OperationInventoryItem : public InventoryBase
	{
	private:
		OperationInventoryItemGroup* _ParentList;
		OperationInventoryItemParent* _Parent;
	public:
		OperationInventoryItem() = delete;
		OperationInventoryItem(ProjectBase* File, OperationInventoryItemGroup* ParentList);
		OperationInventoryItem(ProjectBase* File, OperationInventoryItemGroup* ParentList, OperationInventoryItem* ToClone);
		OperationInventoryItem(ProjectBase* File, OperationInventoryItemGroup* ParentList, std::ifstream& InFile);
		OperationInventoryItem(const OperationInventoryItem& Obj) = delete;
		OperationInventoryItem(const OperationInventoryItem&& Obj) = delete;
		~OperationInventoryItem();

		OperationInventoryItem& operator=(const OperationInventoryItem& Obj) = delete;
		OperationInventoryItem& operator=(const OperationInventoryItem&& Obj) = delete;

		enum ItemState
		{
			IS_WorkingOrder = 1,
			IS_PartialWorkingOrder = 2,
			IS_Broken = 4,
			IS_UnderRepair = 8,
			IS_NotInPossession = 16
		};
		static String ISToString(ItemState Item);
		static AString ISToAString(ItemState Item);

		OperationInventoryItem* Next, * Last;
		OperationInventoryItemParent* const& Parent = _Parent;
		inline static const String ThisName = L"OperationInventoryItem";

		String Title() const override { return SerialNumber; }
		void Title(String New) override { SerialNumber = New; }

		ItemState CurrentState;

		ComponentTypes ObjectType() const override { return ComponentTypes::CT_OperationInventoryItem; }
		String ToString() const override { return L"Oper. Inventory Item | Serial Number: " + SerialNumber + L" | Group: " + Group + L" | State: " + ISToString(CurrentState); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;

	};
	class OperationInventoryItemParent : public ComponentParent
	{
	private:
		OperationInventoryItemGroup* _OperationInventoryItems;
	protected:
		OperationInventoryItemParent(ProjectBase* File) : _OperationInventoryItems(new OperationInventoryItemGroup(File, this)) {}
		OperationInventoryItemParent() : _OperationInventoryItems(nullptr) {}
	public:
		~OperationInventoryItemParent()
		{
			delete _OperationInventoryItems;
			_OperationInventoryItems = nullptr;
		}

		OperationInventoryItemGroup* const& OperationInventoryItems = _OperationInventoryItems;
	};

#pragma endregion

#pragma region Tasks

	class Task : public Component
	{
	private:
		TaskList* _ParentList;
		TaskParent* _Parent;
	public:
		Task() = delete;
		Task(ProjectBase* File, TaskList* ParentList);
		Task(ProjectBase* File, TaskList* ParentList, Task* ToClone);
		Task(ProjectBase* File, TaskList* ParentList, std::ifstream& InFile);
		Task(const Task& Obj) = delete;
		Task(const Task&& Obj) = delete;
		~Task();

		Task& operator=(const Task& Obj) = delete;
		Task& operator=(const Task&& Obj) = delete;

		Task* Next, * Last;
		TaskParent* const& Parent = _Parent;
		inline static const String ThisName = L"Task";
		
		ReferenceList AssignedTo;
		String Instructions;
		bool RequiresAssurance;
		DateTime DueBy;

		ComponentTypes ObjectType() const override { return ComponentTypes::CT_Task; }
		String ToString() const override { return L"Task | Title: " + Title() + L" | Due By: " + DueBy.ToString(DateStringFormat::ShortDate); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;
	};
	class TaskParent : public ComponentParent
	{
	private:
		TaskList* _Tasks;
	protected:
		TaskParent(ProjectBase* File) : _Tasks(new TaskList(File, this)) {}
		TaskParent() : _Tasks(nullptr) {}
	public:
		~TaskParent()
		{
			delete _Tasks;
			_Tasks = nullptr;
		}

		TaskList* const& Tasks = _Tasks;
	};

	class CompletedTask : public Component, public OperationInventoryItemParent, public TaskParent
	{
	private:
		CompletedTaskList* _ParentList;
		CompletedTaskParent* _Parent;
	public:
		CompletedTask() = delete;
		CompletedTask(ProjectBase* File, CompletedTaskList* ParentList);
		CompletedTask(ProjectBase* File, CompletedTaskList* ParentList, CompletedTask* ToClone);
		CompletedTask(ProjectBase* File, CompletedTaskList* ParentList, std::ifstream& InFile);
		CompletedTask(const CompletedTask& Obj) = delete;
		CompletedTask(const CompletedTask&& Obj) = delete;
		~CompletedTask();

		CompletedTask& operator=(const CompletedTask& Obj) = delete;
		CompletedTask& operator=(const CompletedTask&& Obj) = delete;

		CompletedTask* Next, * Last;
		CompletedTaskParent* const& Parent = _Parent;
		inline static const String ThisName = L"CompletedTask";

		String Title() const override;
		void Title(String New) override { }

		DateTime DateCompleted;
		ReferenceList CompletedBy;
		ComponentReference* AssuredBy;
		String ActionsDone;

		ProjectBase* ParentFile() override { return _ParentFile; }

		ComponentTypes ObjectType() const override { return ComponentTypes::CT_CompletedTask; }
		String ToString() const override { return L"Completed Task | Completed On: " + DateCompleted.ToString(DateStringFormat::LongDate) + L" | Assured By: " + (!AssuredBy || !AssuredBy->Target() ? L"No Data" : dynamic_cast<User*>(AssuredBy->Target())->Username) + L" | Title: " + (Tasks->Count == 0 ? L"No Data" : Tasks->Item(0)->Title()); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint PreIndex = 0u) const override;

	};
	class CompletedTaskParent : public ComponentParent
	{
	private:
		CompletedTaskList* _CompletedTasks;
	protected:
		CompletedTaskParent(ProjectBase* File) : _CompletedTasks(new CompletedTaskList(File, this)) {}
		CompletedTaskParent() : _CompletedTasks(nullptr) {}
	public:
		~CompletedTaskParent()
		{
			delete _CompletedTasks;
			_CompletedTasks = nullptr;
		}

		CompletedTaskList* const& CompletedTasks = _CompletedTasks;
	};

#pragma endregion

	class RefrenceGroup : public Component
	{
	private:
		RefrenceGroupList* _ParentList;
		RefrenceGroupParent* _Parent;
	public:
		RefrenceGroup() = delete;
		RefrenceGroup(ProjectBase* File, RefrenceGroupList* ParentList);
		RefrenceGroup(ProjectBase* File, RefrenceGroupList* ParentList, RefrenceGroup* ToClone);
		RefrenceGroup(ProjectBase* File, RefrenceGroupList* ParentList, std::ifstream& OutFile);
		RefrenceGroup(const RefrenceGroup& Obj) = delete;
		RefrenceGroup(const RefrenceGroup&& Obj) = delete;
		~RefrenceGroup();

		RefrenceGroup& operator=(const RefrenceGroup& Obj) = delete;
		RefrenceGroup& operator=(const RefrenceGroup&& Obj) = delete;

		RefrenceGroup* Next, * Last;
		RefrenceGroupParent* const& Parent = _Parent;
		inline static const String ThisName = L"RefrenceGroup";

		ReferenceList Targets;
		String Notes;

		ComponentTypes ObjectType() const override { return CT_RefrenceGroup; }
		String ToString() const override { return L"Refrence Group | Title: " + Title(); }
		void Fill(std::ifstream& InFile) override;
		void Push(std::ofstream& OutFile, uint TabIndex) const override;
	};
	class RefrenceGroupParent : public ComponentParent
	{
	private:
		RefrenceGroupList* _RefrenceGroups;
	protected:
		RefrenceGroupParent(ProjectBase* File) : _RefrenceGroups(new RefrenceGroupList(File, this)) { }
		RefrenceGroupParent() : _RefrenceGroups(nullptr) {}
	public:
		~RefrenceGroupParent()
		{
			delete _RefrenceGroups;
			_RefrenceGroups = nullptr;
		}

		RefrenceGroupList* const& RefrenceGroups = _RefrenceGroups;
	};

	class ProjectBase : public ConfigItemParent, public RefrenceGroupParent
	{
	private:
		String _Path;
		unsigned long long _ID = 0;
	protected:
		void SetID(unsigned long long ID) { _ID = ID; }

	public:
		ProjectBase(String Path) : ProjectBase()
		{ 
			_Path = Path; 
		}
		ProjectBase() : ConfigItemParent(this), RefrenceGroupParent(this)
		{
			_ID = 0;
		}
		virtual ~ProjectBase() {}

		String Title;

		bool RequiresPassword() const;
		void RequiresPassword(bool New);
		String Password() const;
		void Password(String New);

		const String& Path = _Path;
		const unsigned long long& CurrentID = _ID;

		virtual Component* GetFromID(unsigned long long ID, int Filter) const = 0;
		unsigned long long GetNextID() { _ID++; return _ID; }

		virtual void Save() = 0;
		virtual void Load() = 0;
		void ChangePath(String New) { _Path = New; }
	};
}