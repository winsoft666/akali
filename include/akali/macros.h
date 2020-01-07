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

#ifndef AKALI_MACROS_H__
#define AKALI_MACROS_H__

#ifndef SAFE_FREE
#define SAFE_FREE(p)                                                                               \
  do {                                                                                             \
    if ((p) != NULL) {                                                                             \
      free(p);                                                                                     \
      (p) = NULL;                                                                                  \
    }                                                                                              \
  } while (false)
#endif

#ifndef SAFE_CLOSE_ON_NONULL
#define SAFE_CLOSE(p)                                                                              \
  do {                                                                                             \
    if ((p) != NULL) {                                                                             \
      CloseHandle((p));                                                                            \
      (p) = NULL;                                                                                  \
    }                                                                                              \
  } while (false)
#endif

#ifndef SAFE_CLOSE_ON_VALID_HANDLE
#define SAFE_CLOSE_ON_VALID_HANDLE(p)                                                              \
  do {                                                                                             \
    if ((p) != INVALID_HANDLE_VALUE) {                                                             \
      CloseHandle((p));                                                                            \
      (p) = INVALID_HANDLE_VALUE;                                                                  \
    }                                                                                              \
  } while (false)
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)                                                                       \
  do {                                                                                             \
    if ((p) != NULL) {                                                                             \
      delete[](p);                                                                                 \
      (p) = NULL;                                                                                  \
    }                                                                                              \
  } while (false)
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)                                                                             \
  do {                                                                                             \
    if ((p) != NULL) {                                                                             \
      delete (p);                                                                                  \
      (p) = NULL;                                                                                  \
    }                                                                                              \
  } while (false)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)                                                                            \
  do {                                                                                             \
    if ((p)) {                                                                                     \
      (p)->Release();                                                                              \
      (p) = NULL;                                                                                  \
    }                                                                                              \
  } while (false)
#endif

#endif // ! AKALI_MACROS_H__
