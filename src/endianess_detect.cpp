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

#include "akali/endianess_detect.h"

namespace akali {
bool IsHostBigEndian() {
  int nl = 0x12345678;
  short ns = 0x1234;
  bool big_endian = false;

  unsigned char *p = (unsigned char *)(&nl);
  unsigned char *sp = (unsigned char *)(&ns);

  if (p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78) {
    big_endian = true;
  }
  else if (p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12) {
    big_endian = false;
  }
  else {
    big_endian = (*sp != 0x12);
  }

  return big_endian;
}
} // namespace akali