# Copyright Paul Dardeau, 2016
# BSD License

CC = clang
CXX = clang++
CC_OPTS = -c -Wall -O2
CXX_OPTS = -c -Wall -O2 -std=c++11 -I../../chaudiere/src -I../../chapeau/src -I../../tonnerre/src
ARCHIVE_CMD = ar
ARCHIVE_OPTS = rs

LIB_NAME = liblachepas.a

OBJS = aes256.o \
AESEncryption.o \
base64.o \
Data.o \
DataAccess.o \
Encryption.o \
FilePermissions.o \
FileReferenceCount.o \
FileSync.o \
GFS.o \
GFSAdminClient.o \
GFSClient.o \
GFSExclusions.o \
GFSMessage.o \
GFSMessageHandler.o \
GFSNodeAdmin.o \
GFSOptions.o \
GFSServer.o \
LocalDirectory.o \
LocalFile.o \
StorageNode.o \
Vault.o \
VaultFile.o \
VaultFileBlock.o

all : $(LIB_NAME)

clean :
	rm -f *.o
	rm -f $(LIB_NAME)
	
$(LIB_NAME) : $(OBJS)
	$(ARCHIVE_CMD) $(ARCHIVE_OPTS) $(LIB_NAME) $(OBJS)

%.o : %.cpp
	$(CXX) $(CXX_OPTS) $< -o $@

%.o : %.c
	$(CC) $(CC_OPTS) $< -o $@

