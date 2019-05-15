#include "ipc_test.h"
#include "net/msgpack_helper.hpp"

// test02-server.exe -ipc_s:tests
int main() {
	ppx::base::CmdLineParser cml(GetCommandLine());
	IPCTester test_s;
	test_s.StartIPCServer(UnicodeToAnsi(cml.GetVal(L"ipc_s")).GetData());
	getchar();
	test_s.StopIPCServer();
	getchar();
	return 0;
}