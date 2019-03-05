#ifndef _MSGPACK_HELPER_H_
#define _MSGPACK_HELPER_H_
#pragma once

#include "ppx_config.h"

#ifndef PPX_NO_IPC

#include "msgpack.hpp"
#include <string>
#include <sstream>

namespace ppx {
    namespace net {
        template<typename T>
        inline std::string MsgPack(const T& t) {
            std::stringstream s;
            msgpack::pack(s, t);

            return s.str();
        }


        template<typename T>
        inline void MsgUnPack(const void *data, unsigned int data_size, T& t) {
            msgpack::unpacked msg;
            msgpack::unpack(msg, (const char*)data, (int)data_size);
            msg.get().convert(t);
        }
    }
}

#endif

#endif