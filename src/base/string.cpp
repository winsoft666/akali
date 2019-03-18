#include "base/string.h"
#include "base/stringencode.h"
#include "base/assert.h"
#include "base/safe_release_macro.h"
#include <tchar.h>
#include <strsafe.h>
#include <algorithm>
#include <cctype>
#include <locale>

namespace ppx {
    namespace base {
		namespace Internal {
			static wchar_t* InnerMakeStringW(int &iBufLen, int iBufSizeStep, const wchar_t* pstrFormat, va_list Args) {
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

            static char* InnerMakeStringA(int &iBufLen, int iBufSizeStep, const char* pstrFormat, va_list Args) {
                iBufLen = 0;
                if ( !pstrFormat )
                    return NULL;

                char *pMsgBuffer = NULL;
                HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

                while ( hr == STRSAFE_E_INSUFFICIENT_BUFFER ) {
                    iBufLen += iBufSizeStep;
                    if ( pMsgBuffer ) {
                        free(pMsgBuffer);
                        pMsgBuffer = NULL;
                    }

                    pMsgBuffer = (char *)malloc(iBufLen * sizeof(char));
                    if ( !pMsgBuffer ) {
                        iBufLen = 0;
                        return NULL;
                    }

                    hr = StringCchVPrintfA(pMsgBuffer, iBufLen, pstrFormat, Args);
                }

                return pMsgBuffer;
            }

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
		}

		StringA::StringA() {
		}

		StringA::StringA(const char ch) {
			m_str = ch;
		}

		StringA::StringA(const StringA &src) {
			m_str = src.m_str;
		}

		StringA::StringA(const std::string &src) {
			m_str = src;
		}

		StringA::StringA(const char* lpsz, int nLen /*= -1*/) {
			m_str.clear();
			if (!lpsz)
				return;

			std::string str;
			if (nLen != -1)
                m_str.assign(lpsz, nLen);
			else
                m_str = lpsz;
		}

		StringA::~StringA() {
			m_str.clear();
		}

		int StringA::GetLength() const {
			return m_str.size();
		}

		void StringA::Append(const StringA & str) {
			m_str.append(str.m_str);
		}

		void StringA::Append(const char* str, int nLen /*= -1*/) {
			if (nLen == -1) {
				m_str.append(str);
			}
			else {
				m_str.append(str, nLen);
			}
		}

		void StringA::Assign(const char* pstr, int cchMax) {
			if (cchMax == -1)
				m_str.assign(pstr);
			else
				m_str.assign(cchMax, cchMax);
		}

		void StringA::Assign(const StringA & str) {
			m_str.assign(str.m_str);
		}

		void StringA::TrimLeft() {
			m_str.erase(m_str.begin(), std::find_if(m_str.begin(), m_str.end(), [](int ch) {
				return !std::isspace(ch);
			}));
		}

		void StringA::TrimRight() {
			m_str.erase(std::find_if(m_str.rbegin(), m_str.rend(), [](int ch) {
				return !std::isspace(ch);
			}).base(), m_str.end());
		}

		void StringA::Trim() {
			TrimLeft();
			TrimRight();
		}

		bool StringA::IsEmpty() const {
			return m_str.size() == 0;
		}

		void StringA::Empty() {
			m_str.clear();
		}

		const char* StringA::GetDataPointer() const {
			return m_str.c_str();
		}

		std::string StringA::GetData() const {
			return m_str;
		}

		char StringA::GetAt(int nIndex) const {
			PPX_ASSERT(nIndex >= 0);
			return m_str[nIndex];
		}

		char StringA::operator[] (int nIndex) const {
			PPX_ASSERT(nIndex >= 0);
			return m_str[nIndex];
		}

		const StringA &StringA::operator=(const StringA &src) {
			Assign(src);
			return *this;
		}

        StringA operator+(const StringA &src1, const StringA &src2) {
            StringA sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        StringA operator+(const StringA &src1, const std::string &src2) {
            StringA sTemp = src1;
            sTemp.Append(src2.c_str());
            return sTemp;
        }

        StringA operator+(const StringA &src1, const char* src2) {
            StringA sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        StringA operator+(const char* src1, const StringA &src2) {
            StringA sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        StringA operator+(const std::string &src1, const StringA &src2) {
            StringA sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

		const StringA &StringA::operator+=(const StringA &src) {
			Append(src);
			return *this;
		}

        const StringA &StringA::operator+=(const std::string &src) {
            Append(src);
            return *this;
        }

        const StringA &StringA::operator+=(const char* src) {
            Append(src);
            return *this;
        }

        bool StringA::operator == (const StringA & str) const {
			return (Compare(str) == 0);
		}

		bool StringA::operator != (const StringA & str) const {
			return (Compare(str) != 0);
		}

		bool StringA::operator <= (const StringA & str) const {
			return (Compare(str) <= 0);
		}

		bool StringA::operator <  (const StringA & str) const {
			return (Compare(str) < 0);
		}

		bool StringA::operator >= (const StringA & str) const {
			return (Compare(str) >= 0);
		}

		bool StringA::operator >  (const StringA & str) const {
			return (Compare(str) > 0);
		}

		void StringA::SetAt(int nIndex, char ch) {
			if (nIndex >= 0 && (GetLength() == 0 || nIndex < GetLength()))
				m_str[nIndex] = ch;
		}

		int StringA::Compare(const StringA & str) const {
			return strcmp(m_str.c_str(), str.m_str.c_str());
		}

		int StringA::CompareNoCase(const StringA & str) const {
			return stricmp(m_str.c_str(), str.m_str.c_str());
		}

		void StringA::MakeUpper() {
			std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToUpperA);
		}

		void StringA::MakeLower() {
			std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToLowerA);
		}

		StringA StringA::Left(int iLength) const {
			if (iLength < 0)
				iLength = 0;

			if (iLength > GetLength())
				iLength = GetLength();

			return m_str.substr(0, iLength);
		}

		StringA StringA::Mid(int iPos, int iLength) const {
			if (iLength < 0) 
				iLength = GetLength() - iPos;

			if (iPos + iLength > GetLength())
				iLength = GetLength() - iPos;

			if (iLength <= 0) 
				return StringA();

			return m_str.substr(iPos, iLength);
		}

		StringA StringA::Right(int iLength) const {
			int iPos = GetLength() - iLength;

			if (iPos < 0) {
				iPos = 0;
				iLength = GetLength();
			}

			return m_str.substr(iPos, iLength);
		}

		int StringA::Find(char ch, int iOffset /*= 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;

			size_t ret = m_str.find_first_of(ch, iOffset);
			if (ret == std::string::npos)
				return -1;

			return ret;
		}

		int StringA::Find(const StringA & strSub, int iOffset /*= 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;

			const char* p = strstr(m_str.c_str() + iOffset, strSub.GetDataPointer());

			if (p == NULL) return -1;

			return (int)(p - m_str.c_str());
		}

		int StringA::ReverseFind(char ch, int iOffset /* = 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;
			size_t ret = m_str.find_last_of(ch, GetLength() - iOffset - 1);
			if (ret == std::string::npos)
				return -1;

			return ret;
		}

		int StringA::ReverseFind(const StringA & str, int iOffset /*= 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;

			int index = GetLength() - 1 - iOffset;
			int findStrLen = str.GetLength();
			int searchStart = -1;

			for (;index >= 0; index--) {
				searchStart = index - findStrLen + 1;
				if (searchStart > 0) {
					if (strncmp(GetDataPointer() + searchStart, str.GetDataPointer(), findStrLen) == 0) {
						return searchStart;
					}
				}
			}

			return -1;
		}

		int StringA::Replace(const StringA & strFrom, const StringA & strTo) {
			StringA sTemp;
			int nCount = 0;
			int iPos = Find(strFrom);

			if (iPos < 0) 
				return 0;

			int cchFrom = (int)strlen(strFrom.GetDataPointer());
			int cchTo = (int)strlen(strTo.GetDataPointer());

			while (iPos >= 0) {
				sTemp = Left(iPos);
				sTemp += strTo;
				sTemp += Mid(iPos + cchFrom);
				Assign(sTemp);
				iPos = Find(strFrom, iPos + cchTo);
				nCount++;
			}
			
			return nCount;
		}

		int StringA::Format(const char* pstrFormat, ...) {
			int iBufLen = 0;
			va_list Args;

			va_start(Args, pstrFormat);
			char* pBuf = Internal::InnerMakeStringA(iBufLen, 512, pstrFormat, Args);
			Assign(pBuf);
			va_end(Args);

			if (pBuf) {
				free(pBuf);
				pBuf = NULL;
			}

			return iBufLen;
		}

		std::vector<ppx::base::StringA> StringA::Split(const StringA& delimiter) const {
			std::vector<ppx::base::StringA> fields;
			StringA src = m_str;
			int pos = src.Find(delimiter, 0);

			while (pos >= 0) {
				StringA t = src.Left(pos);
				fields.push_back(t);
				src = src.Right(src.GetLength() - pos - delimiter.GetLength());
				pos = src.Find(delimiter);
			}

			fields.push_back(src);
			return fields;
		}

		void StringA::JoinFrom(const std::vector<StringA>& source, const StringA& delimiter) {
			StringA joined_string;
			for (size_t i = 0; i < source.size(); ++i) {
				if (i != 0) {
					joined_string += delimiter;
				}

				joined_string += source[i];
			}

			m_str = joined_string.m_str;
		}

        StringA::operator std::string() const {
            return m_str;
        }



        StringW::StringW() {
        }

        StringW::StringW(const wchar_t ch) {
            m_str = ch;
        }

        StringW::StringW(const StringW &src) {
            m_str = src.m_str;
        }

        StringW::StringW(const std::wstring &src) {
            m_str = src;
        }

        StringW::StringW(const wchar_t* lpsz, int nLen /*= -1*/) {
            m_str.clear();
            if ( !lpsz )
                return;

            if ( nLen != -1 )
                m_str.assign(lpsz, nLen);
            else
                m_str = lpsz;
        }

        StringW::~StringW() {
            m_str.clear();
        }

        int StringW::GetLength() const {
            return m_str.size();
        }

        void StringW::Append(const StringW & str) {
            m_str.append(str.m_str);
        }

        void StringW::Append(const wchar_t* str, int nLen /*= -1*/) {
            if ( nLen == -1 ) {
                m_str.append(str);
            }
            else {
                m_str.append(str, nLen);
            }
        }

        void StringW::Assign(const wchar_t* pstr, int cchMax) {
            if ( cchMax == -1 )
                m_str.assign(pstr);
            else
                m_str.assign(cchMax, cchMax);
        }

        void StringW::Assign(const StringW & str) {
            m_str.assign(str.m_str);
        }

        void StringW::TrimLeft() {
            m_str.erase(m_str.begin(), std::find_if(m_str.begin(), m_str.end(), [] (int ch) {
                return !std::isspace(ch);
            }));
        }

        void StringW::TrimRight() {
            m_str.erase(std::find_if(m_str.rbegin(), m_str.rend(), [] (int ch) {
                return !std::isspace(ch);
            }).base(), m_str.end());
        }

        void StringW::Trim() {
            TrimLeft();
            TrimRight();
        }

        bool StringW::IsEmpty() const {
            return m_str.size() == 0;
        }

        void StringW::Empty() {
            m_str.clear();
        }

        const wchar_t* StringW::GetDataPointer() const {
            return m_str.c_str();
        }

        std::wstring StringW::GetData() const {
            return m_str;
        }

        wchar_t StringW::GetAt(int nIndex) const {
            PPX_ASSERT(nIndex >= 0);
            return m_str[nIndex];
        }

        wchar_t StringW::operator[] (int nIndex) const {
            PPX_ASSERT(nIndex >= 0);
            return m_str[nIndex];
        }

        const StringW &StringW::operator=(const StringW &src) {
            Assign(src);
            return *this;
        }

        StringW operator+(const StringW &src1, const StringW &src2) {
            StringW sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        StringW operator+(const StringW &src1, const std::wstring &src2) {
            StringW sTemp = src1;
            sTemp.Append(src2.c_str());
            return sTemp;
        }

        StringW operator+(const StringW &src1, const wchar_t* src2) {
            StringW sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        StringW operator+(const wchar_t* src1, const StringW &src2) {
            StringW sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        StringW operator+(const std::wstring &src1, const StringW &src2) {
            StringW sTemp = src1;
            sTemp.Append(src2);
            return sTemp;
        }

        const StringW &StringW::operator+=(const StringW &src) {
            Append(src);
            return *this;
        }

        const StringW &StringW::operator+=(const std::wstring &src) {
            Append(src);
            return *this;
        }

        const StringW &StringW::operator+=(const wchar_t* src) {
            Append(src);
            return *this;
        }

        bool StringW::operator == (const StringW & str) const {
            return (Compare(str) == 0);
        }

        bool StringW::operator != (const StringW & str) const {
            return (Compare(str) != 0);
        }

        bool StringW::operator <= (const StringW & str) const {
            return (Compare(str) <= 0);
        }

        bool StringW::operator <  (const StringW & str) const {
            return (Compare(str) < 0);
        }

        bool StringW::operator >= (const StringW & str) const {
            return (Compare(str) >= 0);
        }

        bool StringW::operator >  (const StringW & str) const {
            return (Compare(str) > 0);
        }

        void StringW::SetAt(int nIndex, wchar_t ch) {
            if ( nIndex >= 0 && (GetLength() == 0 || nIndex < GetLength()) )
                m_str[nIndex] = ch;
        }

        int StringW::Compare(const StringW & str) const {
            return wcscmp(m_str.c_str(), str.m_str.c_str());
        }

        int StringW::CompareNoCase(const StringW & str) const {
            return wcsicmp(m_str.c_str(), str.m_str.c_str());
        }

        void StringW::MakeUpper() {
            std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToUpperW);
        }

        void StringW::MakeLower() {
            std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToLowerW);
        }

        StringW StringW::Left(int iLength) const {
            if ( iLength < 0 )
                iLength = 0;

            if ( iLength > GetLength() )
                iLength = GetLength();

            return m_str.substr(0, iLength);
        }

        StringW StringW::Mid(int iPos, int iLength) const {
            if ( iLength < 0 )
                iLength = GetLength() - iPos;

            if ( iPos + iLength > GetLength() )
                iLength = GetLength() - iPos;

            if ( iLength <= 0 )
                return StringW();

            return m_str.substr(iPos, iLength);
        }

        StringW StringW::Right(int iLength) const {
            int iPos = GetLength() - iLength;

            if ( iPos < 0 ) {
                iPos = 0;
                iLength = GetLength();
            }

            return m_str.substr(iPos, iLength);
        }

        int StringW::Find(wchar_t ch, int iOffset /*= 0*/) const {
            if ( iOffset < 0 || iOffset > GetLength() )
                return -1;

            size_t ret = m_str.find_first_of(ch, iOffset);
            if ( ret == std::wstring::npos )
                return -1;

            return ret;
        }

        int StringW::Find(const StringW & strSub, int iOffset /*= 0*/) const {
            if ( iOffset < 0 || iOffset > GetLength() )
                return -1;

            const wchar_t* p = wcsstr(m_str.c_str() + iOffset, strSub.GetDataPointer());

            if ( p == NULL ) return -1;

            return (int)(p - m_str.c_str());
        }

        int StringW::ReverseFind(wchar_t ch, int iOffset /* = 0*/) const {
            if ( iOffset < 0 || iOffset > GetLength() )
                return -1;
            size_t ret = m_str.find_last_of(ch, GetLength() - iOffset - 1);
            if ( ret == std::wstring::npos )
                return -1;

            return ret;
        }

        int StringW::ReverseFind(const StringW & str, int iOffset /*= 0*/) const {
            if ( iOffset < 0 || iOffset > GetLength() )
                return -1;

            int index = GetLength() - 1 - iOffset;
            int findStrLen = str.GetLength();
            int searchStart = -1;

            for ( ; index >= 0; index-- ) {
                searchStart = index - findStrLen + 1;
                if ( searchStart > 0 ) {
                    if ( wcsncmp(GetDataPointer() + searchStart, str.GetDataPointer(), findStrLen) == 0 ) {
                        return searchStart;
                    }
                }
            }

            return -1;
        }

        int StringW::Replace(const StringW & strFrom, const StringW & strTo) {
            StringW sTemp;
            int nCount = 0;
            int iPos = Find(strFrom);

            if ( iPos < 0 )
                return 0;

            int cchFrom = (int)wcslen(strFrom.GetDataPointer());
            int cchTo = (int)wcslen(strTo.GetDataPointer());

            while ( iPos >= 0 ) {
                sTemp = Left(iPos);
                sTemp += strTo;
                sTemp += Mid(iPos + cchFrom);
                Assign(sTemp);
                iPos = Find(strFrom, iPos + cchTo);
                nCount++;
            }

            return nCount;
        }

        int StringW::Format(const wchar_t* pstrFormat, ...) {
            int iBufLen = 0;
            va_list Args;

            va_start(Args, pstrFormat);
            wchar_t* pBuf = Internal::InnerMakeStringW(iBufLen, 512, pstrFormat, Args);
            Assign(pBuf);
            va_end(Args);

            if ( pBuf ) {
                free(pBuf);
                pBuf = NULL;
            }

            return iBufLen;
        }

        std::vector<ppx::base::StringW> StringW::Split(const StringW& delimiter) const {
            std::vector<ppx::base::StringW> fields;
            StringW src = m_str;
            int pos = src.Find(delimiter, 0);

            while ( pos >= 0 ) {
                StringW t = src.Left(pos);
                fields.push_back(t);
                src = src.Right(src.GetLength() - pos - delimiter.GetLength());
                pos = src.Find(delimiter);
            }

            fields.push_back(src);
            return fields;
        }

        void StringW::JoinFrom(const std::vector<StringW>& source, const StringW& delimiter) {
            StringW joined_string;
            for ( size_t i = 0; i < source.size(); ++i ) {
                if ( i != 0 ) {
                    joined_string += delimiter;
                }

                joined_string += source[i];
            }

            m_str = joined_string.m_str;
        }

        StringW::operator std::wstring() const {
            return m_str;
        }


		StringA MakeStringA(const char* pstrFormat, ...) {
            StringA strResult;
            if (!pstrFormat)
                return strResult;

            char *pMsgBuffer = NULL;
            int iMsgBufCount = 0;

            va_list arglist;
            va_start(arglist, pstrFormat);
            pMsgBuffer = Internal::InnerMakeStringA(iMsgBufCount, 512, pstrFormat, arglist);
            va_end(arglist);

            if (pMsgBuffer) {
                strResult.Assign(pMsgBuffer);
            }

            if (pMsgBuffer) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            return strResult;
        }

        StringW MakeStringW(const wchar_t* pstrFormat, ...) {
            StringW strResult;
            if ( !pstrFormat )
                return strResult;

            wchar_t *pMsgBuffer = NULL;
            int iMsgBufCount = 0;

            va_list arglist;
            va_start(arglist, pstrFormat);
            pMsgBuffer = Internal::InnerMakeStringW(iMsgBufCount, 512, pstrFormat, arglist);
            va_end(arglist);

            if ( pMsgBuffer ) {
                strResult.Assign(pMsgBuffer);
            }

            if ( pMsgBuffer ) {
                free(pMsgBuffer);
                pMsgBuffer = NULL;
            }

            return strResult;
        }

        size_t StringSplitA(const StringA& source, const StringA& delimiter, std::vector<StringA>& fields) {
			fields.clear();
			StringA src = source;
			int pos = src.Find(delimiter, 0);

			while (pos >= 0) {
				StringA t = src.Left(pos);
				fields.push_back(t);
				src = src.Right(src.GetLength() - pos - delimiter.GetLength());
				pos = src.Find(delimiter);
			}

			fields.push_back(src);
			return fields.size();
		}

        size_t StringSplitW(const StringW& source, const StringW& delimiter, std::vector<StringW>& fields) {
            fields.clear();
            StringW src = source;
            int pos = src.Find(delimiter, 0);

            while ( pos >= 0 ) {
                StringW t = src.Left(pos);
                fields.push_back(t);
                src = src.Right(src.GetLength() - pos - delimiter.GetLength());
                pos = src.Find(delimiter);
            }

            fields.push_back(src);
            return fields.size();
        }

		StringA StringJoinA(const std::vector<StringA>& source, const StringA& delimiter) {
			StringA joined_string;
			for (size_t i = 0; i < source.size(); ++i) {
				if (i != 0) {
					joined_string += delimiter;
				}

				joined_string += source[i];
			}

			return joined_string;
		}

        StringW StringJoinW(const std::vector<StringW>& source, const StringW& delimiter) {
            StringW joined_string;
            for ( size_t i = 0; i < source.size(); ++i ) {
                if ( i != 0 ) {
                    joined_string += delimiter;
                }

                joined_string += source[i];
            }

            return joined_string;
        }
	}
}