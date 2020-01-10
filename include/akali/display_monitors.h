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

#ifndef AKALI_DISPLAY_MINITORS_H__
#define AKALI_DISPLAY_MINITORS_H__
#pragma once
#include "akali/akali_export.h"

#ifdef AKALI_WIN
#include <vector>

namespace akali {
// 针对高DPI的显示环境，调用该方法的进程需要设置为PROCESS_PER_MONITOR_DPI_AWARE或PROCESS_SYSTEM_DPI_AWARE才能获取到正确的位置、尺寸、缩放
// 关于DPI的内容参考：https://blog.csdn.net/china_jeffery/article/details/83895478
//

typedef struct _DSPMONITOR {
  bool primary;

  int x;
  int y;
  int width;
  int height;

  int workarea_x;
  int workarea_y;

  int workarea_width;
  int workarea_height;

  int scale;
} DSPMONITOR;

class AKALI_API DisplayMonitors {
public:
  DisplayMonitors();
  ~DisplayMonitors();

  // Enumerate all the display devices which PC is connecting.
  bool EnumDisplayMonitors();

  // Return the number of devices
  int GetCount() const;

  // Get the device configuration depending the requesting index.
  bool GetDspInfo(const int index, DSPMONITOR *dsp);

  bool GetPrimaryDspInfo(DSPMONITOR *dsp);

private:
  std::vector<DSPMONITOR> dsp_list_;
};
} // namespace akali
#endif

#endif //! AKALI_DISPLAY_MINITORS_H__
