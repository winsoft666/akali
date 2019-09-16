#include "ppx_base.h"
using namespace ppx;
#include "gtest/gtest.h"
#include <future>

TEST(ppxbase, stringtest) {
    std::string str = "AAbbCCc测试";
    str = base::StringToLower(str);
    EXPECT_EQ(str, "aabbccc测试");
}

TEST(ppxbase, shortcut) {
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

TEST(ppxbase, NetAvaliability) {
    ppx::base::ScopedCOMInitializer com;
    bool avaliable = false;
    EXPECT_TRUE(ppx::base::InternetAvailabilityChecker::InternetConnectionAvailable(avaliable));
    EXPECT_TRUE(avaliable);
}

TEST(ppxbase, Ping) {
    using namespace ppx::base;
    HostResolve hostr;
    std::vector<IPAddress> ip_list;
    EXPECT_TRUE(hostr.Resolve("store.steampowered.com", ip_list));
    EXPECT_TRUE(ip_list.size() > 0);

    ppx::base::Ping pinger(32, 3000, 3000, 128);

    for (auto item : ip_list) {
        printf("%s\n", item.ToString().c_str());
        std::vector<Ping::PingRsp> rsps;
        if (pinger.SyncPing(item, 3, rsps)) {
            for (auto r : rsps) {
                printf("%s %dms\n", r.from_ip.ToString().c_str(), r.used_time_ms);
            }
        }
    }
}

TEST(ppxbase, ProcessTest) {
    using namespace ppx;
    base::RecursiveTerminateProcess(TEXT("C:\\Users\\JEFFERY\\Desktop\\DolphinQ\\"), true);
    base::DeleteDir(L"C:\\Users\\JEFFERY\\Desktop\\DolphinQ\\");
}

//TEST(ppxbase, ConditionTask) {
//    using namespace ppx;
//
//    Concurrency::cancellation_token_source cts;
//
//    base::RunAfter(10000, cts).then([]() {
//        printf("over\n");
//    });
//
//    std::future<void> ss = std::async(std::launch::async, [cts]() {
//        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
//        cts.cancel();
//    });
//}

class A {
  public:
    A() {

    }
    void Test() {

    }
};
A a;
TEST(ppxbase, StackWalker) {
    ppx::base::StackWalker sw;
    sw.ShowCallstack();

    ppx::base::StackWalker sw2;

    sw2.ShowObject(&a);
}

TEST(ppxbase, GetParamCount) {
    base::CmdLineParser cmd_parser(GetCommandLineW());
    int c = cmd_parser.GetParamCount();

}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);

    RUN_ALL_TESTS();

    getchar();
    return 0;
}
