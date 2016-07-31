// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_LOCALFILE_H
#define LACHEPAS_LOCALFILE_H

#include <string>

#include "DateTime.h"

namespace lachepas {

/**
 *
 */
class LocalFile {

private:
   std::string m_filePath;
   int m_localFileId;
   int m_localDirectoryId;
   chaudiere::DateTime m_createTime;
   chaudiere::DateTime m_modifyTime;
   chaudiere::DateTime m_scanTime;
   chaudiere::DateTime m_copyTime;
      
public:
   /**
    * Default constructor
    */
   LocalFile();
      
   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   LocalFile(const LocalFile& copy);
      
   /**
    * Copy operator
    * @param copy the source of the copy
    * @return target of the copy
    */
   LocalFile& operator=(const LocalFile& copy);
      
   /**
    *
    * @param filePath
    * @return
    */
   void setFilePath(const std::string& filePath);
      
   /**
    *
    * @return
    */
   const std::string& getFilePath() const;

   /**
    *
    * @param localFileId
    */
   void setLocalFileId(int localFileId);
      
   /**
    *
    * @return
    */
   int getLocalFileId() const;
      
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
    * @param createTime
    * @see chaudiere::DateTime()
    */
   void setCreateTime(const chaudiere::DateTime& createTime);
      
   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getCreateTime();

   /**
    *
    * @param modifyTime
    * @see chaudiere::DateTime()
    */
   void setModifyTime(const chaudiere::DateTime& modifyTime);
      
   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getModifyTime();

   /**
    *
    * @param scanTime
    * @see chaudiere::DateTime()
    */
   void setScanTime(const chaudiere::DateTime& scanTime);
      
   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getScanTime();

   /**
    *
    * @param copyTime
    * @see chaudiere::DateTime()
    */
   void setCopyTime(const chaudiere::DateTime& copyTime);
      
   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getCopyTime();
      
};

}

#endif

