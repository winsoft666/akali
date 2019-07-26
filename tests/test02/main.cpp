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
	bool b = ppx::base::RegKey::DeleteSubKeys(HKEY_LOCAL_MACHINE, TEXT("Software\\V"), false);
}

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);

    RUN_ALL_TESTS();

	getchar();
	return 0;
}
