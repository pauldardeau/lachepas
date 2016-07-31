// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_FILEREFERENCECOUNT_H
#define LACHEPAS_FILEREFERENCECOUNT_H

#include <string>

namespace lachepas {

class FileReferenceCount {

public:
   long referenceCountForFile(const std::string& filePath);
   bool storeInitialReferenceCount(const std::string& filePath);
   bool storeUpdatedReferenceCount(const std::string& filePath,
                                   long refCountValue);
};

}

#endif

