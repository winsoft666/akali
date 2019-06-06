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

#include "ppxbase/console_print.h"
#include <stdio.h>

namespace ppx {
	namespace base {

		void PrintConsoleProcess(double percentage) {
			const char* PBSTR =
				"============================================================";
			const int PBWIDTH = 60;
			int val = (int)(percentage * 100);
			int lpad = (int)(percentage * PBWIDTH);
			int rpad = PBWIDTH - lpad;

			if (val < 0 || val > 100)
				return;

			if (val == 100 || val == 0) {
				static HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
				if (val == 100 && hConsole) {
					printf("\r[%.*s%*s] ", lpad, PBSTR, rpad, "");
					CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
					GetConsoleScreenBufferInfo(hConsole, &csbiInfo);
					SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
					printf("%3d%%", val);
					SetConsoleTextAttribute(hConsole, csbiInfo.wAttributes);
				}
				else {
					printf("\r[%.*s%*s] %3d%%", lpad, PBSTR, rpad, "", val);
				}
			}
			else {
				printf("\r[%.*s>%*s] %3d%%", lpad, PBSTR, rpad - 1, "", val);
			}
			fflush(stdout);
		}

	}
}