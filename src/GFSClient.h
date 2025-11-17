// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSCLIENT_H
#define LACHEPAS_GFSCLIENT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "DateTime.h"
#include "GFSOptions.h"
#include "GFSExclusions.h"
#include "Vault.h"


namespace lachepas {

class DataAccess;
class LocalDirectory;
class StorageNode;
class VaultFile;

/**
 *
 */
class GFSClient {

private:
   /**
    *
    * @param nodeName
    * @return
    */
   int indexForStorageNode(const std::string& nodeName);

   /**
    *
    * @param dirPath
    * @return
    */
   int indexForLocalDirectory(const std::string& dirPath);

protected:

   /**
    *
    * @param dirPath
    * @param localDirectory
    */
   void scanDir(const std::string& dirPath,
                const LocalDirectory& localDirectory);

   /**
    *
    * @param dirPath
    */
   void scanProcessDirectory(const std::string& dirPath);

   /**
    *
    * @param dirPath
    * @param fileName
    * @param localDirectory
    */
   void scanProcessFile(const std::string& dirPath,
                        const std::string& fileName,
                        const LocalDirectory& localDirectory);

   /**
    *
    * @param filePath
    * @param fileContents
    * @return
    */
   bool readFile(const std::string& filePath, std::string& fileContents);

   /**
    *
    * @param numBlockFiles
    * @param filePath
    * @param encrypt
    * @param nodeBlockFlags
    * @param mapVaultIdToVaultFile
    * @param createTime
    * @param modifyTime
    * @return
    */
   int sendFile(int numBlockFiles,
                const std::string& filePath,
                bool encrypt,
                std::string& nodeBlockFlags,
                std::map<int, VaultFile>& mapVaultIdToVaultFile,
                chaudiere::DateTime& createTime,
                chaudiere::DateTime& modifyTime);

   /**
    *
    * @param encryptionKey
    * @param storageNode
    * @param sourceDirectory
    * @param targetDirectory
    * @return
    */
   bool fullRestore(const std::string& encryptionKey,
                    const StorageNode& storageNode,
                    const LocalDirectory& sourceDirectory,
                    const std::string& targetDirectory);

public:

   /**
    * Constructs a GFSClient instance using the specified options
    * @param gfsOptions
    * @see GFSOptions()
    */
   GFSClient(const GFSOptions& gfsOptions);

   /**
    * Destructor
    */
   ~GFSClient();

   /**
    *
    */
   void initializeDirectory();

   /**
    *
    */
   void sync();

   /**
    *
    */
   void listFiles();

   /**
    *
    * @return
    */
   int getNumberActiveStorageNodes() const;

   /**
    *
    * @param index
    * @return
    * @see StorageNode()
    */
   StorageNode& getActiveStorageNode(int index);

   /**
    *
    * @param vecStorageNodes
    * @return
    */
   bool getActiveStorageNodes(std::vector<StorageNode>& vecStorageNodes);

   /**
    *
    * @param vecStorageNodes
    * @return
    */
   bool getInactiveStorageNodes(std::vector<StorageNode>& vecStorageNodes);

   /**
    *
    */
   void listNodes();

   /**
    *
    * @return
    */
   bool listNodeDirFiles();

   /**
    *
    * @return
    */
   int getNumberActiveLocalDirectories() const;

   /**
    *
    * @param index
    * @return
    * @see LocalDirectory()
    */
   LocalDirectory& getActiveLocalDirectory(int index);

   /**
    *
    * @param vecLocalDirectories
    * @param refresh
    * @return
    */
   bool getActiveLocalDirectories(std::vector<LocalDirectory>& vecLocalDirectories,
                                  bool refresh=false);

   /**
    *
    * @param vecLocalDirectories
    * @return
    */
   bool getInactiveLocalDirectories(std::vector<LocalDirectory>& vecLocalDirectories);

   /**
    *
    * @param localDirectory
    * @return
    */
   bool updateLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @param localDirectory
    * @return
    */
   bool addLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @param localDirectory
    * @return
    */
   bool deactivateLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @param localDirectory
    * @return
    */
   bool activateLocalDirectory(LocalDirectory& localDirectory);

   /**
    *
    * @return
    */
   bool listNodeDirectories();

   /**
    *
    * @param storageNode
    * @return
    */
   bool updateStorageNode(StorageNode& storageNode);

   /**
    *
    * @param storageNode
    * @return
    */
   bool addStorageNode(StorageNode& storageNode);

   /**
    *
    * @param storageNode
    * @return
    */
   bool deactivateStorageNode(StorageNode& storageNode);

   /**
    *
    * @param storageNode
    * @return
    */
   bool activateStorageNode(StorageNode& storageNode);

   /**
    *
    */
   void addStorageNode();

   /**
    *
    */
   void removeStorageNode();

   /**
    *
    * @return
    */
   bool restore();

   /**
    *
    * @param dirPath
    * @return
    */
   bool restoreSubdirectory(const std::string& dirPath);

   /**
    *
    * @return
    */
   bool restoreFile();

   /**
    *
    * @param nodeName
    * @param directory
    * @param fileName
    * @param fileContents
    * @return
    */
   bool retrieveFile(const std::string& nodeName,
                     const std::string& directory,
                     const std::string& fileName,
                     std::string& fileContents);


private:
   std::map<std::string, Vault> m_mapNodeToVault;
   std::vector<StorageNode> m_activeNodes;
   std::vector<LocalDirectory> m_activeDirectories;
   GFSExclusions m_exclusions;
   std::string m_currentDir;
   std::string m_baseDir;
   std::string m_metaDataDBFile;
   std::string m_messagingService;
   DataAccess* m_dataAccess;
   GFSOptions m_gfsOptions;
   int m_localDirectoryId;
   int m_localDirectoryPathLength;
   bool m_debugPrint;
   bool m_previewOnly;

};

}

#endif

