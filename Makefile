#
# Copyright (c) 2015, Tom Huang <huangtao117@gmail.com>
#

CC = gcc

CFLAGS = -Wall -g \
		 -Isrc \
		 -Iinclude

INCLUDES = src/card.h \
           src/card_sort.h

OBJS = src/card.o \
	   src/gp.o \
	   src/texas.o \
       src/card_sort.o

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
