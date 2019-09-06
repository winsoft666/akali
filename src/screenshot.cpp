#include "ppxbase/screenshot.h"
#include <windows.h>


namespace ppx {
    namespace base {
        namespace {
            static bool SaveBmpBufToFile(const std::wstring &file_path, const void *buf, int width, int height, BITMAPINFOHEADER *bi_header) {
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
        }

        bool Screenshot::FullScreenToFile(const std::wstring &file_path) {
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

        bool Screenshot::WindowToFile(HWND h, const std::wstring &file_path) {
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
    }
}