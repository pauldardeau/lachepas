// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFS_H
#define LACHEPAS_GFS_H

#include <string>


namespace lachepas {

class GFS {

public:
   static std::string uniqueIdentifierForString(const std::string& s);
   static std::string uniqueIdentifierForBuffer(const char* s, int length);
   static bool uniqueIdentifierForFile(const std::string& filePath,
                                       std::string& uniqueIdentifier);
   static bool readFile(const std::string& filePath,
                        std::string& fileContents);
};

}

#endif

