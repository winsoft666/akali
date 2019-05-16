#pragma once
#include "ppx.h"

class IPCTester {
public:
    void StartIPCServer(const std::string &ipc_name);
    void StopIPCServer();

    void OnMsgRec(const void* data, unsigned int data_size);
    void BatchSend(const std::string &ipc_name);
private:
    ppx::net::IPC ipc_;
    std::string ipc_name_;
};