// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_GFSOPTIONS_H
#define LACHEPAS_GFSOPTIONS_H

#include <string>

namespace lachepas {

/**
 *
 */
class GFSOptions {

private:
   std::string m_directory;
   std::string m_file;
   std::string m_targetDirectory;
   std::string m_encryptionKey;
   std::string m_encryptionKeyFile;
   std::string m_encryptionIV;
   std::string m_configFile;
   std::string m_node;
   int m_copyCount;
   bool m_debugMode;
   bool m_useEncryption;
   bool m_useCompression;
   bool m_recurse;
      
public:
   /**
    * Default constructor
    */
   GFSOptions();
      
   /**
    * Copy constructor
    * @param copy source of copy
    */
   GFSOptions(const GFSOptions& copy);
      
   /**
    * Copy operator
    * @param copy source of the copy
    * @return reference to the target of the copy
    */
   GFSOptions& operator=(const GFSOptions& copy);
      
   /**
    *
    * @return
    */
   bool validateOptions() const;
      
   /**
    *
    * @param directory
    */
   void setDirectory(const std::string& directory);
      
   /**
    *
    * @return
    */
   const std::string& getDirectory() const;

   /**
    *
    * @param targetDirectory
   */
   void setTargetDirectory(const std::string& targetDirectory);
     
   /**
    *
    * @return
    */
   const std::string& getTargetDirectory() const;

   /**
    *
    * @param file
    */
   void setFile(const std::string& file);
     
   /**
    *
    * @return
    */
   const std::string& getFile() const;

   /**
    *
    * @param encryptionKey
    */
   void setEncryptionKey(const std::string& encryptionKey);
      
   /**
    *
    * @return
    */
   const std::string& getEncryptionKey() const;

   /**
    *
    * @param encryptionKeyFile
    */
   void setEncryptionKeyFile(const std::string& encryptionKeyFile);
      
   /**
    *
    * @return
    */
   const std::string& getEncryptionKeyFile() const;

   /**
    *
    * @param encryptionIV
    */
   void setEncryptionIV(const std::string& encryptionIV);
      
   /**
    *
    * @return
    */
   const std::string& getEncryptionIV() const;

   /**
    *
    * @param configFile
    */
   void setConfigFile(const std::string& configFile);
      
   /**
    *
    * @return
    */
   const std::string& getConfigFile() const;

   /**
    *
    * @param node
    */
   void setNode(const std::string& node);
      
   /**
    *
    * @return
    */
   const std::string& getNode() const;

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
    * @param debugMode
    */
   void setDebugMode(bool debugMode);
      
   /**
    *
    * @return
    */
   bool getDebugMode() const;

   /**
    *
    * @param useEncryption
    */
   void setUseEncryption(bool useEncryption);
      
   /**
    *
    * @return
    */
   bool getUseEncryption() const;

   /**
    *
    * @param useCompression
    */
   void setUseCompression(bool useCompression);
      
   /**
    *
    * @return
    */
   bool getUseCompression() const;

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
      
};

}

#endif

