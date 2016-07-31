// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSSERVER_H
#define LACHEPAS_GFSSERVER_H

#include <string>
#include <vector>

#include "Message.h"


namespace lachepas {

class FileReferenceCount;
   
/**
 *
 */
class GFSServer {

protected:
   /**
    *
    * @param filePath
    * @param fileContents
    * @param uniqueIdentifier
    * @return
    */
   bool writeFile(const std::string& filePath,
                  const std::string& fileContents,
                  std::string& uniqueIdentifier);
                     
   /**
    *
    * @param filePath
    * @return
    */
   long referenceCountForFile(const std::string& filePath);
      
   /**
    *
    * @param filePath
    * @return
    */
   bool storeInitialReferenceCount(const std::string& filePath);
      
   /**
    *
    * @param filePath
    * @param refCountValue
    * @return
    */
   bool storeUpdatedReferenceCount(const std::string& filePath,
                                   long refCountValue);
                                                 
   /**
    *
    * @param filePath
    * @return
    */
   bool incrementReferenceCount(const std::string& filePath);
      
   /**
    *
    * @param filePath
    * @return
    */
   bool decrementReferenceCount(const std::string& filePath);
      
public:
   /**
    * Default constructor
    */
   GFSServer();
      
   /**
    * Destructor
    */
   ~GFSServer();
      
   /**
    * Initializes the specified directory (one-time) in preparation for serving files
    * @param directory the directory path to initialize
    * @return boolean indicating whether the directory has been initialized
    */
   bool initializeDirectory(const std::string& directory);

   /**
    * Runs the server using the specified directory and messaging parameters
    * @param directory the directory to serve
    * @param iniFilePath the file path to the INI configuration file for messaging
    * @param serviceName the name of the messaging service to provide service on
    * @return boolean indicating whether the server was able to run successfully
    */
   bool run(const std::string& directory,
            const std::string& iniFilePath,
            const std::string& serviceName);


   // --------------------------------------
   // Operations once the server is running
   // --------------------------------------
               
   /**
    *
    * @param directory
    * @return
    */
   bool dirStat(const std::string& directory);
      
   /**
    *
    * @param listDirectories
    * @return
    */
   bool dirList(std::vector<std::string>& listDirectories);

   /**
    *
    * @param fileName
    * @param fileContents
    * @param directory
    * @param uniqueIdentifier
    * @return
    */
   bool fileAdd(const std::string& fileName,
                const std::string& fileContents,
                std::string& directory,
                std::string& uniqueIdentifier);
      
   /**
    *
    * @param fileContents
    * @param directory
    * @param fileName
    * @param uniqueIdentifier
    * @return
    */
   bool fileUpdate(const std::string& fileContents,
                   std::string& directory,
                   std::string& fileName,
                   std::string& uniqueIdentifier);

   /**
    *
    * @param directory
    * @param fileName
    * @return
    */
   bool fileDelete(const std::string& directory,
                   const std::string& fileName);
      
   /**
    *
    * @param directory
    * @param fileName
    * @param uniqueIdentifier
    * @return
    */
   bool fileUniqueIdentifier(const std::string& directory,
                             const std::string& fileName,
                             std::string& uniqueIdentifier);
      
   //TODO: how to return file stat info?
   /**
    *
    * @param directory
    * @param fileName
    * @return
    */
   bool fileStat(const std::string& directory,
                 const std::string& fileName);

   /**
    *
    * @param directory
    * @param listFiles
    * @return
    */
   bool fileList(const std::string& directory,
                 std::vector<std::string>& listFiles);

   /**
    *
    * @param directory
    * @param fileName
    * @param filePath
    * @return
    */
   bool getPathForFile(const std::string& directory,
                       const std::string& fileName,
                       std::string& filePath);
                          
   /**
    *
    * @param directory
    * @param fileName
    * @param fileContents
    * @return
    */
   bool retrieveFileContents(const std::string& directory,
                             const std::string& fileName,
                             std::string& fileContents);

private:
   std::vector<std::string> m_listSubdirs;
   FileReferenceCount* m_fileReferenceCount;
   std::string m_baseDir;
   std::string m_messagingService;
   bool m_debugPrint;

};

}

#endif

