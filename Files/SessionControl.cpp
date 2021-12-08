#include "SessionControl.h"

#include "ArminSessions.h"
#include "Components.h"
#include "Files\Stream.h"
#include "..\UserRegistry.h"
#include "..\Editors\EditorFrame.h"
#include "..\Editors\EditorRegistry.h"
#include "..\Config\Ins.h"
#include "..\Config\Recent.h"
#include "..\UI\Main.h"
#include "..\UI\User\CreateUser.h"
#include "..\UI\Tool\FileProgress.h"
#include "..\UI\Tool\PasswordInput.h"
#include "..\UI\Tool\NewFile.h"

#include <cstdio>
#include <filesystem>

namespace Armin::Files
{
    using namespace Config;
    using namespace Editors;
    using namespace UI;
    using namespace UI::Tool;
    using namespace UI::Users;

    bool SessionControl::Open(HINSTANCE ins)
    {
        if (FooterOutput)
            FooterOutput->SetFooterTextTillNext(L"Opening File...");

        String Path = OpenFileEx(NULL, L"All Project Files\0*.arminproj;*.arminrcproj;*.armininvproj;*.arminrcinvproj;*.arminteamproj;*.arminrcteamproj\0Armin Projects\0*.arminproj;*.arminrcproj\0Inventory Projects\0*.armininvproj;*.arminrcinvproj\0Team Projects\0*.arminteamproj;*.arminrcteamproj\0Resource Enabled Projects\0*.arminrcproj;*.arminrcinvproj;*.arminrcteamproj", L"");
        if (Path == L"")
        {
            if (FooterOutput)
                FooterOutput->SetFooterText(L"Open file was canceled.");
            return false;
        }

        return Open(Path, ins);
    }
    bool SessionControl::Open(String Path, HINSTANCE ins)
    {
        if (FooterOutput)
            FooterOutput->SetFooterTextTillNext(L"Opening File...");

        if (!FileExists(Path))
        {
            if (FooterOutput)
                FooterOutput->SetFooterText(L"The path provided for open does not exist.");
            return false;
        }

        if (Path == LoadedProjectPath)
        {
            if (FooterOutput)
                FooterOutput->SetFooterText(L"The path provided is the same as the currently loaded project.");
            return true;
        }

        if (Ins::IsLoaded())
        {
            if (!Close(ins))
            {
                if (FooterOutput)
                    FooterOutput->SetFooterText(L"The close file failed, please correct errors before trying again.");
                return false;
            }
        }

        FileProgress* Progress = new FileProgress(false, ins);

        String Ext = FileExt(Path);

        try
        {
            if (Ext == L"arminproj")
                LoadedProject = new Project(Path);
            else if (Ext == L"arminrcproj")
                LoadedProject = new ProjectRc(Path);
            else if (Ext == L"armininvproj")
                LoadedProject = new InventoryProject(Path);
            else if (Ext == L"arminrcinvproj")
                LoadedProject = new InventoryProjectRc(Path);
            else if (Ext == L"arminteamproj")
                LoadedProject = new TeamProject(Path);
            else if (Ext == L"arminrcteamproj")
                LoadedProject = new TeamProjectRc(Path);
        }
        catch (...)
        {
            MessageBoxW(nullptr, L"The file selected is corrupted and cannot be opened.", L"Open File:", MB_OK | MB_ICONERROR);

            delete Progress;
            return false;
        }

        delete Progress;

        if (LoadedProject && LoadedProject->RequiresPassword())
        {
            String Password = PasswordInput::Execute(ins);
            if (Password != LoadedProject->Password())
            {
                MessageBoxW(nullptr, L"The password provided was incorrect. Please try again later.", L"Open File:", MB_OK | MB_ICONERROR);
                if (FooterOutput)
                    FooterOutput->SetFooterText(L"Could not open the selected project.");

                delete LoadedProject;
                LoadedProject = nullptr;
                LoadedProjectPath = L"";
                return false;
            }
        }

        LoadedProjectPath = Path;
        InsInstance->LastLoaded = Path;
        RecentInstance->AddRecent(Path);

        UserSystem* UserSys = dynamic_cast<UserSystem*>(LoadedProject);
        UserRegistry::Initalize(UserSys, ins);
        if (UserSys && FooterOutput) //If the main window has already been loaded, then prompt a sign in.
            UserRegistry::SignIn();

        ResourceSystem* Resources = dynamic_cast<ResourceSystem*>(LoadedProject);
        if (Resources)
        {
            Vector<AString> Missing;
            bool Result = Resources->CheckResources(Missing);
            if (!Result)
            {
                String Text = L"The following resource files could not be found:\n";
                for (AString Item : Missing)
                    Text += L'\n' + (String)Item;

                Text += L"\n\nPlease re-locate the files to the current project to access the resources.";

                MessageBoxW(NULL, (LPCWSTR)Text, L"Resources:", MB_OK | MB_ICONWARNING);
            }
        }

        {
            InventorySystem* Conv = dynamic_cast<InventorySystem*>(LoadedProject);
            if (Conv && !UserSys && FooterOutput)
                EditorRegistry::OpenEditor(new Inventory::InventoryEditor(nullptr), nullptr);
        }

        if (MasterRibbon)
            MasterRibbon->SetRibbonStatusDef();

        if (FooterOutput)
           FooterOutput->SetFooterText(L"Sucessfully Opened File");

        return true;
    }
    bool SessionControl::Close(HINSTANCE ins)
    {
        if (FooterOutput)
            FooterOutput->SetFooterTextTillNext(L"Closing File...");

        bool HasEdit = AppState & APS_HasEdit;

        if (HasEdit || EditorRegistry::ApplyableEditorRunning())
        {
            int Result = MessageBoxW(NULL, L"Do you want to save before closing the current project?", L"Armin:", MB_YESNOCANCEL | MB_ICONWARNING);
            if (Result == IDYES)
            {
                if (!Save(ins))
                {
                    if (FooterOutput)
                        FooterOutput->SetFooterText(L"Close file failed, please fix all errors before trying again.");
                    return false;
                }
            }
            else if (Result == IDCANCEL)
            {
                if (FooterOutput)
                    FooterOutput->ResetFooterText();
                return false;
            }
        }

        if (!Ins::IsLoaded())
        {
            if (FooterOutput)
                FooterOutput->SetFooterText(L"Sucessfully Closed File");
            return true;
        }

        EditorRegistry::ForceCloseAllEditors();

        delete LoadedProject;
        LoadedProject = nullptr;
        LoadedProjectPath = L"";

        if (MasterRibbon)
            MasterRibbon->SetRibbonStatusDef();

        if (FooterOutput)
            FooterOutput->SetFooterText(L"Sucessfully Closed File");

        return true;
    }
    bool SessionControl::NewFile(HINSTANCE ins)
    {
        if (FooterOutput)
            FooterOutput->SetFooterTextTillNext(L"Creating New File...");

        if (AppState & APS_HasEdit)
        {
            int Result = MessageBoxW(nullptr, L"Would you like to save first?", L"New File:", MB_YESNOCANCEL | MB_ICONQUESTION);
            if (Result == IDYES)
            {
                if (!Save(ins))
                {
                    Result = MessageBoxW(nullptr, L"Save failed. Would you still like to create a new file?", L"New File:", MB_YESNOCANCEL | MB_ICONQUESTION);
                    if (Result == IDNO || Result == IDCANCEL)
                        return false;
                }
            }
            else if (Result == IDCANCEL)
                return false;
        }

        String Path = NewFile::Execute(ins);
       //String Path = SaveFileEx(NULL, L"All Project Files\0*.arminproj;*.arminrcproj;*.armininvproj;*.arminrcinvproj;*.arminteamproj;*.arminrcteamproj\0Armin Projects\0*.arminproj;*.arminrcproj\0Inventory Projects\0*.armininvproj;*.arminrcinvproj\0Team Projects\0*.arminteamproj;*.arminrcteamproj\0Resource Enabled Projects\0*.arminrcproj;*.arminrcinvproj;*.arminrcteamproj", L"");
        bool Return = false;

        if (Path == L"")
            return false;

        if (!Close(ins))
            return false;

        if (FileExists(Path))
            DeleteFileW(static_cast<const wchar_t*>(Path));

        String FileExt = ::FileExt(Path);

        if (FileExt == L"arminproj" || FileExt == L"arminrcproj" || FileExt == L"arminteamproj" || FileExt == L"arminrcteamproj")
        {
            std::ofstream OutFile(Path);
            if (!OutFile)
            {
                MessageBoxW(NULL, L"The file cannot be created at this time.\n\nPlease try again later.", L"NewFile:", MB_OK | MB_ICONERROR);
                return false;
            }

            OutFile.close();

            LoadedProjectPath = Path;
            if (FileExt == L"arminproj")
                LoadedProject = new Project();
            else if (FileExt == L"arminrcproj")
                LoadedProject = new ProjectRc();
            else if (FileExt == L"arminteamproj")
                LoadedProject = new TeamProject();
            else if (FileExt == L"arminrcteamproj")
                LoadedProject = new TeamProjectRc();
            LoadedProject->ChangePath(Path);

            if (!CreateUser::Execute(true, ins))
            {
                delete LoadedProject;
                LoadedProject = nullptr;
                LoadedProjectPath = String();

                MessageBoxW(NULL, L"The file cannot be created without an Admin User.", L"Assign File:", MB_OK | MB_ICONERROR);
                return false;
            }

            Return = true;
        }
        else if (FileExt == L"armininvproj" || FileExt == L"arminrcinvproj")
        {
            std::ofstream OutFile(Path);
            if (!OutFile)
            {
                MessageBoxW(NULL, L"The file cannot be created at this time.\n\nPlease try again later.", L"NewFile:", MB_OK | MB_ICONERROR);
                return false;
            }

            OutFile.close();

            LoadedProjectPath = Path;
            if (FileExt == L"armininvproj")
                LoadedProject = new InventoryProject();
            else if (FileExt == L"arminrcinvproj")
                LoadedProject = new InventoryProjectRc();
            LoadedProject->ChangePath(Path);
            Return = true;
        }

        if (Return)
        {
            RecentInstance->AddRecent(Path);
            InsInstance->LastLoaded = Path;

            LoadedProject->Save();
            AppState = 0; //Resets app state
            AppState |= APS_FileLoaded; //Loads a file up.

            UserSystem* ConvFile = dynamic_cast<UserSystem*>(LoadedProject);
            UserRegistry::Initalize(ConvFile, ins);
        }

        if (FooterOutput)
            FooterOutput->SetFooterText(L"Sucessfully Created New File");

        if (MasterRibbon)
            MasterRibbon->SetRibbonStatusDef();

        return Return;
    }

    bool SessionControl::Apply(EditorFrame* Host, bool ShowCompleted)
    {
        if (!Host || !Host->IsApplyable())
        {
            if (FooterOutput)
                FooterOutput->SetFooterText(L"The current editor is not applyable.");
            else
                MessageBoxW(NULL, L"The current editor is not applyable.", L"Apply:", MB_OK | MB_ICONERROR);
            return Host != nullptr;
        }

        ProjectBase* File = LoadedProject;
        if (!File)
            return false;

        bool Return = Host->Apply(File, true);

        if (FooterOutput)
        {
            if (ShowCompleted && Return)
                FooterOutput->SetFooterText(L"The editor was applied.");
            else if (ShowCompleted && !Return)
                FooterOutput->SetFooterText(L"The edito was not applied.");
        }
        else
        {
            if (ShowCompleted && Return)
                MessageBoxW(NULL, L"The editor has been applied.", L"Apply:", MB_OK | MB_ICONINFORMATION);
            else if (ShowCompleted && !Return)
                MessageBoxW(NULL, L"The editor was not applied correctly.", L"Apply:", MB_OK | MB_ICONERROR);
        }

        if (Return)
            EditorRegistry::CloseEditor(Host, false);

        AppState |= APS_HasEdit;     
        return Return;
    }
    bool SessionControl::ApplyAll()
    {
        Vector<EditorFrame*> Editors = EditorRegistry::CurrentOpenEditors();

        bool Return = true;
        for (uint i = 0; i < Editors.Size; i++)
            Return &= Apply(Editors[i], false);

        if (Return)
            MessageBoxW(NULL, L"Apply All was sucessfull.", L"Apply All:", MB_OK | MB_ICONINFORMATION);
        else
            MessageBoxW(NULL, L"Some or all editors open could not be applied.\n\nPlease correct all errors before trying again.", L"Apply All:", MB_OK | MB_ICONERROR);

        return Return;
    }

    bool SessionControl::Save(HINSTANCE ins)
    {
        bool HasError = false;
        bool Return = AppState & APS_HasEdit;
        {
            Vector<EditorFrame*> Apply = EditorRegistry::CurrentApplyableEditors();
            bool DoAppylyable = false;
            if (Apply.Size != 0)
            {
                String Prompt = L"Do you want to apply the following editors?\n\n";
                for (EditorFrame* Current : Apply)
                    Prompt += Current->GetName() + L"\n";

                int Result = MessageBoxW(nullptr, static_cast<LPCWSTR>(Prompt), L"Save:", MB_YESNOCANCEL | MB_ICONWARNING);
                DoAppylyable = Result == IDYES;
            }

            if (DoAppylyable)
            {
                bool Status = true;
                Vector<EditorFrame*> ToClose;
                for (EditorFrame* Current : Apply)
                {
                    bool Temp = Current->Apply(LoadedProject);
                    if (Temp)
                        ToClose.Add(Current);
                    Status &= Temp;
                }

                for (EditorFrame* Current : ToClose)
                    EditorRegistry::CloseEditor(Current, false);

                Return |= Status;
                HasError = Status;
            }
        }

        if (Return)
        {
            FileProgress* Progress = new FileProgress(true, ins);

            LoadedProject->Save();
            AppState &= ~APS_HasEdit;

            delete Progress;
        }

        if (FooterOutput)
        {
            if (Return)
                FooterOutput->SetFooterText(L"Save was sucessfull.");
            else if (HasError)
                FooterOutput->SetFooterText(L"Some or all editors open could not be applied. Please apply all editors before trying again.");
            else
                FooterOutput->SetFooterText(L"There is no data to save at this time.");
        }
        else
        {
            if (Return)
                MessageBoxW(NULL, L"Save was sucessfull.", L"Save:", MB_OK | MB_ICONINFORMATION);
            else if (HasError)
                MessageBoxW(NULL, L"Some or all editors open could not be applied.\n\nPlease apply all editors before trying again.", L"Save:", MB_OK | MB_ICONERROR);
            else
                MessageBoxW(NULL, L"There is no data to save at this time.", L"Save:", MB_OK | MB_ICONINFORMATION);
        }

        return Return;
    }
    bool SessionControl::SaveAsCopy(HINSTANCE ins)
    {
        if (!Ins::IsLoaded())
        {
            MessageBoxW(NULL, L"Save as Copy cannot run because no project is currently loaded.\n\nPlease open a project, then try again.", L"Save As Copy:", MB_OK | MB_ICONERROR);
            return false;
        }

        auto Result = MessageBoxW(NULL, L"Do you want to save first?", L"Save as Copy:", MB_YESNO | MB_ICONQUESTION);
        if (Result == IDYES)
        {
            if (!Save(ins))
            {
                Result = MessageBoxW(NULL, L"Save Failed. Do you want to continue?", L"Save As Copy:", MB_YESNO | MB_ICONERROR);
                if (Result == IDNO)
                    return false;
            }
        }

        String Path = SaveFileEx(NULL, L"All Project Files\0*.arminproj;*.arminrcproj;*.armininvproj;*.arminrcinvproj;*.arminteamproj;*.arminrcteamproj\0Armin Projects\0*.arminproj;*.arminrcproj\0Inventory Projects\0*.armininvproj;*.arminrcinvproj\0Team Projects\0*.arminteamproj;*.arminrcteamproj\0Resource Enabled Projects\0*.arminrcproj;*.arminrcinvproj;*.arminrcteamproj", L"");
        if (Path == String())
            return false;

        if (FileExt(Path) != FileExt(LoadedProjectPath))
        {
            MessageBoxW(NULL, L"The file type of Save As Copy must be the same as the current loaded project.\n\nPlease try again later.", L"Save As:", MB_OK | MB_ICONERROR);
            return false;
        }            

        if (FileExists(Path))
            DeleteFileW(static_cast<LPCWSTR>(Path));

        CopyFileW(static_cast<LPCWSTR>(LoadedProjectPath), static_cast<LPCWSTR>(Path), false);
        return true;
    }
    bool SessionControl::SaveAs(HINSTANCE ins)
    {
        if (!Ins::IsLoaded())
        {
            MessageBoxW(NULL, L"Save as cannot run because no project is currently loaded.\n\nPlease open a project, then try again.", L"Save As:", MB_OK | MB_ICONERROR);
            return false;
        }

        String Path = SaveFileEx(NULL, L"All Project Files\0*.arminproj;*.arminrcproj;*.armininvproj;*.arminrcinvproj;*.arminteamproj;*.arminrcteamproj\0Armin Projects\0*.arminproj;*.arminrcproj\0Inventory Projects\0*.armininvproj;*.arminrcinvproj\0Team Projects\0*.arminteamproj;*.arminrcteamproj\0Resource Enabled Projects\0*.arminrcproj;*.arminrcinvproj;*.arminrcteamproj", L"");
        if (Path == L"")
            return false;

        if (FileExt(Path) != FileExt(LoadedProjectPath))
        {
            MessageBoxW(NULL, L"The file type of Save As must be the same as the current loaded project.\n\nPlease try again later.", L"Save As:", MB_OK | MB_ICONERROR);
            return false;
        }

        ProjectBase* File = LoadedProject;
        File->ChangePath(Path);
        File->Save();
        AppState &= ~APS_HasEdit;

        if (!Save(ins))
        {
            auto Result = MessageBoxW(NULL, L"Save failed, but Save As was sucessful.\n\nPlease fix your errors and try again.", L"Save As:", MB_OK | MB_ICONWARNING);
            if (Result == IDNO)
                return false;
        }

        return true;
    }
}