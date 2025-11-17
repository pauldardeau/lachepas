// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_FILESYNC_H
#define LACHEPAS_FILESYNC_H

#include "DBDate.h"

namespace lachepas {

/**
 *
 */
class FileSync {

private:
   chapeau::DBDate m_syncTime;
   int m_vaultFileId;

public:
   /**
    * Default constructor
    */
   FileSync();

   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   FileSync(const FileSync& copy);

   /**
    * Copy operator
    * @param copy the source of the copy
    * @return target of the copy
    */
   FileSync& operator=(const FileSync& copy);

   /**
    * Sets the vault file identifier
    * @param vaultFileId the identifier for the vault file
    */
   void setVaultFileId(int vaultFileId);

   /**
    * Retrieves the vault file identifier
    * @return identifier for the vault file
    */
   int getVaultFileId() const;

};

}

#endif

