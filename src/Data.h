#ifndef LACHEPAS_DATA_H
#define LACHEPAS_DATA_H

#include <vector>


namespace lachepas {

class Data {

private:
   std::vector<unsigned char> m_bytes;

public:
   Data();
   Data(unsigned char c, std::size_t length);
   Data(const Data& copy);
   ~Data();

   Data& operator=(const Data& copy);

   std::size_t length() const;
   std::size_t size() const;

   void append(unsigned char chByte);
   void append(const std::vector<unsigned char> vecBytes);
   void append(const Data& data);

   void bzero();

   unsigned char* data();
   const unsigned char* cdata() const;
};

}

#endif

