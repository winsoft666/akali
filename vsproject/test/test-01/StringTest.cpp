#include "stdafx.h"
#include "StringTest.h"

void StringTest() {
    base::StringA str1 = "≤‚ ‘test";
    base::StringA str2 = "≤‚ ‘test";
    bool b = str1 == str2;

    std::string ss = str1;

    base::StringA str4 = base::StringA(base::GetCurrentProcessDirectoryA() + "\\") + str1 + str2;

    base::StringA str5 =  "\\" + str1 + "\\";

    str4 = str1 + str2;

    str1.MakeLower();
    str1.MakeUpper();

    b = str1 == str2;
    int i = str1.CompareNoCase(str2);

    printf("%s\n", str1.GetDataPointer());

    base::StringA str3 = base::MakeStringA("≤‚ ‘ %s \n %d", str1.GetDataPointer(), 12);
    printf("%s\n", str3.GetDataPointer());


    base::StringW wstr1 = L"123";
    base::StringW wstr2 = L"\\3" + wstr1 + L"sdsd" + wstr1;
}
