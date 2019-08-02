// AESEncryption.cpp

#include <string.h>
#include <openssl/evp.h>

#include "AESEncryption.h"
#include "Data.h"

using namespace std;

static const string EMPTY = "";

using namespace lachepas;

//*****************************************************************************

bool AESEncryption::decrypt(const Data& key,
                            const Data& iv,
                            const Data& cipherText,
                            string& plainText) {
   EVP_CIPHER_CTX ctx;
   int rc;
   bool success = false;
   
   /*
   
   ::EVP_CIPHER_CTX_init(&ctx);
   
   rc = ::EVP_DecryptInit(&ctx, EVP_aes_256_cbc(), key.cdata(), iv.cdata());
   
   if (1 == rc) {
      const size_t cipherLen = cipherText.size();
      size_t remainingBytes = cipherLen;
      
      char *decryptedData = new char[cipherText.size()+1];
      ::memset(decryptedData, 0, cipherText.size()+1);
      int decryptedLength = 0;
      
      Data out(0, 16);
      int outl = 0;
      
      rc = ::EVP_DecryptUpdate(ctx,
                               decryptedData,
                               &decryptedLength,
                               in,
                               inl);
                                   
      if (1 == rc) {
         remainingBytes -= outl;
      }

      rc = ::EVP_DecryptFinal_ex(ctx, outm, &outl);
      if (1 == rc) {
         success = true;
      }
   }
   
   ::EVP_CIPHER_CTX_cleanup(&ctx);
   */

   return success;
}

//*****************************************************************************

string AESEncryption::decrypt(const Data& key,
                                   const Data& iv,
                                   const Data& cipherText) {
   string plainText;
   if (decrypt(key, iv, cipherText, plainText)) {
      return plainText;
   } else {
      return EMPTY;
   }
}

//*****************************************************************************

/*
Data AESEncryption::encrypt(const Data& key,
                            const Data& iv,
                            const Data& plainText) {
   
}
*/
//*****************************************************************************

unsigned char* AESEncryption::encrypt(const Data& key,
                                      const Data& iv,
                                      const string& plainText) {
   EVP_CIPHER_CTX ctx;
   int rc;
   bool success = false;
   
   ::EVP_CIPHER_CTX_init(&ctx);
   
   // set padding to true/on
   rc = EVP_CIPHER_CTX_set_padding(&ctx, 1);
   
   rc = EVP_CIPHER_CTX_set_key_length(&ctx, 32);

   /*
   int EVP_EncryptInit_ex(EVP_CIPHER_CTX *ctx,
                          const EVP_CIPHER *type,
                          ENGINE *impl,
                          unsigned char *key,
                          unsigned char *iv);
   
   EVP_EncryptInit_ex() sets up cipher context ctx for encryption
   with cipher type from ENGINE impl. ctx must be initialized before
   calling this function. type is normally supplied by a function
   such as EVP_des_cbc(). If impl is NULL then the default implementation
   is used. key is the symmetric key to use and iv is the IV to use (if
   necessary), the actual number of bytes used for the key and IV depends
   on the cipher. It is possible to set all parameters to NULL except type
   in an initial call and supply the remaining parameters in subsequent
   calls, all of which have type set to NULL. This is done when the default
   cipher parameters are not appropriate.
   */
   rc = ::EVP_EncryptInit(&ctx, EVP_aes_256_cbc(), key.cdata(), iv.cdata());
   
   if (1 == rc) {
      
      //Data in(0, 16);
      //Data out(0, 32); /* at least one block longer than in[] */
      
      const unsigned blockSize = 16;  // 128-bit block size
      
      unsigned outLen = plainText.size();
      outLen += blockSize;

      unsigned char *outp = (unsigned char*) malloc(outLen);
      
      int outl = outLen;
      int outlen2;
      
      /*
      int EVP_EncryptUpdate(EVP_CIPHER_CTX *ctx,
                            unsigned char *out,
                            int *outl,
                            unsigned char *in,
                            int inl);
      
      EVP_EncryptUpdate() encrypts inl bytes from the buffer in and
      writes the encrypted version to out. This function can be called
      multiple times to encrypt successive blocks of data. The amount
      of data written depends on the block alignment of the encrypted
      data: as a result the amount of data written may be anything from
      zero bytes to (inl + cipher_block_size - 1) so outl should contain
      sufficient room. The actual number of bytes written is placed in outl.
      */
      rc = ::EVP_EncryptUpdate(&ctx,
                               outp,
                               &outl,
                               (unsigned char*)plainText.data(),
                               plainText.size());
      
      if (1 == rc) {
         
         outp += outl;
         outl = outLen - outl;
         
         /*
         int EVP_EncryptFinal_ex(EVP_CIPHER_CTX *ctx,
                                 unsigned char *out,
                                 int *outl);
      
         If padding is enabled (the default) then EVP_EncryptFinal_ex()
         encrypts the ``final'' data, that is any data that remains in a
         partial block. It uses standard block padding (aka PKCS padding).
         The encrypted final data is written to out which should have
         sufficient space for one cipher block. The number of bytes written
         is placed in outl. After this function is called the encryption
         operation is finished and no further calls to EVP_EncryptUpdate()
         should be made.

         If padding is disabled then EVP_EncryptFinal_ex() will not encrypt
         any more data and it will return an error if any data remains in a
         partial block: that is if the total data length is not a multiple
         of the block size.
         */
         rc = ::EVP_EncryptFinal_ex(&ctx, outp, &outl);
         
         if (1 == rc) {
            return outp;
            //success = true;
            //cipherText.append(out);
         } else {
            ::printf("error: EVP_EncryptFinal_ex failed\n");
         }
      } else {
         ::printf("error: EVP_EncryptUpdate failed\n");
      }
      
      ::free(outp);
   } else {
      ::printf("error: EVP_EncryptInit_ex failed\n");
   }

   ::EVP_CIPHER_CTX_cleanup(&ctx);

   //return success;
   return nullptr;
}

//*****************************************************************************
/*
Data AESEncryption::encrypt(const Data& key,
                            const Data& iv,
                            const string& plainText) {
   Data cipherText;
   if (encrypt(key, iv, plainText, cipherText)) {
      return cipherText;
   } else {
      return Data();
   }
}
*/
//*****************************************************************************

void AESEncryption::test() {
   unsigned char key[32] = {0};
   unsigned char iv[16] = {0};
   unsigned char in[16] = {0};
   unsigned char out[32]; /* at least one block longer than in[] */
   int outlen1;
   int outlen2;
   int rc;
   
   ::memset(out, 0, 32);

   //                  0        1         2         3
   //                  12345678901234567890123456789012
   ::strcpy((char*)key, "sksiewk48wjaKkl2#2ln120,4os@+x<}");

   //                 0        1
   //                 1234567890123456
   ::strcpy((char*)iv, "*dlk3EosMdi50;x");
   
   //                 0        1
   //                 1234567890123456
   ::strcpy((char*)in, "Now is the time");
}

//*****************************************************************************

