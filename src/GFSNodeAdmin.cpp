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

using namespace std;
using namespace lachepas;
using namespace chaudiere;
using namespace tonnerre;

static const string EMPTY_STRING           = "";
static const string SLASH                  = "/";
static const string ZERO                   = "0";

static const string KEY_LAST_FILE_RETRIEVE = "lastRetrieve";
static const string KEY_LAST_FILE_UPDATE   = "lastUpdate";

static const string KEY_SYS_SYSNAME        = "sysName";
static const string KEY_SYS_NODENAME       = "nodeName";
static const string KEY_SYS_RELEASE        = "release";
static const string KEY_SYS_VERSION        = "version";
static const string KEY_SYS_MACHINE        = "machine";

static const string KEY_SYS_UPTIME         = "uptime";

static const string MSG_LAST_FILE_RETRIEVE = "fileRetrieveLast";
static const string MSG_LAST_FILE_UPDATE   = "fileUpdateLast";

static const string MSG_LIST_DEVICES       = "deviceList";

static const string MSG_STAT_CPU           = "cpuStat";
static const string MSG_STAT_DEVICE        = "deviceStat";
static const string MSG_STAT_IO            = "ioStat";
static const string MSG_STAT_VM            = "vmStat";

static const string MSG_SYS_INFO           = "infoSys";
static const string MSG_SYS_UPTIME         = "uptimeSys";

static const string ERR_NOT_IMPLEMENTED    = "not implemented";
static const string ERR_NOT_RECOGNIZED     = "unrecognized message";


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
                          const string& requestName,
                          const string& requestPayload,
                          string& responsePayload) {
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
         string uptimeSeconds;
         if (m_nodeAdmin.uptimeSys(uptimeSeconds)) {
            GFSMessage::setKeyValue(responseMessage,
                                    KEY_SYS_UPTIME,
                                    uptimeSeconds);
            encodeSuccess(responseMessage);
         } else {
            encodeError(responseMessage, "unable to retrieve system uptime");
         }
      } else if (requestName == MSG_LIST_DEVICES) {
         vector<string> listDevices;

         if (m_nodeAdmin.deviceList(listDevices)) {
            encodeSuccess(responseMessage);
            GFSMessage::setDirList(responseMessage, listDevices);
         } else {
            encodeError(responseMessage, "unable to obtain device list");
         }
      } else if (requestName == MSG_LAST_FILE_RETRIEVE) {
         string fileRetrieveDate;
         if (m_nodeAdmin.lastFileRetrieve(fileRetrieveDate)) {
            GFSMessage::setKeyValue(responseMessage,
                                    KEY_LAST_FILE_RETRIEVE,
                                    fileRetrieveDate);
            encodeSuccess(responseMessage);
         } else {
            encodeError(responseMessage, "unable to obtain last file retrieve");
         }
      } else if (requestName == MSG_LAST_FILE_UPDATE) {
         string fileUpdateDate;
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

bool GFSNodeAdmin::run(const string& directory,
                       const string& iniFilePath,
                       const string& serviceName) {
   if (OSUtils::directoryExists(directory)) {
      if (OSUtils::pathExists(iniFilePath)) {
         m_baseDir = directory;

         MessagingServer server(iniFilePath, serviceName);
         GFSAdminMessageHandler handler(*this);
         server.setMessageHandler(&handler);
         const int rc = server.run();
         return (rc == 0);
      } else {
         Logger::error(string("ini file path does not exist: '") +
                       iniFilePath +
                       string("'"));
         return false;
      }
   } else {
      Logger::error(string("directory to serve does not exist: ") +
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

bool GFSNodeAdmin::deviceStat(const string& device) {
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

bool GFSNodeAdmin::uptimeSys(string& uptimeSeconds) {
   bool success = false;
   long long uptimeSecondsValue = 0L;

   if (SystemStats::uptimeSeconds(uptimeSecondsValue)) {
      uptimeSeconds = StrUtils::toString(uptimeSecondsValue);
      success = true;
   } else {
      ::printf("SystemStats::uptimeSeconds failed\n");
   }

   return success;
}

//******************************************************************************

bool GFSNodeAdmin::deviceList(vector<string>& listDevices) {
   if (m_debugPrint) {
      Logger::debug("deviceList called");
   }

   bool success = false;

   //TODO: implement deviceList

   return success;
}

//******************************************************************************

bool GFSNodeAdmin::lastFileUpdate(string& fileUpdateDate) {
   //TODO: implement lastFileUpdate
   return false;
}

//******************************************************************************

bool GFSNodeAdmin::lastFileRetrieve(string& fileRetrieveDate) {
   //TODO: implement lastFileRetrieve
   return false;
}

//******************************************************************************
//******************************************************************************

