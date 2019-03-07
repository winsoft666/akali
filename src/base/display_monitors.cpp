#include "base/display_monitors.h"
#include <windows.h>

namespace ppx {
    namespace base {

        static BOOL CALLBACK Monitorenumproc(HMONITOR hMonitor, HDC hdc, LPRECT pRect, LPARAM arg) {
            std::vector<HMONITOR>* monitors = (std::vector<HMONITOR>*)arg;
            monitors->push_back(hMonitor);

            return TRUE;
        }

        DisplayMonitors::DisplayMonitors() {
        }

        DisplayMonitors::~DisplayMonitors() {
            dsp_list_.clear();
        }

        typedef enum MONITOR_DPI_TYPE {
            MDT_EFFECTIVE_DPI = 0,
            MDT_ANGULAR_DPI = 1,
            MDT_RAW_DPI = 2,
            MDT_DEFAULT = MDT_EFFECTIVE_DPI
        } MONITOR_DPI_TYPE;

        typedef HRESULT(WINAPI *LPGetDpiForMonitor)(
            _In_  HMONITOR         hmonitor,
            _In_  MONITOR_DPI_TYPE dpiType,
            _Out_ UINT             *dpiX,
            _Out_ UINT             *dpiY
            );

        int GetDPIOfMonitor(HMONITOR hMonitor) {
            UINT dpix = 96, dpiy = 96;
            HRESULT  hr = E_FAIL;
            HMODULE hModule = ::LoadLibraryW(L"Shcore.dll");
            if (hModule != NULL) {
                LPGetDpiForMonitor GetDpiForMonitor = (LPGetDpiForMonitor)GetProcAddress(hModule, "GetDpiForMonitor");

                if (GetDpiForMonitor != NULL && GetDpiForMonitor(hMonitor, MDT_EFFECTIVE_DPI, &dpix, &dpiy) != S_OK) {
                    return 96;
                }
            }
            else {
                HDC screen = GetDC(0);
                dpix = GetDeviceCaps(screen, LOGPIXELSX);
                ReleaseDC(0, screen);
            }

            return dpix;
        }

        bool DisplayMonitors::EnumDisplayMonitors() {
            std::vector<HMONITOR> monitors;
            if (!::EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)Monitorenumproc, (LPARAM)&monitors)) {
                return false;
            }

            for (size_t i = 0; i < monitors.size(); i++) {
                MONITORINFOEX minfo;
                DSPMONITOR dspinfo;
                ZeroMemory(&dspinfo, sizeof(DSPMONITOR));

                minfo.cbSize = sizeof(MONITORINFOEX);
                if (GetMonitorInfo(monitors[i], &minfo)) {
                    dspinfo.primary = (minfo.dwFlags & MONITORINFOF_PRIMARY) == MONITORINFOF_PRIMARY;

                    dspinfo.width = minfo.rcMonitor.right - minfo.rcMonitor.left;
                    dspinfo.height = minfo.rcMonitor.bottom - minfo.rcMonitor.top;
                    dspinfo.x = minfo.rcMonitor.left;
                    dspinfo.y = minfo.rcMonitor.top;

                    dspinfo.workarea_width = minfo.rcWork.right - minfo.rcWork.left;
                    dspinfo.workarea_height = minfo.rcWork.bottom - minfo.rcWork.top;
                    dspinfo.workarea_x = minfo.rcWork.left;
                    dspinfo.workarea_y = minfo.rcWork.top;

                    dspinfo.scale = GetDPIOfMonitor(monitors[i]) * 100 / 96;

                    dsp_list_.push_back(dspinfo);
                }
            }

            return true;
        }

        int DisplayMonitors::GetCount() const {
            return (int)dsp_list_.size();
        }

        bool DisplayMonitors::GetDspInfo(const int index, DSPMONITOR * dsp) {
            if (index >= 0 && index < (int)dsp_list_.size()) {
                if (dsp != NULL) {
                    *dsp = dsp_list_[index];
                    return true;
                }
            }
            return false;
        }

        bool DisplayMonitors::GetPrimaryDspInfo(DSPMONITOR* dsp) {
            for (size_t i = 0; i < dsp_list_.size(); i++) {
                if (dsp_list_[i].primary) {
                    *dsp = dsp_list_[i];
                    return true;
                }
            }
            return false;
        }
    }
}