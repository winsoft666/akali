#include "ipc_test.h"

// test02-client.exe -ipc_c:testc -target:tests
int main() {
	ppx::base::CmdLineParser cml(GetCommandLine());
	IPCTester test_c;
	test_c.StartIPCServer(ppx::base::UnicodeToAnsi(cml.GetVal(L"ipc_c")).c_str());
	test_c.BatchSend(ppx::base::UnicodeToAnsi(cml.GetVal(L"target")).c_str());

	getchar();
	test_c.StopIPCServer();

    getchar();
    return 0;
}
