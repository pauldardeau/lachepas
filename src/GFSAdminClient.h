// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSADMINCLIENT_H
#define LACHEPAS_GFSADMINCLIENT_H

#include <string>
#include <vector>
#include <map>
#include <memory>

#include "GFSOptions.h"
#include "KeyValuePairs.h"
#include "Vault.h"


namespace lachepas {

class DataAccess;
class LocalDirectory;
class StorageNode;
   
/**
 *
 */
class GFSAdminClient {

public:
   /**
    * Constructs a GFSAdminClient instance using the specified options
    * @param gfsOptions
    * @see GFSOptions()
    */
   GFSAdminClient(const GFSOptions& gfsOptions);
      
   /**
    * Destructor
    */
   ~GFSAdminClient();
      
   /**
    *
    * @return
    */
   bool listDevices();
      
   /**
    *
    * @return
    */
   bool statCpu();
      
   /**
    *
    * @return
    */
   bool statDevice();
      
   /**
    *
    * @return
    */
   bool statIO();
      
   /**
    *
    * @return
    */
   bool statVM();
      
   /**
    *
    * @return
    */
   bool sysInfo();
      
   /**
    *
    * @return
    */
   bool sysUptime();
      
   /**
    *
    * @return
    */
   bool lastFileRetrieve();
      
   /**
    *
    * @return
    */
   bool lastFileUpdate();
      
private:
   std::map<std::string, Vault> m_mapNodeToVault;
   std::vector<StorageNode> m_listNodes;
   std::string m_currentDir;
   std::string m_baseDir;
   std::string m_metaDataDBFile;
   std::string m_messagingService;
   DataAccess* m_dataAccess;
   GFSOptions m_gfsOptions;
   int m_localDirectoryId;
   int m_localDirectoryPathLength;
   bool m_debugPrint;

};

}

#endif

