// Copyright Paul Dardeau, 2016
// GFSExclusions.cpp

#include <algorithm>

#include "GFSExclusions.h"
#include "Logger.h"
#include "StringTokenizer.h"
#include "StrUtils.h"

using namespace std;

static const string SEC_EXCLUSIONS              = "Exclusions:";
static const string KEY_DIR_EXCLUSION_NAMES     = "dir_exclusion_names";
static const string KEY_DIR_EXCLUSION_PREFIXES  = "dir_exclusion_prefixes";
static const string KEY_FILE_EXCLUSION_NAMES    = "file_exclusion_names";
static const string KEY_FILE_EXCLUSION_SUFFIXES = "file_exclusion_suffixes";

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

bool GFSExclusions::retrieveExclusions(const string& dirName,
                                       const SectionedConfigDataSource& dataSource) {

   string dirExclusionKey = SEC_EXCLUSIONS;
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

const vector<string>& GFSExclusions::getDirExclusionNames() const {
   return m_dirExclusionNames;
}

//******************************************************************************

const vector<string>& GFSExclusions::getDirExclusionPrefixes() const {
   return m_dirExclusionPrefixes;
}

//******************************************************************************

const vector<string>& GFSExclusions::getFileExclusionNames() const {
   return m_fileExclusionNames;
}

//******************************************************************************

const vector<string>& GFSExclusions::getFileExclusionSuffixes() const {
   return m_fileExclusionSuffixes;
}

//******************************************************************************

bool GFSExclusions::parseExclusionList(const KeyValuePairs& kvpExclusions,
                                       const string& key,
                                       vector<string>& listValues) {
   bool foundValues = false;

   if (kvpExclusions.hasKey(key)) {
      const string& listExclusions =
         kvpExclusions.getValue(key);
      if (!listExclusions.empty()) {
         StringTokenizer st(listExclusions, ",");
         const int numTokens = st.countTokens();
         if (numTokens > 0) {
            for (int i = 0; i < numTokens; ++i) {
               const string& exclusion = st.nextToken();
               Logger::debug(string("adding exclusion: '") +
                             exclusion + string("'"));
               listValues.push_back(exclusion);
            }
            foundValues = true;
         }
      }
   }

   return foundValues;
}

//******************************************************************************

bool GFSExclusions::excludeDirectory(const string& dirName) const {
   bool excludeDir = false;

   if (!m_dirExclusionNames.empty()) {
      if (find(m_dirExclusionNames.begin(),
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
            const string& dirExclusionPrefix = *it;
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

bool GFSExclusions::excludeFile(const string& fileName) const {
   bool excludeFile = false;

   if (!m_fileExclusionNames.empty()) {
      if (find(m_fileExclusionNames.begin(),
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
            const string& fileExclusionSuffix = *it;
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


