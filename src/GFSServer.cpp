// Copyright Paul Dardeau, 2016
// GFSServer.cpp

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "GFSServer.h"
#include "MessagingServer.h"
#include "GFSMessageHandler.h"
#include "OSUtils.h"
#include "StrUtils.h"
#include "GFSMessage.h"
#include "FileReferenceCount.h"
#include "GFS.h"
#include "Encryption.h"

using namespace lachepas;
using namespace chaudiere;
using namespace tonnerre;

static const std::string EMPTY_STRING           = "";
static const std::string SLASH                  = "/";
static const std::string ZERO                   = "0";

static const std::string MSG_DIR_STAT           = "dirStat";
static const std::string MSG_DIR_LIST           = "dirList";

static const std::string MSG_FILE_ADD           = "fileAdd";
static const std::string MSG_FILE_UPDATE        = "fileUpdate";
static const std::string MSG_FILE_DELETE        = "fileDelete";
static const std::string MSG_FILE_RETRIEVE      = "fileRetrieve";
static const std::string MSG_FILE_ID            = "fileId";
static const std::string MSG_FILE_STAT          = "fileStat";
static const std::string MSG_FILE_LIST          = "fileList";

static const std::string ERR_MISSING_DIRECTORY  = "missing directory name";
static const std::string ERR_MISSING_FILE       = "missing file name";
static const std::string ERR_NOT_IMPLEMENTED    = "not implemented";


//******************************************************************************
//******************************************************************************
                                    
class GFSStorageMessageHandler : public GFSMessageHandler {

private:
   GFSServer& m_server;
   
public:
   GFSStorageMessageHandler(GFSServer& server) :
      m_server(server) {
   }
   
   void handleTextMessage(const Message& requestMessage,
                          Message& responseMessage,
                          const std::string& requestName,
                          const std::string& requestPayload,
                          std::string& responsePayload) {
      if (requestName == MSG_DIR_STAT) {
         if (GFSMessage::hasDirectory(requestMessage)) {
            //const std::string& directory =
            //   GFSMessage::getDirectory(requestMessage);
            encodeBool(responseMessage, false);
            encodeError(responseMessage, ERR_NOT_IMPLEMENTED);
            //encodeBool(responseMessage, m_server.dirStat(directory));
         } else {
            encodeError(responseMessage, ERR_MISSING_DIRECTORY);
         }
      } else if (requestName == MSG_DIR_LIST) {
         std::vector<std::string> listDirectories;
            
         if (m_server.dirList(listDirectories)) {
            encodeSuccess(responseMessage);
            GFSMessage::setDirList(responseMessage, listDirectories);
         } else {
            encodeError(responseMessage, "unable to obtain directory list");
         }
      } else if (requestName == MSG_FILE_ADD) {
         if (GFSMessage::hasFile(requestMessage)) {
            if (GFSMessage::hasUniqueIdentifier(requestMessage)) {
               const std::string& file = GFSMessage::getFile(requestMessage);
               const std::string& b64FileContents = requestMessage.getTextPayload();
               std::string directory;
               std::string uniqueIdentifier =
                  GFSMessage::getUniqueIdentifier(requestMessage);
               
               const std::string& fileContents = b64FileContents;
               
               if (!fileContents.empty() &&
                   m_server.fileAdd(file, fileContents, directory, uniqueIdentifier)) {
                  encodeBool(responseMessage, true);
                  GFSMessage::setUniqueIdentifier(responseMessage, uniqueIdentifier);
                  GFSMessage::setFile(responseMessage, file);
                  GFSMessage::setDirectory(responseMessage, directory);
               } else {
                  ::printf("fileAdd failed\n");
                  ::printf("=====\n");
                  ::printf("file='%s'\n", file.c_str());
                  ::printf("fileContents.length() = %lu\n", fileContents.length());
                  ::printf("directory='%s'\n", directory.c_str());
                  ::printf("uniqueId='%s'\n", uniqueIdentifier.c_str());
                  encodeError(responseMessage, "fileAdd failed");
               }
            } else {
               ::printf("hasUniqueIdentifier returned false\n");
               encodeError(responseMessage, "missing unique identifier");
            }
         } else {
            ::printf("hasFile returned false\n");
            encodeError(responseMessage, ERR_MISSING_FILE);
         }
      } else if (requestName == MSG_FILE_UPDATE) {
         if (GFSMessage::hasFile(requestMessage)) {
            if (GFSMessage::hasDirectory(requestMessage)) {
               std::string directory = GFSMessage::getDirectory(requestMessage);
               std::string file = GFSMessage::getFile(requestMessage);
               const std::string& fileContents = requestMessage.getTextPayload();
               std::string uniqueIdentifier;
               
               if (m_server.fileUpdate(fileContents, directory, file, uniqueIdentifier)) {
                  encodeBool(responseMessage, true);
                  GFSMessage::setUniqueIdentifier(responseMessage, uniqueIdentifier);
                  GFSMessage::setDirectory(responseMessage, directory);
                  GFSMessage::setFile(responseMessage, file);
               } else {
                  encodeError(responseMessage, "fileUpdate failed");
               }
            } else {
               encodeError(responseMessage, ERR_MISSING_DIRECTORY);
            }
         } else {
            encodeError(responseMessage, ERR_MISSING_FILE);
         }
      } else if (requestName == MSG_FILE_DELETE) {
         if (GFSMessage::hasDirectory(requestMessage)) {
            if (GFSMessage::hasFile(requestMessage)) {
               const std::string& directory =
                  GFSMessage::getDirectory(requestMessage);
               const std::string& file = GFSMessage::getFile(requestMessage);
               
               if (m_server.fileDelete(directory, file)) {
                  encodeBool(responseMessage, true);
               } else {
                  encodeError(responseMessage, "deleteFile failed");
               }
            } else {
               encodeError(responseMessage, ERR_MISSING_FILE);
            }
         } else {
            encodeError(responseMessage, ERR_MISSING_DIRECTORY);
         }
      } else if (requestName == MSG_FILE_ID) {
         if (GFSMessage::hasDirectory(requestMessage)) {
            if (GFSMessage::hasFile(requestMessage)) {
               const std::string& directory =
                  GFSMessage::getDirectory(requestMessage);
               const std::string& file = GFSMessage::getFile(requestMessage);
               std::string uniqueIdentifier;
               
               if (m_server.fileUniqueIdentifier(directory, file, uniqueIdentifier)) {
                  encodeSuccess(responseMessage);
                  GFSMessage::setUniqueIdentifier(responseMessage, uniqueIdentifier);
               } else {
                  encodeError(responseMessage, "unable to obtain unique identifier");
               }
            } else {
               encodeError(responseMessage, ERR_MISSING_FILE);
            }
         } else {
            encodeError(responseMessage, ERR_MISSING_DIRECTORY);
         }
      } else if (requestName == MSG_FILE_STAT) {
         if (GFSMessage::hasDirectory(requestMessage)) {
            if (GFSMessage::hasFile(requestMessage)) {
               //const std::string& directory =
               //   GFSMessage::getDirectory(requestMessage);
               //const std::string& file = GFSMessage::getFile(requestMessage);
               encodeError(responseMessage, ERR_NOT_IMPLEMENTED);
            } else {
               encodeError(responseMessage, ERR_MISSING_FILE);
            }
         } else {
            encodeError(responseMessage, ERR_MISSING_DIRECTORY);
         }
      } else if (requestName == MSG_FILE_LIST) {
         if (GFSMessage::hasDirectory(requestMessage)) {
            const std::string& directory =
               GFSMessage::getDirectory(requestMessage);
            std::vector<std::string> listFiles;
            
            if (m_server.fileList(directory, listFiles)) {
               encodeSuccess(responseMessage);
               GFSMessage::setFileList(responseMessage, listFiles);
            } else {
               encodeError(responseMessage, "unable to obtain file list");
            }
         } else {
            encodeError(responseMessage, ERR_MISSING_DIRECTORY);
         }
      } else if (requestName == MSG_FILE_RETRIEVE) {
         if (GFSMessage::hasDirectory(requestMessage)) {
            if (GFSMessage::hasFile(requestMessage)) {
               const std::string& directory =
                  GFSMessage::getDirectory(requestMessage);
               const std::string& fileName =
                  GFSMessage::getFile(requestMessage);
               std::string fileContents;
               if (m_server.retrieveFileContents(directory, fileName, fileContents)) {
                  encodeSuccess(responseMessage);
                  responsePayload = fileContents;
               } else {
                  encodeError(responseMessage, "unable to retrieve file contents");
               }
            } else {
               encodeError(responseMessage, ERR_MISSING_FILE);
            }
         } else {
            encodeError(responseMessage, ERR_MISSING_DIRECTORY);
         }
      }
   }
};

//******************************************************************************
//******************************************************************************

GFSServer::GFSServer() :
   m_debugPrint(true) {
   m_fileReferenceCount = new FileReferenceCount;
}

//******************************************************************************

GFSServer::~GFSServer() {
}

//******************************************************************************

bool GFSServer::initializeDirectory(const std::string& directory) {
   const int numDirs = 100;
   int createdDirs = 0;
   char buffer[20];
   
   for (int i = 0; i < numDirs; ++i) {
      ::memset(buffer, 0, 20);
      ::snprintf(buffer, 20, "%02d", i);
      std::string dirName = buffer;
      
      std::string dirPath = directory;
      dirPath += "/";
      dirPath += dirName;
      
      if (OSUtils::createPrivateDirectory(dirPath)) {
         ++createdDirs;
      } else {
         break;
      }
   }
   
   return (numDirs == createdDirs);
}

//******************************************************************************

bool GFSServer::run(const std::string& directory,
                    const std::string& iniFilePath,
                    const std::string& serviceName) {
   if (OSUtils::directoryExists(directory)) {
      if (OSUtils::pathExists(iniFilePath)) {
         m_baseDir = directory;
         
         const char* pszDirPath = m_baseDir.c_str();
         DIR* dir;
         struct dirent* entry;
   
         if ((dir = ::opendir(pszDirPath)) != nullptr) {
            while ((entry = ::readdir(dir)) != nullptr) {
               if (entry->d_type & DT_DIR) {
                  if ((::strcmp(entry->d_name, "..") != 0) &&
                      (::strcmp(entry->d_name, ".") != 0)) {
                     m_listSubdirs.push_back(std::string(entry->d_name));
                  }
               }
            }
         } else {
            ::printf("error: opendir failed for '%s'\n", m_baseDir.c_str());
            return false;
         }

         MessagingServer server(iniFilePath, serviceName);
         GFSStorageMessageHandler handler(*this);
         server.setMessageHandler(&handler);
         const int rc = server.run();
         return (rc == 0);
      } else {
         Logger::error(std::string("ini file path does not exist: '") +
                       iniFilePath +
                       std::string("'"));
         return false;
      }
   } else {
      Logger::error(std::string("directory to serve does not exist: ") +
                    directory);
      return false;
   }
}

//******************************************************************************

bool GFSServer::dirStat(const std::string& directory) {
   //TODO: implement dirStat
   return false;
}

//******************************************************************************

bool GFSServer::dirList(std::vector<std::string>& listDirectories) {
   if (m_debugPrint) {
      Logger::debug("dirList called");
   }
   
   const char* pszDirPath = m_baseDir.c_str();
   DIR* dir;
   struct dirent* entry;
   bool success = false;
   
   if ((dir = ::opendir(pszDirPath)) != nullptr) {
      while ((entry = ::readdir(dir)) != nullptr) {
         if (entry->d_type & DT_DIR) {
            listDirectories.push_back(std::string(entry->d_name));
         }
      }
      
      ::closedir(dir);
      success = true;
   } else {
      Logger::error(std::string("unable to open directory '") +
                    std::string(pszDirPath) +
                    std::string("'"));
   }

   return success;
}

//******************************************************************************

bool GFSServer::getPathForFile(const std::string& directory,
                               const std::string& file,
                               std::string& filePath) {
   std::string dirPath = m_baseDir;
   
   if (!directory.empty()) {
      if (!StrUtils::endsWith(dirPath, SLASH) &&
          !StrUtils::startsWith(directory, SLASH)) {
         dirPath += SLASH;
      }
      
      dirPath += directory;
   }
   
   filePath = OSUtils::pathJoin(dirPath, file);
   
   return true;
}

//******************************************************************************

bool GFSServer::fileUniqueIdentifier(const std::string& directory,
                                     const std::string& file,
                                     std::string& uniqueIdentifier) {
   std::string filePath;
   if (getPathForFile(directory, file, filePath)) {
      return GFS::uniqueIdentifierForFile(filePath, uniqueIdentifier);
   } else {
      return false;
   }
}

//******************************************************************************

bool GFSServer::writeFile(const std::string& filePath,
                          const std::string& fileContents,
                          std::string& uniqueIdentifier) {
   FILE* f = ::fopen(filePath.c_str(), "wt");
   if (f != nullptr) {
      const size_t objectsWritten = 
           ::fwrite(fileContents.c_str(), fileContents.length(), 1, f);
      
      if (objectsWritten > 0) {
         ::fflush(f);
         ::fsync(::fileno(f));
         ::fclose(f);
         f = nullptr;

         std::string storedFileId;
         if (GFS::uniqueIdentifierForFile(filePath, storedFileId)) {
            uniqueIdentifier = storedFileId;
            return true;
         } else {
            // delete the file that we just wrote, since we're unable to verify
            // its integrity
            ::unlink(filePath.c_str());
            ::printf("error: unable to obtain unique identifier for file '%s'\n",
                     filePath.c_str());
            Logger::debug("file deleted");
         }
      } else {
         ::printf("error: unable to write to file '%s'\n", filePath.c_str());
      }
      
      if (f != nullptr) {
         ::fclose(f);
      }
   } else {
      ::printf("error: unable to open file '%s'\n", filePath.c_str());
   }
   
   return false;
}

//******************************************************************************

long GFSServer::referenceCountForFile(const std::string& filePath) {
   return m_fileReferenceCount->referenceCountForFile(filePath);
}

//******************************************************************************

bool GFSServer::storeInitialReferenceCount(const std::string& filePath) {
   return m_fileReferenceCount->storeInitialReferenceCount(filePath);
}

//******************************************************************************

bool GFSServer::storeUpdatedReferenceCount(const std::string& filePath,
                                           long refCountValue) {
   if (refCountValue < 1L) {
      return false;
   }

   return m_fileReferenceCount->storeUpdatedReferenceCount(filePath,
                                                           refCountValue);
}

//******************************************************************************

bool GFSServer::incrementReferenceCount(const std::string& filePath) {
   long refCountValue = referenceCountForFile(filePath);
   if (refCountValue < 1L) {
      return false;
   }

   ++refCountValue;
   
   return storeUpdatedReferenceCount(filePath, refCountValue);
}

//******************************************************************************

bool GFSServer::decrementReferenceCount(const std::string& filePath) {
   long refCountValue = referenceCountForFile(filePath);
   if (refCountValue < 1L) {
      return false;
   }

   --refCountValue;
   
   return storeUpdatedReferenceCount(filePath, refCountValue);
}

//******************************************************************************

bool GFSServer::fileAdd(const std::string& fileName,
                        const std::string& fileContents,
                        std::string& directory,
                        std::string& uniqueIdentifier) {
   if (m_debugPrint) {
      Logger::debug("fileAdd called");
   }
   
   if (uniqueIdentifier.empty()) {
      ::printf("fileAdd: uniqueIdentifier empty, returning\n");
      return false;
   }
   
   std::string dirName;
   int digitsFound = 0;
   
   const int uniqueIdLength = uniqueIdentifier.length();
   
   for (int i = 0; i < uniqueIdLength; ++i) {
      const char ch = uniqueIdentifier[i];
      if (ch >= '0' && ch <= '9') {
         ++digitsFound;
         
         // is it a leading 0?
         if ((ch == '0') && (0 == digitsFound)) {
            // don't put it
         } else {
            dirName += ch;
         }
         
         if (digitsFound == 2) {
            break;
         }
      }
   }
   
   if (dirName.empty()) {
      dirName = "00";
   } else {
      if (dirName.length() == 1) {
         // add a leading zero
         dirName.insert(0, ZERO);
      }
   }
   
   directory = dirName;

   std::string filePath;
   getPathForFile(directory, fileName, filePath);
   
   if (filePath.empty()) {
      ::printf("error: filePath is empty for directory='%s', fileName='%s'\n",
               directory.c_str(),
               fileName.c_str());
      return false;
   }
   
   if (OSUtils::pathExists(filePath)) {
      bool rc = incrementReferenceCount(filePath);
      return rc;
   } else {
      std::string nodeUniqueIdentifier;
      if (writeFile(filePath, fileContents, nodeUniqueIdentifier)) {
         const bool refCountStored = storeInitialReferenceCount(filePath);
         if (refCountStored) {
            uniqueIdentifier = nodeUniqueIdentifier;
         } else {
            ::printf("storeInitialRefCount failed\n");
         }
         return refCountStored;
      } else {
         ::printf("writeFile failed\n");
         return false;
      }
   }
}

//******************************************************************************

bool GFSServer::fileUpdate(const std::string& fileContents,
                           std::string& directory,
                           std::string& fileName,
                           std::string& uniqueIdentifier) {
   if (m_debugPrint) {
      Logger::debug("fileUpdate called");
   }

   // what's weird about fileUpdate is that since the name of the
   // directory and file that stores the data is based on the unique
   // identifier of the file contents, an update of the data will mean
   // that the data will be stored in a new directory and file.
   
   const std::string uniqueIDFileContents =
      GFS::uniqueIdentifierForString(fileContents);
   
   if (uniqueIDFileContents != fileName) {
      if (fileDelete(directory, fileName)) {
         fileName = uniqueIDFileContents;
         return fileAdd(fileName, fileContents, directory, uniqueIdentifier);
      } else {
         Logger::error("fileUpdate failed - unable to delete old file");
         return false;
      }
   } else {
      // not really an update -- nothing changed
      return true;
   }
}

//******************************************************************************

bool GFSServer::fileDelete(const std::string& directory,
                           const std::string& fileName) {
   if (m_debugPrint) {
      Logger::debug("fileDelete called");
   }
   
   std::string filePath;
   getPathForFile(directory, fileName, filePath);

   if (OSUtils::pathExists(filePath)) {
      const long refCountValue = referenceCountForFile(filePath);
      if (refCountValue < 1L) {
         return false;
      } else {
         if (refCountValue > 1L) {
            return decrementReferenceCount(filePath);
         } else {
            const int rc = ::unlink(filePath.c_str());
            if (rc == 0) {
               return true;
            }
         }
      }
   }

   return false;
}

//******************************************************************************

bool GFSServer::fileStat(const std::string& directory,
                         const std::string& fileName) {
   if (m_debugPrint) {
      Logger::debug("fileStat called");
   }
   
   std::string filePath;
   getPathForFile(directory, fileName, filePath);
   
   if (OSUtils::pathExists(filePath)) {
      
   }

   //TODO: implement fileStat
   return false;
}

//******************************************************************************

bool GFSServer::fileList(const std::string& directory,
                         std::vector<std::string>& listFiles) {
   if (m_debugPrint) {
      Logger::debug("fileList called");
   }
   
   std::string dirPath = m_baseDir;
   
   if (!directory.empty()) {
      if (!StrUtils::endsWith(dirPath, SLASH) &&
          !StrUtils::startsWith(directory, SLASH)) {
         dirPath += SLASH;
      }
      
      dirPath += directory;
   } else {
      printf("no directory specified\n");
      return false;
   }
   
   const char* pszDirPath = dirPath.c_str();
   DIR* dir;
   struct dirent* entry;
   bool success = false;
   
   if ((dir = ::opendir(pszDirPath)) != nullptr) {
      while ((entry = ::readdir(dir)) != nullptr) {
         if (!(entry->d_type & DT_DIR)) {
            listFiles.push_back(std::string(entry->d_name));
         }
      }
      
      ::closedir(dir);
      success = true;
   } else {
      Logger::error(std::string("unable to open directory '") +
                    std::string(pszDirPath) +
                    std::string("'"));
   }

   return success;
}

//******************************************************************************

bool GFSServer::retrieveFileContents(const std::string& directory,
                                     const std::string& fileName,
                                     std::string& fileContents) {
   bool retrievalSuccess = false;
   
   if (directory.empty()) {
      ::printf("retrieveFileContents: missing directory\n");
      return false;
   }
   
   if (fileName.empty()) {
      ::printf("retrieveFileContents: missing file name\n");
      return false;
   }
   
   std::string filePath;
   getPathForFile(directory, fileName, filePath);
   
   if (filePath.empty()) {
      ::printf("error: filePath is empty for directory='%s', fileName='%s'\n",
               directory.c_str(),
               fileName.c_str());
      return false;
   }
   
   if (OSUtils::pathExists(filePath)) {
      retrievalSuccess = GFS::readFile(filePath, fileContents);
   } else {
      ::printf("error: file does not exist\n");
   }
   
   return retrievalSuccess;                       
}

//******************************************************************************
//******************************************************************************

