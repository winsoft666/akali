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

#include "akali/networkprotocoldef.h"

namespace akali {
__u16 GetCheckSum(__u16* header, __u32 size) {
  unsigned long checksum = 0;

  while (size > 1) {
    checksum += *header;
    header++;
    size -= 2;
  }

  if (size == 1) {
    checksum += *(unsigned char*)header;
  }
  checksum = (checksum >> 16) + (checksum & 0xffff);

  checksum += (checksum >> 16);

  return (unsigned short)(~checksum);
}
}  // namespace akali