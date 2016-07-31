// Copyright Paul Dardeau, 2016
// Vault.cpp

#include "Vault.h"

using namespace lachepas;

//******************************************************************************
      
Vault::Vault() :
   m_vaultId(-1),
   m_storageNodeId(-1),
   m_localDirectoryId(-1),
   m_compress(false),
   m_encrypt(false) {
}

//******************************************************************************

Vault::Vault(const Vault& copy) :
   m_vaultId(copy.m_vaultId),
   m_storageNodeId(copy.m_storageNodeId),
   m_localDirectoryId(copy.m_localDirectoryId),
   m_compress(copy.m_compress),
   m_encrypt(copy.m_encrypt) {
}

//******************************************************************************

Vault& Vault::operator=(const Vault& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_vaultId = copy.m_vaultId;
   m_storageNodeId = copy.m_storageNodeId;
   m_localDirectoryId = copy.m_localDirectoryId;
   m_compress = copy.m_compress;
   m_encrypt = copy.m_encrypt;
   
   return *this;
}

//******************************************************************************

void Vault::setVaultId(int vaultId) {
   m_vaultId = vaultId;
}

//******************************************************************************

int Vault::getVaultId() const {
   return m_vaultId;
}

//******************************************************************************

void Vault::setStorageNodeId(int storageNodeId) {
   m_storageNodeId = storageNodeId;
}

//******************************************************************************

int Vault::getStorageNodeId() const {
   return m_storageNodeId;
}

//******************************************************************************

void Vault::setLocalDirectoryId(int localDirectoryId) {
   m_localDirectoryId = localDirectoryId;
}

//******************************************************************************

int Vault::getLocalDirectoryId() const {
   return m_localDirectoryId;
}

//******************************************************************************

void Vault::setCompress(bool compress) {
   m_compress = compress;
}

//******************************************************************************

bool Vault::getCompress() const {
   return m_compress;
}

//******************************************************************************

void Vault::setEncrypt(bool encrypt) {
   m_encrypt = encrypt;
}

//******************************************************************************

bool Vault::getEncrypt() const {
   return m_encrypt;
}

//******************************************************************************

