// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSNODEADMIN_H
#define LACHEPAS_GFSNODEADMIN_H

#include <string>
#include <vector>

#include "Message.h"


namespace lachepas {

/**
 *
 */
class GFSNodeAdmin {

private:
   std::string m_baseDir;
   std::string m_messagingService;
   bool m_debugPrint;

public:
   /**
    * Default constructor
    */
   GFSNodeAdmin();
      
   /**
    * Destructor
    */
   ~GFSNodeAdmin();
      
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
    * @return
    */
   bool cpuStat();
      
   /**
    *
    * @param device
    * @return
    */
   bool deviceStat(const std::string& device);

   /**
    *
    * @return
    */
   bool ioStat();

   /**
    *
    * @return
    */
   bool vmStat();
      
   /**
    *
    * @param responseMessage
    * @return
    */
   bool infoSys(tonnerre::Message& responseMessage);
      
   /**
    *
    * @param uptimeSeconds
    * @return
    */
   bool uptimeSys(std::string& uptimeSeconds);

   /**
    *
    * @param listDevices
    * @return
    */
   bool deviceList(std::vector<std::string>& listDevices);

   /**
    *
    * @param fileUpdateDate
    * @return
    */
   bool lastFileUpdate(std::string& fileUpdateDate);

   /**
    *
    * @param fileRetrieveDate
    * @return
    */
   bool lastFileRetrieve(std::string& fileRetrieveDate);

};

}

#endif

