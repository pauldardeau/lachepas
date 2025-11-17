// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSMESSAGE_H
#define LACHEPAS_GFSMESSAGE_H

#include <string>
#include <string>

#include "Message.h"


namespace lachepas {

/**
 *
 */
class GFSMessage {

public:
   /**
    *
    * @param message
    * @param key
    * @param value
    */
   static void setKeyValue(tonnerre::Message& message,
                           const std::string& key,
                           const std::string& value);

   /**
    *
    * @param message
    * @param key
    * @return
    */
   static bool hasKey(const tonnerre::Message& message,
                      const std::string& key);

   /**
    *
    * @param message
    * @param key
    * @return
    */
   static const std::string& getKeyValue(const tonnerre::Message& message,
                                         const std::string& key);

   /**
    *
    * @param message
    * @param rc
    */
   static void setRC(tonnerre::Message& message, bool rc);

   /**
    *
    * @param message
    * @return
    */
   static bool getRC(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @param error
    */
   static void setError(tonnerre::Message& message,
                        const std::string& error);

   /**
    *
    * @param message
    * @return
    */
   static bool hasError(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @return
    */
   static const std::string& getError(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @param directory
    */
   static void setDirectory(tonnerre::Message& message,
                            const std::string& directory);

   /**
    *
    * @param message
    * @return
    */
   static bool hasDirectory(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @return
    */
   static const std::string& getDirectory(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @param fileName
    */
   static void setFile(tonnerre::Message& message,
                       const std::string& fileName);

   /**
    *
    * @param message
    * @return
    */
   static bool hasFile(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @return
    */
   static const std::string& getFile(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @param uniqueIdentifier
    */
   static void setUniqueIdentifier(tonnerre::Message& message,
                                   const std::string& uniqueIdentifier);

   /**
    *
    * @param message
    * @return
    */
   static bool hasUniqueIdentifier(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @return
    */
   static const std::string& getUniqueIdentifier(const tonnerre::Message& message);

   /**
    *
    * @param message
    * @param fileSize
    */
   static void setOriginFileSize(tonnerre::Message& message,
                                 unsigned long fileSize);

   /**
    *
    * @param message
    * @return
    */
   static bool hasOriginFileSize(tonnerre::Message& message);

   /**
    *
    * @param message
    * @return
    */
   static unsigned long getOriginFileSize(tonnerre::Message& message);

   /**
    *
    * @param message
    * @param fileSize
    */
   static void setStoredFileSize(tonnerre::Message& message,
                                 unsigned long fileSize);

   /**
    *
    * @param message
    * @return
    */
   static bool hasStoredFileSize(tonnerre::Message& message);

   /**
    *
    * @param message
    * @return
    */
   static unsigned long getStoredFileSize(tonnerre::Message& message);

   /**
    *
    * @param message
    * @param listFiles
    */
   static void setFileList(tonnerre::Message& message,
                           const std::vector<std::string>& listFiles);

   /**
    *
    * @param message
    * @return
    */
   static bool hasFileList(tonnerre::Message& message);

   /**
    *
    * @param message
    * @param listFiles
    * @return
    */
   static bool getFileList(tonnerre::Message& message,
                           std::vector<std::string>& listFiles);

   /**
    *
    * @param message
    * @param listDirectories
    */
   static void setDirList(tonnerre::Message& message,
                          const std::vector<std::string>& listDirectories);

   /**
    *
    * @param message
    * @return
    */
   static bool hasDirList(tonnerre::Message& message);

  /**
   *
   * @param message
   * @param listDirectories
   * @return
   */
  static bool getDirList(tonnerre::Message& message,
                         std::vector<std::string>& listDirectories);

  /**
   *
   * @param message
   * @param listDevices
   */
  static void setDeviceList(tonnerre::Message& message,
                            const std::vector<std::string>& listDevices);

  /**
   *
   * @param message
   * @return
   */
  static bool hasDeviceList(tonnerre::Message& message);

  /**
   *
   * @param message
   * @param listDevices
   * @return
   */
  static bool getDeviceList(tonnerre::Message& message,
                            std::vector<std::string>& listDevices);

};

}

#endif

