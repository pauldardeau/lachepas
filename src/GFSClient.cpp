// Copyright Paul Dardeau, 2016
// GFSClient.cpp

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include <algorithm>
#include <string>


#include "GFSClient.h"
#include "Logger.h"
#include "DataAccess.h"
#include "OSUtils.h"
#include "Messaging.h"
#include "Message.h"
#include "GFSMessage.h"
#include "GFSMessageCommands.h"
#include "BasicException.h"
#include "aes256.h"
#include "IniReader.h"
#include "GFS.h"
#include "Encryption.h"
#include "StringTokenizer.h"
#include "FilePermissions.h"

#define PAGE_SIZE_2X   8192
#define PAGE_SIZE_3X  12288
#define PAGE_SIZE_4X  16384
#define PAGE_SIZE_5X  20480
#define PAGE_SIZE_6X  24576
#define PAGE_SIZE_7X  28672
#define PAGE_SIZE_8X  32768

#define FILE_BLOCK_SIZE PAGE_SIZE_4X

static const std::string DB_FILE                = "gfs_db.sqlite3";

static const std::string EMPTY_STRING           = "";
static const std::string SINGLE_QUOTE           = "'";

static const char FLAG_BLOCK_ALL        = 'A'; // process all blocks
static const char FLAG_BLOCK_NONE       = 'N'; // process no blocks
static const char FLAG_BLOCK_SELECTIVE  = 'S'; // process blocks individually (as needed)


//******************************************************************************

using namespace lachepas;
using namespace chaudiere;
using namespace tonnerre;

//******************************************************************************

std::string Decrypt(const std::string& cipherText,
                    const std::string& encryptionKey) {
   //TODO: implement Decrypt
   return EMPTY_STRING;
}

//******************************************************************************

std::string Encrypt(const std::string& fileContents,
                    const std::string& encryptionKey,
                    int& padChars) {
   aes256_context ctx;
   bool encrypting = true;
   int offset = 0;
   uint8_t key[32];
   uint8_t buffer[16];
   char cipherChars[17];
   std::string cipherText;
   
   if (encryptionKey.length() < 32) {
      ::printf("error: encryption key must be 32 bytes\n");
      return EMPTY_STRING;
   }

   int remainingBytes = fileContents.length();

   ::memset(key, 0, sizeof(key));
   ::memset(buffer, 0, sizeof(buffer));
   ::memset(cipherChars, 0, sizeof(cipherChars));

   padChars = 0;
   
   ::strncpy((char*)key, encryptionKey.c_str(), 32);
   
   while (encrypting) {
      ::aes256_init(&ctx, key);
      
      if (remainingBytes > 15) {
         ::memcpy(buffer, fileContents.c_str() + offset, 16);
      } else {
         ::memcpy(buffer, fileContents.c_str() + offset, remainingBytes);
         if (remainingBytes < 16) {
            // add padding
            const int numPadChars = 16 - remainingBytes;
            ::memset(buffer+remainingBytes, 0, numPadChars);
            padChars = numPadChars;
         }
      }
      
      ::aes256_encrypt_ecb(&ctx, buffer);
      
      ::memcpy(cipherChars, buffer, 16);
      cipherChars[16] = '\0';
      
      cipherText += std::string(cipherChars);
      
      ::aes256_done(&ctx);
      
      remainingBytes -= 16;
      offset += 16;
      
      if (remainingBytes <= 0) {
         encrypting = false;
      }
   }
   
   return cipherText;
}

//******************************************************************************

bool TimeTToDateTime(time_t t, DateTime& dateTime) {
   time_t timeValue = t;
   struct tm* tm = ::localtime(&timeValue);
   
   if (tm != nullptr) {
      DateTime timestamp(tm->tm_year + 1900,
                         tm->tm_mon + 1,
                         tm->tm_mday,
                         tm->tm_hour,
                         tm->tm_min,
                         tm->tm_sec);
      dateTime = timestamp;
      return true;
   }
   
   return false;
}

//******************************************************************************

bool TimeSpecToDateTime(struct timespec& ts, DateTime& dateTime) {
   return TimeTToDateTime(ts.tv_sec, dateTime);
}

//******************************************************************************

GFSClient::GFSClient(const GFSOptions& gfsOptions) :
                     m_currentDir(OSUtils::getCurrentDirectory()),
                     m_metaDataDBFile(DB_FILE),
                     m_dataAccess(nullptr),
                     m_gfsOptions(gfsOptions),
                     m_localDirectoryId(-1),
                     m_localDirectoryPathLength(0),
                     m_debugPrint(false),
                     m_previewOnly(false) {
   ::srand(::time(nullptr));

   m_baseDir = m_currentDir;
   
   if (gfsOptions.getDebugMode()) {
      m_debugPrint = true;
   }
   
   if (m_debugPrint) {
      Logger::debug(std::string("baseDir = '") +
                    m_baseDir +
                    SINGLE_QUOTE);
   }
   
   const std::string& configFile = gfsOptions.getConfigFile();
   
   if (!configFile.empty()) {
      if (OSUtils::pathExists(configFile)) {
         try {
            Logger::debug("reading configuration file");
            Messaging::initialize(configFile);
         } catch (const BasicException& be) {
            Logger::error("unable to read configuration file");
            return;
         }
         
         Logger::info(std::string("initializing database with file: '") +
                      m_metaDataDBFile +
                      SINGLE_QUOTE);
         
         m_dataAccess = new DataAccess(m_metaDataDBFile);
         if (m_dataAccess->open()) {
            Logger::info("successfully opened database");
            
            if (m_dataAccess->getActiveStorageNodes(m_activeNodes)) {
               if (m_activeNodes.empty()) {
                  Logger::warning("no storage nodes defined");
               } else {
                  Logger::debug("found 1 or more storage nodes");
               }
            } else {
               Logger::error("unable to retrieve storage nodes");
            }
            
            if (m_dataAccess->getActiveLocalDirectories(m_activeDirectories)) {
               if (m_activeDirectories.empty()) {
                  Logger::warning("no local directories defined");
               } else {
                  Logger::debug("found 1 or more local directories");
               }
            } else {
               Logger::error("unable to retrieve local directories");
            }
         } else {
            Logger::error("unable to open database");
         }
      } else {
         Logger::error(std::string("config file doesn't exist '") +
                       configFile +
                       SINGLE_QUOTE);
      }
   } else {
      Logger::error("missing config file");
   }
}

//******************************************************************************

GFSClient::~GFSClient() {
}

//******************************************************************************
     
void GFSClient::initializeDirectory() {
   if (!m_dataAccess) {
      Logger::error("no database connection");
      return;
   }
   
   const std::string& directory = m_gfsOptions.getDirectory();
   
   if (!directory.empty()) {
      bool dirInitialized = false;
      // has it already been initialized?
      if (!m_activeDirectories.empty()) {
         const int dirIndex = indexForLocalDirectory(directory);
         if (dirIndex > -1) {
            dirInitialized = true;
            Logger::debug("directory already initialized");
         }
      }
      
      if (!dirInitialized) {
         // does the directory exist?
         if (OSUtils::directoryExists(directory)) {
            LocalDirectory localDirectory;
            localDirectory.setDirectoryPath(directory);
            localDirectory.setRecurse(m_gfsOptions.getRecurse());
            localDirectory.setCompress(m_gfsOptions.getUseCompression());
            localDirectory.setEncrypt(m_gfsOptions.getUseEncryption());
            localDirectory.setCopyCount(m_gfsOptions.getCopyCount());
            
            if (m_dataAccess->insertLocalDirectory(localDirectory)) {
               if (localDirectory.getLocalDirectoryId() > -1) {
                  m_activeDirectories.push_back(localDirectory);
               } else {
                  Logger::error("directory id missing from database insert");
               }
            } else {
               Logger::error("unable to add directory to database");
            }
         } else {
            Logger::error("directory does not exist");
         }
      }
   } else {
      Logger::error("unable to initialize directory -- none specified");
   }
}

//******************************************************************************

int GFSClient::sendFile(int numBlockFiles,
                        const std::string& filePath,
                        bool encrypt,
                        std::string& nodeBlockFlags,
                        std::map<int, VaultFile>& mapVaultIdToVaultFile,
                        chaudiere::DateTime& createTime,
                        chaudiere::DateTime& modifyTime) {
   FILE* f = nullptr;
   char fileBuffer[FILE_BLOCK_SIZE];
   std::string fileContents;
   
   if (numBlockFiles > 1) {
      f = ::fopen(filePath.c_str(), "rb");
      if (f == nullptr) {
         Logger::error(std::string("unable to open file '") +
                       filePath +
                       SINGLE_QUOTE);
         return 0;
      }
   }
   
   // are we using encryption, if so, we need an encryption key
   std::string encryptionKey;
   if (encrypt) {
      encryptionKey = m_gfsOptions.getEncryptionKey();
   }

   std::string b64FileContents;
   std::string localUniqueIdentifier;
   int numNodeBlocksCopied = 0;
   const int blockSize = FILE_BLOCK_SIZE;
   
   for (int i = 0; i < numBlockFiles; ++i) {
      
      int originBlockSize = 0;
      int padCharCount = 0;
   
      if (numBlockFiles == 1) {
         fileContents = EMPTY_STRING;

         if (!readFile(filePath, fileContents)) {
            Logger::error(std::string("unable to read file '") +
                          filePath +
                          SINGLE_QUOTE);
            return numNodeBlocksCopied;
         }
         
         originBlockSize = fileContents.size();
         
         if (originBlockSize > 0) {
            if (encrypt) {
               padCharCount = 0;
            
               const std::string encryptedFileContents =
                  Encrypt(fileContents,
                          encryptionKey,
                          padCharCount);

               b64FileContents =
                  Encryption::base64Encode((const unsigned char*) encryptedFileContents.data(),
                                           encryptedFileContents.size());
                                           
               localUniqueIdentifier =
                  GFS::uniqueIdentifierForString(b64FileContents);

            } else {
               b64FileContents =
                  Encryption::base64Encode((const unsigned char*) fileContents.data(),
                                           fileContents.size());

               localUniqueIdentifier =
                  GFS::uniqueIdentifierForString(b64FileContents);
            }
         }
      } else {
         ::memset(fileBuffer, 0, sizeof(fileBuffer));
         const size_t bytesRead = ::fread(fileBuffer, 1, blockSize, f);
         originBlockSize = bytesRead;
         
         if (bytesRead < blockSize) {
            if (i < (numBlockFiles-1)) {
               Logger::error(std::string("error reading file '") +
                             filePath +
                             SINGLE_QUOTE);
               ::fclose(f);
               return numNodeBlocksCopied;
            }
         }
         
         if (encrypt) {
            padCharCount = 0;
            
            const std::string encryptedFileContents =
               Encrypt(fileContents,
                       encryptionKey,
                       padCharCount);

            b64FileContents =
               Encryption::base64Encode((const unsigned char*) encryptedFileContents.data(),
                                        encryptedFileContents.size());
                       
            localUniqueIdentifier =
               GFS::uniqueIdentifierForString(b64FileContents);
            
         } else {
            b64FileContents =
               Encryption::base64Encode((const unsigned char*) fileBuffer,
                                        bytesRead);

            localUniqueIdentifier =
               GFS::uniqueIdentifierForString(b64FileContents);
         }
      }
      
      if ((originBlockSize == 0) || (b64FileContents.size() == 0)) {
         // nothing to send
         return numNodeBlocksCopied;
      }
      
      // for each node
      auto itNodeList = m_activeNodes.cbegin();
      const auto itNodeListEnd = m_activeNodes.cend();
      
      for (int j = 0; itNodeList != itNodeListEnd; ++itNodeList, ++j) {
         const char nodeFlag = nodeBlockFlags[j];
         bool addBlockToNode = false;
         
         if (nodeFlag == FLAG_BLOCK_NONE) {
            continue;
         } else if (nodeFlag == FLAG_BLOCK_ALL) {
            // all blocks are being added for this node, no
            // comparisons/checks are needed
            addBlockToNode = true;
         }
         
         const StorageNode& node = *itNodeList;
         const std::string& nodeName = node.getNodeName();
         
         auto itVault = m_mapNodeToVault.find(nodeName);
         if (itVault == m_mapNodeToVault.end()) {
            nodeBlockFlags[j] = FLAG_BLOCK_NONE;
            continue;
         }
      
         Vault& vault = (*itVault).second;

         // determine if this block exists on storage node
         
         if (nodeFlag == FLAG_BLOCK_SELECTIVE) {
            // if the unique identifier on this block matches what's last
            // stored on this node, then we don't need to send it
            // (i.e., it's still current)
            //TODO: check if unique identifier of file block matches what may
            // be stored on the node vault
            bool uniqueIdentifierMatches = false;
            
            if (uniqueIdentifierMatches) {
               addBlockToNode = false;
            } else {
               addBlockToNode = true;
            }
         }
      
         if (addBlockToNode) {
            Message message(GFSMessageCommands::MSG_FILE_ADD, MessageType::MessageTypeText);
            message.setTextPayload(b64FileContents);
            const int storedBlockSize = b64FileContents.size();
            GFSMessage::setStoredFileSize(message, storedBlockSize);
            
            GFSMessage::setFile(message, localUniqueIdentifier);
            GFSMessage::setUniqueIdentifier(message, localUniqueIdentifier);
   
            Message response;
            bool msgSent;
            
            try {
               msgSent = message.send(nodeName, response);
            } catch (const BasicException& be) {
               msgSent = false;
            }
            
            if (msgSent) {
               if (GFSMessage::getRC(response)) {
                  ++numNodeBlocksCopied;
                  
                  if (GFSMessage::hasUniqueIdentifier(response)) {
                     const std::string& nodeUniqueIdentifier =
                        GFSMessage::getUniqueIdentifier(response);
                     
                     if (nodeUniqueIdentifier == localUniqueIdentifier) {
                        if (GFSMessage::hasDirectory(response) &&
                            GFSMessage::hasFile(response)) {
                           const std::string& directory =
                              GFSMessage::getDirectory(response);
                           const std::string& file =
                              GFSMessage::getFile(response);
                           
                           const int vaultId = vault.getVaultId();
                           auto it = mapVaultIdToVaultFile.find(vaultId);
                           if (it != mapVaultIdToVaultFile.cend()) {
                              const VaultFile& vaultFile = (*it).second;
                              const int vaultFileId = vaultFile.getVaultFileId();
                              
                              chaudiere::DateTime storedTime;
                              
                              VaultFileBlock vaultFileBlock;
                              vaultFileBlock.setCreateTime(createTime);
                              vaultFileBlock.setModifyTime(modifyTime);
                              vaultFileBlock.setStoredTime(storedTime);
                              vaultFileBlock.setUniqueIdentifier(nodeUniqueIdentifier);
                              vaultFileBlock.setNodeDirectory(directory);
                              vaultFileBlock.setNodeFile(file);
                              vaultFileBlock.setVaultFileId(vaultFileId);
                              vaultFileBlock.setOriginFileSize(originBlockSize);
                              vaultFileBlock.setStoredFileSize(storedBlockSize);
                              vaultFileBlock.setBlockSequenceNumber(i+1);
                              vaultFileBlock.setPadCharCount(padCharCount);
                           
                              if (m_dataAccess->insertVaultFileBlock(vaultFileBlock)) {
                                 //Logger::debug("inserted vault file block");
                              } else {
                                 Logger::error("unable to insert vault file block");
                                 return numNodeBlocksCopied;
                              }
                           } else {
                              Logger::error("unable to find vault file using vault id");
                           }
                        } else {
                           Logger::error("addFile - response missing file or directory");
                        }
                     } else {
                        Logger::error("local unique identifier mismatch with node unique identifier");
                        ::printf("local identifier='%s'\n", localUniqueIdentifier.c_str());
                        ::printf("node identifier='%s'\n", nodeUniqueIdentifier.c_str());
                        return numNodeBlocksCopied;
                     }
                  }
               } else {
                  if (GFSMessage::hasError(response)) {
                     const std::string& error = GFSMessage::getError(response);
                     Logger::error(std::string("error from node: '") +
                                   error +
                                   SINGLE_QUOTE);
                     return numNodeBlocksCopied;
                  } else {
                     Logger::error("request failed, no error provided by node");
                  }
               }
            } else {
               Logger::error(std::string("unable to send message to service '") +
                             nodeName +
                             SINGLE_QUOTE);
               return numNodeBlocksCopied;
            }
         } // if addBlockToNode
      } // for each node
   } // for each block in file
   
   if (f != nullptr) {
      ::fclose(f);
   }
   
   return numNodeBlocksCopied;
}

//******************************************************************************

void GFSClient::scanProcessDirectory(const std::string& dirPath) {
   //Logger::debug(std::string("scanProcessDirectory: ") + dirPath);
}

//******************************************************************************

bool GFSClient::readFile(const std::string& filePath,
                         std::string& fileContents) {
   return GFS::readFile(filePath, fileContents);
}

//******************************************************************************

void GFSClient::scanProcessFile(const std::string& dirPath,
                                const std::string& fileName,
                                const LocalDirectory& localDirectory) {
   char path[PATH_MAX];
   const int pathLength = ::snprintf(path,
                                     PATH_MAX,
                                     "%s/%s",
                                     dirPath.c_str(),
                                     fileName.c_str());
   const std::string fullFilePath(path);
   
   if (!m_previewOnly) {
      Logger::debug(fullFilePath);
   }
   
   if (pathLength >= PATH_MAX) {
      ::fprintf(stderr, "Path length too long: %s\n", path);
   } else {
      struct stat st;
      const int rc = ::stat(path, &st);
      if (rc == 0) {
         const std::string relativeFilePath =
            fullFilePath.substr(m_localDirectoryPathLength);
         
         const off_t fileSize = st.st_size;
         chaudiere::DateTime createTime;
         chaudiere::DateTime modifyTime;
         FilePermissions userPermissions;
         FilePermissions groupPermissions;
         FilePermissions otherPermissions;

         const mode_t fileMode = st.st_mode;

         // --------  user --------
         // user read
         if ((fileMode & S_IRUSR) == S_IRUSR) {
            userPermissions.setReadPermission();
         }
         
         // user write
         if ((fileMode & S_IWUSR) == S_IWUSR) {
            userPermissions.setWritePermission();
         }
         
         // user execute
         if ((fileMode & S_IXUSR) == S_IXUSR) {
            userPermissions.setExecutePermission();
         }
         
         // --------  group --------
         // group read
         if ((fileMode & S_IRGRP) == S_IRGRP) {
            groupPermissions.setReadPermission();
         }
         
         // group write
         if ((fileMode & S_IWGRP) == S_IWGRP) {
            groupPermissions.setWritePermission();
         }
         
         // group execute
         if ((fileMode & S_IXGRP) == S_IXGRP) {
            groupPermissions.setExecutePermission();
         }

         // --------  other --------
         // other read
         if ((fileMode & S_IROTH) == S_IROTH) {
            otherPermissions.setReadPermission();
         }
         
         // other write
         if ((fileMode & S_IWOTH) == S_IWOTH) {
            otherPermissions.setWritePermission();
         }
         
         // other execute
         if ((fileMode & S_IXOTH) == S_IXOTH) {
            otherPermissions.setExecutePermission();
         }


#ifdef __linux__
         time_t ctimeValue = st.st_ctime;
         time_t mtimeValue = st.st_mtime;
         TimeTToDateTime(ctimeValue, createTime);
         TimeTToDateTime(mtimeValue, modifyTime);
#else
         struct timespec ctimespec = st.st_ctimespec;
         struct timespec mtimespec = st.st_mtimespec;
         TimeSpecToDateTime(ctimespec, createTime);
         TimeSpecToDateTime(mtimespec, modifyTime);
#endif

         chaudiere::DateTime scanTime;
         
         const int blockSize = FILE_BLOCK_SIZE;
         int numBlockFiles;

         bool existingLocalFile = false;
         
         // have we seen this file before?
         LocalFile localFile;
         if (!m_dataAccess->getLocalFile(m_localDirectoryId,
                                         relativeFilePath,
                                         localFile)) {
            
            if (m_previewOnly) {
               Logger::debug("new file");
            } else {
               // we have NOT seen this file before (it's new)
               localFile.setLocalDirectoryId(m_localDirectoryId);
               localFile.setFilePath(relativeFilePath);
               localFile.setCreateTime(createTime);
               localFile.setModifyTime(modifyTime);
               localFile.setScanTime(scanTime);

               if (!m_dataAccess->insertLocalFile(localFile)) {
                  Logger::error("unable to insert local file");
                  return;
               }
            }
         } else {
            // we have seen this file before
            existingLocalFile = true;
            
            if (!m_previewOnly) {
               Logger::debug("existing file");
               
               // update the scan time
               localFile.setScanTime(scanTime);
               m_dataAccess->updateLocalFile(localFile);
            }
         }
         
         const int localFileId = localFile.getLocalFileId();

         if (fileSize <= blockSize) {
            numBlockFiles = 1;
         } else {
            numBlockFiles = fileSize / blockSize;
            if ((fileSize % blockSize) > 0) {
               ++numBlockFiles;
            }
         }
         
         std::map<int, VaultFile> mapVaultIdToVaultFile;
         std::string nodeBlockFlags(m_activeNodes.size(), FLAG_BLOCK_SELECTIVE);
         
         // for each node
         auto itNodeList = m_activeNodes.cbegin();
         const auto itNodeListEnd = m_activeNodes.cend();
         
         for (int j = 0; itNodeList != itNodeListEnd; ++itNodeList, ++j) {
            const StorageNode& node = *itNodeList;
            const std::string& nodeName = node.getNodeName();
            
            auto itVault = m_mapNodeToVault.find(nodeName);
            if (itVault == m_mapNodeToVault.end()) {
               nodeBlockFlags[j] = FLAG_BLOCK_NONE;
               continue;
            }
            
            Vault& vault = (*itVault).second;
            const int vaultId = vault.getVaultId();
            
            VaultFile vaultFile;
            bool addVaultFileToMap = true;
            
            if (!m_dataAccess->getVaultFile(vaultId,
                                            localFileId,
                                            vaultFile)) {

               vaultFile.setLocalFileId(localFileId);
               vaultFile.setVaultId(vaultId);
               vaultFile.setCreateTime(createTime);
               vaultFile.setModifyTime(modifyTime);
               vaultFile.setOriginFileSize(fileSize);
               vaultFile.setBlockCount(numBlockFiles);
               vaultFile.setUserPermissions(userPermissions);
               vaultFile.setGroupPermissions(groupPermissions);
               vaultFile.setOtherPermissions(otherPermissions);
               
               if (m_previewOnly) {
                  Logger::debug("file needs to be added to vault");
               } else {
                  if (m_dataAccess->insertVaultFile(vaultFile)) {
                     nodeBlockFlags[j] = FLAG_BLOCK_ALL;
                  } else {
                     addVaultFileToMap = false;
                     nodeBlockFlags[j] = FLAG_BLOCK_NONE;
                     Logger::error("unable to create vault file");
                  }
               }
            } else {
               // existing vault file
               
               // same file size as before?
               if (fileSize == vaultFile.getOriginFileSize()) {
                  // same size as before, check file modify time
                  const bool fileModifyTimesMatch =
                     (vaultFile.getModifyTime() == modifyTime);

                  if (!fileModifyTimesMatch) {
                     if (modifyTime < vaultFile.getModifyTime()) {
                        Logger::error("filesystem time earlier than vault file time");
                     }
                  }
                  
                  if (fileModifyTimesMatch) {
                     addVaultFileToMap = false;
                     nodeBlockFlags[j] = FLAG_BLOCK_NONE;
                  } else {
                     ::printf("%s\n", fileName.c_str());
                     ::printf("+++ newer modify time on disk\n");
                     ::printf("*** db modify time: '%s'\n", vaultFile.getModifyTime().formattedString().c_str());
                     ::printf("*** fs modify time: '%s'\n", modifyTime.formattedString().c_str());

                     addVaultFileToMap = true;
                     nodeBlockFlags[j] = FLAG_BLOCK_SELECTIVE;
                  }
               } else {
                  // different file size, we need to update (at least 1 block)
                  ::printf("%s\n", fileName.c_str());
                  ::printf("different file size\n");
                  nodeBlockFlags[j] = FLAG_BLOCK_SELECTIVE;
               }
            }
            
            if (addVaultFileToMap) {
               mapVaultIdToVaultFile[vaultId] = vaultFile;
            } else {
               if (!m_previewOnly) {
                  Logger::debug("not adding vault file to map");
               }
            }
         }
         
         if (!m_previewOnly) {
            
            
         } else {
            const int numNodeBlocksCopied =
               sendFile(numBlockFiles,
                        fullFilePath,
                        localDirectory.getEncrypt(),
                        nodeBlockFlags,
                        mapVaultIdToVaultFile,
                        createTime,
                        modifyTime);
         
            // did we copy any data for this file to a storage node?
            if (numNodeBlocksCopied > 0) {
               // update the copy time
               chaudiere::DateTime copyTime;
               localFile.setCopyTime(copyTime);
               m_dataAccess->updateLocalFile(localFile);
            }
         }
         
         //m_dataAccess->commit();
      } else {
         Logger::error(std::string("unable to stat file '") +
                       path +
                       SINGLE_QUOTE);
      }
   }
}

//******************************************************************************

void GFSClient::scanDir(const std::string& dirPath,
                        const LocalDirectory& localDirectory) {
   const bool recurse = localDirectory.getRecurse();
   const char* pszDirPath = dirPath.c_str();
   DIR* dir;
   m_previewOnly = true;
   
   if ((dir = ::opendir(pszDirPath)) != nullptr) {
      int pathLength;
      char path[PATH_MAX];
      struct dirent* entry;

      while ((entry = ::readdir(dir)) != nullptr) {
         if (entry->d_type & DT_DIR) {
            if ((::strcmp(entry->d_name, "..") != 0) &&
                (::strcmp(entry->d_name, ".") != 0)) {
 
               pathLength = ::snprintf(path,
                                       PATH_MAX,
                                       "%s/%s",
                                       pszDirPath,
                                       entry->d_name);
               
               if (pathLength >= PATH_MAX) {
                  ::fprintf(stderr, "Path length too long: %s\n", path);
               } else {
                  scanProcessDirectory(std::string(path));

                  if (recurse) {
                     const std::string dirName = std::string(entry->d_name);
                     const bool excludeDir = m_exclusions.excludeDirectory(dirName);
                     
                     if (!excludeDir) {
                        scanDir(path, localDirectory);
                     } else {
                        ::printf("excluding directory: '%s'\n", dirName.c_str());
                     }
                  }
               }
            }
         } else {
            // regular file?
            if (entry->d_type & DT_REG) {
               const std::string fileName(entry->d_name);
               if (m_exclusions.excludeFile(fileName)) {
                  if (!m_previewOnly) {
                     ::printf("excluding file: '%s'\n", fileName.c_str());
                  }
               } else {
                  scanProcessFile(dirPath, fileName, localDirectory);
               }
            } else {
               ::printf("ignoring file: %s\n", entry->d_name);
            }
         }
      }
      
      ::closedir(dir);
   } else {
      Logger::error(std::string("unable to open directory '") +
                    std::string(pszDirPath) +
                    SINGLE_QUOTE);
   }
}

//******************************************************************************

void GFSClient::sync() {
   const std::string& directory = m_gfsOptions.getDirectory();
   
   if (!directory.empty()) {
      const int localDirectoryIndex = indexForLocalDirectory(directory);
      if (-1 == localDirectoryIndex) {
         Logger::error("directory not initialized");
         return;
      } else {
         const LocalDirectory& localDirectory =
            m_activeDirectories[localDirectoryIndex];
         m_localDirectoryId = localDirectory.getLocalDirectoryId();

         const bool compress = localDirectory.getCompress();
         const bool encrypt = localDirectory.getEncrypt();
         
         if (encrypt) {
            if (m_gfsOptions.getEncryptionKey().empty()) {
               Logger::error("encryption specified, but no key present");
               return;
            }
         }
         
         // see if we have any directories or files that need to be excluded
         const std::string& configFile =
            m_gfsOptions.getConfigFile();
   
         if (!configFile.empty()) {
            if (OSUtils::pathExists(configFile)) {
               try {
                  IniReader reader(configFile);
                  m_exclusions.retrieveExclusions(directory, reader);
               } catch (const BasicException&)
               {
                  Logger::error("exception caught reading configuration file");
               }
            }
         }
         
         auto itNodeList = m_activeNodes.cbegin();
         const auto itNodeListEnd = m_activeNodes.cend();
         
         for (; itNodeList != itNodeListEnd; ++itNodeList) {
            const StorageNode& node = *itNodeList;
            const std::string& nodeName = node.getNodeName();
            const int storageNodeId = node.getStorageNodeId();
            
            // if we don't have an existing vault for this storage
            // node, create one now
            Vault vault;
            
            if (!m_dataAccess->getVault(storageNodeId,
                                        m_localDirectoryId,
                                        vault)) {
               vault.setStorageNodeId(storageNodeId);
               vault.setLocalDirectoryId(m_localDirectoryId);
               vault.setCompress(compress);
               vault.setEncrypt(encrypt);
               
               if (m_dataAccess->insertVault(vault)) {
                  m_mapNodeToVault[nodeName] = vault;
               } else {
                  Logger::error("unable to insert vault");
               }
            } else {
               m_mapNodeToVault[nodeName] = vault;
            }
         }
         
         if (!m_mapNodeToVault.empty()) {
            m_localDirectoryPathLength = directory.size();
         
            Logger::info(std::string("scanning directory '") +
                         directory +
                         SINGLE_QUOTE);
            scanDir(directory, localDirectory);
         } else {
            Logger::error("unable to sync -- no vaults available");
         }
      }
   } else {
      Logger::error("unable to sync -- no directory specified");
   }
}

//******************************************************************************

void GFSClient::listFiles() {
   //Logger::debug("listFiles called");
   
   if (!m_dataAccess) {
      Logger::error("no database connection");
      return;
   }
   
   const std::string& directory = m_gfsOptions.getDirectory();
   
   if (!directory.empty()) {
      if (!m_activeDirectories.empty()) {
         const int dirIndex = indexForLocalDirectory(directory);
         if (dirIndex > -1) {
            const LocalDirectory& localDirectory =
               m_activeDirectories[dirIndex];
            const int dirId = localDirectory.getLocalDirectoryId();
            std::vector<LocalFile> listFiles;
            if (m_dataAccess->getLocalFilesForDirectory(dirId, listFiles)) {
               auto it = listFiles.cbegin();
               const auto itEnd = listFiles.cend();
               for (; it != itEnd; ++it) {
                  const LocalFile& localFile = *it;
                  const std::string& filePath = localFile.getFilePath();
                  Logger::debug(filePath);
               }
            } else {
               Logger::error("unable to retrieve files for directory");
            }
         } else {
            Logger::error("directory not initialized");
         }
      }
   } else {
      Logger::error("no directories initialized");
   }
}

//******************************************************************************

int GFSClient::getNumberActiveStorageNodes() const {
   return m_activeNodes.size();
}

//******************************************************************************

StorageNode& GFSClient::getActiveStorageNode(int index) {
   return m_activeNodes[index];
}

//******************************************************************************

bool GFSClient::getActiveStorageNodes(std::vector<StorageNode>& vecStorageNodes) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      auto it = m_activeNodes.cbegin();
      const auto itEnd = m_activeNodes.cend();
      for (; it != itEnd; ++it) {
         const StorageNode& storageNode = *it;
         vecStorageNodes.push_back(storageNode);
      }
      success = true;
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::getInactiveStorageNodes(std::vector<StorageNode>& vecStorageNodes) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      if (m_dataAccess->getInactiveStorageNodes(vecStorageNodes)) {
         if (vecStorageNodes.empty()) {
            Logger::warning("no inactive storage nodes defined");
         }
         success = true;
      } else {
         Logger::error("unable to retrieve inactive storage nodes");
      }
   }
   
   return success;
}

//******************************************************************************

void GFSClient::listNodes() {
   //Logger::debug("listNodes called");
   
   if (m_activeNodes.empty()) {
      Logger::warning("no nodes defined");
   } else {
      auto it = m_activeNodes.cbegin();
      const auto itEnd = m_activeNodes.cend();
      for (; it != itEnd; ++it) {
         const StorageNode& storageNode = *it;
         ::printf("%s\n", storageNode.getNodeName().c_str());
      }
   }
}

//******************************************************************************

int GFSClient::getNumberActiveLocalDirectories() const {
   return m_activeDirectories.size();
}

//******************************************************************************

LocalDirectory& GFSClient::getActiveLocalDirectory(int index) {
   return m_activeDirectories[index];
}

//******************************************************************************

bool GFSClient::getActiveLocalDirectories(std::vector<LocalDirectory>& vecLocalDirectories,
                                          bool refresh) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      if (refresh || m_activeDirectories.empty()) {
         m_activeDirectories.clear();
         
         if (m_dataAccess->getActiveLocalDirectories(m_activeDirectories)) {
            if (m_activeDirectories.empty()) {
               Logger::warning("no active local directories defined");
            }
            
            success = true;
         } else {
            Logger::error("unable to retrieve active local directories");
            return false;
         }
      } else {
         success = true;
      }
      
      auto it = m_activeDirectories.cbegin();
      const auto itEnd = m_activeDirectories.cend();
      for (; it != itEnd; ++it) {
         const LocalDirectory& localDirectory = *it;
         vecLocalDirectories.push_back(localDirectory);
      }
   } else {
      Logger::error("no data access available");
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::getInactiveLocalDirectories(std::vector<LocalDirectory>& vecLocalDirectories) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      if (m_dataAccess->getInactiveLocalDirectories(vecLocalDirectories)) {
         if (vecLocalDirectories.empty()) {
            Logger::warning("no inactive local directories defined");
         }
         
         success = true;
      } else {
         Logger::error("unable to retrieve inactive local directories");
      }
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::addStorageNode(StorageNode& storageNode) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      success = m_dataAccess->insertStorageNode(storageNode);
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::updateStorageNode(StorageNode& storageNode) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      success = m_dataAccess->updateStorageNode(storageNode);
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::activateStorageNode(StorageNode& storageNode) {
   storageNode.setActive(true);
   return updateStorageNode(storageNode);
}

//******************************************************************************

bool GFSClient::deactivateStorageNode(StorageNode& storageNode) {
   storageNode.setActive(false);
   return updateStorageNode(storageNode);
}

//******************************************************************************

bool GFSClient::deactivateLocalDirectory(LocalDirectory& localDirectory) {
   localDirectory.setActive(false);
   return updateLocalDirectory(localDirectory);
}

//******************************************************************************

bool GFSClient::activateLocalDirectory(LocalDirectory& localDirectory) {
   localDirectory.setActive(true);
   return updateLocalDirectory(localDirectory);
}

//******************************************************************************

bool GFSClient::addLocalDirectory(LocalDirectory& localDirectory) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      success = m_dataAccess->insertLocalDirectory(localDirectory);
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::updateLocalDirectory(LocalDirectory& localDirectory) {
   bool success = false;
   
   if (m_dataAccess != nullptr) {
      success = m_dataAccess->updateLocalDirectory(localDirectory);
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::listNodeDirectories() {
   bool success = false;
   
   const std::string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      Message message(GFSMessageCommands::MSG_DIR_LIST, MessageType::MessageTypeText);

      Message response;
      if (message.send(nodeName, response)) {
         if (GFSMessage::getRC(response)) {
            if (GFSMessage::hasDirList(response)) {
               std::vector<std::string> listDirectories;
               if (GFSMessage::getDirList(response, listDirectories)) {
                  const int numDirs = listDirectories.size();
                  for (int i = 0; i < numDirs; ++i) {
                     ::printf("%s\n", listDirectories[i].c_str());
                  }
               } else {
                  Logger::error("unable to retrieve directory list");
               }
            } else {
               Logger::error("response missing directory list");
            }
         } else {
            Logger::error("request failed on storage node");
         }
      } else {
         Logger::error("unable to send message");
      }
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::listNodeDirFiles() {
   bool success = false;
   
   const std::string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      const std::string& directory = m_gfsOptions.getDirectory();
      
      if (!directory.empty()) {
         Message message(GFSMessageCommands::MSG_FILE_LIST, MessageType::MessageTypeText);
         GFSMessage::setDirectory(message, directory);

         Message response;
         if (message.send(nodeName, response)) {
            if (GFSMessage::getRC(response)) {
               if (GFSMessage::hasFileList(response)) {
                  std::vector<std::string> listFiles;
                  if (GFSMessage::getFileList(response, listFiles)) {
                     const int numFiles = listFiles.size();
                     for (int i = 0; i < numFiles; ++i) {
                        ::printf("%s\n", listFiles[i].c_str());
                     }
                  } else {
                     Logger::error("unable to retrieve file list");
                  }
               } else {
                  Logger::error("response missing file list");
               }
            } else {
               Logger::error("request failed on storage node");
            }
         } else {
            Logger::error("unable to send message");
         }
      } else {
         Logger::error("missing directory name");
      }
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}   

//******************************************************************************

void GFSClient::addStorageNode() {
   const std::string& nodeName = m_gfsOptions.getNode();

   if (!nodeName.empty()) {
      Logger::debug(std::string("addStorageNode '") +
                    nodeName +
                    SINGLE_QUOTE);
      
      // is the node already defined?
      const int nodeIndex = indexForStorageNode(nodeName);
      
      if (nodeIndex == -1) {
         if (m_dataAccess != nullptr) {
            StorageNode storageNode;
            storageNode.setNodeName(nodeName);
            storageNode.setActive(true);
            
            if (m_dataAccess->insertStorageNode(storageNode)) {
               if (storageNode.getStorageNodeId() > -1) {
                  m_activeNodes.push_back(storageNode);
                  Logger::info("storage node added");
               } else {
                  Logger::error("storage node id missing from database insert");
               }
            } else {
               Logger::error("unable to insert storage node in database");
            }
         } else {
            Logger::error("no database connection");
         }
      } else {
         Logger::error("storage node already defined");
      }
   } else {
      Logger::error("addStorageNode called. no node given");
   }
}

//******************************************************************************

void GFSClient::removeStorageNode() {
   //Logger::debug("removeStorageNode called");
   
   const std::string& nodeName = m_gfsOptions.getNode();

   if (!nodeName.empty()) {
      Logger::debug(std::string("removeStorageNode '") + nodeName + SINGLE_QUOTE);
      
      // is the node defined?
      const int nodeIndex = indexForStorageNode(nodeName);
      
      if (nodeIndex > -1) {
         StorageNode& storageNode = m_activeNodes[nodeIndex];
         if (m_dataAccess != nullptr) {
            if (m_dataAccess->deleteActiveStorageNode(storageNode)) {
               m_activeNodes.erase(m_activeNodes.begin() + nodeIndex);
               Logger::info("storage node removed");
            } else {
               Logger::error("unable to remove storage node from database");
            }
         } else {
            Logger::error("no database connection");
         }
      }
   } else {
      Logger::error("removeStorageNode called. no node given");
   }
}

//******************************************************************************

int GFSClient::indexForStorageNode(const std::string& nodeName) {
   int nodeIndex = -1;
   auto it = m_activeNodes.cbegin();
   const auto itEnd = m_activeNodes.cend();
   int i = 0;
   for (; it != itEnd; ++it) {
      const StorageNode& storageNode = *it;
      if (storageNode.getNodeName() == nodeName) {
         nodeIndex = i;
         break;
      } else {
         ++i;
      }
   }
   
   return nodeIndex; 
}

//******************************************************************************

int GFSClient::indexForLocalDirectory(const std::string& dirPath) {
   int dirIndex = -1;
   auto it = m_activeDirectories.cbegin();
   const auto itEnd = m_activeDirectories.cend();
   int i = 0;
   for (; it != itEnd; ++it) {
      const LocalDirectory& directory = *it;
      if (directory.getDirectoryPath() == dirPath) {
         dirIndex = i;
         break;
      } else {
         ++i;
      }
   }
   
   return dirIndex; 
}

//******************************************************************************

bool GFSClient::restore() {
   bool success = false;
   
   const std::string& nodeName = m_gfsOptions.getNode();
   const std::string& sourceDirectory = m_gfsOptions.getDirectory();
   const std::string& targetDirectory = m_gfsOptions.getTargetDirectory();

   if (!nodeName.empty()) {
      if (!sourceDirectory.empty()) {
         if (!targetDirectory.empty()) {
            
            // source and target directory cannot be same
            if (sourceDirectory == targetDirectory) {
               Logger::error("source and target directory cannot be the same");
               return false;
            }
            
            // valid node name?
            const int nodeIndex = indexForStorageNode(nodeName);
      
            if (nodeIndex == -1) {
               Logger::error("unrecognized storage node name");
               return false;
            }
            
            // valid source directory?
            const int sourceDirectoryIndex = indexForLocalDirectory(sourceDirectory);
            
            if (sourceDirectoryIndex == -1) {
               Logger::error("unrecognized local directory (source)");
               return false;
            }
            
            // valid target directory?
            //TODO: validate target directory
            
            const std::string& encryptionKey = m_gfsOptions.getEncryptionKey();
            const StorageNode& storageNode = m_activeNodes[nodeIndex];
            const LocalDirectory& sourceDirectory = m_activeDirectories[sourceDirectoryIndex];
            
            return fullRestore(encryptionKey, storageNode, sourceDirectory, targetDirectory);
         
         } else {
            Logger::error("missing target directory");
         }
      } else {
         Logger::error("missing source directory");
      }
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::restoreSubdirectory(const std::string& dirPath) {
   bool success = false;
   
   const std::string& nodeName = m_gfsOptions.getNode();
   const std::string& vaultDirectory = m_gfsOptions.getDirectory();

   if (!nodeName.empty()) {
      if (!vaultDirectory.empty()) {
         if (!dirPath.empty()) {
            //TODO: implement restoreSubdirectory
         } else {
            Logger::error("missing directory name/path to restore");
         }
      } else {
         Logger::error("missing vault directory");
      }
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::restoreFile() {
   bool success = false;
   
   const std::string& nodeName = m_gfsOptions.getNode();
   const std::string& directory = m_gfsOptions.getDirectory();
   const std::string& filePath = m_gfsOptions.getFile();

   if (!nodeName.empty()) {
      if (!directory.empty()) {
         if (!filePath.empty()) {
            //TODO: implement restoreFile
         } else {
            Logger::error("missing file name/path");
         }
      } else {
         Logger::error("missing directory");
      }
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::retrieveFile(const std::string& nodeName,
                             const std::string& directory,
                             const std::string& fileName,
                             std::string& fileContents) {
   bool success = false;
   
   if (!nodeName.empty()) {
      if (!directory.empty()) {
         if (!fileName.empty()) {
            Message message(GFSMessageCommands::MSG_FILE_RETRIEVE, MessageType::MessageTypeText);
            GFSMessage::setDirectory(message, directory);
            GFSMessage::setFile(message, fileName);
   
            Message response;
            if (message.send(nodeName, response)) {
               if (GFSMessage::getRC(response)) {
                  fileContents = response.getTextPayload();
                  success = true;
               } else {
                  Logger::error("file retrieve failed on storage node");
               }
            } else {
               Logger::error("message send of MSG_FILE_RETRIEVE failed");
            }
         } else {
            Logger::error("missing file name");
         }
      } else {
         Logger::error("missing directory");
      }
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSClient::fullRestore(const std::string& encryptionKey,
                            const StorageNode& storageNode,
                            const LocalDirectory& sourceDirectory,
                            const std::string& targetDirectory) {
   bool success = false;
   
   const int nodeIndex = storageNode.getStorageNodeId();
   const int sourceDirectoryId = sourceDirectory.getLocalDirectoryId();
   
   Vault vault;
   if (m_dataAccess->getVault(nodeIndex, sourceDirectoryId, vault)) {
      const bool encrypted = vault.getEncrypt();
      const int vaultId = vault.getVaultId();
      std::vector<LocalFile> listLocalFiles;
      if (m_dataAccess->getLocalFilesForDirectory(sourceDirectoryId,
                                                  listLocalFiles)) {
         if (!listLocalFiles.empty()) {
            const std::string& nodeName = storageNode.getNodeName();
            
            auto itLocalFile = listLocalFiles.cbegin();
            const auto itLocalFileEnd = listLocalFiles.cend();
            const std::string FILE_DIR_DELIMITER = "/";
            
            for (; itLocalFile != itLocalFileEnd; ++itLocalFile) {
               const LocalFile& localFile = *itLocalFile;
               const std::string& localFilePath = localFile.getFilePath();
               
               StringTokenizer st(localFilePath, FILE_DIR_DELIMITER);
               const int numTokens = st.countTokens();
               
               if (numTokens > 1) {
                  // walk the directory path and create any subdirectories
                  // that are missing
                  std::string dirPath = targetDirectory;
                  const int numSubDirs = numTokens - 1;
                  
                  for (int i = 0; i < numSubDirs; ++i) {
                     dirPath += FILE_DIR_DELIMITER;
                     dirPath += st.nextToken();
                     
                     if (!OSUtils::directoryExists(dirPath)) {
                        if (!OSUtils::createDirectory(dirPath)) {
                           Logger::error(std::string("unable to create directory: ") + dirPath);
                        }
                     }
                  }
               } else {
                  // if we only have a single token, that means we're not
                  // dealing with subdirectories, so we don't need to
                  // check for existence (or create any)
               }
               
               VaultFile vaultFile;
               if (m_dataAccess->getVaultFile(vaultId,
                                              localFile.getLocalFileId(),
                                              vaultFile)) {
                  const int vaultFileId = vaultFile.getVaultFileId();
                  const int numBlocks = vaultFile.getBlockCount();
                  std::vector<VaultFileBlock> listFileBlocks;
                  
                  if (m_dataAccess->getBlocksForVaultFile(vaultFileId,
                                                          listFileBlocks)) {
                     if (listFileBlocks.size() == numBlocks) {
                        const std::string vaultFilePath =
                           OSUtils::pathJoin(targetDirectory, localFilePath);
                        FILE* f = ::fopen(vaultFilePath.c_str(), "wb");
                        if (f != nullptr) {
                           // determine the permissions needed for the file
                           const FilePermissions& userPermissions =
                              vaultFile.getUserPermissions();
                           const FilePermissions& groupPermissions =
                              vaultFile.getGroupPermissions();
                           const FilePermissions& otherPermissions =
                              vaultFile.getOtherPermissions();
                           
                           mode_t fileMode = 0;
                           
                           // ------------  user ---------------
                           if (userPermissions.hasReadPermission()) {
                              fileMode |= S_IRUSR;
                           }
                           
                           if (userPermissions.hasWritePermission()) {
                              fileMode |= S_IWUSR;
                           }
                           
                           if (userPermissions.hasExecutePermission()) {
                              fileMode |= S_IXUSR;
                           }

                           // ------------  group ---------------
                           if (groupPermissions.hasReadPermission()) {
                              fileMode |= S_IRGRP;
                           }
                           
                           if (groupPermissions.hasWritePermission()) {
                              fileMode |= S_IWGRP;
                           }
                           
                           if (groupPermissions.hasExecutePermission()) {
                              fileMode |= S_IXGRP;
                           }

                           // ------------  other ---------------
                           if (otherPermissions.hasReadPermission()) {
                              fileMode |= S_IROTH;
                           }
                           
                           if (otherPermissions.hasWritePermission()) {
                              fileMode |= S_IWOTH;
                           }
                           
                           if (otherPermissions.hasExecutePermission()) {
                              fileMode |= S_IXOTH;
                           }
                           
                           // set the file's permissions
                           const int rc = ::fchmod(fileno(f), fileMode);
                           if (rc != 0) {
                              Logger::error("unable to set file permissions");
                           }
                           
                           auto itListFileBlocks = listFileBlocks.cbegin();
                           const auto itListFileBlocksEnd = listFileBlocks.cend();
                           
                           for (; itListFileBlocks != itListFileBlocksEnd; ++itListFileBlocks) {
                              const VaultFileBlock& vaultFileBlock = *itListFileBlocks;
                              std::string fileContents;
                              
                              if (retrieveFile(nodeName,
                                               vaultFileBlock.getNodeDirectory(),
                                               vaultFileBlock.getNodeFile(),
                                               fileContents)) {
                                                  
                                 const std::string calcUniqueId =
                                    GFS::uniqueIdentifierForString(fileContents);
                                    
                                 // pass integrity check?
                                 if (calcUniqueId == vaultFileBlock.getUniqueIdentifier()) {
                                    // does it match the stored size?
                                    if (fileContents.length() == vaultFileBlock.getStoredFileSize()) {
                                       // remove base64 encoding
                                       const std::string unencodedFileContents =
                                          Encryption::base64Decode(fileContents);
                                       std::string finalText;
                                       
                                       if (encrypted) {
                                          const int padCharCount =
                                             vaultFileBlock.getPadCharCount();
                                          
                                          // unencrypt
                                          finalText = Decrypt(unencodedFileContents,
                                                              encryptionKey);
                                          
                                          // remove padding chars
                                          if (padCharCount > 0) {
                                             finalText = finalText.substr(0,
                                                                          finalText.size() - padCharCount);
                                          }
                                       } else {
                                          finalText = unencodedFileContents;
                                       }

                                       // does it match the original size?
                                       if (finalText.size() == vaultFileBlock.getOriginFileSize()) {
                                          // write the block out to file
                                          const size_t objectsWritten = 
                                               ::fwrite(finalText.data(), finalText.size(), 1, f);
      
                                          if (objectsWritten > 0) {
                                             //Logger::debug("restored file block");
                                          } else {
                                             Logger::error("fwrite failed");
                                          }             
                                       } else {
                                          Logger::error("block mismatch with original size");
                                          ::printf("block size = %lu\n", finalText.size());
                                          ::printf("origin file size = %d\n", vaultFileBlock.getOriginFileSize());
                                       }
                                    } else {
                                       Logger::error("block mismatch with stored size");
                                    }
                                 } else {
                                    Logger::error("block failed unique id (integrity) check");
                                    ::printf("calcUniqueId='%s'\n", calcUniqueId.c_str());
                                    ::printf("block unique='%s'\n", vaultFileBlock.getUniqueIdentifier().c_str());
                                 }                             
                              } else {
                                 Logger::error("retrieveFile failed");
                              }
                           }
                           
                           ::fclose(f);
                        }
                     } else {
                        Logger::error("number of blocks returned mismatch with number blocks stored");
                        ::printf("number blocks returned = %lu\n", listFileBlocks.size());
                        ::printf("number blocks stored = %d\n", numBlocks);
                        ::printf("file path=%s\n", localFilePath.c_str());
                     }
                  } else {
                     Logger::error("unable to retrieve blocks for vault file");
                  }
               } else {
                  Logger::error("unable to retrieve vault file from DB");
               }
            }
         } else {
            Logger::warning("no files found for directory");
         }
      } else {
         Logger::error("unable to retrieve file list for directory");
      }
   } else {
      Logger::error("unable to find vault for node/directory");
   }
   
   return success;
}

//******************************************************************************

