// Copyright Paul Dardeau, 2016
// FilePermissions.cpp

#include "FilePermissions.h"

using namespace std;

static const string DEFAULT_PERMISSIONS  = "rw-";
static const string NO_PERMISSIONS       = "---";

using namespace lachepas;

//******************************************************************************

FilePermissions::FilePermissions() :
   m_permissions(DEFAULT_PERMISSIONS) {
}

//******************************************************************************

FilePermissions::FilePermissions(const string& permissions) :
   m_permissions(permissions) {
}

//******************************************************************************

FilePermissions::FilePermissions(const FilePermissions& copy) :
   m_permissions(copy.m_permissions) {
}

//******************************************************************************

FilePermissions::~FilePermissions() {
}

//******************************************************************************

FilePermissions& FilePermissions::operator=(const FilePermissions& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_permissions = copy.m_permissions;
   
   return *this;
}

//******************************************************************************

const string& FilePermissions::getPermissionsString() const {
   return m_permissions;
}

//******************************************************************************

void FilePermissions::setPermissionString(const string& permissions) {
   m_permissions = permissions;
}

//******************************************************************************

short FilePermissions::octalValue() const {
   short octalValue = 0;
   
   if (hasReadPermission()) {
      octalValue += 4;
   }
   
   if (hasWritePermission()) {
      octalValue += 2;
   }
   
   if (hasExecutePermission()) {
      octalValue += 1;
   }
   
   return octalValue;
}

//******************************************************************************

bool FilePermissions::hasReadPermission() const {
   return m_permissions[0] == 'r';
}

//******************************************************************************

bool FilePermissions::hasWritePermission() const {
   return m_permissions[1] == 'w';
}

//******************************************************************************

bool FilePermissions::hasExecutePermission() const {
   return m_permissions[2] == 'x';
}

//******************************************************************************

void FilePermissions::setReadPermission() {
   m_permissions[0] = 'r';
}

//******************************************************************************

void FilePermissions::setWritePermission() {
   m_permissions[1] = 'w';
}

//******************************************************************************

void FilePermissions::setExecutePermission() {
   m_permissions[2] = 'x';
}

//******************************************************************************

void FilePermissions::clearReadPermission() {
   m_permissions[0] = '-';
}

//******************************************************************************

void FilePermissions::clearWritePermission() {
   m_permissions[1] = '-';
}

//******************************************************************************

void FilePermissions::clearExecutePermissions() {
   m_permissions[2] = '-';
}

//******************************************************************************

void FilePermissions::clearAllPermissions() {
   m_permissions = NO_PERMISSIONS;
}

//******************************************************************************

