// Copyright Paul Dardeau, 2016
// DataAccess.cpp

#include <stddef.h>

#include "AutoPointer.h"
#include "DataAccess.h"
#include "Logger.h"
#include "DBBool.h"
#include "DBDate.h"
#include "DBInt.h"
#include "DBNull.h"
#include "DBStatementArgs.h"
#include "DBString.h"
#include "SQLiteDatabase.h"

using namespace chapeau;

static const std::string MSG_NO_DB_CONNECTION = "no database connection";

//******************************************************************************

//TODO: this is SQLite specific!!
static const std::string SQL_QUERY_HAVE_TABLES =
   "SELECT name "
   "FROM sqlite_master "
   "WHERE type='table' "
   "AND name='local_directory'";

//******************************************************************************

// A “local directory” means a directory on your PC that will be replicated to
// one (or more) storage nodes. For example, if want to replicate my
// /Users/paul/Documents, there would be a single record in this table and
// dir_path would have the value “/Users/paul/Documents”.
// local_directory_id - auto increment integer identifier for the row in the
//                      database (populated automatically by SQLite on insert)
// dir_path - string value for the directory path
// active - 0/1 (boolean) to indicate whether the directory should be used (or not). normally this is true (1)
// recurse - 0/1 (boolean) to indicate whether subdirectories should be traversed (or not)
// compress - 0/1 (boolean) to indicate whether to use compression (currently ignored)
// encrypt - 0/1 (boolean) to indicate whether to encrypt the data (or not)
// copy_count - integer value to specify how many copies (replicas) you want
//              stored. normally, this is 1.
static const std::string SQL_CREATE_LOCAL_DIRECTORY =
   "CREATE TABLE local_directory ("
      "local_directory_id INTEGER PRIMARY KEY, "
      "dir_path TEXT NOT NULL, "
      "active INTEGER NOT NULL, "
      "recurse INTEGER NOT NULL, "
      "compress INTEGER NOT NULL, "
      "encrypt INTEGER NOT NULL, "
      "copy_count INTEGER NOT NULL"
   ")";

// Every file that is found under a “local directory” will result in a record in this table
// If I have 3 files in my local directory (“moe.txt”, “larry.txt”, “curly.txt”), then I will get
// 3 records populated in this table.
// local_file_id - auto increment integer identifier for the row in the database
//                 (populated automatically by SQLite on insert)
// local_directory_id - references the directory identifier for the record in the
//                      local_directory table (used for joins with the local_directory table)
// file_path - the path for the file relative to the dir_path specified in the local_directory table
// create_time - unix timestamp for the time the file was created (read from the local filesystem)
// modify_time - unix timestamp for the time the file was modified (read from the local filesystem)
// scan_time - unix timestamp for the time when the file was last scanned (this
//             helps identify whether the file was changed since it was last scanned)
static const std::string SQL_CREATE_LOCAL_FILE =
   "CREATE TABLE local_file ("
      "local_file_id INTEGER PRIMARY KEY, "
      "local_directory_id INTEGER REFERENCES local_directory(local_directory_id), "
      "file_path TEXT NOT NULL, "
      "create_time REAL NOT NULL, "
      "modify_time REAL NOT NULL, "
      "scan_time REAL"
   ")";

// Every “storage node” (remote computer where data is copied to) will have a record in this table
// storage_node_id - auto increment integer identifier for the row in the database (populated automatically by SQLite on insert)
// node_name - a textual name for the storage node (the value must match up with what’s stored in .INI file)
// active - 0/1 (boolean) to indicate whether the storage node should be used (or not). normally this is true (1)
// ping_time - unix timestamp to indicate the last time we communicated with the storage node (not currently populated)
// copy_time - unix timestamp to indicate the last time we copied a file block to the storage node (not currently populated)
static const std::string SQL_CREATE_STORAGE_NODE =
   "CREATE TABLE storage_node ("
      "storage_node_id INTEGER PRIMARY KEY, "
      "node_name TEXT NOT NULL, "
      "active INTEGER NOT NULL, "
      "ping_time REAL, "
      "copy_time REAL"
   ")";

// A “vault” is an association/grouping of a local_directory to a storage_node
// vault-id - auto increment integer identifier for the row in the database (populated automatically by SQLite on insert)
// storage_node_id - the row identifier for the storage node in the storage_node table (use for joins with storage_node table)
// local_directory_id - the row identifier for the local directory in the local_directory table (use for joins with local_directory table)
// compress - 0/1 (boolean) to indicate whether the vault files are compressed (currently not used)
// encrypt - 0/1 (boolean) to indicate whether the vault files are encrypted
static const std::string SQL_CREATE_VAULT =
   "CREATE TABLE vault ("
      "vault_id INTEGER PRIMARY KEY, "
      "storage_node_id INTEGER REFERENCES storage_node(storage_node_id), "
      "local_directory_id INTEGER REFERENCES local_directory(local_directory_id), "
      "compress INTEGER NOT NULL, "
      "encrypt INTEGER NOT NULL"
   ")";

// A “vault file” is the occurrence of a local file stored on a storage node
// vault_file_id - auto increment integer identifier for the row in the database (populated automatically by SQLite on insert)
// local_file_id - the row identifier for the local file in the local_file table (use for joins with local_file table)
// vault_id - the row identifier for the vault in the vault table (use for joins with vault table)
// create_time - unix timestamp for when file was created (read from local filesystem)
// modify_time - unix timestamp for when file was modified (read from local filesystem)
// origin_filesize - size of the file (in bytes) for when the file was last stored/updated on the storage node
// block_count - how many blocks make up this file
// user_permissions - unix permissions for user (rwx)
// group_permissions - unix permissions for group (rwx)
// other_permissions - unix permissions for others (rwx)
static const std::string SQL_CREATE_VAULT_FILE =
   "CREATE TABLE vault_file ("
      "vault_file_id INTEGER PRIMARY KEY, "
      "local_file_id INTEGER REFERENCES local_file(local_file_id), "
      "vault_id INTEGER REFERENCES vault(vault_id), "
      "create_time REAL NOT NULL, "
      "modify_time REAL NOT NULL, "
      "origin_filesize INTEGER NOT NULL, "
      "block_count INTEGER NOT NULL, "
      "user_permissions TEXT NOT NULL, "
      "group_permissions TEXT NOT NULL, "
      "other_permissions TEXT NOT NULL"
   ")";

// A “file block” is a portion of a larger sized file, or the whole file if the file size <= block size
// vault_file_block_id - auto increment integer identifier for the row in the database (populated automatically by SQLite on insert)
// vault_file_id - the row identifier for the vault file (used for joins with the vault_file table)
// create_time - unix timestamp of when the block was created
// modify_time - unix timestamp of when the block was last updated
// stored_time - unix timestamp for when the block was sent to the storage node
// origin_filesize - block size used
// stored_filesize - how many bytes did we send over to  the storage node (due to encryption and base-64 encoding it is often a little larger)
// block_sequence_number - integer to indicate the position of this block within the local file
// padchar_count - the number of padding characters that were added as part of encryption (0 otherwise)
// unique_identifier - the unique identifier of the file block (*** REALLY IMPORTANT ***)
// node_directory - the name of the directory where the block is stored on the storage node
// node_file - the name of the file where the block is stored on the storage node
static const std::string SQL_CREATE_FILE_BLOCK =
   "CREATE TABLE vault_file_block ("
      "vault_file_block_id INTEGER PRIMARY KEY, "
      "vault_file_id INTEGER REFERENCES vault_file(vault_file_id), "
      "create_time REAL NOT NULL, "
      "modify_time REAL NOT NULL, "
      "stored_time REAL NOT NULL, "
      "origin_filesize INTEGER NOT NULL, "
      "stored_filesize INTEGER NOT NULL, "
      "block_sequence_number INTEGER NOT NULL, "
      "padchar_count INTEGER NOT NULL, "
      "unique_identifier TEXT NOT NULL, "
      "node_directory TEXT NOT NULL, "
      "node_file TEXT NOT NULL"
   ")";

//******************************************************************************

static const std::string SQL_INSERT_LOCAL_DIRECTORY =
   "INSERT INTO local_directory "
   "(dir_path,active,recurse,compress,encrypt,copy_count) "
   "VALUES (?,?,?,?,?,?)";

static const std::string SQL_INSERT_LOCAL_FILE =
   "INSERT INTO local_file "
   "(local_directory_id,file_path,create_time,modify_time,scan_time) "
   "VALUES (?,?,?,?,?)";

static const std::string SQL_INSERT_STORAGE_NODE =
   "INSERT INTO storage_node "
   "(node_name,active) "
   "VALUES (?,?);";

static const std::string SQL_INSERT_VAULT =
   "INSERT INTO vault "
   "(storage_node_id,local_directory_id,compress,encrypt) "
   "VALUES (?,?,?,?)";

static const std::string SQL_INSERT_VAULT_FILE =
   "INSERT INTO vault_file "
   "(local_file_id,vault_id,create_time,modify_time,origin_filesize,block_count,"
      "user_permissions, group_permissions, other_permissions) "
   "VALUES (?,?,?,?,?,?,?,?,?)";

static const std::string SQL_INSERT_FILE_BLOCK =
   "INSERT INTO vault_file_block "
   "(vault_file_id,create_time,modify_time,stored_time,origin_filesize,stored_filesize,"
      "block_sequence_number,padchar_count,unique_identifier,node_directory,node_file) "
   "VALUES (?,?,?,?,?,?,?,?,?,?,?)";

//******************************************************************************

static const std::string SQL_SELECT_ACTIVE_LOCAL_DIRECTORY =
   "SELECT "
      "local_directory_id, dir_path, active, recurse, compress, encrypt, copy_count "
   "FROM local_directory "
   "WHERE active = 1";

static const std::string SQL_SELECT_INACTIVE_LOCAL_DIRECTORY =
   "SELECT "
      "local_directory_id, dir_path, active, recurse, compress, encrypt, copy_count "
   "FROM local_directory "
   "WHERE active = 0";

static const std::string SQL_SELECT_LOCAL_FILE =
   "SELECT "
      "local_file_id, create_time, modify_time, scan_time "
   "FROM local_file "
   "WHERE local_directory_id = ? "
   "AND file_path = ?";

static const std::string SQL_SELECT_LOCAL_FILE_LIST =
   "SELECT "
      "local_file_id, file_path, create_time, modify_time, scan_time "
   "FROM local_file "
   "WHERE local_directory_id = ?";

static const std::string SQL_SELECT_ACTIVE_STORAGE_NODE =
   "SELECT "
      "storage_node_id, node_name, ping_time, copy_time "
   "FROM storage_node "
   "WHERE active = 1";

static const std::string SQL_SELECT_INACTIVE_STORAGE_NODE =
   "SELECT "
      "storage_node_id, node_name, ping_time, copy_time "
   "FROM storage_node "
   "WHERE active = 0";

static const std::string SQL_SELECT_NODE_VAULT =
   "SELECT "
      "vault_id, compress, encrypt "
   "FROM vault "
   "WHERE storage_node_id = ? "
   "AND local_directory_id = ?";

static const std::string SQL_SELECT_VAULT_FILE =
   "SELECT "
      "vault_file_id, create_time, "
      "modify_time, origin_filesize, block_count, "
      "user_permissions, group_permissions, other_permissions "
   "FROM vault_file "
   "WHERE local_file_id = ? "
   "AND vault_id = ?";

static const std::string SQL_SELECT_FILE_BLOCK =
   "SELECT "
      "vault_file_block_id, create_time, modify_time, stored_time, "
      "origin_filesize, stored_filesize, block_sequence_number, "
      "padchar_count, unique_identifier, node_directory, node_file "
   "FROM vault_file_block "
   "WHERE vault_file_id = ? "
   "ORDER BY block_sequence_number";

//******************************************************************************

static const std::string SQL_DELETE_LOCAL_DIRECTORY =
   "DELETE "
   "FROM local_directory "
   "WHERE local_directory_id = ?";

static const std::string SQL_DELETE_LOCAL_FILE =
   "DELETE "
   "FROM local_file "
   "WHERE local_file_id = ?";

static const std::string SQL_DELETE_ACTIVE_STORAGE_NODE =
   "UPDATE storage_node "
   "SET active = 0 "
   "WHERE storage_node_id = ?";

static const std::string SQL_DELETE_NODE_VAULT =
   "DELETE "
   "FROM vault "
   "WHERE vault_id = ?";

static const std::string SQL_DELETE_VAULT_FILE =
   "DELETE "
   "FROM vault_file "
   "WHERE vault_file_id = ?";

static const std::string SQL_DELETE_FILE_BLOCK =
   "DELETE "
   "FROM vault_file_block "
   "WHERE vault_file_block_id = ?";

//******************************************************************************

static const std::string SQL_UPDATE_LOCAL_DIRECTORY =
   "UDPATE local_directory "
   "SET dir_path = ?, "
      "active = ?, "
      "recurse = ?, "
      "compress = ?, "
      "encrypt = ?, "
      "copy_count = ? "
   "WHERE local_directory_id = ?";

static const std::string SQL_UPDATE_LOCAL_FILE =
   "UPDATE local_file "
   "SET local_directory_id = ?, "
      "file_path = ?, "
      "create_time = ?, "
      "modify_time = ?, "
      "scan_time = ? "
   "WHERE local_file_id = ?";

static const std::string SQL_UPDATE_ACTIVE_STORAGE_NODE =
   "UPDATE storage_node "
   "SET node_name = ?, "
      "active = ?, "
      "ping_time = ?, "
      "copy_time = ? "
   "WHERE storage_node_id = ?";

static const std::string SQL_UPDATE_NODE_VAULT =
   "UPDATE vault "
   "SET storage_node_id = ?, "
      "local_directory_id = ?, "
      "compress = ?, "
      "encrypt = ? "
   "WHERE vault_id = ?";

static const std::string SQL_UPDATE_VAULT_FILE =
   "UPDATE vault_file "
   "SET local_file_id = ?, "
      "vault_id = ?, "
      "create_time = ?, "
      "modify_time = ?, "
      "origin_filesize = ?, "
      "block_count = ?, "
      "user_permissions = ?, "
      "group_permissions = ?, "
      "other_permissions = ? "
   "WHERE vault_file_id = ?";

static const std::string SQL_UPDATE_FILE_BLOCK =
   "UPDATE vault_file_block "
   "SET vault_file_id = ?, "
      "create_time = ?, "
      "modify_time = ?, "
      "stored_time = ?, "
      "origin_filesize = ?, "
      "stored_filesize = ?, "
      "block_sequence_number = ?, "
      "padchar_count = ?, "
      "unique_identifier = ?, "
      "node_directory = ?, "
      "node_file = ? "
   "WHERE vault_file_block_id = ?";

//******************************************************************************

using namespace lachepas;
//using namespace chapeau;
using namespace chaudiere;

//******************************************************************************

DataAccess::DataAccess(const std::string& filePath) :
                       m_dbConnection(nullptr),
                       m_dbFilePath(filePath),
                       m_debugPrint(true) {
}

//******************************************************************************

DataAccess::~DataAccess() {
   if (m_dbConnection != nullptr) {
      m_dbConnection->close();
   }
}

//******************************************************************************

bool DataAccess::open() {
   bool dbInitialized = false;
   
   m_dbConnection = new SQLiteDatabase(m_dbFilePath);
   if (m_dbConnection->open()) {
      //Logger::info("successfully opened database");
   
      if (!haveTables()) {
         if (createTables()) {
            Logger::info("db tables created");
            dbInitialized = true;
         } else {
            Logger::error("unable to create db tables");
         }
      } else {
         //Logger::info("already have db tables");
         dbInitialized = true;
      }
   } else {
      Logger::error("unable to open database");
   }
   
   return dbInitialized;
}

//******************************************************************************

bool DataAccess::createTable(const std::string& sql) {
   if (m_dbConnection != nullptr) {
      return m_dbConnection->executeUpdate(sql);
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
      return false;
   }   
}

//******************************************************************************

bool DataAccess::createTables() {
   if (m_dbConnection != nullptr) {
      if (m_debugPrint) {
         Logger::debug("creating tables");
      }
      
      int numTables = 0;
      
      if (createTable(SQL_CREATE_LOCAL_DIRECTORY)) {
         ++numTables;
      }
      
      if (createTable(SQL_CREATE_LOCAL_FILE)) {
         ++numTables;
      }
      
      if (createTable(SQL_CREATE_STORAGE_NODE)) {
         ++numTables;
      }

      if (createTable(SQL_CREATE_VAULT)) {
         ++numTables;
      }

      if (createTable(SQL_CREATE_VAULT_FILE)) {
         ++numTables;
      }

      if (createTable(SQL_CREATE_FILE_BLOCK)) {
         ++numTables;
      }

      if (numTables == 6) {
         return true;
      } else {
         return false;
      }
   } else {
      Logger::error("unable to create tables: no database connection");
      return false;
   }
}

//******************************************************************************

bool DataAccess::haveTables() {
   bool haveTablesInDB = false;
   if (m_dbConnection != nullptr) {
      AutoPointer<DBResultSet*> rs(
         m_dbConnection->executeQuery(SQL_QUERY_HAVE_TABLES));
      if (rs.haveObject()) {
         if (rs->next()) {
            haveTablesInDB = true;
         }
      }
   }
      
   return haveTablesInDB;
}

//******************************************************************************

bool DataAccess::commit() {
   if (m_dbConnection != nullptr) {
      return m_dbConnection->commit();
   }
   
   return false;
}

//******************************************************************************

bool DataAccess::rollback() {
   if (m_dbConnection != nullptr) {
      return m_dbConnection->rollback();
   }
   
   return false;
}

//******************************************************************************

bool DataAccess::insertStorageNode(StorageNode& storageNode) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const std::string& nodeName = storageNode.getNodeName();
      if (!nodeName.empty()) {
         DBStatementArgs args;
         args.add(new DBString(nodeName));
         args.add(new DBBool(storageNode.getActive()));
            
         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_INSERT_STORAGE_NODE, args);
         if (dbUpdateSuccess) {
            storageNode.setStorageNodeId(m_dbConnection->lastInsertRowId());
         }
      } else {
         Logger::error("unable to add storageNode, missing node name");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::insertLocalDirectory(LocalDirectory& localDirectory) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const std::string& directoryPath = localDirectory.getDirectoryPath();
      if (!directoryPath.empty()) {
         DBStatementArgs args;
         args.add(new DBString(directoryPath));
         args.add(new DBBool(localDirectory.getActive()));
         args.add(new DBBool(localDirectory.getRecurse()));
         args.add(new DBBool(localDirectory.getCompress()));
         args.add(new DBBool(localDirectory.getEncrypt()));
         args.add(new DBInt(localDirectory.getCopyCount()));
            
         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_INSERT_LOCAL_DIRECTORY, args);
         if (dbUpdateSuccess) {
            localDirectory.setLocalDirectoryId(m_dbConnection->lastInsertRowId());
         }
      } else {
         Logger::error("unable to add localDirectory, missing directory path");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::insertLocalFile(LocalFile& localFile) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const std::string& filePath = localFile.getFilePath();
      if (!filePath.empty()) {
         const int localDirectoryId = localFile.getLocalDirectoryId();
         if (localDirectoryId > -1) {
            DBStatementArgs args;
            args.add(new DBInt(localDirectoryId));
            args.add(new DBString(filePath));
            args.add(new DBDate(localFile.getCreateTime()));
            args.add(new DBDate(localFile.getModifyTime()));
            args.add(new DBDate(localFile.getScanTime()));
            
            dbUpdateSuccess =
               m_dbConnection->executeUpdate(SQL_INSERT_LOCAL_FILE, args);
            if (dbUpdateSuccess) {
               localFile.setLocalFileId(m_dbConnection->lastInsertRowId());
            }
         } else {
            Logger::error("unable to add localFile, invalid local directory id");
         }
      } else {
         Logger::error("unable to add localFile, missing file path");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::insertVault(Vault& vault) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int storageNodeId = vault.getStorageNodeId();
      const int localDirectoryId = vault.getLocalDirectoryId();
      if (storageNodeId > -1) {
         if (localDirectoryId > -1) {
            DBStatementArgs args;
            args.add(new DBInt(storageNodeId));
            args.add(new DBInt(localDirectoryId));
            args.add(new DBBool(vault.getCompress()));
            args.add(new DBBool(vault.getEncrypt()));
            
            dbUpdateSuccess = m_dbConnection->executeUpdate(SQL_INSERT_VAULT, args);
            if (dbUpdateSuccess) {
               vault.setVaultId(m_dbConnection->lastInsertRowId());
            }
         } else {
            Logger::error("unable to insert vault, invalid local directory id");
         }
      } else {
         Logger::error("unable to insert vault, invalid storage node id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::insertVaultFile(VaultFile& vaultFile) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int localFileId = vaultFile.getLocalFileId();
      const int vaultId = vaultFile.getVaultId();
         
      if (localFileId > -1) {
         if (vaultId > -1) {
            DBStatementArgs args;
            args.add(new DBInt(localFileId));
            args.add(new DBInt(vaultId));
            args.add(new DBDate(vaultFile.getCreateTime()));
            args.add(new DBDate(vaultFile.getModifyTime()));
               
            args.add(new DBInt(vaultFile.getOriginFileSize()));
            args.add(new DBInt(vaultFile.getBlockCount()));
            args.add(new DBString(vaultFile.getUserPermissions().getPermissionsString()));
            args.add(new DBString(vaultFile.getGroupPermissions().getPermissionsString()));
            args.add(new DBString(vaultFile.getOtherPermissions().getPermissionsString()));

               
            dbUpdateSuccess = m_dbConnection->executeUpdate(SQL_INSERT_VAULT_FILE, args);
            if (dbUpdateSuccess) {
               vaultFile.setVaultFileId(m_dbConnection->lastInsertRowId());
            }
         } else {
            Logger::error("unable to insert vault file, invalid vault id");
         }
      } else {
         Logger::error("unable to insert vault file, invalid local file id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::insertVaultFileBlock(VaultFileBlock& vaultFileBlock) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const std::string& uniqueIdentifier = vaultFileBlock.getUniqueIdentifier();
      const std::string& nodeDirectory = vaultFileBlock.getNodeDirectory();
      const std::string& nodeFile = vaultFileBlock.getNodeFile();
         
      if (!uniqueIdentifier.empty()) {
         if (!nodeDirectory.empty()) {
            if (!nodeFile.empty()) {
               DBStatementArgs args;
               args.add(new DBInt(vaultFileBlock.getVaultFileId()));
               args.add(new DBDate(vaultFileBlock.getCreateTime()));
               args.add(new DBDate(vaultFileBlock.getModifyTime()));
               args.add(new DBDate(vaultFileBlock.getStoredTime()));
               args.add(new DBInt(vaultFileBlock.getOriginFileSize()));
               args.add(new DBInt(vaultFileBlock.getStoredFileSize()));
               args.add(new DBInt(vaultFileBlock.getBlockSequenceNumber()));
               args.add(new DBInt(vaultFileBlock.getPadCharCount()));
               args.add(new DBString(uniqueIdentifier));
               args.add(new DBString(nodeDirectory));
               args.add(new DBString(nodeFile));

               dbUpdateSuccess = m_dbConnection->executeUpdate(SQL_INSERT_FILE_BLOCK, args);
               if (dbUpdateSuccess) {
                  vaultFileBlock.setVaultFileBlockId(m_dbConnection->lastInsertRowId());
               } else {
                  ::printf("error: insert vault file block failed\n");
                  ::printf("vaultFileId=%d\n", vaultFileBlock.getVaultFileId());
                  ::printf("createTime=%s\n", vaultFileBlock.getCreateTime().formattedString().c_str());
                  ::printf("modifyTime=%s\n", vaultFileBlock.getModifyTime().formattedString().c_str());
                  ::printf("storedTime=%s\n", vaultFileBlock.getStoredTime().formattedString().c_str());
                  ::printf("originFileSize=%d\n", vaultFileBlock.getOriginFileSize());
                  ::printf("storedFileSize=%d\n", vaultFileBlock.getStoredFileSize());
                  ::printf("sequenceNumber=%d\n", vaultFileBlock.getBlockSequenceNumber());
                  ::printf("padCharCount=%d\n", vaultFileBlock.getPadCharCount());
                  ::printf("uniqueIdentifier='%s'\n", uniqueIdentifier.c_str());
                  ::printf("nodeDirectory='%s'\n", nodeDirectory.c_str());
                  ::printf("nodeFile='%s'\n", nodeFile.c_str());
               }
            } else {
               Logger::error("unable to insert vault file block, missing node file");
            }
         } else {
            Logger::error("unable to insert vault file block, missing node directory");
         }
      } else {
         Logger::error("unable to insert vault file block, missing uniqueIdentifier");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::updateStorageNode(StorageNode& storageNode) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int storageNodeId = storageNode.getStorageNodeId();
      if (storageNodeId > -1) {
         const std::string& nodeName = storageNode.getNodeName();
         if (!nodeName.empty()) {
            DBStatementArgs args;
            args.add(new DBString(nodeName));
            args.add(new DBBool(storageNode.getActive()));
            args.add(new DBDate(storageNode.getPingTime()));
            args.add(new DBDate(storageNode.getCopyTime()));
            args.add(new DBInt(storageNode.getStorageNodeId()));

            dbUpdateSuccess =
               m_dbConnection->executeUpdate(SQL_UPDATE_ACTIVE_STORAGE_NODE, args);
         } else {
            Logger::error("unable to update storage node, node name missing");
         }
      } else {
         Logger::error("unable to update storage node, invalid storage node id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::updateLocalDirectory(LocalDirectory& localDirectory) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const std::string& directoryPath = localDirectory.getDirectoryPath();
      if (!directoryPath.empty()) {
         const int localDirectoryId = localDirectory.getLocalDirectoryId();
         if (localDirectoryId > -1) {
            const int copyCount = localDirectory.getCopyCount();
            const bool active = localDirectory.getActive();
            const bool recurse = localDirectory.getRecurse();
            const bool compress = localDirectory.getCompress();
            const bool encrypt = localDirectory.getEncrypt();

            DBStatementArgs args;
            args.add(new DBString(directoryPath));
            args.add(new DBBool(active));
            args.add(new DBBool(recurse));
            args.add(new DBBool(compress));
            args.add(new DBBool(encrypt));
            args.add(new DBInt(copyCount));
            args.add(new DBInt(localDirectoryId));
               
            dbUpdateSuccess =
               m_dbConnection->executeUpdate(SQL_UPDATE_LOCAL_DIRECTORY, args);
         } else {
            Logger::error("unable to update local directory, invalid local directory id");
         }
      } else {
         Logger::error("unable to update local directory, missing directory path");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::updateLocalFile(LocalFile& localFile) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const std::string& filePath = localFile.getFilePath();
      if (!filePath.empty()) {
         const int localDirectoryId = localFile.getLocalDirectoryId();
         if (localDirectoryId > -1) {
            const int localFileId = localFile.getLocalFileId();
            if (localFileId > -1) {
               DBStatementArgs args;
               args.add(new DBInt(localDirectoryId));
               args.add(new DBString(filePath));
               args.add(new DBDate(localFile.getCreateTime()));
               args.add(new DBDate(localFile.getModifyTime()));
               args.add(new DBDate(localFile.getScanTime()));
               args.add(new DBInt(localFileId));

               dbUpdateSuccess =
                  m_dbConnection->executeUpdate(SQL_UPDATE_LOCAL_FILE, args);
            } else {
               Logger::error("unable to update local file, invalid local file id");
            }
         } else {
            Logger::error("unable to update local file, invalid local directory id");
         }
      } else {
         Logger::error("unable to update local file, missing file path");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::updateVault(Vault& vault) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int vaultId = vault.getVaultId();
      const int storageNodeId = vault.getStorageNodeId();
      const int localDirectoryId = vault.getLocalDirectoryId();
         
      if (vaultId > -1) {
         if (storageNodeId > -1) {
            if (localDirectoryId > -1) {
               const bool compress = vault.getCompress();
               const bool encrypt = vault.getEncrypt();
               
               DBStatementArgs args;
               args.add(new DBInt(storageNodeId));
               args.add(new DBInt(localDirectoryId));
               args.add(new DBBool(compress));
               args.add(new DBBool(encrypt));
               args.add(new DBInt(vaultId));
               
               dbUpdateSuccess = m_dbConnection->executeUpdate(SQL_UPDATE_NODE_VAULT, args);
            } else {
               Logger::error("unable to update vault, invalid local directory id");
            }
         } else {
            Logger::error("unable to update vault, invalid storage node id");
         }
      } else {
         Logger::error("unable to update vault, invalid vault id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::updateVaultFile(VaultFile& vaultFile) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int vaultFileId = vaultFile.getVaultFileId();
      const int localFileId = vaultFile.getLocalFileId();
      const int vaultId = vaultFile.getVaultId();
         
      if (vaultFileId > -1) {
         if (localFileId > -1) {
            if (vaultId > -1) {
               const int originFileSize = vaultFile.getOriginFileSize();
               const int blockCount = vaultFile.getBlockCount();
               const std::string& userPermissions =
                  vaultFile.getUserPermissions().getPermissionsString();
               const std::string& groupPermissions =
                  vaultFile.getGroupPermissions().getPermissionsString();
               const std::string& otherPermissions =
                  vaultFile.getOtherPermissions().getPermissionsString();

               DBStatementArgs args;
               args.add(new DBInt(localFileId));
               args.add(new DBInt(vaultId));
               args.add(new DBNull("DBString"));
               args.add(new DBNull("DBString"));
               args.add(new DBInt(originFileSize));
               args.add(new DBInt(blockCount));
               args.add(new DBString(userPermissions));
               args.add(new DBString(groupPermissions));
               args.add(new DBString(otherPermissions));
               args.add(new DBInt(vaultFileId));
                  
               dbUpdateSuccess =
                  m_dbConnection->executeUpdate(SQL_UPDATE_VAULT_FILE, args);
            } else {
               Logger::error("unable to update vault file, invalid vault id");
            }
         } else {
            Logger::error("unable to update vault file, invalid local file id");
         }
      } else {
         Logger::error("unable to update vault file, invalid vault file id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::updateVaultFileBlock(VaultFileBlock& vaultFileBlock) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int vaultFileBlockId = vaultFileBlock.getVaultFileBlockId();
      const int vaultFileId = vaultFileBlock.getVaultFileId();
      const std::string& uniqueIdentifier = vaultFileBlock.getUniqueIdentifier();
      const std::string& nodeDirectory = vaultFileBlock.getNodeDirectory();
      const std::string& nodeFile = vaultFileBlock.getNodeFile();
         
      if (vaultFileBlockId > -1) {
         if (vaultFileId > -1) {
            if (!uniqueIdentifier.empty()) {
               if (!nodeDirectory.empty()) {
                  if (!nodeFile.empty()) {
                     const int originFileSize = vaultFileBlock.getOriginFileSize();
                     const int storedFileSize = vaultFileBlock.getStoredFileSize();
                     const int blockSequenceNumber = vaultFileBlock.getBlockSequenceNumber();
                     const int padCharCount = vaultFileBlock.getPadCharCount();

                     DBStatementArgs args;
                     args.add(new DBInt(vaultFileId));
                     args.add(new DBNull("DBString"));
                     args.add(new DBNull("DBString"));
                     args.add(new DBNull("DBString"));
                     args.add(new DBInt(originFileSize));
                     args.add(new DBInt(storedFileSize));
                     args.add(new DBInt(blockSequenceNumber));
                     args.add(new DBInt(padCharCount));
                     args.add(new DBString(uniqueIdentifier));
                     args.add(new DBString(nodeDirectory));
                     args.add(new DBString(nodeFile));
                     args.add(new DBInt(vaultFileBlockId));
                        
                     dbUpdateSuccess =
                        m_dbConnection->executeUpdate(SQL_UPDATE_FILE_BLOCK, args);
                  } else {
                     Logger::error("unable to update vault file block, missing node file");
                  }
               } else {
                  Logger::error("unable to update vault file block, missing node directory");
               }
            } else {
               Logger::error("unable to update vault file block, missing uniqueIdentifier");
            }
         } else {
            Logger::error("unable to update vault file block, invalid vault file id");
         }
      } else {
         Logger::error("unable to update vault file block, invalid vault file block id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::deleteActiveStorageNode(StorageNode& storageNode) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int storageNodeId = storageNode.getStorageNodeId();
      if (storageNodeId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(storageNodeId));
            
         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_DELETE_ACTIVE_STORAGE_NODE, args);
      } else {
         Logger::error("unable to delete storage node, invalid storage node id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::deleteLocalDirectory(LocalDirectory& localDirectory) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int localDirectoryId = localDirectory.getLocalDirectoryId();
      if (localDirectoryId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(localDirectoryId));

         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_DELETE_LOCAL_DIRECTORY, args);
         if (dbUpdateSuccess) {
            localDirectory.setLocalDirectoryId(-1);
         }
      } else {
         Logger::error("invalid localDirectoryId for delete");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::deleteLocalFile(LocalFile& localFile) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int localFileId = localFile.getLocalFileId();
      if (localFileId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(localFileId));

         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_DELETE_LOCAL_FILE, args);
         if (dbUpdateSuccess) {
            localFile.setLocalFileId(-1);
         }
      } else {
         Logger::error("invalid localFileId for delete");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::deleteVault(Vault& vault) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int vaultId = vault.getVaultId();
      if (vaultId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(vaultId));

         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_DELETE_NODE_VAULT, args);
         if (dbUpdateSuccess) {
            vault.setVaultId(-1);
         }
      } else {
         Logger::error("invalid vaultId for delete");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::deleteVaultFile(VaultFile& vaultFile) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int vaultFileId = vaultFile.getVaultFileId();
      if (vaultFileId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(vaultFileId));

         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_DELETE_VAULT_FILE, args);
         if (dbUpdateSuccess) {
            vaultFile.setVaultFileId(-1);
         }
      } else {
         Logger::error("invalid vaultFileId for delete");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::deleteVaultFileBlock(VaultFileBlock& vaultFileBlock) {
   bool dbUpdateSuccess = false;
   if (m_dbConnection != nullptr) {
      const int vaultFileBlockId = vaultFileBlock.getVaultFileBlockId();
      if (vaultFileBlockId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(vaultFileBlockId));
            
         dbUpdateSuccess =
            m_dbConnection->executeUpdate(SQL_DELETE_FILE_BLOCK, args);
         if (dbUpdateSuccess) {
            vaultFileBlock.setVaultFileBlockId(-1);
         }
      } else {
         Logger::error("invalid vaultFileBlockId for delete");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbUpdateSuccess;
}

//******************************************************************************

bool DataAccess::getStorageNodes(const std::string& query,
                                 std::vector<StorageNode>& listNodes) {
   bool dbAccessSuccess = false;
   
   if (m_dbConnection != nullptr) {
      //Logger::debug("executing query");
      AutoPointer<DBResultSet*> rs(
         m_dbConnection->executeQuery(query));
      //Logger::debug("back from query");

      if (rs.haveObject()) {
         //Logger::debug("have non-null resultSet");
         while (rs->next()) {
            //Logger::debug("retrieving storageNodeId");
            const int storageNodeId = rs->intForColumnIndex(0);
            AutoPointer<std::string*> nodeName(
               rs->stringForColumnIndex(1));
            
            if ((storageNodeId > 0) && (nodeName.haveObject())) {
               AutoPointer<std::string*> pingTime(
                  rs->stringForColumnIndex(2));
               AutoPointer<std::string*> copyTime(
                  rs->stringForColumnIndex(3));

               StorageNode storageNode;
               storageNode.setStorageNodeId(storageNodeId);
               storageNode.setNodeName(*(nodeName()));
               
               if (pingTime.haveObject()) {
                  storageNode.setPingTime(chaudiere::DateTime(*(pingTime())));
               }
               
               if (copyTime.haveObject()) {
                  storageNode.setCopyTime(chaudiere::DateTime(*(copyTime())));
               }
               
               listNodes.push_back(storageNode);
            }
         }
         dbAccessSuccess = true;
      } else {
         //Logger::debug("null resultset");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;                        
}

//******************************************************************************

bool DataAccess::getActiveStorageNodes(std::vector<StorageNode>& listNodes) {
   return getStorageNodes(SQL_SELECT_ACTIVE_STORAGE_NODE, listNodes);
}

//******************************************************************************

bool DataAccess::getInactiveStorageNodes(std::vector<StorageNode>& listNodes) {
   return getStorageNodes(SQL_SELECT_INACTIVE_STORAGE_NODE, listNodes);
}

//******************************************************************************

bool DataAccess::getLocalDirectories(const std::string& query,
                                     std::vector<LocalDirectory>& listDirectories) {
   bool dbAccessSuccess = false;
   if (m_dbConnection != nullptr) {
      //Logger::debug("executing query for getLocalDirectories");
      AutoPointer<DBResultSet*> rs(
         m_dbConnection->executeQuery(query));
      //Logger::debug("back from query");

      if (rs.haveObject()) {
         //Logger::debug("have non-null resultSet");
         while (rs->next()) {
            //Logger::debug("processing row");
            const int localDirectoryId = rs->intForColumnIndex(0);
            AutoPointer<std::string*> dirPath(
               rs->stringForColumnIndex(1));
            
            if ((localDirectoryId > 0) && (dirPath.haveObject())) {
               const bool active = rs->boolForColumnIndex(2);
               const bool recurse = rs->boolForColumnIndex(3);
               const bool compress = rs->boolForColumnIndex(4);
               const bool encrypt = rs->boolForColumnIndex(5);
               const int copyCount = rs->intForColumnIndex(6);

               LocalDirectory localDirectory;
               localDirectory.setLocalDirectoryId(localDirectoryId);
               localDirectory.setDirectoryPath(*(dirPath()));
               localDirectory.setActive(active);
               localDirectory.setRecurse(recurse);
               localDirectory.setCompress(compress);
               localDirectory.setEncrypt(encrypt);
               localDirectory.setCopyCount(copyCount);
               
               listDirectories.push_back(localDirectory);
            }
         }
         dbAccessSuccess = true;
      } else {
         Logger::debug("null resultset");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;                                        
}

//******************************************************************************

bool DataAccess::getActiveLocalDirectories(std::vector<LocalDirectory>& listDirectories) {
   return getLocalDirectories(SQL_SELECT_ACTIVE_LOCAL_DIRECTORY, listDirectories);   
}

//******************************************************************************

bool DataAccess::getInactiveLocalDirectories(std::vector<LocalDirectory>& listDirectories) {
   return getLocalDirectories(SQL_SELECT_INACTIVE_LOCAL_DIRECTORY, listDirectories);   
}

//******************************************************************************

bool DataAccess::getLocalFile(int localDirectoryId,
                              const std::string& filePath,
                              LocalFile& localFile) {
   //Logger::debug("getLocalFile called");

   bool dbAccessSuccess = false;
   
   if (m_dbConnection != nullptr) {
      if (localDirectoryId > -1) {
         if (!filePath.empty()) {
            DBStatementArgs args;
            args.add(new DBInt(localDirectoryId));
            args.add(new DBString(filePath));

            //Logger::debug("executing query for getLocalFile");
      
            AutoPointer<DBResultSet*> rs(
               m_dbConnection->executeQuery(SQL_SELECT_LOCAL_FILE, args));
            //Logger::debug("back from query");

            if (rs.haveObject()) {
               //Logger::debug("have non-null resultSet");
               if (rs->next()) {
                  const int localFileId = rs->intForColumnIndex(0);
          
                  if (localFileId > -1) {
                     AutoPointer<std::string*> createTime(
                        rs->stringForColumnIndex(1));
                     AutoPointer<std::string*> modifyTime(
                        rs->stringForColumnIndex(2));
                     AutoPointer<std::string*> scanTime(
                        rs->stringForColumnIndex(3));

                     localFile.setLocalDirectoryId(localDirectoryId);
                     localFile.setLocalFileId(localFileId);
                     localFile.setFilePath(filePath);
                     
                     if (createTime.haveObject()) {
                        localFile.setCreateTime(chaudiere::DateTime(*(createTime())));
                     }
                     
                     if (modifyTime.haveObject()) {
                        localFile.setModifyTime(chaudiere::DateTime(*(modifyTime())));
                     }
                     
                     if (scanTime.haveObject()) {
                        localFile.setScanTime(chaudiere::DateTime(*(scanTime())));
                     }
                     
                     dbAccessSuccess = true;
                  }
               }
            } else {
               //Logger::debug("null resultset");
            }
         } else {
            Logger::error("unable to retrieve local file, missing filePath");
         }
      } else {
         Logger::error("unable to retrieve local file, invalid directory id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;
}

//******************************************************************************

bool DataAccess::getLocalFilesForDirectory(int localDirectoryId,
                                           std::vector<LocalFile>& listFiles) {
   //Logger::debug("getLocalFilesForDirectory called");
   
   bool dbAccessSuccess = false;
   if (m_dbConnection != nullptr) {
      DBStatementArgs args;
      args.add(new DBInt(localDirectoryId));

      //Logger::debug("executing query for getLocalFilesForDirectory");
      AutoPointer<DBResultSet*> rs(
         m_dbConnection->executeQuery(SQL_SELECT_LOCAL_FILE_LIST, args));
      //Logger::debug("back from query");

      if (rs.haveObject()) {
         //Logger::debug("have non-null resultSet");
         while (rs->next()) {
            const int localFileId = rs->intForColumnIndex(0);
            AutoPointer<std::string*> filePath(
               rs->stringForColumnIndex(1));
          
            if ((localFileId > 0) && (filePath.haveObject())) {
               AutoPointer<std::string*> createTime(
                  rs->stringForColumnIndex(2));
               AutoPointer<std::string*> modifyTime(
                  rs->stringForColumnIndex(3));
               AutoPointer<std::string*> scanTime(
                  rs->stringForColumnIndex(4));

               LocalFile localFile;
               localFile.setLocalDirectoryId(localDirectoryId);
               localFile.setLocalFileId(localFileId);
               localFile.setFilePath(*(filePath()));
               
               if (createTime.haveObject()) {
                  localFile.setCreateTime(chaudiere::DateTime(*(createTime())));
               }
               
               if (modifyTime.haveObject()) {
                  localFile.setModifyTime(chaudiere::DateTime(*(modifyTime())));
               }
               
               if (scanTime.haveObject()) {
                  localFile.setScanTime(chaudiere::DateTime(*(scanTime())));
               }
               
               listFiles.push_back(localFile);
            }
         }
         dbAccessSuccess = true;
      } else {
         //Logger::debug("null resultset");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;
}

//******************************************************************************

bool DataAccess::getVault(int storageNodeId,
                          int localDirectoryId,
                          Vault& vault) {
   //Logger::debug("getVault called");
   
   bool dbAccessSuccess = false;
   
   if (m_dbConnection != nullptr) {
      if (storageNodeId > -1) {
         if (localDirectoryId > -1) {
            DBStatementArgs args;
            args.add(new DBInt(storageNodeId));
            args.add(new DBInt(localDirectoryId));

            //Logger::debug("executing query for getVault");
            AutoPointer<DBResultSet*> rs(
               m_dbConnection->executeQuery(SQL_SELECT_NODE_VAULT, args));
            //Logger::debug("back from query");

            if (rs.haveObject()) {
               //Logger::debug("have non-null resultSet");
               if (rs->next()) {
                  const int vaultId = rs->intForColumnIndex(0);
                  if (vaultId > -1) {
                     const bool compress = rs->boolForColumnIndex(1);
                     const bool encrypt = rs->boolForColumnIndex(2);

                     vault.setVaultId(vaultId);
                     vault.setStorageNodeId(storageNodeId);
                     vault.setLocalDirectoryId(localDirectoryId);
                     vault.setCompress(compress);
                     vault.setEncrypt(encrypt);
                     
                     dbAccessSuccess = true;
                  }
               }
            } else {
               //Logger::debug("null resultset");
            }
         } else {
            Logger::error("unable to retrieve vault, invalid local directory id");
         }
      } else {
         Logger::error("unable to retrieve vault, invalid storage node id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;      
}

//******************************************************************************

bool DataAccess::getVaultFile(int vaultId, int localFileId, VaultFile& vaultFile) {
   //Logger::debug("getVaultFile called");
   
   bool dbAccessSuccess = false;
   
   if (m_dbConnection != nullptr) {
      if (vaultId > -1) {
         if (localFileId > -1) {
            DBStatementArgs args;
            args.add(new DBInt(localFileId));
            args.add(new DBInt(vaultId));

            //Logger::debug("executing query for getVaultFile");
            AutoPointer<DBResultSet*> rs(
               m_dbConnection->executeQuery(SQL_SELECT_VAULT_FILE, args));
            //Logger::debug("back from query");

            if (rs.haveObject()) {
               //Logger::debug("have non-null resultSet");
               if (rs->next()) {
                  const int vaultFileId = rs->intForColumnIndex(0);
          
                  if (vaultFileId > 0) {
                     AutoPointer<DBDate*> createTime(
                        rs->dateForColumnIndex(1));
                     AutoPointer<DBDate*> modifyTime(
                        rs->dateForColumnIndex(2));
                     const int originFileSize = rs->intForColumnIndex(3);
                     const int blockCount = rs->intForColumnIndex(4);
                     AutoPointer<std::string*> userPermissions(
                        rs->stringForColumnIndex(5));
                     AutoPointer<std::string*> groupPermissions(
                        rs->stringForColumnIndex(6));
                     AutoPointer<std::string*> otherPermissions(
                        rs->stringForColumnIndex(7));

                     vaultFile.setVaultFileId(vaultFileId);
                     vaultFile.setLocalFileId(localFileId);
                     vaultFile.setVaultId(vaultId);
                     vaultFile.setOriginFileSize(originFileSize);
                     vaultFile.setBlockCount(blockCount);
                     
                     if (userPermissions.haveObject()) {
                        vaultFile.setUserPermissions(*(userPermissions()));
                     }
                     
                     if (groupPermissions.haveObject()) {
                        vaultFile.setGroupPermissions(*(groupPermissions()));
                     }
                     
                     if (otherPermissions.haveObject()) {
                        vaultFile.setOtherPermissions(*(otherPermissions()));
                     }
                     
                     if (createTime.haveObject()) {
                        vaultFile.setCreateTime(createTime->getDateTime());
                     }
                     
                     if (modifyTime.haveObject()) {
                        vaultFile.setModifyTime(modifyTime->getDateTime());
                     }
                     
                     dbAccessSuccess = true;
                  }
               }
            } else {
               //Logger::debug("null resultset");
            }            
         } else {
            Logger::error("unable to retrieve vault file, invalid local file id");
         }
      } else {
         Logger::error("unable to retrieve vault file, invalid vault id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;
}

//******************************************************************************

bool DataAccess::getBlocksForVaultFile(int vaultFileId,
                                       std::vector<VaultFileBlock>& listFileBlocks) {
   bool dbAccessSuccess = false;
   if (m_dbConnection != nullptr) {
      if (vaultFileId > -1) {
         DBStatementArgs args;
         args.add(new DBInt(vaultFileId));

         //Logger::debug("executing query for getBlocksForVaultFile");
         AutoPointer<DBResultSet*> rs(
            m_dbConnection->executeQuery(SQL_SELECT_FILE_BLOCK, args));
         //Logger::debug("back from query");

         if (rs.haveObject()) {
            //Logger::debug("have non-null resultSet");
            while (rs->next()) {
               const int vaultFileBlockId = rs->intForColumnIndex(0);
          
               if (vaultFileBlockId > 0) {
                  AutoPointer<std::string*> createTime(
                     rs->stringForColumnIndex(1));
                  AutoPointer<std::string*> modifyTime(
                     rs->stringForColumnIndex(2));
                  AutoPointer<std::string*> storedTime(
                     rs->stringForColumnIndex(3));
                  const int originFileSize = rs->intForColumnIndex(4);
                  const int storedFileSize = rs->intForColumnIndex(5);
                  const int blockSequenceNumber = rs->intForColumnIndex(6);
                  const int padCharCount = rs->intForColumnIndex(7);
                  AutoPointer<std::string*> uniqueIdentifier(
                     rs->stringForColumnIndex(8));
                  AutoPointer<std::string*> nodeDirectory(
                     rs->stringForColumnIndex(9));
                  AutoPointer<std::string*> nodeFile(
                     rs->stringForColumnIndex(10));

                  VaultFileBlock vaultFileBlock;
                  vaultFileBlock.setVaultFileBlockId(vaultFileBlockId);
                  vaultFileBlock.setVaultFileId(vaultFileId);
                  vaultFileBlock.setOriginFileSize(originFileSize);
                  vaultFileBlock.setStoredFileSize(storedFileSize);
                  vaultFileBlock.setBlockSequenceNumber(blockSequenceNumber);
                  vaultFileBlock.setPadCharCount(padCharCount);
               
                  if (uniqueIdentifier.haveObject()) {
                     vaultFileBlock.setUniqueIdentifier(*(uniqueIdentifier()));
                  }
               
                  if (nodeDirectory.haveObject()) {
                     vaultFileBlock.setNodeDirectory(*(nodeDirectory()));
                  }
               
                  if (nodeFile.haveObject()) {
                     vaultFileBlock.setNodeFile(*(nodeFile()));
                  }
               
                  if (createTime.haveObject()) {
                     vaultFileBlock.setCreateTime(chaudiere::DateTime(*(createTime())));
                  }

                  if (modifyTime.haveObject()) {
                     vaultFileBlock.setModifyTime(chaudiere::DateTime(*(modifyTime())));
                  }

                  if (storedTime.haveObject()) {
                     vaultFileBlock.setStoredTime(chaudiere::DateTime(*(storedTime())));
                  }
               
                  listFileBlocks.push_back(vaultFileBlock);
               }
            }
            dbAccessSuccess = true;
         } else {
            //Logger::debug("null resultset");
         }         
      } else {
         Logger::error("unable to retrieve blocks for vault file, invalid vault file id");
      }
   } else {
      Logger::error(MSG_NO_DB_CONNECTION);
   }
   
   return dbAccessSuccess;
}

//******************************************************************************

