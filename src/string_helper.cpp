#include "ppxbase/string_helper.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <stdarg.h>
#include <string.h>
#if (defined _WIN32 || defined WIN32)
#include <strsafe.h>
#endif

namespace ppx {
    namespace base {
        namespace {
            static char EasyToLowerA(char in) {
                if (in <= 'Z' && in >= 'A')
                    return in - ('Z' - 'z');
                return in;
            }

            static char EasyToUpperA(char in) {
                if (in <= 'z' && in >= 'a')
                    return in + ('Z' - 'z');
                return in;
            }

            static wchar_t EasyToLowerW(wchar_t in) {
                if (in <= 'Z' && in >= 'A')
                    return in - (L'Z' - L'z');
                return in;
            }

            static wchar_t EasyToUpperW(wchar_t in) {
                if (in <= L'z' && in >= L'a')
                    return in + (L'Z' - L'z');
                return in;
            }
#if (defined _WIN32 || defined WIN32)
            static wchar_t *InnerMakeStringW(int &iBufLen, int iBufSizeStep, const wchar_t *pstrFormat, va_list Args) {
                iBufLen = 0;
                if (!pstrFormat)
                    return NULL;

                wchar_t *pMsgBuffer = NULL;
                HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

                while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
                    iBufLen += iBufSizeStep;
                    if (pMsgBuffer) {
                        free(pMsgBuffer);
                        pMsgBuffer = NULL;
                    }

                    pMsgBuffer = (wchar_t *)malloc(iBufLen * sizeof(wchar_t));
                    if (!pMsgBuffer) {
                        iBufLen = 0;
                        return NULL;
                    }

                    hr = StringCchVPrintfW(pMsgBuffer, iBufLen, pstrFormat, Args);
                }

                return pMsgBuffer;
            }

            static char *InnerMakeStringA(int &iBufLen, int iBufSizeStep, const char *pstrFormat, va_list Args) {
                iBufLen = 0;
                if (!pstrFormat)
                    return NULL;

                char *pMsgBuffer = NULL;
                HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

                while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
                    iBufLen += iBufSizeStep;
                    if (pMsgBuffer) {
                        free(pMsgBuffer);
                        pMsgBuffer = NULL;
                    }

                    pMsgBuffer = (char *)malloc(iBufLen * sizeof(char));
                    if (!pMsgBuffer) {
                        iBufLen = 0;
                        return NULL;
                    }

                    hr = StringCchVPrintfA(pMsgBuffer, iBufLen, pstrFormat, Args);
                }

                return pMsgBuffer;
            }
#endif
        }

        std::string StringToLower(const std::string &str) {
            std::string ret = str;
            std::transform(ret.begin(), ret.end(), ret.begin(), EasyToLowerA);
            return ret;
        }

        std::wstring StringToLower(const std::wstring &str) {
            std::wstring ret = str;
            std::transform(ret.begin(), ret.end(), ret.begin(), EasyToLowerW);
            return ret;
        }

        std::string StringToUpper(const std::string &str) {
            std::string ret = str;
            std::transform(ret.begin(), ret.end(), ret.begin(), EasyToUpperA);
            return ret;
        }

        std::wstring StringToUpper(const std::wstring &str) {
            std::wstring ret = str;
            std::transform(ret.begin(), ret.end(), ret.begin(), EasyToUpperW);
            return ret;
        }

        std::vector<std::wstring> StringSplit(const std::wstring &src, const std::wstring &delimiter) {
            std::vector<std::wstring> fields;
            std::wstring src_copy = src;
            int pos = StringFind(src_copy, delimiter, 0);

            while (pos >= 0) {
                std::wstring t = src_copy.substr(0, pos);
                fields.push_back(t);
                size_t l = src_copy.length() - pos - delimiter.length();
                src_copy = src_copy.substr(src_copy.length() - l, l);
                pos = StringFind(src_copy, delimiter);
            }

            fields.push_back(src_copy);
            return fields;
        }

        std::vector<std::string> StringSplit(const std::string &src, const std::string &delimiter) {
            std::vector<std::string> fields;
            std::string src_copy = src;
            int pos = StringFind(src_copy, delimiter, 0);

            while (pos >= 0) {
                std::string t = src_copy.substr(0, pos);
                fields.push_back(t);
                size_t l = src_copy.length() - pos - delimiter.length();
                src_copy = src_copy.substr(src_copy.length() - l, l);
                pos = StringFind(src_copy, delimiter);
            }

            fields.push_back(src_copy);
            return fields;
        }

        int StringFind(const std::wstring &src, wchar_t ch, int iOffset /*= 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;

            size_t ret = src.find_first_of(ch, iOffset);
            if (ret == std::wstring::npos)
                return -1;

            return ret;
        }

        int StringFind(const std::wstring &src, const std::wstring &search, int iOffset /*= 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;

            const wchar_t *p = wcsstr(src.c_str() + iOffset, search.c_str());

            if (p == NULL) return -1;

            return (int)(p - src.c_str());
        }

        int StringReverseFind(const std::wstring &src, wchar_t ch, int iOffset /* = 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;
            size_t ret = src.find_last_of(ch, src.length() - iOffset - 1);
            if (ret == std::wstring::npos)
                return -1;

            return ret;
        }

        int StringReverseFind(const std::wstring &src, const std::wstring &search, int iOffset /*= 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;

            int index = src.length() - 1 - iOffset;
            int findStrLen = search.length();
            int searchStart = -1;

            for (; index >= 0; index--) {
                searchStart = index - findStrLen + 1;
                if (searchStart > 0) {
                    if (wcsncmp(src.c_str() + searchStart, search.c_str(), findStrLen) == 0) {
                        return searchStart;
                    }
                }
            }

            return -1;
        }

        int StringFind(const std::string &src, char ch, int iOffset /*= 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;

            size_t ret = src.find_first_of(ch, iOffset);
            if (ret == std::string::npos)
                return -1;

            return ret;
        }

        int StringFind(const std::string &src, const std::string &search, int iOffset /*= 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;

            const char *p = strstr(src.c_str() + iOffset, search.c_str());

            if (p == NULL) return -1;

            return (int)(p - src.c_str());
        }

        int StringReverseFind(const std::string &src, char ch, int iOffset /* = 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;
            size_t ret = src.find_last_of(ch, src.length() - iOffset - 1);
            if (ret == std::string::npos)
                return -1;

            return ret;
        }

        int StringReverseFind(const std::string &src, const std::string &search, int iOffset /*= 0*/) {
            if (iOffset < 0 || iOffset > (int)src.length())
                return -1;

            int index = src.length() - 1 - iOffset;
            int findStrLen = search.length();
            int searchStart = -1;

            for (; index >= 0; index--) {
                searchStart = index - findStrLen + 1;
                if (searchStart > 0) {
                    if (strncmp(src.c_str() + searchStart, search.c_str(), findStrLen) == 0) {
                        return searchStart;
                    }
                }
            }

            return -1;
        }
#if (defined _WIN32 || defined WIN32)
        std::string MakeString(const char *pstrFormat, ...) {
            std::string strResult;
            if (!pstrFormat)
                return strResult;

            char *pMsgBuffer = NULL;
            int iMsgBufCount = 0;

            va_list arglist;
            va_start(arglist, pstrFormat);
            pMsgBuffer = InnerMakeStringA(iMsgBufCount, 512, pstrFormat, arglist);
            va_end(arglist);

            if (pMsgBuffer) {
                strResult.assign(pMsgBuffer);
            }

            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            return strResult;
        }

        std::wstring MakeString(const wchar_t *pstrFormat, ...) {
            std::wstring strResult;
            if (!pstrFormat)
                return strResult;

            wchar_t *pMsgBuffer = NULL;
            int iMsgBufCount = 0;

            va_list arglist;
            va_start(arglist, pstrFormat);
            pMsgBuffer = InnerMakeStringW(iMsgBufCount, 512, pstrFormat, arglist);
            va_end(arglist);

            if (pMsgBuffer) {
                strResult.assign(pMsgBuffer);
            }

            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            return strResult;
        }
#endif
    }
}