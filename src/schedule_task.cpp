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

#include "ppxbase/schedule_task.h"
#ifdef _WIN32
#include <Atlbase.h>
#include <comdef.h>
#include <taskschd.h>
#include "ppxbase/safe_release_macro.h"

#pragma comment(lib, "taskschd.lib")

namespace ppx {
    namespace base {

        class ScheduleTask::ScheduleTaskImpl {
          public:
            ScheduleTaskImpl() {
                m_lpITS = NULL;
                m_lpRootFolder = NULL;
                HRESULT hr = ::CoInitialize(NULL);
                if ( FAILED(hr) ) {
                }

                hr = ::CoCreateInstance(CLSID_TaskScheduler,
                                        NULL,
                                        CLSCTX_INPROC_SERVER,
                                        IID_ITaskService,
                                        (LPVOID *)(&m_lpITS));
                if ( FAILED(hr) ) {
                }

                hr = m_lpITS->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
                if ( FAILED(hr) ) {

                }

                hr = m_lpITS->GetFolder(_bstr_t(TEXT("\\")), &m_lpRootFolder);
                if ( FAILED(hr) ) {
                }
            }

            ~ScheduleTaskImpl() {
                if ( m_lpITS ) {
                    m_lpITS->Release();
                }
                if ( m_lpRootFolder ) {
                    m_lpRootFolder->Release();
                }
                ::CoUninitialize();
            }

            BOOL NewTask(LPCTSTR pszTaskName, LPCTSTR pszProgramPath,
                         LPCTSTR pszParameters, LPCTSTR pszDescription, LPCTSTR pszAuthor) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }

                Delete(pszTaskName);

                ITaskDefinition *pTaskDefinition = NULL;
                HRESULT hr = m_lpITS->NewTask(0, &pTaskDefinition);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                IRegistrationInfo *pRegInfo = NULL;
                CComVariant variantAuthor(NULL);
                variantAuthor = pszAuthor;
                CComVariant variantDescription(NULL);
                variantDescription = pszDescription;
                hr = pTaskDefinition->get_RegistrationInfo(&pRegInfo);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                hr = pRegInfo->put_Author(variantAuthor.bstrVal);
                hr = pRegInfo->put_Description(variantDescription.bstrVal);
                pRegInfo->Release();

                IPrincipal *pPrincipal = NULL;
                hr = pTaskDefinition->get_Principal(&pPrincipal);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
                hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
                pPrincipal->Release();


                ITaskSettings *pSettting = NULL;
                hr = pTaskDefinition->get_Settings(&pSettting);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                hr = pSettting->put_StopIfGoingOnBatteries(VARIANT_FALSE);
                hr = pSettting->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
                hr = pSettting->put_AllowDemandStart(VARIANT_TRUE);
                hr = pSettting->put_StartWhenAvailable(VARIANT_FALSE);
                hr = pSettting->put_MultipleInstances(TASK_INSTANCES_PARALLEL);
                pSettting->Release();


                IActionCollection *pActionCollect = NULL;
                hr = pTaskDefinition->get_Actions(&pActionCollect);
                if ( FAILED(hr) ) {
                    return FALSE;
                }
                IAction *pAction = NULL;

                hr = pActionCollect->Create(TASK_ACTION_EXEC, &pAction);
                pActionCollect->Release();


                CComVariant variantProgramPath(NULL);
                CComVariant variantParameters(NULL);
                IExecAction *pExecAction = NULL;
                hr = pAction->QueryInterface(IID_IExecAction, (PVOID *)(&pExecAction));
                if ( FAILED(hr) ) {
                    pAction->Release();
                    return FALSE;
                }
                pAction->Release();

                variantProgramPath = pszProgramPath;
                variantParameters = pszParameters;
                pExecAction->put_Path(variantProgramPath.bstrVal);
                pExecAction->put_Arguments(variantParameters.bstrVal);
                pExecAction->Release();


                ITriggerCollection *pTriggers = NULL;
                hr = pTaskDefinition->get_Triggers(&pTriggers);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                ITrigger *pTrigger = NULL;
                hr = pTriggers->Create(TASK_TRIGGER_LOGON, &pTrigger);
                if ( FAILED(hr) ) {
                    return FALSE;
                }


                IRegisteredTask *pRegisteredTask = NULL;
                CComVariant variantTaskName(NULL);
                variantTaskName = pszTaskName;
                hr = m_lpRootFolder->RegisterTaskDefinition(variantTaskName.bstrVal,
                        pTaskDefinition,
                        TASK_CREATE_OR_UPDATE,
                        _variant_t(),
                        _variant_t(),
                        TASK_LOGON_INTERACTIVE_TOKEN,
                        _variant_t(""),
                        &pRegisteredTask);
                if ( FAILED(hr) ) {
                    pTaskDefinition->Release();
                    return FALSE;
                }
                pTaskDefinition->Release();
                pRegisteredTask->Release();

                return TRUE;
            }

            BOOL Delete(LPCTSTR pszTaskName) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                CComVariant variantTaskName(NULL);
                variantTaskName = pszTaskName;
                HRESULT hr = m_lpRootFolder->DeleteTask(variantTaskName.bstrVal, 0);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                return TRUE;
            }

            BOOL DeleteFolder(LPCTSTR pszFolderName) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                CComVariant variantFolderName(NULL);
                variantFolderName = pszFolderName;
                HRESULT hr = m_lpRootFolder->DeleteFolder(variantFolderName.bstrVal, 0);
                if ( FAILED(hr) ) {
                    return FALSE;
                }

                return TRUE;
            }

            BOOL IsExist(LPCTSTR pszTaskName) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                HRESULT hr = S_OK;
                CComVariant variantTaskName(NULL);
                CComVariant variantEnable(NULL);
                variantTaskName = pszTaskName;
                IRegisteredTask *pRegisteredTask = NULL;

                hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
                if ( FAILED(hr) || (NULL == pRegisteredTask) ) {
                    return FALSE;
                }
                pRegisteredTask->Release();

                return TRUE;
            }

            BOOL IsTaskValid(LPCTSTR pszTaskName) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                HRESULT hr = S_OK;
                CComVariant variantTaskName(NULL);
                CComVariant variantEnable(NULL);
                variantTaskName = pszTaskName;
                IRegisteredTask *pRegisteredTask = NULL;

                hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
                if ( FAILED(hr) || (NULL == pRegisteredTask) ) {
                    return FALSE;
                }

                TASK_STATE taskState;
                hr = pRegisteredTask->get_State(&taskState);
                if ( FAILED(hr) ) {
                    pRegisteredTask->Release();
                    return FALSE;
                }
                pRegisteredTask->Release();

                if ( TASK_STATE_DISABLED == taskState ) {
                    return FALSE;
                }

                return TRUE;
            }

            BOOL Run(LPCTSTR pszTaskName, LPCTSTR pszParam) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                HRESULT hr = S_OK;
                CComVariant variantTaskName(NULL);
                CComVariant variantParameters(NULL);
                variantTaskName = pszTaskName;
                variantParameters = pszParam;

                IRegisteredTask *pRegisteredTask = NULL;
                hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
                if ( FAILED(hr) || (NULL == pRegisteredTask) ) {
                    return FALSE;
                }

                hr = pRegisteredTask->Run(variantParameters, NULL);
                if ( FAILED(hr) ) {
                    pRegisteredTask->Release();
                    return FALSE;
                }
                pRegisteredTask->Release();

                return TRUE;
            }

            BOOL IsEnable(LPCTSTR pszTaskName) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                HRESULT hr = S_OK;
                CComVariant variantTaskName(NULL);
                CComVariant variantEnable(NULL);
                variantTaskName = pszTaskName;
                IRegisteredTask *pRegisteredTask = NULL;

                hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
                if ( FAILED(hr) || (NULL == pRegisteredTask) ) {
                    return FALSE;
                }

                pRegisteredTask->get_Enabled(&variantEnable.boolVal);
                pRegisteredTask->Release();
                if ( ATL_VARIANT_FALSE == variantEnable.boolVal ) {
                    return FALSE;
                }

                return TRUE;
            }

            BOOL SetEnable(LPCTSTR pszTaskName, bool bEnable) {
                if ( NULL == m_lpRootFolder ) {
                    return FALSE;
                }
                HRESULT hr = S_OK;
                CComVariant variantTaskName(NULL);
                CComVariant variantEnable(NULL);
                variantTaskName = pszTaskName;
                variantEnable = bEnable;
                IRegisteredTask *pRegisteredTask = NULL;

                hr = m_lpRootFolder->GetTask(variantTaskName.bstrVal, &pRegisteredTask);
                if ( FAILED(hr) || (NULL == pRegisteredTask) ) {
                    return FALSE;
                }

                pRegisteredTask->put_Enabled(variantEnable.boolVal);
                pRegisteredTask->Release();

                return TRUE;
            }

          protected:
            ITaskService *m_lpITS;
            ITaskFolder *m_lpRootFolder;
        };

        ScheduleTask::ScheduleTask() : impl_( new ScheduleTaskImpl()) {

        }

        ScheduleTask::~ScheduleTask() {
            SAFE_DELETE(impl_);
        }

        bool ScheduleTask::Delete(LPCTSTR pszTaskName) {
            return impl_->Delete(pszTaskName) == TRUE;
        }

        bool ScheduleTask::DeleteFolder(LPCTSTR pszFolderName) {
            return impl_->DeleteFolder(pszFolderName) == TRUE;
        }

        bool ScheduleTask::NewTask(LPCTSTR pszTaskName, LPCTSTR pszProgramPath,
                                   LPCTSTR pszParameters, LPCTSTR pszDescription, LPCTSTR pszAuthor) {
            return impl_->NewTask(pszTaskName, pszProgramPath, pszParameters, pszDescription, pszAuthor) == TRUE;
        }

        bool ScheduleTask::IsExist(LPCTSTR pszTaskName) {
            return impl_->IsExist(pszTaskName) == TRUE;
        }

        bool ScheduleTask::IsTaskValid(LPCTSTR pszTaskName) {
            return impl_->IsTaskValid(pszTaskName) == TRUE;
        }

        bool ScheduleTask::Run(LPCTSTR pszTaskName, LPCTSTR pszParam) {
            return impl_->Run(pszTaskName, pszParam) == TRUE;
        }

        bool ScheduleTask::IsEnable(LPCTSTR pszTaskName) {
            return impl_->IsEnable(pszTaskName) == TRUE;
        }

        bool ScheduleTask::SetEnable(LPCTSTR pszTaskName, bool bEnable) {
            return impl_->SetEnable(pszTaskName, bEnable) == TRUE;
        }
    }
}

#endif