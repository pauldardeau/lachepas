// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSMESSAGECOMMANDS_H
#define LACHEPAS_GFSMESSAGECOMMANDS_H

#include <string>


namespace lachepas {

/**
 *
 */
class GFSMessageCommands {

public:
   static const std::string MSG_DIR_STAT;
   static const std::string MSG_DIR_LIST;

   static const std::string MSG_FILE_ADD;
   static const std::string MSG_FILE_UPDATE;
   static const std::string MSG_FILE_DELETE;
   static const std::string MSG_FILE_RETRIEVE;
   static const std::string MSG_FILE_ID;
   static const std::string MSG_FILE_STAT;
   static const std::string MSG_FILE_LIST;
};

}

#endif

