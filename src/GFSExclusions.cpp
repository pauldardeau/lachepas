// Copyright Paul Dardeau, 2016
// GFSExclusions.cpp

#include <algorithm>

#include "GFSExclusions.h"
#include "Logger.h"
#include "StringTokenizer.h"
#include "StrUtils.h"

static const std::string SEC_EXCLUSIONS              = "Exclusions:";
static const std::string KEY_DIR_EXCLUSION_NAMES     = "dir_exclusion_names";
static const std::string KEY_DIR_EXCLUSION_PREFIXES  = "dir_exclusion_prefixes";
static const std::string KEY_FILE_EXCLUSION_NAMES    = "file_exclusion_names";
static const std::string KEY_FILE_EXCLUSION_SUFFIXES = "file_exclusion_suffixes";

using namespace lachepas;
using namespace chaudiere;

//******************************************************************************

GFSExclusions::GFSExclusions() :
   m_exclusionsPopulated(false) {
}

//******************************************************************************

GFSExclusions::GFSExclusions(const GFSExclusions& copy) :
   m_dirExclusionNames(copy.m_dirExclusionNames),
   m_dirExclusionPrefixes(copy.m_dirExclusionPrefixes),
   m_fileExclusionNames(copy.m_fileExclusionNames),
   m_fileExclusionSuffixes(copy.m_fileExclusionSuffixes),
   m_exclusionsPopulated(copy.m_exclusionsPopulated) {
}

//******************************************************************************

GFSExclusions::~GFSExclusions() {
}

//******************************************************************************

GFSExclusions& GFSExclusions::operator=(const GFSExclusions& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_dirExclusionNames = copy.m_dirExclusionNames;
   m_dirExclusionPrefixes = copy.m_dirExclusionPrefixes;
   m_fileExclusionNames = copy.m_fileExclusionNames;
   m_fileExclusionSuffixes = copy.m_fileExclusionSuffixes;
   m_exclusionsPopulated = copy.m_exclusionsPopulated;
   
   return *this;
}

//******************************************************************************

bool GFSExclusions::haveExclusions() const {
   return m_exclusionsPopulated;
}

//******************************************************************************

bool GFSExclusions::retrieveExclusions(const std::string& dirName,
                                       const SectionedConfigDataSource& dataSource) {

   std::string dirExclusionKey = SEC_EXCLUSIONS;
   dirExclusionKey += dirName;
   m_exclusionsPopulated = false;
   
   if (dataSource.hasSection(dirExclusionKey)) {
      KeyValuePairs kvpDirExclusions;
      if (dataSource.readSection(dirExclusionKey, kvpDirExclusions)) {
         parseExclusionList(kvpDirExclusions,
                            KEY_DIR_EXCLUSION_NAMES,
                            m_dirExclusionNames);

         parseExclusionList(kvpDirExclusions,
                            KEY_DIR_EXCLUSION_PREFIXES,
                            m_dirExclusionPrefixes);
         
         parseExclusionList(kvpDirExclusions,
                            KEY_FILE_EXCLUSION_NAMES,
                            m_fileExclusionNames);

         parseExclusionList(kvpDirExclusions,
                            KEY_FILE_EXCLUSION_SUFFIXES,
                            m_fileExclusionSuffixes);
                            
         m_exclusionsPopulated = true;
      }
   }
   
   return m_exclusionsPopulated;           
}

//******************************************************************************

const std::vector<std::string>& GFSExclusions::getDirExclusionNames() const {
   return m_dirExclusionNames;
}

//******************************************************************************

const std::vector<std::string>& GFSExclusions::getDirExclusionPrefixes() const {
   return m_dirExclusionPrefixes;
}

//******************************************************************************

const std::vector<std::string>& GFSExclusions::getFileExclusionNames() const {
   return m_fileExclusionNames;
}

//******************************************************************************

const std::vector<std::string>& GFSExclusions::getFileExclusionSuffixes() const {
   return m_fileExclusionSuffixes;
}

//******************************************************************************

bool GFSExclusions::parseExclusionList(const KeyValuePairs& kvpExclusions,
                                       const std::string& key,
                                       std::vector<std::string>& listValues) {
   bool foundValues = false;
   
   if (kvpExclusions.hasKey(key)) {
      const std::string& listExclusions =
         kvpExclusions.getValue(key);
      if (!listExclusions.empty()) {
         StringTokenizer st(listExclusions, ",");
         const int numTokens = st.countTokens();
         if (numTokens > 0) {
            for (int i = 0; i < numTokens; ++i) {
               const std::string& exclusion = st.nextToken();
               Logger::debug(std::string("adding exclusion: '") +
                             exclusion + std::string("'"));
               listValues.push_back(exclusion);
            }
            foundValues = true;
         }
      }
   }
   
   return foundValues;                           
}

//******************************************************************************

bool GFSExclusions::excludeDirectory(const std::string& dirName) const {
   bool excludeDir = false;
   
   if (!m_dirExclusionNames.empty()) {
      if (std::find(m_dirExclusionNames.begin(),
                    m_dirExclusionNames.end(),
                    dirName) !=
          m_dirExclusionNames.end()) {
         excludeDir = true;
      }
   }

   if (!excludeDir) {
      if (!m_dirExclusionPrefixes.empty()) {
         auto it = m_dirExclusionPrefixes.cbegin();
         const auto itEnd = m_dirExclusionPrefixes.cend();
      
         for (; it != itEnd; ++it) {
            const std::string& dirExclusionPrefix = *it;
            if (StrUtils::startsWith(dirName, dirExclusionPrefix)) {
               excludeDir = true;
               break;
            }
         }
      }
   }
   
   return excludeDir;
}

//******************************************************************************

bool GFSExclusions::excludeFile(const std::string& fileName) const {
   bool excludeFile = false;
   
   if (!m_fileExclusionNames.empty()) {
      if (std::find(m_fileExclusionNames.begin(),
                    m_fileExclusionNames.end(),
                    fileName) !=
          m_fileExclusionNames.end()) {
         excludeFile = true;
      }
   }
   
   if (!excludeFile) {
      if (!m_fileExclusionSuffixes.empty()) {
         auto it = m_fileExclusionSuffixes.cbegin();
         const auto itEnd = m_fileExclusionSuffixes.cend();
      
         for (; it != itEnd; ++it) {
            const std::string& fileExclusionSuffix = *it;
            if (StrUtils::endsWith(fileName, fileExclusionSuffix)) {
               excludeFile = true;
               break;
            }
         }
      }
   }   
   
   return excludeFile;
}

//******************************************************************************

