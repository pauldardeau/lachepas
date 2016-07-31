// Copyright Paul Dardeau, 2016
// Encryption.cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>

#include <openssl/md5.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

#include "Encryption.h"
#include "base64.h"


#define STACK_BUFFER_BYTES 512
#define HALF_BUFFER_SIZE STACK_BUFFER_BYTES / 2


static const std::string EMPTY_STRING = "";


using namespace lachepas;

//******************************************************************************

std::string Encryption::digestToHexString(const unsigned char* digest,
                                          int len) {
   char stackBuffer[STACK_BUFFER_BYTES];
   char* heapBuffer = nullptr;
   char* hexstring = stackBuffer;
   bool usingStackBuffer = true;
   
   if (len >= HALF_BUFFER_SIZE) {
      const int heapBufferSize = (len * 2) + 1;
      heapBuffer = (char*) ::malloc(heapBufferSize);
      hexstring = heapBuffer;
      usingStackBuffer = false;
      ::memset(heapBuffer, 0, heapBufferSize);

      // cap it at 510 bytes
      if (len > 510) {
         len = 510;
      }
   } else {
      ::memset(stackBuffer, 0, sizeof(stackBuffer));
   }
   
   for (int i = 0; i < len; ++i) {
      ::sprintf(&hexstring[2*i], "%02x", digest[i]);
   }

   if (usingStackBuffer) {
      return std::string(hexstring);
   } else {
      std::string hex(hexstring);
      ::free(hexstring);
      return hex;   
   }
}

//******************************************************************************

std::string Encryption::MD5ForString(const char* str, int length) {
   EVP_MD_CTX mdctx;
   unsigned char md_value[EVP_MAX_MD_SIZE];
   unsigned int md_len;

   EVP_DigestInit(&mdctx, EVP_md5());
   EVP_DigestUpdate(&mdctx, str, length);
   EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
   EVP_MD_CTX_cleanup(&mdctx);

   return Encryption::digestToHexString(md_value, md_len);
}

//******************************************************************************

std::string Encryption::SHA1ForString(const char* str, int length) {
   EVP_MD_CTX mdctx;
   unsigned char md_value[EVP_MAX_MD_SIZE];
   unsigned int md_len;

   EVP_DigestInit(&mdctx, EVP_sha1());
   EVP_DigestUpdate(&mdctx, str, length);
   EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
   EVP_MD_CTX_cleanup(&mdctx);

   return Encryption::digestToHexString(md_value, md_len);
}

//******************************************************************************

std::string Encryption::computeHMAC(const char* str,
                                    int length,
                                    const std::string& key) {
   HMAC_CTX ctx;
   ::HMAC_CTX_init(&ctx);
   ::HMAC_Init_ex(&ctx, key.c_str(), key.length(), EVP_sha1(), NULL);

   if (length > 0) {
      uint32_t msgLengthBigEndian = htonl(length);
      ::HMAC_Update(&ctx, reinterpret_cast<unsigned char*>(&msgLengthBigEndian),
            sizeof(uint32_t));
      ::HMAC_Update(&ctx, reinterpret_cast<const unsigned char*>(str),
            length);
   }

   unsigned char result[SHA_DIGEST_LENGTH];
   unsigned int resultLength = SHA_DIGEST_LENGTH;
   ::HMAC_Final(&ctx, result, &resultLength);
   ::HMAC_CTX_cleanup(&ctx);

   return std::string(reinterpret_cast<char*>(result), resultLength);
}

//******************************************************************************

std::string Encryption::base64Encode(unsigned char const* buffer,
                                     unsigned int len) {
   const int encodedLength = ::Base64encode_len(len);
   if (encodedLength > 0) {
      char* encodedStr = new char[encodedLength];
      const int bytesEncoded = ::Base64encode(encodedStr, (const char*) buffer, len);
      std::string encodedString;
      
      if (bytesEncoded > 0) {
         encodedString = encodedStr;
      }
      
      delete [] encodedStr;
      return encodedString;
   } else {
      return EMPTY_STRING;
   }
}

//******************************************************************************

std::string Encryption::base64Decode(std::string const& s) {
   const int decodedLength = ::Base64decode_len(s.data());
   if (decodedLength > 0) {
      char* decodedStr = new char[decodedLength];
      const int bytesDecoded = ::Base64decode(decodedStr, s.data());
      if (bytesDecoded > 0) {
         std::string decodedString(decodedStr, bytesDecoded);
         delete [] decodedStr;
         return decodedString;
      } else {
         delete [] decodedStr;
         return EMPTY_STRING;
      }
   } else {
      return EMPTY_STRING;
   }
}

//******************************************************************************

