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

#include "akali/miscellaneous.h"
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <strsafe.h>
#endif

namespace akali {
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

bool FloatNotEqual(const float &f1, const float &f2) { return !FloatEqual(f1, f2); }

int VersionCompare(const std::string &str_ver1, const std::string &str_ver2) {
  int ver1[4] = {0};
  int ver2[4] = {0};

  sscanf(str_ver1.c_str(), "%d.%d.%d.%d", &ver1[0], &ver1[1], &ver1[2], &ver1[3]);
  sscanf(str_ver2.c_str(), "%d.%d.%d.%d", &ver2[0], &ver2[1], &ver2[2], &ver2[3]);

  for (int i = 0; i < 4; i++) {
    if (ver1[i] > ver2[i])
      return 1;
    else if (ver1[i] < ver2[i])
      return -1;
  }
  return 0;
}

#if (defined _WIN32 || defined WIN32)
static bool SaveBmpBufToFile(const std::wstring &file_path, const void *buf, int width, int height,
                             BITMAPINFOHEADER *bi_header) {
  BITMAPFILEHEADER Header;
  memset(&Header, 0, sizeof(Header));
  Header.bfType = 0x4D42;
  Header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

  FILE *fFile = NULL;
  if (_wfopen_s(&fFile, file_path.c_str(), L"wb") != 0 || fFile == NULL) {
    return false;
  }

  fwrite(&Header, 1, sizeof(Header), fFile);
  fwrite(bi_header, 1, sizeof(BITMAPINFOHEADER), fFile);
  fwrite(buf, 1, (((32 * width + 31) & ~31) / 8) * height, fFile);
  fclose(fFile);

  return true;
}

bool FullScreenToFile(const std::wstring &file_path) {
  HDC hScreenDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
  HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

  int width = GetDeviceCaps(hScreenDC, HORZRES);
  int height = GetDeviceCaps(hScreenDC, VERTRES);

  void *pPixelsBuf = NULL;
  BITMAPINFO bi;
  memset(&bi, 0, sizeof(bi));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = int(width);
  bi.bmiHeader.biHeight = -int(height);
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = width * height * (32 > 24 ? 4 : 3);

  HBITMAP hBitmap = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &pPixelsBuf, NULL, 0);

  HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
  BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
  SelectObject(hMemoryDC, hOldBitmap);

  DeleteDC(hMemoryDC);
  DeleteDC(hScreenDC);

  // save pPixelsBuf to file.
  bool ret = SaveBmpBufToFile(file_path, pPixelsBuf, width, height, &bi.bmiHeader);

  DeleteObject(hBitmap);

  return ret;
}

bool WindowToFile(HWND h, const std::wstring &file_path) {
  if (!h)
    return false;

  HDC hScreenDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
  HDC hMemoryDC = CreateCompatibleDC(hScreenDC);

  int width = GetDeviceCaps(hScreenDC, HORZRES);
  int height = GetDeviceCaps(hScreenDC, VERTRES);

  RECT rectWnd;
  GetWindowRect(h, &rectWnd);

  int nLeft = rectWnd.left;
  int nRight = rectWnd.right;
  int nTop = rectWnd.top;
  int nBottom = rectWnd.bottom;

  if (nLeft < 0)
    nLeft = 0;

  if (nTop < 0)
    nTop = 0;

  if (nRight > width)
    nRight = width;
  if (nBottom > height)
    nBottom = height;

  width = nRight - nLeft;
  height = nBottom - nTop;

  void *pPixelsBuf = NULL;
  BITMAPINFO bi;
  memset(&bi, 0, sizeof(bi));
  bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bi.bmiHeader.biWidth = int(width);
  bi.bmiHeader.biHeight = -int(height);
  bi.bmiHeader.biPlanes = 1;
  bi.bmiHeader.biBitCount = 32;
  bi.bmiHeader.biCompression = BI_RGB;
  bi.bmiHeader.biSizeImage = width * height * (32 > 24 ? 4 : 3);

  HBITMAP hBitmap = ::CreateDIBSection(0, &bi, DIB_RGB_COLORS, &pPixelsBuf, NULL, 0);

  HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
  BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, nLeft, nTop, SRCCOPY);
  SelectObject(hMemoryDC, hOldBitmap);

  DeleteDC(hMemoryDC);
  DeleteDC(hScreenDC);

  // save pPixelsBuf to file.
  bool ret = SaveBmpBufToFile(file_path, pPixelsBuf, width, height, &bi.bmiHeader);

  DeleteObject(hBitmap);

  return ret;
}

void PrintConsoleProcess(double percentage) {
  const char *PBSTR = "============================================================";
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
#endif


void TraceMsgW(const wchar_t *lpFormat, ...) {
  if (!lpFormat)
    return;

#ifdef WIN32
  wchar_t *pMsgBuffer = NULL;
  unsigned int iMsgBufCount = 0;

  va_list arglist;
  va_start(arglist, lpFormat);
  HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

  while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
    iMsgBufCount += 1024;

    if (pMsgBuffer) {
      free(pMsgBuffer);
      pMsgBuffer = NULL;
    }

    pMsgBuffer = (wchar_t *)malloc(iMsgBufCount * sizeof(wchar_t));

    if (!pMsgBuffer) {
      break;
    }

    hr = StringCchVPrintfW(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
  }

  va_end(arglist);

  if (hr == S_OK) {
    if (pMsgBuffer)
      OutputDebugStringW(pMsgBuffer);
  }

  if (pMsgBuffer) {
    free(pMsgBuffer);
    pMsgBuffer = NULL;
  }
#else
  wchar_t msgBuf[1024] = {0};
  va_list arglist;
  va_start(arglist, lpFormat);
  int err = vswprintf(msgBuf, 1024, lpFormat, arglist);
  va_end(arglist);
  printf("%ls\n", msgBuf);
#endif
}

void TraceMsgA(const char *lpFormat, ...) {
  if (!lpFormat)
    return;

#ifdef WIN32
  char *pMsgBuffer = NULL;
  unsigned int iMsgBufCount = 0;

  va_list arglist;
  va_start(arglist, lpFormat);
  HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

  while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
    iMsgBufCount += 1024;

    if (pMsgBuffer) {
      free(pMsgBuffer);
      pMsgBuffer = NULL;
    }

    pMsgBuffer = (char *)malloc(iMsgBufCount * sizeof(char));

    if (!pMsgBuffer) {
      break;
    }

    hr = StringCchVPrintfA(pMsgBuffer, iMsgBufCount, lpFormat, arglist);
  }

  va_end(arglist);

  if (hr == S_OK) {
    if (pMsgBuffer)
      OutputDebugStringA(pMsgBuffer);
  }

  if (pMsgBuffer) {
    free(pMsgBuffer);
    pMsgBuffer = NULL;
  }
#else
  char msgBuf[1024] = {0};
  va_list arglist;
  va_start(arglist, lpFormat);
  int err = vsnprintf(msgBuf, 1024, lpFormat, arglist);
  va_end(arglist);
  printf("%s\n", msgBuf);
#endif
}
} // namespace akali
