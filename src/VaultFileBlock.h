// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_VAULTFILEBLOCK_H
#define LACHEPAS_VAULTFILEBLOCK_H

#include "DateTime.h"

namespace lachepas {

/**
 *
 */
class VaultFileBlock {
      
public:
   /**
    * Default constructor
    */
   VaultFileBlock();
      
   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   VaultFileBlock(const VaultFileBlock& copy);
      
   /**
    * Copy operator
    * @param copy the source of the copy
    * @return the target of the copy
    */
   VaultFileBlock& operator=(const VaultFileBlock& copy);
      
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
    * @param storedTime
    * @see chaudiere::DateTime()
    */
   void setStoredTime(const chaudiere::DateTime& storedTime);
      
   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getStoredTime() const;
      
   /**
    *
    * @param uniqueIdentifier
    */
   void setUniqueIdentifier(const std::string& uniqueIdentifier);
      
   /**
    *
    * @return
    */
   const std::string& getUniqueIdentifier() const;

   /**
    *
    * @param nodeDirectory
    */
   void setNodeDirectory(const std::string& nodeDirectory);
      
   /**
    *
    * @return
    */
   const std::string& getNodeDirectory() const;

   /**
    *
    * @param nodeFile
    */
   void setNodeFile(const std::string& nodeFile);
      
   /**
    *
    * @return
    */
   const std::string& getNodeFile() const;

   /**
    *
    * @param vaultFileBlockId
    */
   void setVaultFileBlockId(int vaultFileBlockId);
      
   /**
    *
    * @return
    */
   int getVaultFileBlockId() const;

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
    * @param storedFileSize
    */
   void setStoredFileSize(int storedFileSize);
      
   /**
    *
    * @return
    */
   int getStoredFileSize() const;

   /**
    *
    * @param blockSequenceNumber
    */
   void setBlockSequenceNumber(int blockSequenceNumber);
      
   /**
    *
    * @return
    */
   int getBlockSequenceNumber() const;

   /**
    *
    * @param padCharCount
    */
   void setPadCharCount(int padCharCount);
      
   /**
    *
    * @return
    */
   int getPadCharCount() const;


private:
   chaudiere::DateTime m_createTime;
   chaudiere::DateTime m_modifyTime;
   chaudiere::DateTime m_storedTime;
   std::string m_uniqueIdentifier;
   std::string m_nodeDirectory;
   std::string m_nodeFile;
   int m_vaultFileBlockId;
   int m_vaultFileId;
   int m_originFileSize;
   int m_storedFileSize;
   int m_blockSequenceNumber;
   int m_padCharCount;

};

}

#endif

