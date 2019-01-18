/*******************************************************************************
* Copyright (C) 2018 - 2020, Jeffery Jiang, <china_jeffery@163.com>.
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

#include "encrypt/encrypt_helper.h"
#include "hex.h"
#include "modes.h"
#include "aes.h"
#include "rsa.h"
#include "randpool.h"
#include "files.h"
using namespace CryptoPP;

namespace ppx {
	namespace encrypt {

		std::string AESEncryptStr(const std::string& key, const std::string& plaintext) {
			std::string outstr;

			SecByteBlock make_key(AES::MAX_KEYLENGTH);
			::memset(make_key, 0x30, make_key.size());
			key.size() <= AES::MAX_KEYLENGTH ? memcpy(make_key, key.c_str(), key.size()) : memcpy(make_key, key.c_str(), AES::MAX_KEYLENGTH);

			AES::Encryption aesEncryption((byte *)make_key, AES::MAX_KEYLENGTH);

			ECB_Mode_ExternalCipher::Encryption ecbEncryption(aesEncryption);
			StreamTransformationFilter ecbEncryptor(ecbEncryption, new HexEncoder(new StringSink(outstr)));
			ecbEncryptor.Put((byte *)plaintext.c_str(), plaintext.length());
			ecbEncryptor.MessageEnd();

			return outstr;
		}

		std::string AESDecryptStr(const std::string& key, const std::string& ciphertext) {
			std::string outstr;

			SecByteBlock make_key(AES::MAX_KEYLENGTH);
			memset(make_key, 0x30, make_key.size());
			key.size() <= AES::MAX_KEYLENGTH ? memcpy(make_key, key.c_str(), key.size()) : memcpy(make_key, key.c_str(), AES::MAX_KEYLENGTH);

			ECB_Mode<AES >::Decryption ecbDecryption((byte *)make_key, AES::MAX_KEYLENGTH);

			HexDecoder decryptor(new StreamTransformationFilter(ecbDecryption, new StringSink(outstr)));
			decryptor.Put((byte *)ciphertext.c_str(), ciphertext.length());
			decryptor.MessageEnd();

			return outstr;
		}

		RandomPool & GlobalRNG() {
			static RandomPool randomPool;
			return randomPool;
		}

		void GenerateRSAKey(unsigned int key_length, const char* priv_filename, const char* pub_filename, const char* seed) {
			if (pub_filename == nullptr || pub_filename == nullptr || seed == nullptr)
				return;

			RandomPool randPool;
			randPool.IncorporateEntropy((byte *)seed, strlen(seed));

			RSAES_OAEP_SHA_Decryptor priv(randPool, key_length);
			HexEncoder privFile(new FileSink(priv_filename));
			priv.AccessMaterial().Save(privFile);
			privFile.MessageEnd();

			RSAES_OAEP_SHA_Encryptor pub(priv);
			HexEncoder pubFile(new FileSink(pub_filename));
			pub.AccessMaterial().Save(pubFile);
			pubFile.MessageEnd();
		}

		std::string RSAEncryptStr(const char* pub_filename, const char* seed, const char* message) {
			if (pub_filename == nullptr || seed == nullptr || message == nullptr)
				return "";

			FileSource pubFile(pub_filename, true, new HexDecoder);
			RSAES_OAEP_SHA_Encryptor pub(pubFile);

			RandomPool randPool;
			randPool.IncorporateEntropy((byte *)seed, strlen(seed));

			std::string result;
			StringSource(message, true, new PK_EncryptorFilter(randPool, pub, new HexEncoder(new StringSink(result))));
			return result;
		}

		std::string RSADecryptStr(const char* priv_filename, const char* ciphertext) {
			if (priv_filename == nullptr || ciphertext == nullptr)
				return "";

			FileSource privFile(priv_filename, true, new HexDecoder);
			RSAES_OAEP_SHA_Decryptor priv(privFile);

			std::string result;
			StringSource(ciphertext, true, new HexDecoder(new PK_DecryptorFilter(GlobalRNG(), priv, new StringSink(result))));
			return result;
		}
	}
}
