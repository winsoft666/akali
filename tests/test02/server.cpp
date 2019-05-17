#include "ipc_test.h"

// test02-server.exe -ipc_s:tests
int main() {
	ppx::base::CmdLineParser cml(GetCommandLine());
	IPCTester test_s;
	test_s.StartIPCServer(ppx::base::UnicodeToAnsi(cml.GetVal(L"ipc_s")).c_str());
	getchar();
	test_s.StopIPCServer();
	getchar();
	return 0;
}