#include "ipc_test.h"

// test02-client.exe -ipc_c:testc -target:tests
int main() {
	ppx::base::CmdLineParser cml(GetCommandLine());
	IPCTester test_c;
	test_c.StartIPCServer(UnicodeToAnsi(cml.GetVal(L"ipc_c")).GetData());
	test_c.BatchSend(UnicodeToAnsi(cml.GetVal(L"target")).GetData());

	getchar();
	test_c.StopIPCServer();

    getchar();
    return 0;
}
