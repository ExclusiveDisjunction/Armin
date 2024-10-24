#include "SearchCriteria.h"

#include "ArminSessions.h"
#include "Components.h"
#include "Mapping.h"
#include "..\Common.h"
#include "..\UserRegistry.h"

namespace Armin::Files
{
    Vector<Component*> SearchCriteria::GetComponents(ProjectBase* File) const
    {
        if (!File)
            return Vector<Component*>();

        bool Users = AllowedTypes & CT_User,
            JobPositions = AllowedTypes & CT_JobPosition,
            Tasks = AllowedTypes & CT_Task,
            CompletedTasks = AllowedTypes & CT_CompletedTask,
            InventoryItems = (AllowedTypes & CT_InventoryItem) && (AppState & APS_HasAdminUser),
            OperationInventoryItems = AllowedTypes & CT_OperationInventoryItem,
            RefrenceGroups = AllowedTypes & CT_RefrenceGroup;

        String Args = Arguments;
        if (!Args.Contains(L'\"'))
        {
            if (String(Args.ToLong()) == Args) //Its an ID
                Args = L"ID=\"" + Args + L"\"";
            else if (Args.RemoveWhiteSpace() != L"")
                Args = L"Title=\"" + Args + L"\"";
        }
        bool NoFilter = Args.RemoveWhiteSpace() == L"";

        Mapping<String, String> Mask;
        if (!NoFilter)
        {
            StringList Split;
            String Temp;
            wchar_t Last = L'\0';
            bool Opened = false;
            for (uint i = 0, j = 0; i < Args.Length(); i++)
            {
                if (Args[i] == L'\"' && !Opened)
                    Opened = true;
                else if (Args[i] == L'\"')
                {
                    Opened = false;
                    Temp += L'\"';
                    continue;
                }

                if (Args[i] == L',' && !Opened)
                {
                    Split.Add(Temp);
                    Temp = L"";
                    continue;
                }

                if (!Opened)
                {
                    wchar_t Current = Args[i];
                    if (Current == L' ')
                        continue;

                    if (Current >= L'A' && Current <= L'Z') //Is uppercase
                        Current += L'a' - L'A'; //Should Be 32

                    if ((Current < L'a' || Current > L'z') && (Current < L'0' || Current > L'9') && Current != L'=')
                        continue;
                    Temp += Current;
                }
                else
                    Temp += Args[i];
            }
            Split.Add(Temp);

            for (uint i = 0; i < Split.Size; i++)
            {
                String Current = Split[i];
                String First;
                String Second;
                bool Mode = false, Contained = false; //False for first half
                for (uint j = 0; j < Current.Length(); j++)
                {
                    wchar_t This = Current[j];
                    if (This == L'=' && !Mode)
                    {
                        Mode = true;
                        continue;
                    }

                    if (Mode && This == L'\"')
                    {
                        if (Contained)
                            break;
                        else
                        {
                            Contained = true;
                            continue;
                        }
                    }

                    if (Contained)
                        Second += This;
                    else if (!Mode)
                        First += This;
                }

                Mask.Add(First, Second);
            }
        } 

        Vector<Component*> Return;

        UserSystem* UserFile = dynamic_cast<class UserSystem*>(File);
        TaskSystem* TaskFile = dynamic_cast<class TaskSystem*>(File);
        InventorySystem* InventoryFile = dynamic_cast<class InventorySystem*>(File);

        if (Users && UserFile && UserFile->Users)
        {
            UserSet* Users = UserFile->Users;
            for (uint i = 0; i < Users->Count; i++)
            {
                User* Item = Users->Item(i);
                Return.Add(Item);
            }
        }
        if (JobPositions && UserFile && UserFile->Positions)
        {
            JobPositionList* Positions = UserFile->Positions;
            for (uint i = 0; i < Positions->Count; i++)
            {
                JobPosition* Item = Positions->Item(i);
                Return.Add(Item);
            }
        }
        if (Tasks && TaskFile && TaskFile->Tasks)
        {
            TaskList* Tasks = TaskFile->Tasks;
            for (uint i = 0; i < Tasks->Count; i++)
            {
                Task* Item = Tasks->Item(i);
                Return.Add(Item);
            }
        }
        if (CompletedTasks && TaskFile && TaskFile->CompletedTasks)
        {
            CompletedTaskList* Tasks = TaskFile->CompletedTasks;
            for (uint i = 0; i < Tasks->Count; i++)
            {
                CompletedTask* Item = Tasks->Item(i);
                Return.Add(Item);
            }
        }
        if (InventoryItems && InventoryFile && InventoryFile->InventoryItems)
        {
            InventoryItemGroup* Items = InventoryFile->InventoryItems;
            for (uint i = 0; i < Items->Count; i++)
            {
                InventoryItem* Item = Items->Item(i);
                Return.Add(Item);
            }
        }
        if (OperationInventoryItems && InventoryFile && InventoryFile->OperationInventoryItems)
        {
            OperationInventoryItemGroup* Items = InventoryFile->OperationInventoryItems;
            for (uint i = 0; i < Items->Count; i++)
            {
                OperationInventoryItem* Item = Items->Item(i);
                Return.Add(Item);
            }
        }
        if (RefrenceGroups && File->RefrenceGroups)
        {
            ReferenceGroupList* Groups = File->RefrenceGroups;
            for (uint i = 0; i < Groups->Count; i++)
                Return.Add(Groups->Item(i));
        }

        //The masking filter is not working properly. It needs to apply one mask, then another, and keep with this process until everything is masked. This will need to be done in an order, for example
        /*
        * 1. Allowed Types,
        * 2. Title,
        * 3. ID,
        * 4. Username,
        * 5. SerialNumber,
        * 6. Group
        */

        if (!NoFilter && Return.Size != 0)
        {
            for (uint i = 0; i < Mask.Size; i++)
            {
                MappingPair<String, String> Current = Mask[i];
                StringList Parts = Current.Value.Split(L',');
                for (uint i = 0; i < Parts.Size; i++)
                {
                    String& Current = Parts[i];
                    Current = Current.RemoveFromFront(L' ');
                    Current = Current.RemoveFromEnd(L' ');
                    Current = Current.ToLower();
                }
                if (Current.Key == L"title")
                {
                    Vector<Component*> Revised;
                    for (uint j = 0; j < Return.Size; j++)
                    {
                        Component* Current = Return[j];
                        if (Parts.Contains(Current->Title().ToLower()))
                            Revised.Add(Current);
                    }
                    Return = Revised;
                }
                else if (Current.Key == L"username")
                {
                    Vector<Component*> Revised;
                    for (uint j = 0; j < Return.Size; j++)
                    {
                        Component* Current = Return[j];
                        User* Conv = dynamic_cast<User*>(Current);
                        if (Conv && Parts.Contains(Conv->Username.ToLower()))
                            Revised.Add(Current);
                    }
                    Return = Revised;
                }
                else if (Current.Key == L"serialnumber")
                {
                    Vector<Component*> Revised;
                    for (uint j = 0; j < Return.Size; j++)
                    {
                        Component* Current = Return[j];
                        InventoryBase* Conv = dynamic_cast<InventoryBase*>(Current);
                        if (Conv && Parts.Contains(Conv->SerialNumber.ToLower()))
                            Revised.Add(Current);
                    }
                    Return = Revised;
                }
                else if (Current.Key == L"group")
                {
                    Vector<Component*> Revised;
                    for (uint j = 0; j < Return.Size; j++)
                    {
                        Component* Current = Return[j];
                        InventoryBase* Conv = dynamic_cast<InventoryBase*>(Current);
                        if (Conv && Parts.Contains(Conv->Group.ToLower()))
                            Revised.Add(Current);
                    }
                    Return = Revised;
                }
                else if (Current.Key == L"id")
                {
                    Vector<long long> IDs;
                    for (uint j = 0; j < Parts.Size; j++)
                        IDs.Add(Parts[j].ToLong());

                    Vector<Component*> Revised;
                    for (uint j = 0; j < Return.Size; j++)
                    {
                        Component* Current = Return[j];
                        if (IDs.Contains(Current->ID))
                            Revised.Add(Current);
                    }
                    Return = Revised;
                }
            }
        }

        return Return;
    }
    bool SearchCriteria::IsWithinCriteria(Component* Object, ProjectBase* File) const
    {
        Vector<Component*> Range = GetComponents(File);

        return Range.Contains(Object);
    }

    bool operator ==(const SearchCriteria& One, const SearchCriteria& Two)
    {
        return One.AllowedTypes == Two.AllowedTypes &&
            One.Multiselect == Two.Multiselect &&
            One.PreSelected == Two.PreSelected;
    }
    bool operator !=(const SearchCriteria& One, const SearchCriteria& Two)
    {
        return !(One == Two);
    }
}