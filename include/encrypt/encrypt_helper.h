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


#ifndef PPX_ENCRYPT_H__
#define PPX_ENCRYPT_H__
#pragma once

#include "ppx_config.h"

#ifndef PPX_NO_ENCRYPT
#include <string>

namespace ppx {
    namespace encrypt {
        std::string AESEncryptStr(const std::string& key, const std::string& plaintext);
        std::string AESDecryptStr(const std::string& key, const std::string& ciphertext);

        void GenerateRSAKey(unsigned int key_length, const char* priv_filename, const char* pub_filename, const char* seed);
        std::string RSAEncryptStr(const char* pub_filename, const char* seed, const char* message);
        std::string RSADecryptStr(const char* priv_filename, const char* ciphertext);
    }
}
#endif // !PPX_NO_ENCRYPT

#endif // !PPX_ENCRYPT_H__
