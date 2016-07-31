// Copyright Paul Dardeau, 2016
#ifndef LACHEPAS_STORAGENODE_H
#define LACHEPAS_STORAGENODE_H

#include <string>

#include "DateTime.h"
#include "ServiceInfo.h"

namespace lachepas {

/**
 *
 */
class StorageNode {

private:
   chaudiere::ServiceInfo m_serviceInfo;
   chaudiere::DateTime m_pingTime;
   chaudiere::DateTime m_copyTime;
   std::string m_nodeName;
   int m_storageNodeId;
   bool m_active;
   bool m_compress;
   bool m_encrypt;
      
public:
   /**
    * Default constructor
    */
   StorageNode();
      
   /**
    * Copy constructor
    * @param copy the source of the copy
    */
   StorageNode(const StorageNode& copy);
      
   /**
    * Copy operator
    * @param copy the source of the copy
    * @return the target of the copy
    */
   StorageNode& operator=(const StorageNode& copy);
      
   /**
    *
    * @param serviceInfo
    * @see chaudiere::ServiceInfo()
    */
   void setServiceInfo(const chaudiere::ServiceInfo& serviceInfo);
      
   /**
    *
    * @return
    * @see chaudiere::ServiceInfo()
    */
   const chaudiere::ServiceInfo& getServiceInfo() const;
      
   /**
    *
    * @param pingTime
    * @see chaudiere::DateTime()
    */
   void setPingTime(const chaudiere::DateTime& pingTime);
      
   /**
    *
    * @return
    * @see chaudiere::DateTime()
    */
   const chaudiere::DateTime& getPingTime() const;

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
   const chaudiere::DateTime& getCopyTime() const;
      
   /**
    *
    * @param nodeName
    */
   void setNodeName(const std::string& nodeName);
      
   /**
    *
    * @return
    */
   const std::string& getNodeName() const;
      
   /**
    *
    * @param storageNodeId
    */
   void setStorageNodeId(int storageNodeId);
      
   /**
    *
    * @return
    */
   int getStorageNodeId() const;
      
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

