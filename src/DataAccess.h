// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_DATAACCESS_H
#define LACHEPAS_DATAACCESS_H

#include <string>
#include <vector>
#include <memory>

#include "LocalDirectory.h"
#include "LocalFile.h"
#include "StorageNode.h"
#include "Vault.h"
#include "VaultFile.h"
#include "VaultFileBlock.h"
#include "Database.h"

/**
 */

namespace lachepas {

class DataAccess {

public:
   /**
    *
    * @param filePath
    */
   explicit DataAccess(const std::string& filePath);

   /**
    * Destructor
    */
   ~DataAccess();

   /**
    *
    * @return
    */
   bool open();

   /**
    *
    * @param sql
    * @return
    */
   bool createTable(const std::string& sql);

   /**
    *
    * @return
    */
   bool createTables();

   /**
    *
    * @return
    */
   bool haveTables();

   /**
    *
    * @return
    */
   bool commit();

   /**
    *
    * @return
    */
   bool rollback();

   /**
    *
    * @param storageNode
    * @return
    * @see StorageNode()
    */
   bool insertStorageNode(StorageNode& storageNode);

   /**
    *
    * @param localDirectory
    * @return
    * @see LocalDirectory()
    */
   bool insertLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @param localFile
    * @return
    * @see LocalFile()
    */
   bool insertLocalFile(LocalFile& localFile);

   /**
    *
    * @param vault
    * @return
    * @see Vault()
    */
   bool insertVault(Vault& vault);

   /**
    *
    * @param vaultFile
    * @return
    * @see VaultFile()
    */
   bool insertVaultFile(VaultFile& vaultFile);

   /**
    *
    * @param vaultFileBlock
    * @return
    * @see VaultFileBlock()
    */
   bool insertVaultFileBlock(VaultFileBlock& vaultFileBlock);

   /**
    *
    * @param storageNode
    * @return
    * @see StorageNode()
    */
   bool updateStorageNode(StorageNode& storageNode);

   /**
    *
    * @param localDirectory
    * @return
    * @see LocalDirectory()
    */
   bool updateLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @param localFile
    * @return
    * @see LocalFile()
    */
   bool updateLocalFile(LocalFile& localFile);

   /**
    *
    * @param vault
    * @return
    * @see Vault()
    */
   bool updateVault(Vault& vault);

   /**
    *
    * @param vaultFile
    * @return
    * @see VaultFile()
    */
   bool updateVaultFile(VaultFile& vaultFile);

   /**
    *
    * @param vaultFileBlock
    * @return
    * @see VaultFileBlock()
    */
   bool updateVaultFileBlock(VaultFileBlock& vaultFileBlock);

   /**
    *
    * @param storageNode
    * @return
    * @see StorageNode()
    */
   bool deleteActiveStorageNode(StorageNode& storageNode);

   /**
    *
    * @param localDirectory
    * @return
    * @see LocalDirectory()
    */
   bool deleteLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @param localFile
    * @return
    * @see LocalFile()
    */
   bool deleteLocalFile(LocalFile& localFile);

   /**
    *
    * @param vault
    * @return
    * @see Vault()
    */
   bool deleteVault(Vault& vault);

   /**
    *
    * @param vaultFile
    * @return
    * @see VaultFile()
    */
   bool deleteVaultFile(VaultFile& vaultFile);

   /**
    *
    * @param vaultFileBlock
    * @return
    * @see VaultFileBlock()
    */
   bool deleteVaultFileBlock(VaultFileBlock& vaultFileBlock);

   /**
    *
    * @param listNodes
    * @return
    * @see StorageNode()
    */
   bool getActiveStorageNodes(std::vector<StorageNode>& listNodes);

   /**
    *
    * @param listNodes
    * @return
    * @see StorageNode()
    */
   bool getInactiveStorageNodes(std::vector<StorageNode>& listNodes);

   /**
    *
    * @param listDirectories
    * @return
    * @see LocalDirectory()
    */
   bool getActiveLocalDirectories(std::vector<LocalDirectory>& listDirectories);

   /**
    *
    * @param listDirectories
    * @return
    * @see LocalDirectory()
    */
   bool getInactiveLocalDirectories(std::vector<LocalDirectory>& listDirectories);

   /**
    *
    * @param localDirectoryId
    * @param filePath
    * @param localFile
    * @return
    * @see LocalFile()
    */
   bool getLocalFile(int localDirectoryId,
                     const std::string& filePath,
                     LocalFile& localFile);

   /**
    *
    * @param localDirectoryId
    * @param listFiles
    * @return
    * @see LocalFile()
    */
   bool getLocalFilesForDirectory(int localDirectoryId,
                                  std::vector<LocalFile>& listFiles);

   /**
    *
    * @param storageNodeId
    * @param localDirectoryId
    * @param vault
    * @return
    * @see Vault()
    */
   bool getVault(int storageNodeId, int localDirectoryId, Vault& vault);

   /**
    *
    * @param vaultId
    * @param localFileId
    * @param vaultFile
    * @return
    * @see VaultFile()
    */
   bool getVaultFile(int vaultId, int localFileId, VaultFile& vaultFile);

   /**
    *
    * @param vaultFileId
    * @param listFileBlocks
    * @return
    * @see VaultFileBlock()
    */
   bool getBlocksForVaultFile(int vaultFileId,
                              std::vector<VaultFileBlock>& listFileBlocks);


protected:
   bool getStorageNodes(const std::string& query,
                        std::vector<StorageNode>& listNodes);
   bool getLocalDirectories(const std::string& query,
                            std::vector<LocalDirectory>& listDirectories);


private:
   chapeau::Database* m_dbConnection;
   std::string m_dbFilePath;
   bool m_debugPrint;

   // not available
   DataAccess(const DataAccess&);
   DataAccess& operator=(const DataAccess&);
};

}

#endif

