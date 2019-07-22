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


#include "ppxbase/byteorder.h"

namespace ppx {
    namespace base {
        void Set8(void *memory, size_t offset, uint8_t v) {
            static_cast<uint8_t *>(memory)[offset] = v;
        }

        uint8_t Get8(const void *memory, size_t offset) {
            return static_cast<const uint8_t *>(memory)[offset];
        }

        void SetBE16(void *memory, uint16_t v) {
            *static_cast<uint16_t *>(memory) = htobe16(v);
        }

        void SetBE32(void *memory, uint32_t v) {
            *static_cast<uint32_t *>(memory) = htobe32(v);
        }

#if _WIN32_WINNT>=0x0602 || WINVER>=0x0602 // Win8
        void SetBE64(void *memory, uint64_t v) {
            *static_cast<uint64_t *>(memory) = htobe64(v);
        }

        uint64_t GetBE64(const void *memory) {
            return be64toh(*static_cast<const uint64_t *>(memory));
        }

        uint64_t HostToNetwork64(uint64_t n) {
            return htobe64(n);
        }

        uint64_t NetworkToHost64(uint64_t n) {
            return be64toh(n);
        }
#endif
        uint16_t GetBE16(const void *memory) {
            return be16toh(*static_cast<const uint16_t *>(memory));
        }

        uint32_t GetBE32(const void *memory) {
            return be32toh(*static_cast<const uint32_t *>(memory));
        }

        void SetLE16(void *memory, uint16_t v) {
            *static_cast<uint16_t *>(memory) = htole16(v);
        }

        void SetLE32(void *memory, uint32_t v) {
            *static_cast<uint32_t *>(memory) = htole32(v);
        }

        void SetLE64(void *memory, uint64_t v) {
            *static_cast<uint64_t *>(memory) = htole64(v);
        }

        uint16_t GetLE16(const void *memory) {
            return le16toh(*static_cast<const uint16_t *>(memory));
        }

        uint32_t GetLE32(const void *memory) {
            return le32toh(*static_cast<const uint32_t *>(memory));
        }

        uint64_t GetLE64(const void *memory) {
            return le64toh(*static_cast<const uint64_t *>(memory));
        }

        uint16_t HostToNetwork16(uint16_t n) {
            return htobe16(n);
        }

        uint32_t HostToNetwork32(uint32_t n) {
            return htobe32(n);
        }

        uint16_t NetworkToHost16(uint16_t n) {
            return be16toh(n);
        }

        uint32_t NetworkToHost32(uint32_t n) {
            return be32toh(n);
        }
    }
}