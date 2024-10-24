#include "BaseTypes.h"

#include "Components.h"
#include "ComponentRefrence.h"

namespace Armin::Files
{
	Component::Component(ProjectBase* File, bool GetID)
	{
		if (GetID)
			_ID = File->GetNextID();
		_ParentFile = File;
	}
	Component::~Component()
	{
		for (ComponentReference* Obj : Refrences)
			Obj->Active = false;
	}

	bool ProjectBase::RequiresPassword() const
	{
		if (!ConfigItems)
			return false;

		for (uint i = 0; i < ConfigItems->Count; i++)
		{
			ConfigItem* Current = ConfigItems->Item(i);
			if (Current && Current->Title() == L"RequiresPassword")
				return Current->Value == L"True";
		}

		return false;
	}
	void ProjectBase::RequiresPassword(bool New)
	{
		if (!ConfigItems)
			return;

		for (uint i = 0; i < ConfigItems->Count; i++)
		{
			ConfigItem* Current = ConfigItems->Item(i);
			if (Current && Current->Title() == L"RequiresPassword")
			{
				Current->Value = (New ? L"True" : L"False");
				return;
			}
		}

		//It is understood that it was not found.
		ConfigItem* CNew = new ConfigItem(this, ConfigItems);
		CNew->Title(L"RequiresPassword");
		CNew->Value = New ? L"True" : L"False";
	}
	String ProjectBase::Password() const
	{
		if (!ConfigItems)
			return String();

		for (uint i = 0; i < ConfigItems->Count; i++)
		{
			ConfigItem* Current = ConfigItems->Item(i);
			if (Current && Current->Title() == L"Password")
				return Current->Value.ShiftEachChar(7);
		}

		return String();
	}
	void ProjectBase::Password(String New)
	{
		if (!ConfigItems)
			return;

		for (uint i = 0; i < ConfigItems->Count; i++)
		{
			ConfigItem* Current = ConfigItems->Item(i);
			if (Current && Current->Title() == L"Password")
			{
				Current->Value = New.ShiftEachChar(-7);
				return;
			}
		}

		//It is understood that it was not found.
		ConfigItem* CNew = new ConfigItem(this, ConfigItems);
		CNew->Title(L"Password");
		CNew->Value = New.ShiftEachChar(-7);
	}
}