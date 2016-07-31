// Copyright Paul Dardeau, 2016
// GFSMessage.cpp

#include <stdlib.h>

#include "GFSMessage.h"
#include "Message.h"
#include "StringTokenizer.h"

using namespace lachepas;

static const std::string LIST_DELIMITER         = "|";

static const std::string KEY_PREFIX             = "gfs_";

static const std::string KEY_RC                 = KEY_PREFIX + "rc";
static const std::string KEY_ERROR              = KEY_PREFIX + "error";

static const std::string KEY_DEVICE             = KEY_PREFIX + "device";
static const std::string KEY_DEVICE_LIST        = KEY_PREFIX + "deviceList";
static const std::string KEY_DIR                = KEY_PREFIX + "dir";
static const std::string KEY_DIR_LIST           = KEY_PREFIX + "dirList";
static const std::string KEY_FILE               = KEY_PREFIX + "file";
static const std::string KEY_FILE_LIST          = KEY_PREFIX + "fileList";
static const std::string KEY_ORIGIN_FS          = KEY_PREFIX + "origin_fs";
static const std::string KEY_STORED_FS          = KEY_PREFIX + "stored_fs";
static const std::string KEY_UNIQUE_IDENTIFIER  = KEY_PREFIX + "unique_id";

static const std::string VALUE_TRUE             = "true";
static const std::string VALUE_FALSE            = "false";

//******************************************************************************

void GFSMessage::setKeyValue(tonnerre::Message& message,
                             const std::string& key,
                             const std::string& value) {
   message.setHeader(key, value);
}

//******************************************************************************

bool GFSMessage::hasKey(const tonnerre::Message& message,
                        const std::string& key) {
   return message.hasHeader(key);
}

//******************************************************************************

const std::string& GFSMessage::getKeyValue(const tonnerre::Message& message,
                                           const std::string& key) {
   return message.getHeader(key);
}

//******************************************************************************

void GFSMessage::setRC(tonnerre::Message& message, bool rc) {
   if (rc) {
      GFSMessage::setKeyValue(message, KEY_RC, VALUE_TRUE);
   } else {
      GFSMessage::setKeyValue(message, KEY_RC, VALUE_FALSE);
   }
}

//******************************************************************************

bool GFSMessage::getRC(const tonnerre::Message& message) {
   bool rc = false;
   if (GFSMessage::hasKey(message, KEY_RC)) {
      const std::string& rcValue = GFSMessage::getKeyValue(message, KEY_RC);
      if (rcValue == VALUE_TRUE) {
         rc = true;
      }
   }
   
   return rc;
}

//******************************************************************************

void GFSMessage::setError(tonnerre::Message& message,
                          const std::string& error) {
   GFSMessage::setKeyValue(message, KEY_ERROR, error);
}

//******************************************************************************

bool GFSMessage::hasError(const tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_ERROR);
}

//******************************************************************************

const std::string& GFSMessage::getError(const tonnerre::Message& message) {
   return GFSMessage::getKeyValue(message, KEY_ERROR);
}

//******************************************************************************

void GFSMessage::setDirectory(tonnerre::Message& message,
                              const std::string& directory) {
   GFSMessage::setKeyValue(message, KEY_DIR, directory);
}

//******************************************************************************

bool GFSMessage::hasDirectory(const tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_DIR);
}

//******************************************************************************

const std::string& GFSMessage::getDirectory(const tonnerre::Message& message) {
   return GFSMessage::getKeyValue(message, KEY_DIR);
}

//******************************************************************************
      
void GFSMessage::setFile(tonnerre::Message& message,
                         const std::string& fileName) {
   GFSMessage::setKeyValue(message, KEY_FILE, fileName);
}

//******************************************************************************

bool GFSMessage::hasFile(const tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_FILE);
}

//******************************************************************************

const std::string& GFSMessage::getFile(const tonnerre::Message& message) {
   return GFSMessage::getKeyValue(message, KEY_FILE);
}

//******************************************************************************

void GFSMessage::setUniqueIdentifier(tonnerre::Message& message,
                                     const std::string& uniqueIdentifier) {
   GFSMessage::setKeyValue(message, KEY_UNIQUE_IDENTIFIER, uniqueIdentifier);
}

//******************************************************************************

bool GFSMessage::hasUniqueIdentifier(const tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_UNIQUE_IDENTIFIER);
}

//******************************************************************************

const std::string& GFSMessage::getUniqueIdentifier(const tonnerre::Message& message) {
   return GFSMessage::getKeyValue(message, KEY_UNIQUE_IDENTIFIER);
}

//******************************************************************************

void GFSMessage::setOriginFileSize(tonnerre::Message& message,
                                   unsigned long fileSize) {
   char fileSizeString[20];
   ::snprintf(fileSizeString, 20, "%lu", fileSize);
   GFSMessage::setKeyValue(message, KEY_ORIGIN_FS, fileSizeString);
}

//******************************************************************************

bool GFSMessage::hasOriginFileSize(tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_ORIGIN_FS);
}

//******************************************************************************

unsigned long GFSMessage::getOriginFileSize(tonnerre::Message& message) {
   unsigned long fileSize = 0L;
   const std::string& fileSizeString = GFSMessage::getKeyValue(message, KEY_ORIGIN_FS);
   if (!fileSizeString.empty()) {
      fileSize = ::strtoul(fileSizeString.c_str(), nullptr, 0);
   }
   
   return fileSize;
}

//******************************************************************************

void GFSMessage::setStoredFileSize(tonnerre::Message& message,
                                   unsigned long fileSize) {
   char fileSizeString[20];
   ::snprintf(fileSizeString, 20, "%lu", fileSize);
   GFSMessage::setKeyValue(message, KEY_STORED_FS, fileSizeString);
}

//******************************************************************************

bool GFSMessage::hasStoredFileSize(tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_STORED_FS);
}

//******************************************************************************

unsigned long GFSMessage::getStoredFileSize(tonnerre::Message& message) {
   unsigned long fileSize = 0L;
   const std::string& fileSizeString = GFSMessage::getKeyValue(message, KEY_STORED_FS);
   if (!fileSizeString.empty()) {
      fileSize = strtoul(fileSizeString.c_str(), nullptr, 0);
   }
   
   return fileSize;
}

//******************************************************************************

void GFSMessage::setFileList(tonnerre::Message& message,
                             const std::vector<std::string>& listFiles) {
   if (!listFiles.empty()) {
      const int numFileNames = listFiles.size();
      std::string encodedList;
      
      for (int i = 0; i < numFileNames; ++i) {
         if (i > 0) {
            encodedList += LIST_DELIMITER;
         }
         
         encodedList += listFiles[i];
      }
      
      GFSMessage::setKeyValue(message, KEY_FILE_LIST, encodedList);
   }
}

//******************************************************************************

bool GFSMessage::hasFileList(tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_FILE_LIST);
}

//******************************************************************************

bool GFSMessage::getFileList(tonnerre::Message& message,
                             std::vector<std::string>& listFiles) {
   bool success = false;
   if (hasFileList(message)) {
      const std::string& fileList = GFSMessage::getKeyValue(message, KEY_FILE_LIST);
      if (!fileList.empty()) {
         chaudiere::StringTokenizer st(fileList, LIST_DELIMITER);
         
         while (st.hasMoreTokens()) {
            listFiles.push_back(st.nextToken());
         }
         
         success = true;
      }
   }
   
   return success;
}

//******************************************************************************

void GFSMessage::setDirList(tonnerre::Message& message,
                            const std::vector<std::string>& listDirectories) {
   if (!listDirectories.empty()) {
      const int numEntries = listDirectories.size();
      std::string encodedList;
      
      for (int i = 0; i < numEntries; ++i) {
         if (i > 0) {
            encodedList += LIST_DELIMITER;
         }
         
         encodedList += listDirectories[i];
      }
      
      GFSMessage::setKeyValue(message, KEY_DIR_LIST, encodedList);
   }   
}

//******************************************************************************

bool GFSMessage::hasDirList(tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_DIR_LIST);
}

//******************************************************************************

bool GFSMessage::getDirList(tonnerre::Message& message,
                            std::vector<std::string>& listDirectories) {
   bool success = false;
   if (hasDirList(message)) {
      const std::string& list = GFSMessage::getKeyValue(message, KEY_DIR_LIST);
      if (!list.empty()) {
         chaudiere::StringTokenizer st(list, LIST_DELIMITER);
         
         while (st.hasMoreTokens()) {
            listDirectories.push_back(st.nextToken());
         }
         
         success = true;
      }
   }
   
   return success;   
}

//******************************************************************************

void GFSMessage::setDeviceList(tonnerre::Message& message,
                               const std::vector<std::string>& listDevices) {
   if (!listDevices.empty()) {
      const int numEntries = listDevices.size();
      std::string encodedList;
      
      for (int i = 0; i < numEntries; ++i) {
         if (i > 0) {
            encodedList += LIST_DELIMITER;
         }
         
         encodedList += listDevices[i];
      }
      
      GFSMessage::setKeyValue(message, KEY_DEVICE_LIST, encodedList);
   }
}

//******************************************************************************

bool GFSMessage::hasDeviceList(tonnerre::Message& message) {
   return GFSMessage::hasKey(message, KEY_DEVICE_LIST);
}

//******************************************************************************

bool GFSMessage::getDeviceList(tonnerre::Message& message,
                               std::vector<std::string>& listDevices) {
   bool success = false;
   if (hasDeviceList(message)) {
      const std::string& list = GFSMessage::getKeyValue(message, KEY_DEVICE_LIST);
      if (!list.empty()) {
         chaudiere::StringTokenizer st(list, LIST_DELIMITER);
         
         while (st.hasMoreTokens()) {
            listDevices.push_back(st.nextToken());
         }
         
         success = true;
      }
   }
   
   return success;   
}

//******************************************************************************

