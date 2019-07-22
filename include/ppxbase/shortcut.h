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
#ifndef PPXBASE_SHORTCUT_H__
#define PPXBASE_SHORTCUT_H__
#pragma once

#include "ppxbase_export.h"
#include <string>

namespace ppx {
	namespace base {
		enum ShortcutOperation {
			// Create a new shortcut (overwriting if necessary).
			SHORTCUT_CREATE_ALWAYS = 0,
			// Overwrite an existing shortcut (fails if the shortcut doesn't exist).
			// If the arguments are not specified on the new shortcut, keep the old
			// shortcut's arguments.
			SHORTCUT_REPLACE_EXISTING,
			// Update specified properties only on an existing shortcut.
			SHORTCUT_UPDATE_EXISTING,
		};

		struct ShortcutProperties {
			enum IndividualProperties {
				PROPERTIES_TARGET = 1 << 0,
				PROPERTIES_WORKING_DIR = 1 << 1,
				PROPERTIES_ARGUMENTS = 1 << 2,
				PROPERTIES_DESCRIPTION = 1 << 3,
				PROPERTIES_ICON = 1 << 4,
			};

			ShortcutProperties() : 
				icon_index(-1)
				, options(0U) {}

			void SetTarget(const std::wstring& target_in) {
				target = target_in;
				options |= PROPERTIES_TARGET;
			}

			void SetWorkingDir(const std::wstring& working_dir_in) {
				working_dir = working_dir_in;
				options |= PROPERTIES_WORKING_DIR;
			}

			void SetArguments(const std::wstring& arguments_in) {
				// Size restriction as per MSDN at http://goo.gl/TJ7q5.
				arguments = arguments_in;
				options |= PROPERTIES_ARGUMENTS;
			}

			void SetDescription(const std::wstring& description_in) {
				// Size restriction as per MSDN at http://goo.gl/OdNQq.
				description = description_in;
				options |= PROPERTIES_DESCRIPTION;
			}

			void SetIcon(const std::wstring& icon_in, int icon_index_in) {
				icon = icon_in;
				icon_index = icon_index_in;
				options |= PROPERTIES_ICON;
			}

			// The target to launch from this shortcut. This is mandatory when creating a shortcut.
			std::wstring target;
			// The name of the working directory when launching the shortcut.
			std::wstring working_dir;
			// The arguments to be applied to |target| when launching from this shortcut.
			// The length of this string must be less than MAX_PATH.
			std::wstring arguments;
			// The localized description of the shortcut.
			// The length of this string must be less than MAX_PATH.
			std::wstring description;
			// The path to the icon (can be a dll or exe, in which case |icon_index| is the resource id).
			std::wstring icon;
			int icon_index;

			// Bitfield made of IndividualProperties. Properties set in |options| will be
			// set on the shortcut, others will be ignored.
			uint32_t options;
		};


		PPXBASE_API bool CreateOrUpdateShortcutLink(
			const std::wstring& shortcut_path,
			const ShortcutProperties& properties,
			ShortcutOperation operation);


		PPXBASE_API bool ResolveShortcut(const std::wstring& shortcut_path,
			std::wstring &target_path,
			std::wstring &args);

		// Pins a shortcut to the Windows 7 taskbar. The shortcut file must already
		// exist and be a shortcut that points to an executable.
		PPXBASE_API bool TaskbarPinShortcutLink(const wchar_t* shortcut);

		// Unpins a shortcut from the Windows 7 taskbar. The shortcut must exist and
		// already be pinned to the taskbar.
		PPXBASE_API bool TaskbarUnpinShortcutLink(const wchar_t* shortcut);
	}
}

#endif // !PPXBASE_SHORTCUT_H__