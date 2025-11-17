// Copyright Paul Dardeau, 2016
// LocalFile.cpp

#include "LocalFile.h"

using namespace std;
using namespace lachepas;

//******************************************************************************

LocalFile::LocalFile() :
   m_localFileId(-1),
   m_localDirectoryId(-1) {
}

//******************************************************************************

LocalFile::LocalFile(const LocalFile& copy) :
   m_filePath(copy.m_filePath),
   m_localFileId(copy.m_localFileId),
   m_localDirectoryId(copy.m_localDirectoryId),
   m_createTime(copy.m_createTime),
   m_modifyTime(copy.m_modifyTime),
   m_scanTime(copy.m_scanTime),
   m_copyTime(copy.m_copyTime) {
}

//******************************************************************************

LocalFile& LocalFile::operator=(const LocalFile& copy) {
   if (this == &copy) {
      return *this;
   }

   m_filePath = copy.m_filePath;
   m_localFileId = copy.m_localFileId;
   m_localDirectoryId = copy.m_localDirectoryId;
   m_createTime = copy.m_createTime;
   m_modifyTime = copy.m_modifyTime;
   m_scanTime = copy.m_scanTime;
   m_copyTime = copy.m_copyTime;

   return *this;
}

//******************************************************************************

void LocalFile::setFilePath(const string& filePath) {
   m_filePath = filePath;
}

//******************************************************************************

const string& LocalFile::getFilePath() const {
   return m_filePath;
}

//******************************************************************************

void LocalFile::setLocalFileId(int localFileId) {
   m_localFileId = localFileId;
}

//******************************************************************************

int LocalFile::getLocalFileId() const {
   return m_localFileId;
}

//******************************************************************************

void LocalFile::setLocalDirectoryId(int localDirectoryId) {
   m_localDirectoryId = localDirectoryId;
}

//******************************************************************************

int LocalFile::getLocalDirectoryId() const {
   return m_localDirectoryId;
}

//******************************************************************************

void LocalFile::setCreateTime(const chaudiere::DateTime& createTime) {
   m_createTime = createTime;
}

//******************************************************************************

const chaudiere::DateTime& LocalFile::getCreateTime() const {
   return m_createTime;
}

//******************************************************************************

void LocalFile::setModifyTime(const chaudiere::DateTime& modifyTime) {
   m_modifyTime = modifyTime;
}

//******************************************************************************

const chaudiere::DateTime& LocalFile::getModifyTime() const {
   return m_modifyTime;
}

//******************************************************************************

void LocalFile::setScanTime(const chaudiere::DateTime& scanTime) {
   m_scanTime = scanTime;
}

//******************************************************************************

const chaudiere::DateTime& LocalFile::getScanTime() const {
   return m_scanTime;
}

//******************************************************************************

void LocalFile::setCopyTime(const chaudiere::DateTime& copyTime) {
   m_copyTime = copyTime;
}

//******************************************************************************

const chaudiere::DateTime& LocalFile::getCopyTime() const {
   return m_copyTime;
}

//******************************************************************************

