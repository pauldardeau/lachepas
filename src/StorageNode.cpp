// Copyright Paul Dardeau, 2016
// StorageNode.cpp

#include "StorageNode.h"

using namespace std;
using namespace lachepas;

//******************************************************************************

StorageNode::StorageNode() :
   m_storageNodeId(-1),
   m_active(true),
   m_compress(false),
   m_encrypt(false) {
}

//******************************************************************************

StorageNode::StorageNode(const StorageNode& copy) :
   m_serviceInfo(copy.m_serviceInfo),
   m_pingTime(copy.m_pingTime),
   m_copyTime(copy.m_copyTime),
   m_nodeName(copy.m_nodeName),
   m_storageNodeId(copy.m_storageNodeId),
   m_active(copy.m_active),
   m_compress(copy.m_compress),
   m_encrypt(copy.m_encrypt) {
}

//******************************************************************************

StorageNode& StorageNode::operator=(const StorageNode& copy) {
   if (this == &copy) {
      return *this;
   }
   
   m_serviceInfo = copy.m_serviceInfo;
   m_pingTime = copy.m_pingTime;
   m_copyTime = copy.m_copyTime;
   m_nodeName = copy.m_nodeName;
   m_storageNodeId = copy.m_storageNodeId;
   m_compress = copy.m_compress;
   m_encrypt = copy.m_encrypt;
   m_active = copy.m_active;
   
   return *this;
}

//******************************************************************************

void StorageNode::setServiceInfo(const chaudiere::ServiceInfo& serviceInfo) {
   m_serviceInfo = serviceInfo;
}

//******************************************************************************

const chaudiere::ServiceInfo& StorageNode::getServiceInfo() const {
   return m_serviceInfo;
}

//******************************************************************************

void StorageNode::setPingTime(const chaudiere::DateTime& pingTime) {
   m_pingTime = pingTime;
}

//******************************************************************************

const chaudiere::DateTime& StorageNode::getPingTime() const {
   return m_pingTime;
}

//******************************************************************************

void StorageNode::setCopyTime(const chaudiere::DateTime& copyTime) {
   m_copyTime = copyTime;
}

//******************************************************************************

const chaudiere::DateTime& StorageNode::getCopyTime() const {
   return m_copyTime;
}

//******************************************************************************

void StorageNode::setNodeName(const string& nodeName) {
   m_nodeName = nodeName;
}

//******************************************************************************

const string& StorageNode::getNodeName() const {
   return m_nodeName;
}

//******************************************************************************

void StorageNode::setStorageNodeId(int storageNodeId) {
   m_storageNodeId = storageNodeId;
}

//******************************************************************************

int StorageNode::getStorageNodeId() const {
   return m_storageNodeId;
}

//******************************************************************************

void StorageNode::setCompress(bool compress) {
   m_compress = compress;
}

//******************************************************************************

bool StorageNode::getCompress() const {
   return m_compress;
}

//******************************************************************************

void StorageNode::setEncrypt(bool encrypt) {
   m_encrypt = encrypt;
}

//******************************************************************************

bool StorageNode::getEncrypt() const {
   return m_encrypt;
}

//******************************************************************************

void StorageNode::setActive(bool active) {
   m_active = active;
}

//******************************************************************************

bool StorageNode::getActive() const {
   return m_active;
}

//******************************************************************************

