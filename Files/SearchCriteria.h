#pragma once

#include "ComponentTypes.h"
#include "Container.h"
#include "Str.h"

namespace Armin::Files
{
	class ProjectBase;
	class Component;
	
	/*
	* Acts as both a filter requirements and a masking tool. In filter terms, it allows callers to send information to searching windows or windows that display specific components to limit searches. When used as a masking tool, it allows the user to limit the set of all components in a file to specific critiera, used by the AllowedTypes and Arguments param.
	*/
	class SearchCriteria
	{
	public:
		//Creates a new object with specified parameters. By default, they are L"" for Arguments, and CT_All for AllowedTypes.
		SearchCriteria(String Args = String(), int AllowedTypes = CT_All) : Arguments(Args), AllowedTypes(AllowedTypes) {}

		bool Multiselect = false; //When true, more than one object can be returned, when false, only one or none objects will be returned.
		Vector<Component*> PreSelected; //The list of the previous components that were selected. When the reciver of this object interprets this parameter, the objects presented in this list will be pre-selected, conforming to the Multiselect parameter.

		int AllowedTypes = CT_All; //Use bitwise operations to limit which kinds of objects are allowed. By default, all components are allowed.
		String Arguments = L""; //Defines any data that creates a 'mask' for the data to be read. See the searching standards for how this parameter works. 

		bool NoFilter() //When true, the search critera allows all objects in the file to be returned.
		{
			return AllowedTypes == CT_All && Arguments == L"";
		}
		void NoFilter(bool New) //If the New is set to true, it resets all of the filters and masks.
		{
			if (New)
			{
				AllowedTypes = CT_All;
				Arguments = L"";
			}
		}

		Vector<Component*> GetComponents(ProjectBase* File) const; //Returns a list of all components that match ALL of the masks and filters present in this object.
		bool IsWithinCriteria(Component* Object, ProjectBase* File) const; //Returns true if the Object parameter is within the bounds set by this object.

		friend bool operator ==(const SearchCriteria& One, const SearchCriteria& Two); //Compares two Search Criteriums
		friend bool operator !=(const SearchCriteria& One, const SearchCriteria& Two); //Compoares two Search Criteriums
	};
}