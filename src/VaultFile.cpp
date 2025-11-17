// Copyright Paul Dardeau, 2016
// VaultFile.cpp

#include "VaultFile.h"

using namespace std;

static const string DEFAULT_USER_PERMISSIONS   = "rw-";
static const string DEFAULT_GROUP_PERMISSIONS  = "rw-";
static const string DEFAULT_OTHER_PERMISSIONS  = "r--";

using namespace lachepas;

//******************************************************************************

VaultFile::VaultFile() :
   m_userPermissions(DEFAULT_USER_PERMISSIONS),
   m_groupPermissions(DEFAULT_GROUP_PERMISSIONS),
   m_otherPermissions(DEFAULT_OTHER_PERMISSIONS),
   m_vaultFileId(-1),
   m_localFileId(-1),
   m_vaultId(-1),
   m_originFileSize(0),
   m_blockCount(0) {
}

//******************************************************************************

VaultFile::VaultFile(const VaultFile& copy) :
   m_createTime(copy.m_createTime),
   m_modifyTime(copy.m_modifyTime),
   m_userPermissions(copy.m_userPermissions),
   m_groupPermissions(copy.m_groupPermissions),
   m_otherPermissions(copy.m_otherPermissions),
   m_vaultFileId(copy.m_vaultFileId),
   m_localFileId(copy.m_localFileId),
   m_vaultId(copy.m_vaultId),
   m_originFileSize(copy.m_originFileSize),
   m_blockCount(copy.m_blockCount) {
}

//******************************************************************************

VaultFile& VaultFile::operator=(const VaultFile& copy) {
   if (this == &copy) {
      return *this;
   }

   m_createTime = copy.m_createTime;
   m_modifyTime = copy.m_modifyTime;
   m_userPermissions = copy.m_userPermissions;
   m_groupPermissions = copy.m_groupPermissions;
   m_otherPermissions = copy.m_otherPermissions;
   m_vaultFileId = copy.m_vaultFileId;
   m_localFileId = copy.m_localFileId;
   m_vaultId = copy.m_vaultId;
   m_originFileSize = copy.m_originFileSize;
   m_blockCount = copy.m_blockCount;

   return *this;
}

//******************************************************************************

void VaultFile::setCreateTime(const chaudiere::DateTime& createTime) {
   m_createTime = createTime;
}

//******************************************************************************

const chaudiere::DateTime& VaultFile::getCreateTime() const {
   return m_createTime;
}

//******************************************************************************

void VaultFile::setModifyTime(const chaudiere::DateTime& modifyTime) {
   m_modifyTime = modifyTime;
}

//******************************************************************************

const chaudiere::DateTime& VaultFile::getModifyTime() const {
   return m_modifyTime;
}

//******************************************************************************

void VaultFile::setVaultFileId(int vaultFileId) {
   m_vaultFileId = vaultFileId;
}

//******************************************************************************

int VaultFile::getVaultFileId() const {
   return m_vaultFileId;
}

//******************************************************************************

void VaultFile::setLocalFileId(int localFileId) {
   m_localFileId = localFileId;
}

//******************************************************************************

int VaultFile::getLocalFileId() const {
   return m_localFileId;
}

//******************************************************************************

void VaultFile::setVaultId(int vaultId) {
   m_vaultId = vaultId;
}

//******************************************************************************

int VaultFile::getVaultId() const {
   return m_vaultId;
}

//******************************************************************************

void VaultFile::setOriginFileSize(int originFileSize) {
   m_originFileSize = originFileSize;
}

//******************************************************************************

int VaultFile::getOriginFileSize() const {
   return m_originFileSize;
}

//******************************************************************************

void VaultFile::setBlockCount(int blockCount) {
   m_blockCount = blockCount;
}

//******************************************************************************

int VaultFile::getBlockCount() const {
   return m_blockCount;
}

//******************************************************************************

void VaultFile::setUserPermissions(const FilePermissions& userPermissions) {
   m_userPermissions = userPermissions;
}

//******************************************************************************

const FilePermissions& VaultFile::getUserPermissions() const {
   return m_userPermissions;
}

//******************************************************************************

void VaultFile::setGroupPermissions(const FilePermissions& groupPermissions) {
   m_groupPermissions = groupPermissions;
}

//******************************************************************************

const FilePermissions& VaultFile::getGroupPermissions() const {
   return m_groupPermissions;
}

//******************************************************************************

void VaultFile::setOtherPermissions(const FilePermissions& otherPermissions) {
   m_otherPermissions = otherPermissions;
}

//******************************************************************************

const FilePermissions& VaultFile::getOtherPermissions() const {
   return m_otherPermissions;
}

//******************************************************************************

