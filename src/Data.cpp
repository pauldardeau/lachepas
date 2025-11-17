// Data.cpp

#include "Data.h"

using namespace std;
using namespace lachepas;

//*****************************************************************************

Data::Data() {
}

//*****************************************************************************

Data::Data(unsigned char c, size_t length) :
   m_bytes(length, c) {
}

//*****************************************************************************

Data::Data(const Data& copy) :
   m_bytes(copy.m_bytes) {
}

//*****************************************************************************

Data::~Data() {
}

//*****************************************************************************

Data& Data::operator=(const Data& copy) {
   if (this == &copy) {
      return *this;
   }

   m_bytes = copy.m_bytes;

   return *this;
}

//*****************************************************************************

size_t Data::length() const {
   return m_bytes.size();
}

//*****************************************************************************

size_t Data::size() const {
   return m_bytes.size();
}

//*****************************************************************************

void Data::append(unsigned char chByte) {
   m_bytes.push_back(chByte);
}

//*****************************************************************************

void Data::append(const vector<unsigned char> vecBytes) {
   m_bytes.insert(m_bytes.end(), vecBytes.begin(), vecBytes.end());
}

//*****************************************************************************

void Data::append(const Data& data) {
   append(data.m_bytes);
}

//*****************************************************************************

void Data::bzero() {
   const size_t numBytes = size();

   for (size_t i = 0; i < numBytes; ++i) {
      m_bytes[i] = 0;
   }
}

//*****************************************************************************

unsigned char* Data::data() {
   return &m_bytes[0];
}

//*****************************************************************************

const unsigned char* Data::cdata() const {
   return &m_bytes[0];
}

//*****************************************************************************

