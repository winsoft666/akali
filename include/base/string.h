#ifndef PPX_BASE_STRING_H__
#define PPX_BASE_STRING_H__
#pragma once

#include "ppx_export.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string>
#include <vector>



namespace ppx {
    namespace base {
#if (defined UNICODE)
		typedef std::wstring tstring;
#else
		typedef std::string tstring;
#endif
		class PPX_API String {
		public:
			String();
			String(const TCHAR ch);
			String(const String &src);
			String(const std::string &src);
			String(const std::wstring &src);
			String(const char* lpsz, int nLen = -1);
			String(const wchar_t* lpsz, int nLen = -1);
			virtual ~String();

			void Empty();
			int GetLength() const;
			bool IsEmpty() const;
			TCHAR GetAt(int nIndex) const;
			void Append(const String & str);
			void Append(const char* str, int nLen = -1);
			void Append(const wchar_t* str, int nLen = -1);
			void Assign(LPCTSTR pstr, int nLength = -1);
			void Assign(const String & str);
			void TrimLeft();
			void TrimRight();
			void Trim();
			LPCTSTR GetDataPointer() const;
			std::string GetDataA() const;
			std::wstring GetDataW() const;
			tstring GetData() const;

			void SetAt(int nIndex, TCHAR ch);
			operator tstring() const;

			TCHAR operator[] (int nIndex) const;

			const String &operator=(const String &src);

			String operator+(const String &src) const;

			const String &operator+=(const String &src);
			const String &operator+=(const std::string &src);
			const String &operator+=(const std::wstring &src);
			const String &operator+=(const char* src);
			const String &operator+=(const wchar_t* src);

			bool operator == (const String &str) const;
			bool operator != (const String &str) const;
			bool operator <= (const String &str) const;
			bool operator <  (const String &str) const;
			bool operator >= (const String &str) const;
			bool operator >  (const String &str) const;

			int Compare(const String & str) const;
			int CompareNoCase(const String & str) const;

			void MakeUpper();
			void MakeLower();

			String Left(int nLength) const;
			String Mid(int iPos, int nLength = -1) const;
			String Right(int nLength) const;

			int Find(TCHAR ch, int iOffset = 0) const;
			int Find(const String & str, int iOffset = 0) const;
			int ReverseFind(TCHAR ch, int iOffset = 0) const;
			int ReverseFind(const String & str, int iOffset = 0) const;

			int Replace(const String &strFrom, const String & strTo);

			int Format(LPCTSTR pstrFormat, ...);

			std::vector<String> Split(const String& delimiter) const;

			void JoinFrom(const std::vector<String>& source, const String& delimiter);

			// Convert arbitrary values to/from a string.
			//
			template <class T>
			bool FromType(const T &t) {
#ifdef _UNICODE
				std::basic_ostringstream<wchar_t> oss;
#else
				std::basic_ostringstream<char> oss;
#endif
				oss << std::boolalpha << t;

				bool ret = !oss.fail();
				if(ret)
					m_str = oss.str();
				return ret;
			}

			template <class T>
			bool ToType(T* t) {
				PPX_ASSERT(t);
#ifdef _UNICODE
				std::basic_istringstream<wchar_t> iss(m_str);
#else
				std::basic_istringstream<char> iss(m_str);
#endif
				iss >> std::boolalpha >> *t;
				bool ret = !iss.fail();
				return ret;
			}
		protected:
			class StringImpl;
			StringImpl* m_pImpl;
		};

        PPX_API String MakeString(LPCTSTR pstrFormat, ...);

		// Splits the source string into multiple fields separated by delimiter, with duplicates of delimiter creating empty fields.
		PPX_API size_t StringSplit(const String& source, const String& delimiter, std::vector<String>& fields);

		// Joins the source vector of strings into a single string, with each field in source being separated by delimiter. 
		// No trailing delimiter is added.
		PPX_API String StringJoin(const std::vector<String>& source, const String& delimiter);

		// Convert arbitrary values to/from a string.
		//
		template <class T>
		bool StringFromType(const T &t, String &str) {
#ifdef _UNICODE
			std::basic_ostringstream<wchar_t> oss;
#else
			std::basic_ostringstream<char> oss;
#endif
			oss << std::boolalpha << t;
			bool ret = !oss.fail();
			if (ret)
				str = oss.str();
			return ret;
		}

		template <class T>
		String StringFromType(const T &t, const String &strDefault = TEXT("")) {
			String str;
			if (!StringFromType(t, str))
				str = strDefault;
			return str;
		}

		template <class T>
		bool StringToType(const String& str, T& t) {
#ifdef _UNICODE
			std::basic_istringstream<wchar_t> iss(str.GetData());
#else
			std::basic_istringstream<char> iss(str.GetData());
#endif
			iss >> std::boolalpha >> t;
			bool ret = !iss.fail();
			return ret;
		}


    }
}

#endif