#include "stdafx.h"

#include <thread>
#include <chrono>
#include <atomic>
#include <iostream>
#include "StringTest.h"
#include "IPCTest.h"
#include "DisplayMonitorTest.h"
#include "ScheduleTaskTest.h"

std::atomic<bool> over;
std::atomic<ppx::net::FileTransferBase::Status> g_state;

void StatusCB(base::StringUTF8 filePath, ppx::net::FileTransferBase::Status state, base::StringUTF8 reason, int64_t used_millsec) {
    g_state = state;

    if (g_state == ppx::net::FileTransferBase::Status::Finished || g_state == ppx::net::FileTransferBase::Status::Failed) {
        over = true;
    }
}

void ProgressCB(base::StringUTF8 filePath, int64_t total, int64_t transfered) {
    static double last_per = 0.f;
    if (total > 0) {
        double per = ((double)transfered / (double)total * 100.0);
        if (per - last_per > 0.1f || total == transfered) {
            last_per = per;
            printf("%%%.1f\n", per);
        }
    }
    else {
        printf("--\n");
    }
}

class SingletionClass : public base::Singleton<A> {
private:
	SingletionClass() {

	}

	SINLETON_CLASS_DECLARE(SingletionClass);
};


int main() {
	SingletionClass::Instance();
    ppx::base::CmdLineParser cml(GetCommandLine());
    if (cml.HasKey(L"ipc_s")) {
        IPCTester test_s;
        test_s.StartIPCServer(UnicodeToAnsi(cml.GetVal(L"ipc_s")).GetData());
        getchar();
        test_s.StopIPCServer();
        getchar();
        return 0;
    }
    else if (cml.HasKey(L"ipc_c")) {
        IPCTester test_c;
        test_c.StartIPCServer(UnicodeToAnsi(cml.GetVal(L"ipc_c")).GetData());
        test_c.BatchSend(UnicodeToAnsi(cml.GetVal(L"target")).GetData());

        getchar();
        test_c.StopIPCServer();
        getchar();
        return 0;
    }

    ScheduleTaskTest();
    DisplayMonitorTest();

    bool bDelRet = ppx::base::RegKey::DeleteKey(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", L"WallpaperEngine", false);
	StringTest();

    int thread_num = 10;
    bool interruption_resuming =  true ;
    std::string url = "https://dl.360safe.com/se/360se_setup.exe";
    std::string md5 = "620156055dfb6b20099593b7027b2ade";

    PPX_LOG(LS_INFO) << "**** [" << url << "] thread_num = " << thread_num << ", interruption_resuming = " << interruption_resuming;
    ppx::net::FileDownload fileDownload;
    fileDownload.SetUrl(url);
    fileDownload.SetFileDir("D:\\");
    fileDownload.SetFileName("test");
    fileDownload.SetFileExt(".exe");
    fileDownload.SetThreadNum(thread_num);
    fileDownload.SetFileMd5(md5);
    fileDownload.SetProgressInterval(100);
    fileDownload.SetInterruptionResuming(interruption_resuming);
    fileDownload.SetStatusCallback(std::bind(StatusCB, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    fileDownload.SetProgressCallback(std::bind(ProgressCB, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    fileDownload.Start();

    while ( !over ) {
        Sleep(100);
    }

    if ( g_state == ppx::net::FileTransferBase::Status::Finished ) {
        PPX_LOG(LS_INFO) << "**** [" << url << "] OK";
    }
    else {
        PPX_LOG(LS_INFO) << "**** [" << url << "] FAILED";
    }

    getchar();

    PPX_LOG(LS_INFO) << "**** TEST END";


	base::BufferQueue resp;
	net::HttpRequest request;
	int iHttpCode = request.Get("https://www.baidu.com", resp);

	char* pBuf = NULL;
	int64_t iBufSize = resp.ToOneBuffer(&pBuf);

	if (iBufSize > 0 && pBuf) {
		FILE *f;
		fopen_s(&f, "D:\\test.html", "wb");
		fwrite(pBuf, 1, resp.GetTotalDataSize(), f);
		fclose(f);

		printf("%s\n", pBuf);
		base::TraceMsgA("\n%s\n", pBuf);
	}

	if (pBuf)
		free(pBuf);

    getchar();
    return 0;
}
