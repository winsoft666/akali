#include "ppx_base.h"
using namespace ppx;
#include "gtest/gtest.h"

TEST(ppxbase, stringtest)
{
	std::string str = "AAbbCCc测试";
	str = base::StringToLower(str);
	EXPECT_EQ(str, "aabbccc测试");
}

TEST(ppxbase, shortcut)
{
	using namespace ppx::base;
	ScopedCOMInitializer com;
	ShortcutProperties sp;
	sp.SetArguments(L"this is arguments");
	sp.SetDescription(L"this is description");
	sp.SetTarget(L"D:\\test.exe");
	sp.SetIcon(L"D:\\test.exe", 0);
	EXPECT_TRUE(CreateOrUpdateShortcutLink(L"D:\\测试.lnk", sp, SHORTCUT_CREATE_ALWAYS));

	ShortcutProperties sp2;
	EXPECT_TRUE(ResolveShortcut(L"D:\\测试.lnk", sp2));
	EXPECT_EQ(sp2.target, L"D:\\test.exe");
	EXPECT_EQ(sp2.arguments, L"this is arguments");
	EXPECT_EQ(sp2.icon, L"D:\\test.exe");
	EXPECT_EQ(sp2.icon_index, 0);
}


TEST(ppxbase, Screenshot) {
	using namespace ppx::base;
	EXPECT_TRUE(Screenshot::FullScreenToFile(L"D:\\fullscreen.bmp"));
}

TEST(ppxbase, RegisterTest) {
	std::vector<std::wstring> subkeys;
	ppx::base::RegKey reg(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\NET Framework Setup\\NDP\\");
	if (reg.Open(KEY_READ | KEY_WOW64_64KEY, FALSE) == S_OK) {
		reg.GetSubKeys(subkeys);
	}
}
TEST(ppxbase, Logging) {
	PPX_LOG(LS_INFO) << "test";
}

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);

    RUN_ALL_TESTS();

	getchar();
	return 0;
}
