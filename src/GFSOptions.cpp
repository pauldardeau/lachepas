// Copyright Paul Dardeau, 2016
// GFSOptions.cpp

#include "GFSOptions.h"

using namespace std;
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

void GFSOptions::setDirectory(const string& directory) {
   m_directory = directory;
}

//******************************************************************************

const string& GFSOptions::getDirectory() const {
   return m_directory;
}

//******************************************************************************

void GFSOptions::setTargetDirectory(const string& targetDirectory) {
   m_targetDirectory = targetDirectory;
}

//******************************************************************************

const string& GFSOptions::getTargetDirectory() const {
   return m_targetDirectory;
}

//******************************************************************************

void GFSOptions::setFile(const string& file) {
   m_file = file;
}

//******************************************************************************

const string& GFSOptions::getFile() const {
   return m_file;
}

//******************************************************************************

void GFSOptions::setEncryptionKey(const string& encryptionKey) {
   m_encryptionKey = encryptionKey;
}

//******************************************************************************

const string& GFSOptions::getEncryptionKey() const {
   return m_encryptionKey;
}

//******************************************************************************

void GFSOptions::setEncryptionKeyFile(const string& encryptionKeyFile) {
   m_encryptionKeyFile = encryptionKeyFile;
}

//******************************************************************************

const string& GFSOptions::getEncryptionKeyFile() const {
   return m_encryptionKeyFile;
}

//******************************************************************************

void GFSOptions::setEncryptionIV(const string& encryptionIV) {
   m_encryptionIV = encryptionIV;
}

//******************************************************************************

const string& GFSOptions::getEncryptionIV() const {
   return m_encryptionIV;
}

//******************************************************************************

void GFSOptions::setConfigFile(const string& configFile) {
   m_configFile = configFile;
}

//******************************************************************************

const string& GFSOptions::getConfigFile() const {
   return m_configFile;
}

//******************************************************************************

void GFSOptions::setNode(const string& node) {
   m_node = node;
}

//******************************************************************************

const string& GFSOptions::getNode() const {
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

