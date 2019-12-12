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

#ifndef PPX_BASE_SCHEDULE_TASK_H__
#define PPX_BASE_SCHEDULE_TASK_H__
#pragma once

#if (defined _WIN32 || defined WIN32)
#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <tchar.h>
#include "ppxbase_export.h"

namespace ppx {
    namespace base {
        class PPXBASE_API ScheduleTask {
          public:
            ScheduleTask();
            virtual ~ScheduleTask();

            bool Delete(LPCTSTR pszTaskName);

            bool DeleteFolder(LPCTSTR pszFolderName);

            // ITrigger type is TASK_TRIGGER_LOGON
            // Action number is 1
            //
            bool NewLoginTriggerTask(LPCTSTR pszTaskName, LPCTSTR pszProgramPath, LPCTSTR pszParameters, LPCTSTR pszDescription, LPCTSTR pszAuthor);

            bool IsExist(LPCTSTR pszTaskName);

            bool IsTaskValid(LPCTSTR pszTaskName);

            bool Run(LPCTSTR pszTaskName, LPCTSTR pszParam);

            bool IsEnable(LPCTSTR pszTaskName);

            bool SetEnable(LPCTSTR pszTaskName, bool bEnable);

            bool GetProgramPath(LPCTSTR pszTaskName, long lActionIndex, LPTSTR pszProgramPath);

            bool GetParameters(LPCTSTR pszTaskName, long lActionIndex, LPTSTR pszParameters);
          private:
            class ScheduleTaskImpl;
            ScheduleTaskImpl *impl_;
        };
    }
}

#endif

#endif // !PPX_BASE_SCHEDULE_TASK_H__