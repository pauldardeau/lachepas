// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_FILEPERMISSIONS_H
#define LACHEPAS_FILEPERMISSIONS_H

#include <string>

namespace lachepas {

class FilePermissions {

private:
   std::string m_permissions;
      
public:
   FilePermissions();
   FilePermissions(const std::string& permissions);
   FilePermissions(const FilePermissions& copy);
   ~FilePermissions();
    
   FilePermissions& operator=(const FilePermissions& copy);
      
   const std::string& getPermissionsString() const;
   void setPermissionString(const std::string& permissions);
      
   short octalValue() const;
      
   bool hasReadPermission() const;
   bool hasWritePermission() const;
   bool hasExecutePermission() const;
      
   void setReadPermission();
   void setWritePermission();
   void setExecutePermission();
      
   void clearReadPermission();
   void clearWritePermission();
   void clearExecutePermissions();
   void clearAllPermissions();
};

}

#endif

