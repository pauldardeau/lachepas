// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_VAULTFILE_H
#define LACHEPAS_VAULTFILE_H

#include "DateTime.h"
#include "FilePermissions.h"

namespace lachepas {

/**
 *
 */
class VaultFile {

public:
   /**
    * Default constructor
    */
   VaultFile();

   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   VaultFile(const VaultFile& copy);

   /**
    * Copy operator
    * @param copy the source of the copy
    * @return the target of the copy
    */
   VaultFile& operator=(const VaultFile& copy);

   /**
    *
    * @param createTime
    * @see chaudiere::DateTime()
    */
   void setCreateTime(const chaudiere::DateTime& createTime);

   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getCreateTime() const;

   /**
    *
    * @param modifyTime
    * @see chaudiere::DateTime()
    */
   void setModifyTime(const chaudiere::DateTime& modifyTime);

   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getModifyTime() const;

   /**
    *
    * @param vaultFileId
    */
   void setVaultFileId(int vaultFileId);

   /**
    *
    * @return
    */
   int getVaultFileId() const;

   /**
    *
    * @param localFileId
    */
   void setLocalFileId(int localFileId);

   /**
    *
    * @return
    */
   int getLocalFileId() const;

   /**
    *
    * @param vaultId
    */
   void setVaultId(int vaultId);

   /**
    *
    * @return
    */
   int getVaultId() const;

   /**
    *
    * @param originFileSize
    */
   void setOriginFileSize(int originFileSize);

   /**
    *
    * @return
    */
   int getOriginFileSize() const;

   /**
    *
    * @param blockCount
    */
   void setBlockCount(int blockCount);

   /**
    *
    * @return
    */
   int getBlockCount() const;

   /**
    *
    * @param userPermissions
    */
   void setUserPermissions(const FilePermissions& userPermissions);

   /**
    *
    * @return
    */
   const FilePermissions& getUserPermissions() const;

   /**
    *
    * @param groupPermissions
    */
   void setGroupPermissions(const FilePermissions& groupPermissions);

   /**
    *
    * @return
    */
   const FilePermissions& getGroupPermissions() const;

   /**
    *
    * @param otherPermissions
    */
   void setOtherPermissions(const FilePermissions& otherPermissions);

   /**
    *
    * @return
    */
   const FilePermissions& getOtherPermissions() const;


private:
   chaudiere::DateTime m_createTime;
   chaudiere::DateTime m_modifyTime;
   FilePermissions m_userPermissions;
   FilePermissions m_groupPermissions;
   FilePermissions m_otherPermissions;
   int m_vaultFileId;
   int m_localFileId;
   int m_vaultId;
   int m_originFileSize;
   int m_blockCount;

};

}

#endif

