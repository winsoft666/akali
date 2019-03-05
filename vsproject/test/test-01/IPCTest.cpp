#include "stdafx.h"
#include "IPCTest.h"

static void DumpHex(const unsigned char* BinData, unsigned int BinDataSize) {
    for (unsigned int i = 0; i < BinDataSize; i++) {
        printf("%02X ", BinData[i]);
        if (i > 0 && i % 16 == 0)
            printf("\n");
    }
}


struct ClientLoginInfo {
    int mode;
    std::string order;
    std::string user;

    MSGPACK_DEFINE(mode, order, user);
};


void IPCTester::OnMsgRec(const ppx::net::IPCMsg &msg) {
    static int msg_num = 0;

    if (msg.MsgType == 1) {
        ClientLoginInfo login_info;
        ppx::net::MsgUnPack(msg.BinData, msg.BinDataSize, login_info);

        printf("%d\n%s\n%s\n", login_info.mode, login_info.order.c_str(), login_info.user.c_str());
        printf("========================\n");
        std::string str_rsp = "success";

        ppx::net::IPCMsg rsp_msg;
        ZeroMemory(&rsp_msg, sizeof(ppx::net::IPCMsg));
        rsp_msg.MsgType = 8;
        rsp_msg.IntegerData = 200;
        rsp_msg.BinData = (unsigned char *)str_rsp.c_str();
        rsp_msg.BinDataSize = str_rsp.length();
        //strcpy_s(rsp_msg.SourceIPC, 256, ipc_name_.c_str());

        ipc_.SyncSend(msg.SourceIPC, strlen(msg.SourceIPC), rsp_msg);
    }
    else if (msg.MsgType == 8) {
        std::string str_msg;
        str_msg.assign((char*)msg.BinData, msg.BinDataSize);
        printf("%s\n", str_msg.c_str());
    }
}



void IPCTester::BatchSend(const std::string &ipc_name) {

    for (int i = 0; i < 100; i++) {
        ClientLoginInfo cli;
        cli.mode = i;
        cli.order = "order¶©µ¥" + std::to_string(i);
        cli.user = "userÓÃ»§" + std::to_string(i);

        std::string packed_data = ppx::net::MsgPack(cli);

        ppx::net::IPCMsg msg;
        ZeroMemory(&msg, sizeof(ppx::net::IPCMsg));
        msg.MsgType = 1;
        msg.BinData = (unsigned char*)packed_data.c_str();
        msg.BinDataSize = packed_data.size();
        strcpy_s(msg.SourceIPC, 256, ipc_name_.c_str());

        int ret = ipc_.SyncSend(ipc_name.c_str(), ipc_name.length(), msg);

        printf("[MsgType 1]: %s\n", ret ? "OK" : "FAILED");
        printf("========================\n");
        Sleep(200);
    }
}

void IPCTester::StartIPCServer(const std::string &ipc_name) {
    ipc_name_ = ipc_name;
    bool ret = ipc_.StartListen(ipc_name.c_str(), ipc_name.length(), std::bind(&IPCTester::OnMsgRec, this, std::placeholders::_1));
    if (!ret) {
        printf("start listen failed\n");
    }
}

void IPCTester::StopIPCServer() {
    ipc_.StopListen();
}
