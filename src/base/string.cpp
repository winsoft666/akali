#include "base/string.h"
#include "base/stringencode.h"
#include "base/assert.h"
#include <tchar.h>
#include <strsafe.h>
#include <algorithm>
#include <cctype>
#include <locale>

namespace ppx {
    namespace base {
		namespace Internal {
			static LPTSTR InnerMakeString(int &iBufLen, int iBufSizeStep, LPCTSTR pstrFormat, va_list Args) {
				iBufLen = 0;
				if (!pstrFormat)
					return NULL;

				TCHAR *pMsgBuffer = NULL;
				HRESULT hr = STRSAFE_E_INSUFFICIENT_BUFFER;

				while (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
					iBufLen += iBufSizeStep;
					if (pMsgBuffer) {
						free(pMsgBuffer);
						pMsgBuffer = NULL;
					}

					pMsgBuffer = (TCHAR *)malloc(iBufLen * sizeof(TCHAR));
					if (!pMsgBuffer) {
						iBufLen = 0;
						return NULL;
					}

					hr = StringCchVPrintf(pMsgBuffer, iBufLen, pstrFormat, Args);
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


		String::String() {
		}

		String::String(const TCHAR ch) {
			m_str = ch;
		}

		String::String(LPCTSTR lpsz, int nLen) {
			m_str.clear();
			if (!lpsz)
				return;
			if (nLen != -1)
				m_str.assign(lpsz, nLen);
			else
				m_str = lpsz;
		}

		String::String(const String &src) {
			m_str = src.m_str;
		}

		String::String(const tstring &src) {
			m_str = src;
		}

#ifdef _UNICODE
		String::String(const char* lpsz, int nLen /*= -1*/) {
			m_str.clear();
			if (!lpsz)
				return;

			std::string str;
			if (nLen != -1)
				str.assign(lpsz, nLen);
			else
				str = lpsz;

			m_str = AnsiToTCHAR(str);
		}

		String::String(const std::string &src) {
			m_str = AnsiToTCHAR(src);
		}
#else
		String::String(const wchar_t* lpsz, int nLen /*= -1*/) {
			m_str.clear();
			if (!lpsz)
				return;

			std::wstring str;
			if (nLen != -1)
				str.assign(lpsz, nLen);
			else
				str = lpsz;

			m_str = UnicodeToTCHAR(str);
		}

		String::String(const std::wstring &src) {
			m_str = UnicodeToTCHAR(src);
		}
#endif

		String::~String() {
			m_str.clear();
		}

		int String::GetLength() const {
			return m_str.size();
		}

		String::operator tstring() const {
			return m_str;
		}

		void String::Append(const String & str) {
			m_str.append(str.m_str);
		}

		void String::Assign(LPCTSTR pstr, int cchMax) {
			if (cchMax == -1)
				m_str.assign(pstr);
			else
				m_str.assign(cchMax, cchMax);
		}

		void String::Assign(const String & str) {
			m_str.assign(str.m_str);
		}

		void String::TrimLeft() {
			m_str.erase(m_str.begin(), std::find_if(m_str.begin(), m_str.end(), [](int ch) {
				return !std::isspace(ch);
			}));
		}

		void String::TrimRight() {
			m_str.erase(std::find_if(m_str.rbegin(), m_str.rend(), [](int ch) {
				return !std::isspace(ch);
			}).base(), m_str.end());
		}

		void String::Trim() {
			TrimLeft();
			TrimRight();
		}

		bool String::IsEmpty() const {
			return m_str.size() == 0;
		}

		void String::Empty() {
			m_str.clear();
		}

		LPCTSTR String::GetDataPointer() const {
			return m_str.c_str();
		}

		tstring String::GetData() const {
			return m_str;
		}

		TCHAR String::GetAt(int nIndex) const {
			PPX_ASSERT(nIndex >= 0);
			return m_str[nIndex];
		}

		TCHAR String::operator[] (int nIndex) const {
			PPX_ASSERT(nIndex >= 0);
			return m_str[nIndex];
		}

		const String &String::operator=(const String &src) {
			Assign(src);
			return *this;
		}

		const String &String::operator=(LPCTSTR lpStr) {
			if (lpStr) {
				m_str.clear();
				m_str = lpStr;
			}
			else {
				Empty();
			}

			return *this;
		}

#ifdef _UNICODE

		const String &String::operator=(LPCSTR lpStr) {
			if (lpStr) {
				m_str = AnsiToUnicode(lpStr);
			}
			else {
				Empty();
			}

			return *this;
		}

		const String &String::operator+=(LPCSTR lpStr) {
			if (lpStr) {
				m_str += AnsiToUnicode(lpStr);
			}

			return *this;
		}

#else

		const String &String::operator=(LPCWSTR lpwStr) {
			if (lpwStr) {
				m_str = UnicodeToAnsi(lpStr);
			}
			else {
				Empty();
			}

			return *this;
		}

		const String &String::operator+=(LPCWSTR lpwStr) {
			if (lpwStr) {
				m_str += UnicodeToAnsi(lpStr);
			}

			return *this;
		}

#endif // _UNICODE

		const String &String::operator=(const TCHAR ch) {
			m_str.clear();
			m_str = ch;
			return *this;
		}

		String String::operator+(const String &src) const {
			String sTemp = *this;
			sTemp.Append(src);
			return sTemp;
		}

		String String::operator+(LPCTSTR lpStr) const {
			if (lpStr) {
				String sTemp = *this;
				sTemp.Append(lpStr);
				return sTemp;
			}

			return *this;
		}

		const String &String::operator+=(const String &src) {
			Append(src);
			return *this;
		}

		const String &String::operator+=(LPCTSTR lpStr) {
			if (lpStr) {
				Append(lpStr);
			}

			return *this;
		}

		const String &String::operator+=(const TCHAR ch) {
			TCHAR str[] = { ch, '\0' };
			Append(str);
			return *this;
		}

		bool String::operator == (const String & str) const {
			return (Compare(str) == 0);
		}

		bool String::operator != (const String & str) const {
			return (Compare(str) != 0);
		}

		bool String::operator <= (const String & str) const {
			return (Compare(str) <= 0);
		}

		bool String::operator <  (const String & str) const {
			return (Compare(str) < 0);
		}

		bool String::operator >= (const String & str) const {
			return (Compare(str) >= 0);
		}

		bool String::operator >  (const String & str) const {
			return (Compare(str) > 0);
		}

		void String::SetAt(int nIndex, TCHAR ch) {
			if (nIndex >= 0 && (GetLength() == 0 || nIndex < GetLength()))
				m_str[nIndex] = ch;
		}

		int String::Compare(const String & str) const {
			return _tcscmp(m_str.c_str(), str.m_str.c_str());
		}

		int String::CompareNoCase(const String & str) const {
			return _tcsicmp(m_str.c_str(), str.m_str.c_str());
		}

		void String::MakeUpper() {
#ifdef _UNICODE
			std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToUpperW);
#else
			std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToUpperA);
#endif
		}

		void String::MakeLower() {
#ifdef _UNICODE
			std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToLowerW);
#else
			std::transform(m_str.begin(), m_str.end(), m_str.begin(), Internal::EasyToLowerA);
#endif
		}

		String String::Left(int iLength) const {
			if (iLength < 0)
				iLength = 0;

			if (iLength > GetLength())
				iLength = GetLength();

			return m_str.substr(0, iLength);
		}

		String String::Mid(int iPos, int iLength) const {
			if (iLength < 0) 
				iLength = GetLength() - iPos;

			if (iPos + iLength > GetLength())
				iLength = GetLength() - iPos;

			if (iLength <= 0) 
				return String();

			return m_str.substr(iPos, iLength);
		}

		String String::Right(int iLength) const {
			int iPos = GetLength() - iLength;

			if (iPos < 0) {
				iPos = 0;
				iLength = GetLength();
			}

			return m_str.substr(iPos, iLength);
		}

		int String::Find(TCHAR ch, int iOffset /*= 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;

			size_t ret = m_str.find_first_of(ch, iOffset);
			if (ret == tstring::npos)
				return -1;

			return ret;
		}

		int String::Find(const String & strSub, int iOffset /*= 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;

			LPCTSTR p = _tcsstr(m_str.c_str() + iOffset, strSub.GetDataPointer());

			if (p == NULL) return -1;

			return (int)(p - m_str.c_str());
		}

		int String::ReverseFind(TCHAR ch, int iOffset /* = 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;
			size_t ret = m_str.find_last_of(ch, GetLength() - iOffset - 1);
			if (ret == tstring::npos)
				return -1;

			return ret;
		}

		int String::ReverseFind(const String & str, int iOffset /*= 0*/) const {
			if (iOffset < 0 || iOffset > GetLength())
				return -1;

			int index = GetLength() - 1 - iOffset;
			int findStrLen = str.GetLength();
			int searchStart = -1;

			for (;index >= 0; index--) {
				searchStart = index - findStrLen + 1;
				if (searchStart > 0) {
					if (_tcsncmp(GetDataPointer() + searchStart, str.GetDataPointer(), findStrLen) == 0) {
						return searchStart;
					}
				}
			}

			return -1;
		}

		int String::Replace(const String & strFrom, const String & strTo) {
			String sTemp;
			int nCount = 0;
			int iPos = Find(strFrom);

			if (iPos < 0) 
				return 0;

			int cchFrom = (int)_tcslen(strFrom.GetDataPointer());
			int cchTo = (int)_tcslen(strTo.GetDataPointer());

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

		int String::Format(LPCTSTR pstrFormat, ...) {
			int iBufLen = 0;
			va_list Args;

			va_start(Args, pstrFormat);
			LPTSTR pBuf = Internal::InnerMakeString(iBufLen, 512, pstrFormat, Args);
			Assign(pBuf);
			va_end(Args);

			if (pBuf) {
				free(pBuf);
				pBuf = NULL;
			}

			return iBufLen;
		}



		std::vector<ppx::base::String> String::Split(const String& delimiter) const {
			std::vector<ppx::base::String> fields;
			String src = m_str;
			int pos = src.Find(delimiter, 0);

			while (pos >= 0) {
				String t = src.Left(pos);
				fields.push_back(t);
				src = src.Right(src.GetLength() - pos - delimiter.GetLength());
				pos = src.Find(delimiter);
			}

			fields.push_back(src);
			return fields;
		}

		void String::JoinFrom(const std::vector<String>& source, const String& delimiter) {
			String joined_string;
			for (size_t i = 0; i < source.size(); ++i) {
				if (i != 0) {
					joined_string += delimiter;
				}

				joined_string += source[i];
			}

			m_str = joined_string.m_str;
		}

		String MakeString(LPCTSTR pstrFormat, ...) {
            String strResult;
            if (!pstrFormat)
                return strResult;

            TCHAR *pMsgBuffer = NULL;
            int iMsgBufCount = 0;

            va_list arglist;
            va_start(arglist, pstrFormat);
            pMsgBuffer = Internal::InnerMakeString(iMsgBufCount, 512, pstrFormat, arglist);
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

		size_t StringSplit(const String& source, const String& delimiter, std::vector<String>& fields) {
			fields.clear();
			String src = source;
			int pos = src.Find(delimiter, 0);

			while (pos >= 0) {
				String t = src.Left(pos);
				fields.push_back(t);
				src = src.Right(src.GetLength() - pos - delimiter.GetLength());
				pos = src.Find(delimiter);
			}

			fields.push_back(src);
			return fields.size();
		}

		String StringJoin(const std::vector<String>& source, const String& delimiter) {
			String joined_string;
			for (size_t i = 0; i < source.size(); ++i) {
				if (i != 0) {
					joined_string += delimiter;
				}

				joined_string += source[i];
			}

			return joined_string;
		}

	}
}