/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#include "net\ipc.h"
#ifndef PPX_NO_IPC
#include <winsock2.h>
#include <future>
#include <thread>
#include <atomic>
#include <assert.h>
#include <algorithm>
#include <strsafe.h>
#include "base/safe_release_macro.h"
#include "base/timeutils.h"
#include "base/logging.h"
#include "base/stringencode.h"
#include <msgpack.h>

namespace ppx {
    namespace net {

        static char* PackIPCMsg(const IPCMsg& msg, unsigned int &out_size) {
            msgpack_sbuffer sbuf;
            msgpack_sbuffer_init(&sbuf);

            msgpack_packer pk;
            msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

            msgpack_pack_int32(&pk, msg.MsgType);

            msgpack_pack_uint32(&pk, msg.BinDataSize);
            msgpack_pack_bin(&pk, msg.BinDataSize);

            msgpack_pack_bin_body(&pk, msg.BinData, msg.BinDataSize);

            msgpack_pack_int64(&pk, msg.IntegerData);

            msgpack_pack_str(&pk, strlen(msg.SourceIPC));
            msgpack_pack_str_body(&pk, msg.SourceIPC, strlen(msg.SourceIPC));


            char *buf = (char*)malloc(sbuf.size + 1);
            memcpy(buf, sbuf.data, sbuf.size);
            buf[sbuf.size] = 0;

            msgpack_sbuffer_destroy(&sbuf);

            out_size = sbuf.size;
            return buf;
        }

        static bool UnPackIPCMsg(const void* data, unsigned int data_size, IPCMsg &msg) {
            msgpack_unpacker* unp = msgpack_unpacker_new(data_size);
            if (msgpack_unpacker_buffer_capacity(unp) < data_size) {
                bool result = msgpack_unpacker_reserve_buffer(unp, data_size);

                if (msgpack_unpacker_buffer_capacity(unp) < data_size) {
                    msgpack_unpacker_free(unp);
                    return false;
                }
            }
            memcpy(msgpack_unpacker_buffer(unp), data, data_size);

            msgpack_unpacker_buffer_consumed(unp, data_size);

            msgpack_unpack_return ret;

            int i = 0;

            do {
                msgpack_unpacked und;
                msgpack_unpacked_init(&und);
                ret = msgpack_unpacker_next(unp, &und);

                if (ret == MSGPACK_UNPACK_SUCCESS)
                {
                    msgpack_object obj = und.data;

                    i++;
                    /* Extract msgpack_object and use it. */
                    if (i == 1) {
                        msg.MsgType = (int)obj.via.u64;
                    }
                    else if (i == 2) {
                        msg.BinDataSize = (unsigned int)obj.via.u64;
                    }
                    else if (i == 3) {
                        assert(obj.type == MSGPACK_OBJECT_BIN);
                        assert(obj.via.bin.size == msg.BinDataSize);
                        msg.BinData = (unsigned char*)malloc(obj.via.bin.size);
                        memcpy(msg.BinData, obj.via.bin.ptr, msg.BinDataSize);
                    }
                    else if (i == 4) {
                        msg.IntegerData = obj.via.i64;
                    }
                    else if (i == 5) {
                        assert(obj.type == MSGPACK_OBJECT_STR);
                        memset(msg.SourceIPC, 0, 256);
                        memcpy(msg.SourceIPC, obj.via.str.ptr, obj.via.str.size);
                    }

                    msgpack_unpacked_destroy(&und);
                }
                if (ret == MSGPACK_UNPACK_CONTINUE) {
                    msgpack_unpacked_destroy(&und);
                    /* cheking capacity, reserve buffer, copy additional data to the buffer, */
                    /* notify consumed buffer size, then call msgpack_unpacker_next(&unp, &upd) again */
                    break;
                }
                if (ret == MSGPACK_UNPACK_PARSE_ERROR) {
                    msgpack_unpacked_destroy(&und);
                    /* Error process */
                    break;
                }
            } while (true);

            msgpack_unpacker_free(unp);

            return true;
        }

        class IPC::IPCImpl {
        public:
            IPCImpl() :
                buffer_size_(4096),
                exit_(false) {

            }

            ~IPCImpl() {
            }

            bool SyncSend(const char* ipc_name, unsigned int ipc_name_len, const void *msg, unsigned int msg_size, unsigned int timeout = 5000) {
                unsigned char * msg_enc_data = nullptr;
                unsigned int msg_enc_data_size = 0;

                unsigned char *rand_aes_key = nullptr;
                unsigned int rand_aes_key_size = 0;

                unsigned char *aes_key_enc_data = nullptr;
                unsigned int aes_key_enc_data_size = 0;

                unsigned char *full_data = nullptr;
                unsigned int full_data_size = 0;

                std::string str_ipc_name;
                str_ipc_name.assign(ipc_name, ipc_name_len);
                std::string strPipeName = "\\\\.\\pipe\\" + str_ipc_name;
                HANDLE hPipe = INVALID_HANDLE_VALUE;
                DWORD dwGLE = 0;

                while (true) {
                    hPipe = CreateFileA(strPipeName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
                    dwGLE = GetLastError();
                    if (hPipe != INVALID_HANDLE_VALUE)
                        break;
                    if (dwGLE != ERROR_PIPE_BUSY) {
                        CloseHandle(hPipe);

                        SAFE_FREE(full_data);
                        return false;
                    }
                    if (!WaitNamedPipeA(strPipeName.c_str(), timeout)) {
                        CloseHandle(hPipe);
                        SAFE_FREE(full_data);
                        return false;
                    }
                }

                DWORD dwMode = PIPE_READMODE_MESSAGE;
                if (!SetNamedPipeHandleState(hPipe, &dwMode, NULL, NULL)) {
                    CloseHandle(hPipe);
                    SAFE_FREE(full_data);
                    return false;
                }

                DWORD dwWritten = 0;
                BOOL fSuccess = FALSE;

                fSuccess = WriteFile(hPipe, msg, msg_size, &dwWritten, NULL);

                SAFE_FREE(full_data);

                dwGLE = GetLastError();
                if (!fSuccess) {
                    CloseHandle(hPipe);
                    if(dwGLE != 232 || msg_size != 0)
                        PPX_LOG(LS_ERROR) << "WriteFile Failed, GLE: " << dwGLE;
                    return false;
                }

                CloseHandle(hPipe);
                return true;
            }

            bool AsyncSend(const void *msg, unsigned int msg_size) {
                return true;
            }

            void ListenThreadPro() {
                std::string strPipeName = "\\\\.\\pipe\\" + ipc_name_;

                for (;;) {
                    HANDLE hPipe = CreateNamedPipeA(strPipeName.c_str(),
                        PIPE_ACCESS_DUPLEX,
                        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
                        PIPE_UNLIMITED_INSTANCES,
                        buffer_size_, buffer_size_,
                        5000,
                        NULL);
                    if (hPipe == INVALID_HANDLE_VALUE) {
                        return;
                    }

                    BOOL fConnected = ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

                    if (fConnected) {
                        if (exit_) {
                            CloseHandle(hPipe);
                            break;
                        }
                        else {
                            std::future<void> f = std::async(std::launch::async, std::bind(&IPCImpl::InstanceThrProc, this, hPipe));
                            bool add = false;

                            for (std::vector<std::future<void>>::iterator it = instance_thrs_.begin(); it != instance_thrs_.end(); it++) {
                                if ((*it).valid() == false || (*it).wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
                                    *it = std::move(f);
                                    add = true;
                                    break;
                                }
                            }

                            if (!add) {
                                instance_thrs_.push_back(std::move(f));
                            }
                        }
                    }
                    else {
                        CloseHandle(hPipe);
                    }
                }
            }


            void OnRecvMsg(const void * buffer, unsigned int buffer_size) {
                if (cb_) {
                    IPCMsg msg;

                    UnPackIPCMsg(buffer, buffer_size, msg);

                    cb_(msg);

                    SAFE_FREE(msg.BinData);
                }
            }

            void InstanceThrProc(HANDLE hPipe) {
                char *pRequest = (char*)malloc(buffer_size_ + 1);
                if (!pRequest) {
                    return;
                }
                memset(pRequest, 0, buffer_size_ + 1);
                DWORD dwGLE = 0;
                DWORD total_read = 0;

                do {
                    DWORD dwBytesRead = 0;
                    BOOL fSuccess = ReadFile(hPipe, pRequest, buffer_size_, &dwBytesRead, NULL);
                    dwGLE = GetLastError();
                    total_read = dwBytesRead;
                    if (!fSuccess) {
                        if (dwGLE == ERROR_MORE_DATA) {
                            unsigned int max_buf_size = buffer_size_ * 4;
                            char *pNewRequest = (char*)malloc(max_buf_size + 1);
                            if (!pNewRequest)
                                break;
                            memset(pNewRequest, 0, max_buf_size + 1);

                            memcpy(pNewRequest, pRequest, dwBytesRead);
                            free(pRequest);
                            pRequest = pNewRequest;

                            dwBytesRead = 0;
                            fSuccess = ReadFile(hPipe, pRequest + dwBytesRead, max_buf_size - dwBytesRead, &dwBytesRead, NULL);
                            if (!fSuccess) {
                                PPX_LOG(LS_ERROR) << "SRV Receive Data Failed, Size > " << max_buf_size;
                                break;
                            }

                            total_read += dwBytesRead;
                        }
                        else {
                            PPX_LOG(LS_ERROR) << "SRV ReadFile Failed, GLE: " << dwGLE;
                            break;
                        }
                    }

                    OnRecvMsg(pRequest, total_read);

                } while (false);

                FlushFileBuffers(hPipe);
                DisconnectNamedPipe(hPipe);
                CloseHandle(hPipe);

                SAFE_FREE(pRequest);

                return;
            }

            bool StartListen(const char* ipc_name, unsigned int ipc_name_len, std::function<void(const IPCMsg&)> cb) {
                ipc_name_.clear();
                ipc_name_.assign(ipc_name, ipc_name_len);

                cb_ = cb;

                listen_thr_ = std::thread(&IPCImpl::ListenThreadPro, this);
                return true;
            }

            void StopListen() {
                exit_ = true;
                SyncSend(ipc_name_.c_str(), ipc_name_.length(), 0, 0);
                if (listen_thr_.joinable())
                    listen_thr_.join();

                for (std::vector<std::future<void>>::iterator it = instance_thrs_.begin(); it != instance_thrs_.end(); it++) {
                    if ((*it).valid())
                        (*it).wait();
                }

                instance_thrs_.clear();
            }


        public:
            std::string ipc_name_;
            std::function<void(const IPCMsg&)> cb_;
            std::thread listen_thr_;
            std::vector<std::future<void>> instance_thrs_;
            std::atomic_bool exit_;
            unsigned int buffer_size_;
        };

        IPC::IPC() {
            impl_ = new IPCImpl();
        }

        IPC::~IPC() {
            SAFE_DELETE(impl_);
        }

        bool IPC::StartListen(const char* ipc_name, unsigned int ipc_name_len, std::function<void(const IPCMsg&)> cb) {
            return impl_->StartListen(ipc_name, ipc_name_len, cb);
        }

        void IPC::StopListen() {
            impl_->StopListen();
        }

        bool IPC::SyncSend(const char* ipc_name, unsigned int ipc_name_len, const IPCMsg& t) {
            unsigned int out_size = 0;

            char * buf = PackIPCMsg(t, out_size);

            bool ret = impl_->SyncSend(ipc_name, ipc_name_len, buf, out_size, 30000);

            free(buf);

            return ret;
        }
    }
}

#endif