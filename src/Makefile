# Copyright Paul Dardeau, 2016
# BSD License

CC = clang
CXX = clang++
CC_OPTS = -c -Wall -O2
CXX_OPTS = -c -Wall -O2 -std=c++11 -I/usr/local/include -I../../chaudiere/src -I../../chapeau/src -I../../tonnerre/src -I./ThirdParty/aes256 -I./ThirdParty/base64
ARCHIVE_CMD = ar
ARCHIVE_OPTS = rs

LIB_NAME = liblachepas.a

AES_OBJS = ./ThirdParty/aes256/aes256.o

BASE64_OBJS = ./ThirdParty/base64/base64.o

OBJS = AESEncryption.o \
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
GFSMessageCommands.o \
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
	cd ThirdParty/aes256 && gmake clean
	cd ThirdParty/base64 && gmake clean
	
$(AES_OBJS) :
	cd ThirdParty/aes256 && gmake

$(BASE64_OBJS) :
	cd ThirdParty/base64 && gmake

$(LIB_NAME) : $(OBJS) $(AES_OBJS) $(BASE64_OBJS)
	$(ARCHIVE_CMD) $(ARCHIVE_OPTS) $(LIB_NAME) $(OBJS) $(AES_OBJS) $(BASE64_OBJS)

%.o : %.cpp
	$(CXX) $(CXX_OPTS) $< -o $@

%.o : %.c
	$(CC) $(CC_OPTS) $< -o $@

