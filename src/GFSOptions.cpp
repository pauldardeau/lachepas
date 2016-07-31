// Copyright Paul Dardeau, 2016
// GFSOptions.cpp

#include "GFSOptions.h"

using namespace lachepas;

//******************************************************************************

GFSOptions::GFSOptions() :
   m_copyCount(1),
   m_debugMode(false),
   m_useEncryption(false),
   m_useCompression(false),
   m_recurse(false) {
}

//******************************************************************************

GFSOptions::GFSOptions(const GFSOptions& copy) :
   m_directory(copy.m_directory),
   m_file(copy.m_file),
   m_targetDirectory(copy.m_targetDirectory),
   m_encryptionKey(copy.m_encryptionKey),
   m_encryptionKeyFile(copy.m_encryptionKeyFile),
   m_encryptionIV(copy.m_encryptionIV),
   m_configFile(copy.m_configFile),
   m_node(copy.m_node),
   m_copyCount(copy.m_copyCount),
   m_debugMode(copy.m_debugMode),
   m_useEncryption(copy.m_useEncryption),
   m_useCompression(copy.m_useCompression),
   m_recurse(copy.m_recurse) {
}

//******************************************************************************

GFSOptions& GFSOptions::operator=(const GFSOptions& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_directory = copy.m_directory;
   m_file = copy.m_file;
   m_targetDirectory = copy.m_targetDirectory;
   m_encryptionKey = copy.m_encryptionKey;
   m_encryptionKeyFile = copy.m_encryptionKeyFile;
   m_encryptionIV = copy.m_encryptionIV;
   m_configFile = copy.m_configFile;
   m_node = copy.m_node;
   m_copyCount = copy.m_copyCount;
   m_debugMode = copy.m_debugMode;
   m_useEncryption = copy.m_useEncryption;
   m_useCompression = copy.m_useCompression;
   m_recurse = copy.m_recurse;

   return *this;
}

//******************************************************************************

bool GFSOptions::validateOptions() const {
   return true;
}

//******************************************************************************

void GFSOptions::setDirectory(const std::string& directory) {
   m_directory = directory;
}

//******************************************************************************

const std::string& GFSOptions::getDirectory() const {
   return m_directory;
}

//******************************************************************************

void GFSOptions::setTargetDirectory(const std::string& targetDirectory) {
   m_targetDirectory = targetDirectory;
}

//******************************************************************************

const std::string& GFSOptions::getTargetDirectory() const {
   return m_targetDirectory;
}

//******************************************************************************

void GFSOptions::setFile(const std::string& file) {
   m_file = file;
}

//******************************************************************************

const std::string& GFSOptions::getFile() const {
   return m_file;
}

//******************************************************************************

void GFSOptions::setEncryptionKey(const std::string& encryptionKey) {
   m_encryptionKey = encryptionKey;
}

//******************************************************************************

const std::string& GFSOptions::getEncryptionKey() const {
   return m_encryptionKey;
}

//******************************************************************************

void GFSOptions::setEncryptionKeyFile(const std::string& encryptionKeyFile) {
   m_encryptionKeyFile = encryptionKeyFile;
}

//******************************************************************************

const std::string& GFSOptions::getEncryptionKeyFile() const {
   return m_encryptionKeyFile;
}

//******************************************************************************

void GFSOptions::setEncryptionIV(const std::string& encryptionIV) {
   m_encryptionIV = encryptionIV;
}

//******************************************************************************

const std::string& GFSOptions::getEncryptionIV() const {
   return m_encryptionIV;
}

//******************************************************************************

void GFSOptions::setConfigFile(const std::string& configFile) {
   m_configFile = configFile;
}

//******************************************************************************

const std::string& GFSOptions::getConfigFile() const {
   return m_configFile;
}

//******************************************************************************

void GFSOptions::setNode(const std::string& node) {
   m_node = node;
}

//******************************************************************************

const std::string& GFSOptions::getNode() const {
   return m_node;
}

//******************************************************************************

void GFSOptions::setCopyCount(int copyCount) {
   m_copyCount = copyCount;
}

//******************************************************************************

int GFSOptions::getCopyCount() const {
   return m_copyCount;
}

//******************************************************************************

void GFSOptions::setDebugMode(bool debugMode) {
   m_debugMode = debugMode;
}

//******************************************************************************

bool GFSOptions::getDebugMode() const {
   return m_debugMode;
}

//******************************************************************************

void GFSOptions::setUseEncryption(bool useEncryption) {
   m_useEncryption = useEncryption;
}

//******************************************************************************

bool GFSOptions::getUseEncryption() const {
   return m_useEncryption;
}

//******************************************************************************

void GFSOptions::setUseCompression(bool useCompression) {
   m_useCompression = useCompression;
}

//******************************************************************************

bool GFSOptions::getUseCompression() const {
   return m_useCompression;
}

//******************************************************************************

void GFSOptions::setRecurse(bool recurse) {
   m_recurse = recurse;
}

//******************************************************************************

bool GFSOptions::getRecurse() const {
   return m_recurse;
}

//******************************************************************************

