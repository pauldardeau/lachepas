// Copyright Paul Dardeau, 2016
// GFS.cpp

#include <stdio.h>

//#include <openssl/evp.h>

#include "GFS.h"
//#include "Encryption.h"
#include "OSUtils.h"
#include "Logger.h"
#include "StrUtils.h"

using namespace std;

static const string DELIMITER    = "_";
static const string EMPTY_STRING = "";


using namespace lachepas;
using namespace chaudiere;

//******************************************************************************

string GFS::uniqueIdentifierForString(const string& s) {
   return GFS::uniqueIdentifierForBuffer(s.data(), s.size());
}

//******************************************************************************

string GFS::uniqueIdentifierForBuffer(const char* s, int length) {
   if ((nullptr == s) || (length == 0)) {
      return EMPTY_STRING;
   }

   return "";
   //return Encryption::SHA1ForString(s, length);
}

//******************************************************************************

bool GFS::uniqueIdentifierForFile(const string& filePath,
                                  string& uniqueIdentifier) {
/*
   FILE* f = ::fopen(filePath.c_str(), "r");
   if (f != nullptr) {
      const EVP_MD* sha1 = ::EVP_sha1();
      if (nullptr == sha1) {
         ::printf("error: EVP_sha1 returned null\n");
         return false;
      }

      EVP_MD_CTX* mdctx = ::EVP_MD_CTX_create();

      if (nullptr == mdctx) {
         ::printf("error: EVP_MD_CTX_create returned null\n");
         return false;
      }

      //::EVP_DigestInit(&mdctx, sha1);
      if (1 != ::EVP_DigestInit_ex(mdctx, sha1, nullptr)) {
         ::printf("error: EVP_DigestInit_ex failed\n");
         ::EVP_MD_CTX_destroy(mdctx);
         return false;
      }

      unsigned char buf[8192];
      size_t bufLen;

      while (1) {
         bufLen = ::fread(buf, 1, 8192, f);
         if (bufLen == 0) {
            if (::ferror(f)) {
               // error reading file
               ::fclose(f);
               return false;
            }
            break;
         } else {
            if (1 != ::EVP_DigestUpdate(mdctx, buf, bufLen)) {
               ::printf("error: EVP_DigestUpdate failed\n");
               ::fclose(f);
               ::EVP_MD_CTX_destroy(mdctx);
            }
         }
      }

      ::fclose(f);

      unsigned char md_value[EVP_MAX_MD_SIZE];
      unsigned int md_len;

      if (1 == ::EVP_DigestFinal_ex(mdctx, md_value, &md_len)) {
         ::EVP_MD_CTX_destroy(mdctx);
         uniqueIdentifier = Encryption::digestToHexString(md_value, md_len);
         return true;
      } else {
         ::EVP_MD_CTX_destroy(mdctx);
         ::printf("error: EVP_DigestFinal_ex failed\n");
         return false;
      }
   } else {
      // unable to open file
      return false;
   }
   */

   return false;
}

//******************************************************************************

bool GFS::readFile(const string& filePath,
                   string& fileContents) {

   FILE* f = ::fopen(filePath.c_str(), "rb");
   if (f == nullptr) {
      return false;
   }

   ::fseek(f, 0, SEEK_END);
   const long fileBytes = ::ftell(f);
   ::fseek(f, 0, SEEK_SET);

   char* buffer = new char[fileBytes + 1];
   const size_t numObjectsRead = ::fread(buffer, fileBytes, 1, f);
   ::fclose(f);

   if (numObjectsRead < 1) {
      delete [] buffer;
      return false;
   }

   buffer[fileBytes] = '\0';
   fileContents = buffer;
   return true;
}

//******************************************************************************

