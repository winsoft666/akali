/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#include "ppxbase/shortcut.h"
#if (defined _WIN32 || defined WIN32)
#include <windows.h>
#include <shellapi.h>
#include <shlobj.h>
#include <propkey.h>
#include <strsafe.h>
#include "ppxbase/os_ver.h"
#include "ppxbase/assert.h"
#include "ppxbase/file_util.h"
#include "ppxbase/miscellaneous.h"

namespace ppx {
    namespace base {

        namespace {
            void InitializeShortcutInterfaces(const wchar_t *shortcut,
                                              IShellLink **i_shell_link,
                                              IPersistFile **i_persist_file) {
                if((*i_shell_link))
                    (*i_shell_link)->Release();
                if((*i_persist_file))
                    (*i_persist_file)->Release();

                HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                                IID_IShellLink, (void **)i_shell_link);
                if (FAILED(hres)) {
                    return;
                }

                hres = (*i_shell_link)->QueryInterface(IID_IPersistFile, (void **)i_persist_file);
                if (FAILED(hres)) {
                    (*i_shell_link)->Release();
                    return;
                }

                if (shortcut) {
                    if (FAILED((*i_persist_file)->Load(shortcut, STGM_READWRITE))) {
                        (*i_shell_link)->Release();
                        (*i_persist_file)->Release();
                    }
                }
            }
        }

        bool CreateOrUpdateShortcutLink(const std::wstring &shortcut_path,
                                        const ShortcutProperties &properties, ShortcutOperation operation) {
            // A target is required unless |operation| is SHORTCUT_UPDATE_EXISTING.
            if (operation != SHORTCUT_UPDATE_EXISTING &&
                    !(properties.options & ShortcutProperties::PROPERTIES_TARGET)) {
                return false;
            }

            bool shortcut_existed = PathIsExists(shortcut_path.c_str());

            // Interfaces to the old shortcut when replacing an existing shortcut.
            IShellLink *old_i_shell_link = NULL;
            IPersistFile *old_i_persist_file = NULL;

            // Interfaces to the shortcut being created/updated.
            IShellLink *i_shell_link = NULL;
            IPersistFile *i_persist_file = NULL;
            switch (operation) {
                case SHORTCUT_CREATE_ALWAYS:
                    InitializeShortcutInterfaces(NULL, &i_shell_link, &i_persist_file);
                    break;
                case SHORTCUT_UPDATE_EXISTING:
                    InitializeShortcutInterfaces(shortcut_path.c_str(), &i_shell_link,
                                                 &i_persist_file);
                    break;
                case SHORTCUT_REPLACE_EXISTING:
                    InitializeShortcutInterfaces(shortcut_path.c_str(),
                                                 &old_i_shell_link, &old_i_persist_file);
                    // Confirm |shortcut_path| exists and is a shortcut by verifying
                    // |old_i_persist_file| was successfully initialized in the call above. If
                    // so, initialize the interfaces to begin writing a new shortcut (to
                    // overwrite the current one if successful).
                    if (old_i_persist_file)
                        InitializeShortcutInterfaces(NULL, &i_shell_link, &i_persist_file);
                    break;
                default:
                    PPX_NOT_REACHED("");
            }

            // Return false immediately upon failure to initialize shortcut interfaces.
            if (!i_persist_file)
                return false;

            if ((properties.options & ShortcutProperties::PROPERTIES_TARGET) &&
                    FAILED(i_shell_link->SetPath(properties.target.c_str()))) {
                return false;
            }

            if ((properties.options & ShortcutProperties::PROPERTIES_WORKING_DIR) &&
                    FAILED(i_shell_link->SetWorkingDirectory(
                               properties.working_dir.c_str()))) {
                return false;
            }

            if (properties.options & ShortcutProperties::PROPERTIES_ARGUMENTS) {
                if (FAILED(i_shell_link->SetArguments(properties.arguments.c_str())))
                    return false;
            } else if (old_i_persist_file) {
                wchar_t current_arguments[MAX_PATH] = { 0 };
                if (SUCCEEDED(old_i_shell_link->GetArguments(current_arguments,
                              MAX_PATH))) {
                    i_shell_link->SetArguments(current_arguments);
                }
            }

            if ((properties.options & ShortcutProperties::PROPERTIES_DESCRIPTION) &&
                    FAILED(i_shell_link->SetDescription(properties.description.c_str()))) {
                return false;
            }

            if ((properties.options & ShortcutProperties::PROPERTIES_ICON) &&
                    FAILED(i_shell_link->SetIconLocation(properties.icon.c_str(),
                            properties.icon_index))) {
                return false;
            }


            // Release the interfaces to the old shortcut to make sure it doesn't prevent
            // overwriting it if needed.
            if(old_i_persist_file)
                old_i_persist_file->Release();
            if(old_i_shell_link)
                old_i_shell_link->Release();

            HRESULT result = i_persist_file->Save(shortcut_path.c_str(), TRUE);

            // Release the interfaces in case the SHChangeNotify call below depends on
            // the operations above being fully completed.
            i_persist_file->Release();
            i_shell_link->Release();

            // If we successfully created/updated the icon, notify the shell that we have
            // done so.
            const bool succeeded = SUCCEEDED(result);
            if (succeeded) {
                if (shortcut_existed) {
                    // TODO(gab): SHCNE_UPDATEITEM might be sufficient here; further testing
                    // required.
                    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, NULL, NULL);
                } else {
                    SHChangeNotify(SHCNE_CREATE, SHCNF_PATH, shortcut_path.c_str(),
                                   NULL);
                }
            }

            return succeeded;
        }

        bool ResolveShortcut(const std::wstring &shortcut_path,
                             ShortcutProperties &properties) {
            HRESULT result;
            IShellLink *i_shell_link = NULL;
            IPersistFile *persist = NULL;

            HRESULT hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                            IID_IShellLink, (void **)&i_shell_link);
            if (FAILED(hres)) {
                return false;
            }

            hres = i_shell_link->QueryInterface(IID_IPersistFile, (void **)&persist);
            if (FAILED(hres)) {
                i_shell_link->Release();
                return false;
            }


            if (FAILED(persist->Load(shortcut_path.c_str(), STGM_READWRITE))) {
                i_shell_link->Release();
                persist->Release();
                return false;
            }


            WCHAR temp[MAX_PATH] = { 0 };
            // Try to find the target of a shortcut.
            result = i_shell_link->Resolve(0, SLR_NO_UI | SLR_NOSEARCH);
            if (FAILED(result))
                return false;

            result = i_shell_link->GetPath(temp, MAX_PATH, NULL, SLGP_UNCPRIORITY);
            if (FAILED(result))
                return false;
            properties.target = temp;

            result = i_shell_link->GetArguments(temp, MAX_PATH);
            if (FAILED(result))
                return false;
            properties.arguments = temp;

            result = i_shell_link->GetWorkingDirectory(temp, MAX_PATH);
            if (FAILED(result))
                return false;
            properties.working_dir = temp;

            result = i_shell_link->GetDescription(temp, MAX_PATH);
            if (FAILED(result))
                return false;
            properties.description = temp;

            int iconIndex = 0;
            result = i_shell_link->GetIconLocation(temp, MAX_PATH, &iconIndex);
            if (FAILED(result))
                return false;
            properties.icon = temp;
            properties.icon_index = iconIndex;

            if (persist)
                persist->Release();
            if (i_shell_link)
                i_shell_link->Release();

            return true;
        }

        bool TaskbarPinShortcutLink(const wchar_t *shortcut) {
            OSVersion osver;
            if (!osver.AtLeastWinVista())
                return false;

            int result = reinterpret_cast<int>(ShellExecute(NULL, L"taskbarpin", shortcut,
                                               NULL, NULL, 0));
            return result > 32;
        }

        bool TaskbarUnpinShortcutLink(const wchar_t *shortcut) {
            OSVersion osver;
            if (!osver.AtLeastWinVista())
                return false;

            int result = reinterpret_cast<int>(ShellExecute(NULL, L"taskbarunpin",
                                               shortcut, NULL, NULL, 0));
            return result > 32;
        }

    }
}

#endif