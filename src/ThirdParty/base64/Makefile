# Copyright Paul Dardeau, 2016
# BSD License

CC = clang
CXX = clang++
CC_OPTS = -c -Wall -O2
CXX_OPTS = -c -Wall -O2 -std=c++11 -I/usr/local/include

OBJS = base64.o

all : $(OBJS)

clean :
	rm -f *.o
	
%.o : %.cpp
	$(CXX) $(CXX_OPTS) $< -o $@

%.o : %.c
	$(CC) $(CC_OPTS) $< -o $@

