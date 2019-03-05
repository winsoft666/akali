#pragma once

class IPCTester {
public:
    void StartIPCServer(const std::string &ipc_name);
    void StopIPCServer();

    void OnMsgRec(const ppx::net::IPCMsg &msg);
    void BatchSend(const std::string &ipc_name);
private:
    ppx::net::IPC ipc_;
    std::string ipc_name_;
};