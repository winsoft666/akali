#include "ppx_base.h"
using namespace ppx;
#include "gtest/gtest.h"

TEST(ppxbase, string)
{
	std::string str = "AAbbCCc≤‚ ‘";
	str = base::StringToLower(str);
	EXPECT_EQ(str, "aabbccc≤‚ ‘");
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
	EXPECT_TRUE(CreateOrUpdateShortcutLink(L"D:\\≤‚ ‘.lnk", sp, SHORTCUT_CREATE_ALWAYS));

	std::wstring target_path;
	std::wstring args;
	EXPECT_TRUE(ResolveShortcut(L"D:\\≤‚ ‘.lnk", target_path, args));
	EXPECT_EQ(target_path, L"D:\\test.exe");
	EXPECT_EQ(args, L"this is arguments");
}

TEST(ppxbase, timer) 
{
	using namespace ppx::base;
	Timer timer([]() {
		std::cout << "timer tick\n";
	});
	EXPECT_TRUE(timer.Start(1000, FALSE, FALSE));
}

int main(int argc, char *argv[]) {
	::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
