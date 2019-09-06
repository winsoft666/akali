#ifndef PPX_BASE_SCHEDULE_TASK_H__
#define PPX_BASE_SCHEDULE_TASK_H__
#pragma once
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
            //
            bool NewTask(LPCTSTR pszTaskName, LPCTSTR pszProgramPath, LPCTSTR pszParameters, LPCTSTR pszDescription, LPCTSTR pszAuthor);

            bool IsExist(LPCTSTR pszTaskName);

            bool IsTaskValid(LPCTSTR pszTaskName);

            bool Run(LPCTSTR pszTaskName, LPCTSTR pszParam);

            bool IsEnable(LPCTSTR pszTaskName);

            bool SetEnable(LPCTSTR pszTaskName, bool bEnable);
          private:
            class ScheduleTaskImpl;
            ScheduleTaskImpl *impl_;
        };
    }
}

#endif // !PPX_BASE_SCHEDULE_TASK_H__