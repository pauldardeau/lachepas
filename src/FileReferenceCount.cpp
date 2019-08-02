// Copyright Paul Dardeau, 2016
// FileReferenceCount.cpp

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef __APPLE__
#include <sys/xattr.h>
#elif defined(__FreeBSD__)
#include <sys/extattr.h>
#elif defined(__linux__)
#include <sys/xattr.h>
#endif


#include "FileReferenceCount.h"

#ifdef __linux__
static const char ATTR_REF_COUNT[] = "user.refcount";
#else
static const char ATTR_REF_COUNT[] = "refcount";
#endif

using namespace std;
using namespace lachepas;

//******************************************************************************

long FileReferenceCount::referenceCountForFile(const string& filePath) {
   char refcount[12];
   memset(refcount, 0, sizeof(refcount));
   const ssize_t numBytesRead =
#ifdef __FreeBSD__
      ::extattr_get_file(filePath.c_str(),
                         EXTATTR_NAMESPACE_USER,
                         ATTR_REF_COUNT,
                         refcount,
                         sizeof(refcount));
#elif defined(__APPLE__)
      ::getxattr(filePath.c_str(),
                 ATTR_REF_COUNT,
                 refcount,
                 sizeof(refcount),
                 0,
                 0);
#elif defined(__linux__)
      ::getxattr(filePath.c_str(),
                 ATTR_REF_COUNT,
                 refcount,
                 sizeof(refcount));
#else
#error no support for retrieving extended attribute
#endif
                 
   long refCountValue = 0L;
   
   if (numBytesRead > 0) {
      refCountValue = ::atol(refcount);
   } else if (numBytesRead < 0) {
#ifdef __linux__
      if (errno == ENODATA) {
#else
      if (errno == ENOATTR) {
#endif
         refCountValue = 1L;
      } else {
#ifdef __linux__
         if (errno == ENOTSUP) {
            ::printf("extended attributes not supported\n");
         }
#endif
         ::printf("getxattr failed for file '%s'\n", filePath.c_str());
         return -1L;
      }
   } else {
      ::printf("getxattr returned 0 for file '%s'\n", filePath.c_str());
      return -1L;
   }
   
   return refCountValue;   
}

//******************************************************************************

bool FileReferenceCount::storeInitialReferenceCount(const string& filePath) {
   bool storeSuccess = false;
   char refcount[12];
   memset(refcount, 0, sizeof(refcount));
   refcount[0] = '1';

#ifdef __FreeBSD__   
   const ssize_t numBytesWritten =
      ::extattr_set_file(filePath.c_str(),
                         EXTATTR_NAMESPACE_USER,
                         ATTR_REF_COUNT,
                         refcount,
                         sizeof(refcount));
   if (numBytesWritten > 0) {
      storeSuccess = true;
   }
#elif defined(__APPLE__)
   const int rc = ::setxattr(filePath.c_str(),
                 ATTR_REF_COUNT,
                 refcount,
                 sizeof(refcount),
                 0,
                 0);
   if (rc == 0) {
      storeSuccess = true;
   }
#elif defined(__linux__)
   const int rc =
      ::setxattr(filePath.c_str(),
                 ATTR_REF_COUNT,
                 refcount,
                 sizeof(refcount),
                 XATTR_CREATE);
   if (rc == 0) {
      storeSuccess = true;
   } else {
      if (errno == ENOTSUP) {
         ::printf("extended attributes not supported\n");
      }
   }
#else
#error no support for setting extended attribute
#endif

   if (!storeSuccess) {
      ::printf("set initial extended attribute failed for file '%s'\n",
               filePath.c_str());
   }

   return storeSuccess;
}

//******************************************************************************

bool FileReferenceCount::storeUpdatedReferenceCount(const string& filePath,
                                                    long refCountValue) {
   if (refCountValue < 1L) {
      return false;
   }
   
   bool storeSuccess = false;
   char refcount[12];
   ::memset(refcount, 0, sizeof(refcount));
   snprintf(refcount, sizeof(refcount), "%ld", refCountValue);

#ifdef __FreeBSD__
   const ssize_t numBytesWritten =
      ::extattr_set_file(filePath.c_str(),
                         EXTATTR_NAMESPACE_USER,
                         ATTR_REF_COUNT,
                         refcount,
                         sizeof(refcount));
   if (numBytesWritten > 0) {
      storeSuccess = true;
   }
#elif defined(__APPLE__)
   const int rc = ::setxattr(filePath.c_str(),
                 ATTR_REF_COUNT,
                 refcount,
                 sizeof(refcount),
                 0,
                 XATTR_REPLACE);
   if (rc == 0) {
      storeSuccess = true;
   }
#elif defined(__linux__)
   const int rc =
      ::setxattr(filePath.c_str(),
                 ATTR_REF_COUNT,
                 refcount,
                 sizeof(refcount),
                 XATTR_REPLACE);
   if (rc == 0) {
      storeSuccess = true;
   } else {
      if (errno == ENOTSUP) {
         ::printf("extended attributes not supported\n");
      }
   }
#else
#error no support for setting extended attribute
#endif

   if (storeSuccess) {
      //::printf("set refcount to %ld for file '%s'\n", refCountValue, filePath.c_str());
   } else {
      ::printf("set updated extended attribute failed for file '%s'\n",
               filePath.c_str());
   }

   return storeSuccess;
}

//******************************************************************************

