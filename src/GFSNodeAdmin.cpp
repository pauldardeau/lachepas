// Copyright Paul Dardeau, 2016
// GFSNodeAdmin.cpp

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "GFSNodeAdmin.h"
#include "Message.h"
#include "MessagingServer.h"
#include "GFSMessageHandler.h"
#include "OSUtils.h"
#include "StrUtils.h"
#include "GFSMessage.h"
#include "FileReferenceCount.h"
#include "GFS.h"
#include "Encryption.h"

#include "SystemInfo.h"
#include "SystemStats.h"

using namespace lachepas;
using namespace chaudiere;
using namespace tonnerre;

static const std::string EMPTY_STRING           = "";
static const std::string SLASH                  = "/";
static const std::string ZERO                   = "0";

static const std::string KEY_LAST_FILE_RETRIEVE = "lastRetrieve";
static const std::string KEY_LAST_FILE_UPDATE   = "lastUpdate";

static const std::string KEY_SYS_SYSNAME        = "sysName";
static const std::string KEY_SYS_NODENAME       = "nodeName";
static const std::string KEY_SYS_RELEASE        = "release";
static const std::string KEY_SYS_VERSION        = "version";
static const std::string KEY_SYS_MACHINE        = "machine";

static const std::string KEY_SYS_UPTIME         = "uptime";

static const std::string MSG_LAST_FILE_RETRIEVE = "fileRetrieveLast";
static const std::string MSG_LAST_FILE_UPDATE   = "fileUpdateLast";

static const std::string MSG_LIST_DEVICES       = "deviceList";

static const std::string MSG_STAT_CPU           = "cpuStat";
static const std::string MSG_STAT_DEVICE        = "deviceStat";
static const std::string MSG_STAT_IO            = "ioStat";
static const std::string MSG_STAT_VM            = "vmStat";

static const std::string MSG_SYS_INFO           = "infoSys";
static const std::string MSG_SYS_UPTIME         = "uptimeSys";

static const std::string ERR_NOT_IMPLEMENTED    = "not implemented";
static const std::string ERR_NOT_RECOGNIZED     = "unrecognized message";


//******************************************************************************
//******************************************************************************
                                    
class GFSAdminMessageHandler : public GFSMessageHandler
{
private:
   GFSNodeAdmin& m_nodeAdmin;
   
public:
   GFSAdminMessageHandler(GFSNodeAdmin& nodeAdmin) :
      m_nodeAdmin(nodeAdmin) {
   }
   
   void handleTextMessage(const Message& requestMessage,
                          Message& responseMessage,
                          const std::string& requestName,
                          const std::string& requestPayload,
                          std::string& responsePayload) {
      if (requestName == MSG_STAT_CPU) {
         encodeError(responseMessage, ERR_NOT_IMPLEMENTED);
      } else if (requestName == MSG_STAT_DEVICE) {
         encodeError(responseMessage, ERR_NOT_IMPLEMENTED);
      } else if (requestName == MSG_STAT_IO) {
         encodeError(responseMessage, ERR_NOT_IMPLEMENTED);
      } else if (requestName == MSG_STAT_VM) {
         encodeError(responseMessage, ERR_NOT_IMPLEMENTED);
      } else if (requestName == MSG_SYS_INFO) {
         if (m_nodeAdmin.infoSys(responseMessage)) {
            encodeSuccess(responseMessage);
         } else {
            encodeError(responseMessage, "unable to retrieve system info");
         }
      } else if (requestName == MSG_SYS_UPTIME) {
         std::string uptimeSeconds;
         if (m_nodeAdmin.uptimeSys(uptimeSeconds)) {
            GFSMessage::setKeyValue(responseMessage,
                                    KEY_SYS_UPTIME,
                                    uptimeSeconds);
            encodeSuccess(responseMessage);
         } else {
            encodeError(responseMessage, "unable to retrieve system uptime");
         }
      } else if (requestName == MSG_LIST_DEVICES) {
         std::vector<std::string> listDevices;
            
         if (m_nodeAdmin.deviceList(listDevices)) {
            encodeSuccess(responseMessage);
            GFSMessage::setDirList(responseMessage, listDevices);
         } else {
            encodeError(responseMessage, "unable to obtain device list");
         }
      } else if (requestName == MSG_LAST_FILE_RETRIEVE) {
         std::string fileRetrieveDate;
         if (m_nodeAdmin.lastFileRetrieve(fileRetrieveDate)) {
            GFSMessage::setKeyValue(responseMessage,
                                    KEY_LAST_FILE_RETRIEVE,
                                    fileRetrieveDate);
            encodeSuccess(responseMessage);
         } else {
            encodeError(responseMessage, "unable to obtain last file retrieve");
         }
      } else if (requestName == MSG_LAST_FILE_UPDATE) {
         std::string fileUpdateDate;
         if (m_nodeAdmin.lastFileUpdate(fileUpdateDate)) {
            GFSMessage::setKeyValue(responseMessage,
                                    KEY_LAST_FILE_UPDATE,
                                    fileUpdateDate);
            encodeSuccess(responseMessage);
         } else {
            encodeError(responseMessage, "unable to obtain last file update");
         }
      } else {
         encodeError(responseMessage, ERR_NOT_RECOGNIZED);         
      }
   }
};

//******************************************************************************
//******************************************************************************

GFSNodeAdmin::GFSNodeAdmin() :
   m_debugPrint(true) {
}

//******************************************************************************

GFSNodeAdmin::~GFSNodeAdmin() {
}

//******************************************************************************

bool GFSNodeAdmin::run(const std::string& directory,
                       const std::string& iniFilePath,
                       const std::string& serviceName) {
   if (OSUtils::directoryExists(directory)) {
      if (OSUtils::pathExists(iniFilePath)) {
         m_baseDir = directory;
         
         MessagingServer server(iniFilePath, serviceName);
         GFSAdminMessageHandler handler(*this);
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

bool GFSNodeAdmin::cpuStat() {
   //TODO: implement cpuStat
   return false;
}

//******************************************************************************

bool GFSNodeAdmin::deviceStat(const std::string& device) {
   //TODO: implement deviceStat
   return false;
}

//******************************************************************************

bool GFSNodeAdmin::ioStat() {
   //TODO: implement ioStat
   return false;
}

//******************************************************************************

bool GFSNodeAdmin::vmStat() {
   //TODO: implement vmStat
   return false;
}

//******************************************************************************

bool GFSNodeAdmin::infoSys(tonnerre::Message& responseMessage) {
   bool success = false;
   
   chaudiere::SystemInfo systemInfo;
   if (systemInfo.retrievedSystemInfo()) {
      GFSMessage::setKeyValue(responseMessage, KEY_SYS_SYSNAME, systemInfo.sysName());
      GFSMessage::setKeyValue(responseMessage, KEY_SYS_NODENAME, systemInfo.nodeName());
      GFSMessage::setKeyValue(responseMessage, KEY_SYS_RELEASE, systemInfo.release());
      GFSMessage::setKeyValue(responseMessage, KEY_SYS_VERSION, systemInfo.version());
      GFSMessage::setKeyValue(responseMessage, KEY_SYS_MACHINE, systemInfo.machine());
      success = true;
   }
   
   return success;
}

//******************************************************************************

bool GFSNodeAdmin::uptimeSys(std::string& uptimeSeconds) {
   bool success = false;
   long long uptimeSecondsValue = 0L;
   
   if (SystemStats::uptimeSeconds(uptimeSecondsValue)) {
      char secondsString[20];
      ::memset(secondsString, 0, sizeof(secondsString));
      ::sprintf(secondsString, "%lld", uptimeSecondsValue);
      uptimeSeconds = secondsString;
      success = true;
   } else {
      ::printf("SystemStats::uptimeSeconds failed\n");
   }
   
   return success;
}

//******************************************************************************

bool GFSNodeAdmin::deviceList(std::vector<std::string>& listDevices) {
   if (m_debugPrint) {
      Logger::debug("deviceList called");
   }
   
   bool success = false;

   //TODO: implement deviceList

   return success;
}

//******************************************************************************

bool GFSNodeAdmin::lastFileUpdate(std::string& fileUpdateDate) {
   //TODO: implement lastFileUpdate
   return false;
}

//******************************************************************************

bool GFSNodeAdmin::lastFileRetrieve(std::string& fileRetrieveDate) {
   //TODO: implement lastFileRetrieve
   return false;
}

//******************************************************************************
//******************************************************************************

