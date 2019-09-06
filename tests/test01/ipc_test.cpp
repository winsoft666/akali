#include "ipc_test.h"

static void DumpHex(const unsigned char *BinData, unsigned int BinDataSize) {
    for (unsigned int i = 0; i < BinDataSize; i++) {
        printf("%02X ", BinData[i]);
        if (i > 0 && i % 16 == 0)
            printf("\n");
    }
}



void IPCTester::OnMsgRec(const void *data, unsigned int data_size) {
    std::string str_msg;
    str_msg.assign((char *)data, data_size);
    printf("%s\n", str_msg.c_str());
}



void IPCTester::BatchSend(const std::string &ipc_name) {
    for (int i = 0; i < 100; i++) {
        std::string packed_data = "test_" + std::to_string(i);

        int ret = ipc_.SyncSend(ipc_name.c_str(), ipc_name.length(), packed_data.c_str(), packed_data.length());

        printf("[%d]: %s\n", i, ret ? "OK" : "FAILED");
        printf("========================\n");
        Sleep(200);
    }
}

void IPCTester::StartIPCServer(const std::string &ipc_name) {
    ipc_name_ = ipc_name;
    bool ret = ipc_.StartListen(ipc_name.c_str(), ipc_name.length(), std::bind(&IPCTester::OnMsgRec, this, std::placeholders::_1, std::placeholders::_2));
    if (!ret) {
        printf("start listen failed\n");
    }
}

void IPCTester::StopIPCServer() {
    ipc_.StopListen();
}
