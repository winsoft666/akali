#ifndef PPX_BASE_STRING_H__
#define PPX_BASE_STRING_H__
#pragma once

#include "ppx_export.h"
#include <string>
#include <vector>

namespace ppx {
    namespace base {
		class PPX_API StringA {
		public:
			StringA();
			StringA(const char ch);
			StringA(const StringA &src);
			StringA(const std::string &src);
			StringA(const char* lpsz, int nLen = -1);
			virtual ~StringA();

			void Empty();
			int GetLength() const;
			bool IsEmpty() const;
			char GetAt(int nIndex) const;
			void Append(const StringA & str);
			void Append(const char* str, int nLen = -1);
            void Assign(const char* pstr, int cchMax = -1);
			void Assign(const StringA & str);
			void TrimLeft();
			void TrimRight();
			void Trim();
			const char* GetDataPointer() const;
            std::string GetData() const;
            operator std::string() const;

			void SetAt(int nIndex, char ch);

            char operator[] (int nIndex) const;

			const StringA &operator=(const StringA &src);

			const StringA &operator+=(const StringA &src);
			const StringA &operator+=(const std::string &src);
			const StringA &operator+=(const char* src);

			bool operator == (const StringA &str) const;
			bool operator != (const StringA &str) const;
			bool operator <= (const StringA &str) const;
			bool operator <  (const StringA &str) const;
			bool operator >= (const StringA &str) const;
			bool operator >  (const StringA &str) const;

			int Compare(const StringA & str) const;
			int CompareNoCase(const StringA & str) const;

			void MakeUpper();
			void MakeLower();

			StringA Left(int nLength) const;
			StringA Mid(int iPos, int nLength = -1) const;
			StringA Right(int nLength) const;

			int Find(char ch, int iOffset = 0) const;
			int Find(const StringA & str, int iOffset = 0) const;
			int ReverseFind(char ch, int iOffset = 0) const;
			int ReverseFind(const StringA & str, int iOffset = 0) const;

			int Replace(const StringA &strFrom, const StringA & strTo);

			int Format(const char* pstrFormat, ...);

			std::vector<StringA> Split(const StringA& delimiter) const;

			void JoinFrom(const std::vector<StringA>& source, const StringA& delimiter);

		protected:
            std::string m_str;
		};

        PPX_API StringA operator+(const StringA &src1, const StringA &src2);
        PPX_API StringA operator+(const StringA &src1, const std::string &src2);
        PPX_API StringA operator+(const StringA &src1, const char* src2);
        PPX_API StringA operator+(const char* src1, const StringA &src2);
        PPX_API StringA operator+(const std::string &src1, const StringA &src2);

        class PPX_API StringW {
        public:
            StringW();
            StringW(const wchar_t ch);
            StringW(const StringW &src);
            StringW(const std::wstring &src);
            StringW(const wchar_t* lpsz, int nLen = -1);
            virtual ~StringW();

            void Empty();
            int GetLength() const;
            bool IsEmpty() const;
            wchar_t GetAt(int nIndex) const;
            void Append(const StringW & str);
            void Append(const wchar_t* str, int nLen = -1);
            void Assign(const wchar_t* pstr, int cchMax = -1);
            void Assign(const StringW & str);
            void TrimLeft();
            void TrimRight();
            void Trim();
            const wchar_t* GetDataPointer() const;
            std::wstring GetData() const;
            operator std::wstring() const;

            void SetAt(int nIndex, wchar_t ch);

            wchar_t operator[] (int nIndex) const;

            const StringW &operator=(const StringW &src);

            const StringW &operator+=(const StringW &src);
            const StringW &operator+=(const std::wstring &src);
            const StringW &operator+=(const wchar_t* src);

            bool operator == (const StringW &str) const;
            bool operator != (const StringW &str) const;
            bool operator <= (const StringW &str) const;
            bool operator <  (const StringW &str) const;
            bool operator >= (const StringW &str) const;
            bool operator >  (const StringW &str) const;

            int Compare(const StringW & str) const;
            int CompareNoCase(const StringW & str) const;

            void MakeUpper();
            void MakeLower();

            StringW Left(int nLength) const;
            StringW Mid(int iPos, int nLength = -1) const;
            StringW Right(int nLength) const;

            int Find(wchar_t ch, int iOffset = 0) const;
            int Find(const StringW & str, int iOffset = 0) const;
            int ReverseFind(wchar_t ch, int iOffset = 0) const;
            int ReverseFind(const StringW & str, int iOffset = 0) const;

            int Replace(const StringW &strFrom, const StringW & strTo);

            int Format(const wchar_t* pstrFormat, ...);

            std::vector<StringW> Split(const StringW& delimiter) const;

            void JoinFrom(const std::vector<StringW>& source, const StringW& delimiter);

        protected:
            std::wstring m_str;
        };

        PPX_API StringW operator+(const StringW &src1, const StringW &src2);
        PPX_API StringW operator+(const StringW &src1, const std::wstring &src2);
        PPX_API StringW operator+(const StringW &src1, const wchar_t* src2);
        PPX_API StringW operator+(const wchar_t* src1, const StringW &src2);
        PPX_API StringW operator+(const std::wstring &src1, const StringW &src2);
       
#ifdef UNICODE
        typedef StringW String;
        typedef std::wstring tstring;
#else
        typedef StringA String;
        typedef std::string tstring;
#endif


        typedef StringA StringUTF8;
        typedef StringA StringANSI;
        typedef StringW StringUnicode;

        PPX_API StringA MakeStringA(const char* pstrFormat, ...);
        PPX_API StringW MakeStringW(const wchar_t* pstrFormat, ...);

		// Splits the source string into multiple fields separated by delimiter, with duplicates of delimiter creating empty fields.
		PPX_API size_t StringSplitA(const StringA& source, const StringA& delimiter, std::vector<StringA>& fields);
        PPX_API size_t StringSplitW(const StringW& source, const StringW& delimiter, std::vector<StringW>& fields);

		// Joins the source vector of strings into a single string, with each field in source being separated by delimiter. 
		// No trailing delimiter is added.
		PPX_API StringA StringJoinA(const std::vector<StringA>& source, const StringA& delimiter);
        PPX_API StringW StringJoinW(const std::vector<StringW>& source, const StringW& delimiter);

#ifdef UNICODE
#define MakeString MakeStringW
#define StringSplit StringSplitW
#define StringJoin StringJoinW
#else
#define MakeString MakeStringA
#define StringSplit StringSplitA
#define StringJoin StringJoinA
#endif
    }
}

#endif