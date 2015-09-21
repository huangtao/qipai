# Copyright (c) 2015, Tom Huang <huangtao117@gmail.com>
#

CC = gcc

CFLAGS = -Wall -g \
		 -Wno-unused-parameter \
		 -lstdc++ \
		 -Isrc \
		 -Iinclude \
		 -I../libuv/include

INCLUDES = include/ht_comm.h \
		   include/ht_log.h \
		   include/ldcore.h \
		   include/LDTcpClient.h \
		   src/biostream.h \
		   src/ARACrypt.h

OBJS = src/ldcore.o \
	   src/ht_log.o \
	   src/ARACrypt.o \
	   src/LDTcpClient.o \

TEST_OBJS = test/test.o

# build library and test app
all: libqp.a mytest
.PHONY: all

libqp.a: $(OBJS)
	$(AR) crs $@ $^

mytest: $(TEST_OBJS) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $<
 
.PHONY: clean	
clean:
	-$(RM) $(OBJS) libqp.a
