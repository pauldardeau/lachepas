// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSEXCLUSIONS_H
#define LACHEPAS_GFSEXCLUSIONS_H

#include <string>
#include <vector>

#include "SectionedConfigDataSource.h"
#include "KeyValuePairs.h"


namespace lachepas {

class GFSExclusions {

private:

   std::vector<std::string> m_dirExclusionNames;
   std::vector<std::string> m_dirExclusionPrefixes;
   std::vector<std::string> m_fileExclusionNames;
   std::vector<std::string> m_fileExclusionSuffixes;
   bool m_exclusionsPopulated;

public:

   /**
    *
    */
   GFSExclusions();
                    
   /**
    * Copy constructor
    * @param copy source of copy
    */
   GFSExclusions(const GFSExclusions& copy);
      
   /**
    * Destructor
    */
   ~GFSExclusions();
      
   bool haveExclusions() const;
      
   bool retrieveExclusions(const std::string& dirName,
                           const chaudiere::SectionedConfigDataSource& dataSource);
      
   /**
    * Copy operator
    * @param copy source of the copy
    * @return target of the copy
    */
   GFSExclusions& operator=(const GFSExclusions& copy);
   
   /**
    *
    * @param dirName
    * @return
    */
   bool excludeDirectory(const std::string& dirName) const;
      
   /**
    *
    * @param fileName
    * @return
    */
   bool excludeFile(const std::string& fileName) const;
      
   /**
    * Retrieve names of directories to be excluded
    * @return list of directory names to be excluded
    */
   const std::vector<std::string>& getDirExclusionNames() const;
      
   /**
    * Retrieve prefixes of directories to be excluded
    * @return list of directory name prefixes to be excluded
    */
   const std::vector<std::string>& getDirExclusionPrefixes() const;
      
   /**
    * Retrieve names of files to be excluded
    * @return list of file names to be excluded
    */
   const std::vector<std::string>& getFileExclusionNames() const;
      
   /**
    * Retrieve suffixes of files to be excluded
    * @return list of file name suffixes to be excluded
    */
   const std::vector<std::string>& getFileExclusionSuffixes() const;
   
   
   /**
    *
    * @param kvpExclusions
    * @param key
    * @param listValues
    * @return
    */
   bool parseExclusionList(const chaudiere::KeyValuePairs& kvpExclusions,
                           const std::string& key,
                           std::vector<std::string>& listValues);

};

}

#endif

