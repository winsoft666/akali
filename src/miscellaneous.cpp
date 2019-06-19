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

#include "ppxbase/miscellaneous.h"

#ifdef _WIN32
#include <math.h>
#include "ppxbase/assert.h"


namespace ppx {
    namespace base {
		double RoundEx(double v, int digits) {
            return floor(v * pow(10.f, digits) + 0.5) / pow(10.f, digits);
        }

		bool FloatEqual(const float &f1, const float &f2) {
            const float EPSINON = 0.000001f;
            if (abs(f1 - f2) <= EPSINON) {
                return true;
            }

            return false;
        }

		bool FloatNotEqual(const float &f1, const float &f2) {
            return !FloatEqual(f1, f2);
        }


		int VersionCompare(const std::string &str_ver1, const std::string &str_ver2) {
			int ver1[4] = { 0 };
			int ver2[4] = { 0 };

			sscanf_s(str_ver1.c_str(), "%d.%d.%d.%d", &ver1[0], &ver1[1], &ver1[2], &ver1[3]);
			sscanf_s(str_ver2.c_str(), "%d.%d.%d.%d", &ver2[0], &ver2[1], &ver2[2], &ver2[3]);

			for (int i = 0; i < 4; i++) {
				if (ver1[i] > ver2[i])
					return 1;
				else if (ver1[i] < ver2[i])
					return -1;
			}
			return 0;
		}


	}
}

#endif // _WIN32