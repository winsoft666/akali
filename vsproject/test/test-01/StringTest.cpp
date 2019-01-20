#include "stdafx.h"
#include "StringTest.h"

void StringTest() {
	base::String strName = TEXT("Jim");
	std::string strValue = "123";
	
	base::String strPath = base::String(base::GetCurrentProcessDirectoryA()) + strName + base::GetCurrentProcessDirectoryA();
}
