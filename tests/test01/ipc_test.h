#pragma once
#include "ppx_base.h"

class IPCTester {
  public:
    void StartIPCServer(const std::string &ipc_name);
    void StopIPCServer();

    void OnMsgRec(const void *data, unsigned int data_size);
    void BatchSend(const std::string &ipc_name);
  private:
    ppx::base::IPC ipc_;
    std::string ipc_name_;
};