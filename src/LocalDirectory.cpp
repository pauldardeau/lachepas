// Copyright Paul Dardeau, 2016
// LocalDirectory.cpp

#include "LocalDirectory.h"

using namespace std;
using namespace lachepas;

//******************************************************************************

LocalDirectory::LocalDirectory() :
   m_localDirectoryId(-1),
   m_copyCount(1),
   m_active(true),
   m_recurse(false),
   m_compress(false),
   m_encrypt(false) {
}

//******************************************************************************

LocalDirectory::LocalDirectory(const LocalDirectory& copy) :
   m_directoryPath(copy.m_directoryPath),
   m_localDirectoryId(copy.m_localDirectoryId),
   m_copyCount(copy.m_copyCount),
   m_active(copy.m_active),
   m_recurse(copy.m_recurse),
   m_compress(copy.m_compress),
   m_encrypt(copy.m_encrypt) {
}

//******************************************************************************

LocalDirectory& LocalDirectory::operator=(const LocalDirectory& copy) {
   if (this == &copy) {
      return *this;
   }

   m_directoryPath = copy.m_directoryPath;
   m_localDirectoryId = copy.m_localDirectoryId;
   m_copyCount = copy.m_copyCount;
   m_active = copy.m_active;
   m_recurse = copy.m_recurse;
   m_compress = copy.m_compress;
   m_encrypt = copy.m_encrypt;

   return *this;
}

//******************************************************************************

void LocalDirectory::setDirectoryPath(const string& directoryPath) {
   m_directoryPath = directoryPath;
}

//******************************************************************************

const string& LocalDirectory::getDirectoryPath() const {
   return m_directoryPath;
}

//******************************************************************************

void LocalDirectory::setCopyCount(int copyCount) {
   m_copyCount = copyCount;
}

//******************************************************************************

int LocalDirectory::getCopyCount() const {
   return m_copyCount;
}

//******************************************************************************

void LocalDirectory::setLocalDirectoryId(int localDirectoryId) {
   m_localDirectoryId = localDirectoryId;
}

//******************************************************************************

int LocalDirectory::getLocalDirectoryId() const {
   return m_localDirectoryId;
}

//******************************************************************************

void LocalDirectory::setRecurse(bool recurse) {
   m_recurse = recurse;
}

//******************************************************************************

bool LocalDirectory::getRecurse() const {
   return m_recurse;
}

//******************************************************************************

void LocalDirectory::setCompress(bool compress) {
   m_compress = compress;
}

//******************************************************************************

bool LocalDirectory::getCompress() const {
   return m_compress;
}

//******************************************************************************

void LocalDirectory::setEncrypt(bool encrypt) {
   m_encrypt = encrypt;
}

//******************************************************************************

bool LocalDirectory::getEncrypt() const {
   return m_encrypt;
}

//******************************************************************************

void LocalDirectory::setActive(bool active) {
   m_active = active;
}

//******************************************************************************

bool LocalDirectory::getActive() const {
   return m_active;
}

//******************************************************************************

