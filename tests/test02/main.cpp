#include "ppx_base.h"
using namespace ppx;

int main() {
	do 
	{
		std::string str = "AAbbCCc≤‚ ‘";
		str = base::StringToLower(str);

		std::string str2 = base::StringToUpper(str);
	} while (false);


	do 
	{
		std::wstring wstr = L"AAbbCCc≤‚ ‘";
		wstr = base::StringToLower(wstr);

		std::wstring wstr2 = base::StringToUpper(wstr);
	} while (false);



	do 
	{
		std::string str = "Aabc≤‚a ‘12345c≤‚ ‘abc6&*^%$(";
		int pos = base::StringFind(str, "c≤‚ ‘");

		pos = base::StringFind(str, "abc", 2);

		pos = base::StringFind(str, "a", 4);

		pos = base::StringFind(str, "a", 100);
	} while (false);

	do
	{
		std::string str = "Aabc≤‚a ‘12345c≤‚ ‘abc6&*^%$(";
		int pos = base::StringFind(str, "c≤‚ ‘");

		pos = base::StringFind(str, "abc", 2);

		pos = base::StringFind(str, "a", 4);

		pos = base::StringFind(str, "a", 100);
	} while (false);

	do
	{
		std::wstring wstr = L"Aabc≤‚a ‘12345c≤‚ ‘abc6&*^%$(";
		int pos = base::StringFind(wstr, L"c≤‚ ‘");

		pos = base::StringFind(wstr, L"abc", 2);

		pos = base::StringFind(wstr, L"a", 4);

		pos = base::StringFind(wstr, L"a", 100);
	} while (false);

	do 
	{
		std::string str = "1234567890≤‚ ‘12345≤‚ ‘67890";
		int pos = base::StringReverseFind(str, "9");

		pos = base::StringReverseFind(str, "90123", 7);

		pos = base::StringReverseFind(str, "0", 1);

		pos = base::StringReverseFind(str, "≤‚ ‘", 0);

		pos = base::StringReverseFind(str, "≤‚ ‘", 6);

		pos = base::StringReverseFind(str, "≤‚ ‘", 7);
		
	} while (false);

	do 
	{
		std::string s = base::MakeString("%s≤‚ ‘abc%.2f\r\n%d", "hello", 3.1456, 1234545);
		std::wstring ws = base::MakeString(L"%s≤‚ ‘abc%.2f\r\n%d", L"hello", 3.1456, 1234545);
	} while (false);

	do 
	{
		std::vector<std::string> v;
		v = base::StringSplit("123-456-abc-897-df@#$", "-");

		v = base::StringSplit("123≤‚ ‘456≤‚ ‘abc-≤‚ ‘-897-df@#$", "≤‚ ‘");
	} while (false);

    return 0;
}
