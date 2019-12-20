#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include "ppxbase/ipc.h"
#include "gtest/gtest.h"
#include <iostream>
#include <strsafe.h>
#include <windows.h>

using namespace ppx;

TEST(IPCTest, NoExist) {
    base::IPC ipc;
    std::string ipc_name = "no-exist";
    std::string str      = "hello";

    EXPECT_FALSE(ipc.SyncSend(ipc_name, str.c_str(), str.length()));
}

TEST(IPCTest, A) {
    base::IPC ipc;
    std::string ipc_name        = "ipc-a";
    std::string target_ipc_name = "ipc-b";
    std::string str             = "hello, I'm A ";

    ipc.StartListen(ipc_name, [](const void *data, unsigned int data_size) {
        std::string msg;
        msg.assign((char *)data, data_size);
        std::cout << "RECV: " << msg << std::endl;
    });

    int i = 0;
    while (i++ < 1000) {
        std::string msg = str + std::to_string(i);

        bool ret = ipc.SyncSend(target_ipc_name, msg.c_str(), msg.length());

        std::cout << "SEND: " << msg << " RET: " << ret << std::endl;

        Sleep(100);
    }
}

TEST(IPCTest, B) {
    base::IPC ipc;
    std::string ipc_name        = "ipc-b";
    std::string target_ipc_name = "ipc-a";
    std::string str             = "hi, I'm B ";

    ipc.StartListen(ipc_name, [](const void *data, unsigned int data_size) {
        std::string msg;
        msg.assign((char *)data, data_size);
        std::cout << "RECV: " << msg << std::endl;
    });

    int i = 0;
    while (i++ < 1000) {
        std::string msg = str + std::to_string(i);

        bool ret = ipc.SyncSend(target_ipc_name, msg.c_str(), msg.length());

        std::cout << "SEND: " << msg << " RET: " << ret << std::endl;

        Sleep(100);
    }
}

#endif