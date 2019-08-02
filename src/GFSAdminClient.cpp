// Copyright Paul Dardeau, 2016
// GFSAdminClient.cpp

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


#include "GFSAdminClient.h"
#include "Logger.h"
#include "DataAccess.h"
#include "OSUtils.h"
#include "Messaging.h"
#include "Message.h"
#include "GFSMessage.h"
#include "BasicException.h"
#include "aes256.h"
#include "IniReader.h"
#include "KeyValuePairs.h"
#include "StringTokenizer.h"
#include "StrUtils.h"
#include "GFS.h"
#include "Encryption.h"

using namespace std;

static const string DB_FILE                = "gfs_db.sqlite3";

static const string EMPTY_STRING           = "";

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


//******************************************************************************

using namespace lachepas;
using namespace chaudiere;
using namespace tonnerre;

//******************************************************************************

GFSAdminClient::GFSAdminClient(const GFSOptions& gfsOptions) :
                               m_currentDir(OSUtils::getCurrentDirectory()),
                               m_metaDataDBFile(DB_FILE),
                               m_dataAccess(nullptr),
                               m_gfsOptions(gfsOptions),
                               m_localDirectoryId(-1),
                               m_localDirectoryPathLength(0),
                               m_debugPrint(false) {
   ::srand(::time(nullptr));

   m_baseDir = m_currentDir;
   
   if (gfsOptions.getDebugMode()) {
      m_debugPrint = true;
   }
   
   if (m_debugPrint) {
      Logger::debug(string("baseDir = '") +
                    m_baseDir +
                    string("'"));
   }
   
   const string& configFile = gfsOptions.getConfigFile();
   
   if (!configFile.empty()) {
      if (OSUtils::pathExists(configFile)) {
         try {
            Logger::debug("reading configuration file");
            Messaging::initialize(configFile);
         } catch (const BasicException& be) {
            Logger::error("unable to read configuration file");
            return;
         }
         
         Logger::info(string("initializing database with file: '") +
                      m_metaDataDBFile +
                      string("'"));
         
         m_dataAccess = new DataAccess(m_metaDataDBFile);
         if (m_dataAccess->open()) {
            //Logger::info("successfully opened database");
            
            if (m_dataAccess->getActiveStorageNodes(m_listNodes)) {
               if (m_listNodes.empty()) {
                  Logger::warning("no storage nodes defined");
               }
            } else {
               Logger::error("unable to retrieve storage nodes");
            }
            
         } else {
            Logger::error("unable to open database");
         }
      } else {
         Logger::error(string("config file doesn't exist '") +
                       configFile +
                       string("'"));
      }
   } else {
      Logger::error("missing config file");
   }
}

//******************************************************************************

GFSAdminClient::~GFSAdminClient() {
}

//******************************************************************************

bool GFSAdminClient::lastFileRetrieve() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      Message message(MSG_LAST_FILE_RETRIEVE, MessageType::MessageTypeText);
      Message response;
      if (message.send(nodeName, response)) {
         if (GFSMessage::getRC(response)) {
            if (GFSMessage::hasKey(response, KEY_LAST_FILE_RETRIEVE)) {
               const string& lastFileRetrieve =
                  GFSMessage::getKeyValue(response, KEY_LAST_FILE_RETRIEVE);
               if (!lastFileRetrieve.empty()) {
                  ::printf("%s\n", lastFileRetrieve.c_str());
               } else {
                  Logger::error("unable to retrieve last file retrieve");
               }
            } else {
               Logger::error("response missing last file retrieve");
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

bool GFSAdminClient::lastFileUpdate() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      Message message(MSG_LAST_FILE_UPDATE, MessageType::MessageTypeText);
      Message response;
      if (message.send(nodeName, response)) {
         if (GFSMessage::getRC(response)) {
            if (GFSMessage::hasKey(response, KEY_LAST_FILE_UPDATE)) {
               const string& lastFileUpdate =
                  GFSMessage::getKeyValue(response, KEY_LAST_FILE_UPDATE);
               if (!lastFileUpdate.empty()) {
                  ::printf("%s\n", lastFileUpdate.c_str());
               } else {
                  Logger::error("unable to retrieve last file update");
               }
            } else {
               Logger::error("response missing last file update");
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

bool GFSAdminClient::listDevices() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      Message message(MSG_LIST_DEVICES, MessageType::MessageTypeText);
      Message response;
      if (message.send(nodeName, response)) {
         if (GFSMessage::getRC(response)) {
            if (GFSMessage::hasDeviceList(response)) {
               vector<string> listDevices;
               if (GFSMessage::getDeviceList(response, listDevices)) {
                  const int numDevices = listDevices.size();
                  for (int i = 0; i < numDevices; ++i) {
                     ::printf("%s\n", listDevices[i].c_str());
                  }
               } else {
                  Logger::error("unable to retrieve device list");
               }
            } else {
               Logger::error("response missing list devices");
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

bool GFSAdminClient::statCpu() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      //TODO: implement statCpu
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSAdminClient::statDevice() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      //TODO: implement statDevice
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSAdminClient::statIO() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      //TODO: implement statIO
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSAdminClient::statVM() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      //TODO: implement statVM
   } else {
      Logger::error("missing node name");
   }
   
   return success;
}

//******************************************************************************

bool GFSAdminClient::sysInfo() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      Message message(MSG_SYS_INFO, MessageType::MessageTypeText);
      Message response;
      if (message.send(nodeName, response)) {
         if (GFSMessage::getRC(response)) {
            if (GFSMessage::hasKey(response, KEY_SYS_SYSNAME)) {
               const string& sysName =
                  GFSMessage::getKeyValue(response, KEY_SYS_SYSNAME);
               if (!sysName.empty()) {
                  ::printf("sysName: %s\n", sysName.c_str());
               }
            }
            
            if (GFSMessage::hasKey(response, KEY_SYS_NODENAME)) {
               const string& nodeName =
                  GFSMessage::getKeyValue(response, KEY_SYS_NODENAME);
               if (!nodeName.empty()) {
                  ::printf("nodeName: %s\n", nodeName.c_str());
               }
            }

            if (GFSMessage::hasKey(response, KEY_SYS_RELEASE)) {
               const string& release =
                  GFSMessage::getKeyValue(response, KEY_SYS_RELEASE);
               if (!release.empty()) {
                  ::printf("release: %s\n", release.c_str());
               }
            }

            if (GFSMessage::hasKey(response, KEY_SYS_VERSION)) {
               const string& version =
                  GFSMessage::getKeyValue(response, KEY_SYS_VERSION);
               if (!version.empty()) {
                  ::printf("version: %s\n", version.c_str());
               }
            }

            if (GFSMessage::hasKey(response, KEY_SYS_MACHINE)) {
               const string& machine =
                  GFSMessage::getKeyValue(response, KEY_SYS_MACHINE);
               if (!machine.empty()) {
                  ::printf("machine: %s\n", machine.c_str());
               }
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

bool GFSAdminClient::sysUptime() {
   bool success = false;
   const string& nodeName = m_gfsOptions.getNode();
   
   if (!nodeName.empty()) {
      Message message(MSG_SYS_UPTIME, MessageType::MessageTypeText);
      Message response;
      if (message.send(nodeName, response)) {
         if (GFSMessage::getRC(response)) {
            if (GFSMessage::hasKey(response, KEY_SYS_UPTIME)) {
               const string& sysUptime =
                  GFSMessage::getKeyValue(response, KEY_SYS_UPTIME);
               if (!sysUptime.empty()) {
                  ::printf("%s\n", sysUptime.c_str());
               } else {
                  Logger::error("unable to retrieve system uptime");
               }
            } else {
               Logger::error("response missing system uptime");
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

