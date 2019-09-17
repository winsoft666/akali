#include "ppxbase/directory_monitor.h"
#include "ppxbase/safe_release_macro.h"
#include "ppxbase/logging.h"

namespace ppx {
    namespace base {


        DirectoryMonitor::DirectoryMonitor() :
            directory_(INVALID_HANDLE_VALUE) {
            ::ZeroMemory(&overlapped_, sizeof(OVERLAPPED));
            overlapped_.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
        }

        DirectoryMonitor::~DirectoryMonitor() {
            if (monitor_task_.valid())
                monitor_task_.get();


            SAFE_CLOSE(overlapped_.hEvent);
            ::ZeroMemory(&overlapped_, sizeof(OVERLAPPED));
        }

        bool DirectoryMonitor::StartMonitor(const std::wstring &direcotry, bool watch_sub_dir,
                                            DWORD filter, ChangeNotify cn) {
            std::wstring str_direcotry = direcotry;
            if (str_direcotry.length() > 0) {
                if (str_direcotry[str_direcotry.length() - 1] != L'\\')
                    str_direcotry += L"\\";
            }

            SAFE_CLOSE_ON_VALID_HANDLE(directory_);

            directory_ = ::CreateFileW(
                             str_direcotry.c_str(),
                             FILE_LIST_DIRECTORY,
                             FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                             NULL);

            if (directory_ == INVALID_HANDLE_VALUE) {
                return false;
            }

            int watch_buffer_size = 1024;
            void *watch_buffer = malloc(watch_buffer_size);
            memset(watch_buffer, 0, watch_buffer_size);

            DWORD has_read = 0;
            BOOL success = ::ReadDirectoryChangesW(
                               directory_,
                               watch_buffer,
                               watch_buffer_size,
                               watch_sub_dir,
                               filter,
                               &has_read,
                               &overlapped_,
                               NULL);
            if (!success) {
                SAFE_FREE(watch_buffer);
                SAFE_CLOSE_ON_VALID_HANDLE(directory_);
                return false;
            }

            monitor_task_ = std::async(std::launch::async,
            [watch_buffer, watch_buffer_size, filter, cn, str_direcotry, watch_sub_dir, this]() {

                do {
                    DWORD transferred = 0;
                    BOOL bret = GetOverlappedResult(directory_, &overlapped_, &transferred, TRUE);
                    DWORD gle = GetLastError();
                    if (bret && gle == 0) {
                        BYTE* base = (BYTE*)watch_buffer;
                        for (;;) {
                            FILE_NOTIFY_INFORMATION *fni = (FILE_NOTIFY_INFORMATION *)base;
                            if (cn) {
                                std::wstring str_path;
                                str_path.assign(fni->FileName, fni->FileNameLength / sizeof(WCHAR));
                                cn(fni->Action, str_direcotry + str_path);
                            }
                            if (!fni->NextEntryOffset)
                                break;
                            base += fni->NextEntryOffset;
                        }
  

                        DWORD has_read = 0;
                        BOOL success = ::ReadDirectoryChangesW(
                                           directory_,
                                           watch_buffer,
                                           watch_buffer_size,
                                           watch_sub_dir,
                                           filter,
                                           &has_read,
                                           &overlapped_,
                                           NULL);
                        if (!success) {
                            SAFE_CLOSE_ON_VALID_HANDLE(directory_);
                            return;
                        }
                    } else {
                        base::TraceMsgW(L"ReadDirectoryChangesW failed, GLE: %ld", gle);
                        if (watch_buffer)
                            free(watch_buffer);
                        break;
                    }
                } while (true);


            });

            return true;
        }

        void DirectoryMonitor::StopMonitor() {
            if (directory_ != INVALID_HANDLE_VALUE) {
                CancelIo(directory_);
                ::CloseHandle(directory_);
                directory_ = INVALID_HANDLE_VALUE;
            }
        }

    }
}