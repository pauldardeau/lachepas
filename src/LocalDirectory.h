// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_LOCALDIRECTORY_H
#define LACHEPAS_LOCALDIRECTORY_H

#include <string>


namespace lachepas {

/**
 *
 */
class LocalDirectory {

private:
   std::string m_directoryPath;
   int m_localDirectoryId;
   int m_copyCount;
   bool m_active;
   bool m_recurse;
   bool m_compress;
   bool m_encrypt;

public:
   /**
    * Default constructor
    */
   LocalDirectory();

   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   LocalDirectory(const LocalDirectory& copy);

   /**
    * Copy operator
    * @param copy the source of the copy
    * @return target of the copy
    */
   LocalDirectory& operator=(const LocalDirectory& copy);

   /**
    *
    * @param directoryPath
    */
   void setDirectoryPath(const std::string& directoryPath);

   /**
    *
    * @return
    */
   const std::string& getDirectoryPath() const;

   /**
    *
    * @param localDirectoryId
    */
   void setLocalDirectoryId(int localDirectoryId);

   /**
    *
    * @return
    */
   int getLocalDirectoryId() const;

   /**
    *
    * @param copyCount
    */
   void setCopyCount(int copyCount);

   /**
    *
    * @return
    */
   int getCopyCount() const;

   /**
    *
    * @param recurse
    */
   void setRecurse(bool recurse);

   /**
    *
    * @return
    */
   bool getRecurse() const;

   /**
    *
    * @param compress
    */
   void setCompress(bool compress);

   /**
    *
    * @return
    */
   bool getCompress() const;

   /**
    *
    * @param encrypt
    */
   void setEncrypt(bool encrypt);

   /**
    *
    * @return
    */
   bool getEncrypt() const;

   /**
    *
    * @param active
    */
   void setActive(bool active);

   /**
    *
    * @return
    */
   bool getActive() const;
};

}

#endif

