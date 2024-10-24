#include "ComponentRefrence.h"

namespace Armin::Files
{
	ComponentReference::ComponentReference(Component* Obj) : Active(Obj != nullptr), Ref(Obj)
	{
		if (Active)
			Ref->Refrences.Add(this);
	}
	ComponentReference::~ComponentReference()
	{
		if (Active)
			Ref->Refrences.Remove(this);
	}

	ComponentReference& ComponentReference::operator=(Component* Obj)
	{
		if (Active)
			Ref->Refrences.Remove(this);

		Ref = Obj;
		Active = Obj != nullptr;
		return *this;
	}

	bool operator==(const ComponentReference& One, const ComponentReference& Two)
	{
		return One.Active == Two.Active &&
			One.Ref == Two.Ref;
	}
	bool operator!=(const ComponentReference& One, const ComponentReference& Two)
	{
		return !(One == Two);
	}

	void ComponentReference::SetTarget(Component* New)
	{
		if (Active)
			Ref->Refrences.Remove(this);

		Active = New != nullptr;
		Ref = New;

		if (Active)
			Ref->Refrences.Add(this);
	}

	ComponentReference::operator Component* () const
	{
		if (Active)
			return Ref;
		return nullptr;
	}
	
	AString ComponentReference::ToString() const
	{
		return !Active ? "0" : (Ref ? AString(Ref->ID) : "0");
	}
}