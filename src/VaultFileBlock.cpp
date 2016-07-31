// Copyright Paul Dardeau, 2016
// VaultFileBlock.cpp

#include "VaultFileBlock.h"

using namespace lachepas;

//******************************************************************************

VaultFileBlock::VaultFileBlock() :
   m_vaultFileBlockId(-1),
   m_vaultFileId(-1),
   m_originFileSize(0),
   m_storedFileSize(0),
   m_blockSequenceNumber(0),
   m_padCharCount(0) {
}

//******************************************************************************

VaultFileBlock::VaultFileBlock(const VaultFileBlock& copy) :
   m_createTime(copy.m_createTime),
   m_modifyTime(copy.m_modifyTime),
   m_storedTime(copy.m_storedTime),
   m_uniqueIdentifier(copy.m_uniqueIdentifier),
   m_nodeDirectory(copy.m_nodeDirectory),
   m_nodeFile(copy.m_nodeFile),
   m_vaultFileBlockId(copy.m_vaultFileBlockId),
   m_vaultFileId(copy.m_vaultFileId),
   m_originFileSize(copy.m_originFileSize),
   m_storedFileSize(copy.m_storedFileSize),
   m_blockSequenceNumber(copy.m_blockSequenceNumber),
   m_padCharCount(copy.m_padCharCount) {
}

//******************************************************************************

VaultFileBlock& VaultFileBlock::operator=(const VaultFileBlock& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_createTime = copy.m_createTime;
   m_modifyTime = copy.m_modifyTime;
   m_storedTime = copy.m_storedTime;
   m_uniqueIdentifier = copy.m_uniqueIdentifier;
   m_nodeDirectory = copy.m_nodeDirectory;
   m_nodeFile = copy.m_nodeFile;
   m_vaultFileBlockId = copy.m_vaultFileBlockId;
   m_vaultFileId = copy.m_vaultFileId;
   m_originFileSize = copy.m_originFileSize;
   m_storedFileSize = copy.m_storedFileSize;
   m_blockSequenceNumber = copy.m_blockSequenceNumber;
   m_padCharCount = copy.m_padCharCount;

   return *this;
}

//******************************************************************************

void VaultFileBlock::setCreateTime(const chaudiere::DateTime& createTime) {
   m_createTime = createTime;
}

//******************************************************************************

const chaudiere::DateTime& VaultFileBlock::getCreateTime() const {
   return m_createTime;
}

//******************************************************************************

void VaultFileBlock::setModifyTime(const chaudiere::DateTime& modifyTime) {
   m_modifyTime = modifyTime;
}

//******************************************************************************

const chaudiere::DateTime& VaultFileBlock::getModifyTime() const {
   return m_modifyTime;
}

//******************************************************************************

void VaultFileBlock::setStoredTime(const chaudiere::DateTime& storedTime) {
   m_storedTime = storedTime;
}

//******************************************************************************

const chaudiere::DateTime& VaultFileBlock::getStoredTime() const {
   return m_storedTime;
}

//******************************************************************************

void VaultFileBlock::setVaultFileBlockId(int vaultFileBlockId) {
   m_vaultFileBlockId = vaultFileBlockId;
}

//******************************************************************************

int VaultFileBlock::getVaultFileBlockId() const {
   return m_vaultFileBlockId;
}

//******************************************************************************

void VaultFileBlock::setVaultFileId(int vaultFileId) {
   m_vaultFileId = vaultFileId;
}

//******************************************************************************

int VaultFileBlock::getVaultFileId() const {
   return m_vaultFileId;
}

//******************************************************************************

void VaultFileBlock::setOriginFileSize(int originFileSize) {
   m_originFileSize = originFileSize;
}

//******************************************************************************

int VaultFileBlock::getOriginFileSize() const {
   return m_originFileSize;
}

//******************************************************************************

void VaultFileBlock::setStoredFileSize(int storedFileSize) {
   m_storedFileSize = storedFileSize;
}

//******************************************************************************

int VaultFileBlock::getStoredFileSize() const {
   return m_storedFileSize;
}

//******************************************************************************

void VaultFileBlock::setBlockSequenceNumber(int blockSequenceNumber) {
   m_blockSequenceNumber = blockSequenceNumber;
}

//******************************************************************************

int VaultFileBlock::getBlockSequenceNumber() const {
   return m_blockSequenceNumber;
}

//******************************************************************************

void VaultFileBlock::setPadCharCount(int padCharCount) {
   m_padCharCount = padCharCount;
}

//******************************************************************************

int VaultFileBlock::getPadCharCount() const {
   return m_padCharCount;
}

//******************************************************************************

void VaultFileBlock::setUniqueIdentifier(const std::string& uniqueIdentifier) {
   m_uniqueIdentifier = uniqueIdentifier;
}

//******************************************************************************

const std::string& VaultFileBlock::getUniqueIdentifier() const {
   return m_uniqueIdentifier;
}

//******************************************************************************

void VaultFileBlock::setNodeDirectory(const std::string& nodeDirectory) {
   m_nodeDirectory = nodeDirectory;
}

//******************************************************************************

const std::string& VaultFileBlock::getNodeDirectory() const {
   return m_nodeDirectory;
}

//******************************************************************************

void VaultFileBlock::setNodeFile(const std::string& nodeFile) {
   m_nodeFile = nodeFile;
}

//******************************************************************************

const std::string& VaultFileBlock::getNodeFile() const {
   return m_nodeFile;
}

//******************************************************************************

