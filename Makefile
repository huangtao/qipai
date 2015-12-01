#
# Copyright (c) 2015, Tom Huang <huangtao117@gmail.com>
#

CC = gcc

CFLAGS = -Wall -g \
		 -Wno-unused-parameter \
		 -Isrc \
		 -Iinclude

INCLUDES = src/card.h \
		   src/hand.h

OBJS = src/card.o \
	   src/hand.o

TEST_OBJS = test/test.o

# build library and test app
all: libqp.a test
.PHONY: all

libqp.a: $(OBJS)
	$(AR) crs $@ $^

test: $(TEST_OBJS) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $<
 
.PHONY: clean	
clean:
	-$(RM) $(OBJS) libqp.a
