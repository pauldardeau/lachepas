// Copyright Paul Dardeau, 2016
// FileSync.cpp

#include "FileSync.h"

using namespace lachepas;

//******************************************************************************

FileSync::FileSync() :
   m_vaultFileId(-1) {
}

//******************************************************************************

FileSync::FileSync(const FileSync& copy) :
   m_syncTime(copy.m_syncTime),
   m_vaultFileId(copy.m_vaultFileId) {
}

//******************************************************************************

FileSync& FileSync::operator=(const FileSync& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_syncTime = copy.m_syncTime;
   m_vaultFileId = copy.m_vaultFileId;
   
   return *this;
}

//******************************************************************************

void FileSync::setVaultFileId(int vaultFileId) {
   m_vaultFileId = vaultFileId;
}

//******************************************************************************

int FileSync::getVaultFileId() const {
   return m_vaultFileId;
}

//******************************************************************************

