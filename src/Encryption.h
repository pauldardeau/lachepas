// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_ENCRYPTION_H
#define LACHEPAS_ENCRYPTION_H

#include <string>


namespace lachepas {

class Encryption {

public:
   static std::string MD5ForString(const char* str, int length);
   static std::string SHA1ForString(const char* str, int length);
   static std::string computeHMAC(const char* str, int length, const std::string& key);
   static std::string digestToHexString(const unsigned char* digest, int len);
   static std::string base64Encode(unsigned char const* , unsigned int len);
   static std::string base64Decode(std::string const& s);
};

}

#endif

