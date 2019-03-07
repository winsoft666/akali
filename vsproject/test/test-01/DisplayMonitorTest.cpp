#include "stdafx.h"
#include "DisplayMonitorTest.h"


void DisplayMonitorTest() {
    ppx::base::DisplayMonitors dd;
    bool ret = dd.EnumDisplayMonitors();

    for (int i = 0; i < dd.GetCount(); i++) {
        ppx::base::DSPMONITOR dev;
        if (dd.GetDspInfo(i, &dev)) {
            printf("\n[%d]: primary:%d, scale=%d,   x=%d, y=%d, width=%d, height=%d, workarea_x=%d, workarea_x=%d, workarea_width=%d, workarea_height=%d\n==============\n",
                i,
                dev.primary,
                dev.scale,
                dev.x, dev.y, dev.width, dev.height,
                dev.workarea_x, dev.workarea_y, dev.workarea_width, dev.workarea_height
            );
        }
    }

}

