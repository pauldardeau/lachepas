#ifndef LACHEPAS_AESENCRYPTION_H
#define LACHEPAS_AESENCRYPTION_H

#include <string>

namespace lachepas {

class Data;
   
class AESEncryption {

public:
   bool decrypt(const Data& key,
                const Data& iv,
                const Data& cipherText,
                std::string& plainText);
   std::string decrypt(const Data& key,
                       const Data& iv,
                       const Data& cipherText);
   //Data encrypt(const Data& key, const Data& iv, const Data& plainText);
   //Data encrypt(const Data& key, const Data& iv, const std::string& plainText);
   //bool encrypt(const Data& key, const Data& iv, const std::string& plainText, Data& cipherText);
   unsigned char* encrypt(const Data& key,
                          const Data& iv,
                          const std::string& plainText);
      
   void test();
};

}

#endif

