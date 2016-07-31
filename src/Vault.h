// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_VAULT_H
#define LACHEPAS_VAULT_H


namespace lachepas {

/**
 *
 */
class Vault {
      
public:
   /**
    * Default constructor
    */
   Vault();
      
   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   Vault(const Vault& copy);
      
   /**
    * Copy operator
    * @param copy the source of the copy
    * @return the target of the copy
    */
   Vault& operator=(const Vault& copy);
      
   /**
    * Sets the vault identifier
    * @param vaultId the identifier for the vault
    */
   void setVaultId(int vaultId);
      
   /**
    * Retrieves the vault identifier
    * @return the vault identifier
    */
   int getVaultId() const;
      
   /**
    * Sets the storage node identifier
    * @param storageNodeId the identifier for the storage node
    */
   void setStorageNodeId(int storageNodeId);
      
   /**
    * Retrieves the storage node identifier
    * @return the storage node identifier
    */
   int getStorageNodeId() const;

   /**
    * Sets the local directory identifier
    * @param localDirectoryId the identifier for the local directory
    */
   void setLocalDirectoryId(int localDirectoryId);
      
   /**
    * Retrieves the local directory identifier
    * @return the local directory identifier
    */
   int getLocalDirectoryId() const;
      
   /**
    * Sets the option for compression
    * @param compress new option value
    */
   void setCompress(bool compress);
      
   /**
    * Retrieves the option for compression
    * @return option value for compression
    */
   bool getCompress() const;

   /**
    * Sets the option for encryption
    * @param encrypt new option value
    */
   void setEncrypt(bool encrypt);
      
   /**
    * Retrieves the option for encryption
    * @return option value for encryption
    */
   bool getEncrypt() const;
     
private:
   int m_vaultId;
   int m_storageNodeId;
   int m_localDirectoryId;
   bool m_compress;
   bool m_encrypt;

};

}

#endif

