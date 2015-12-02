#
# Copyright (c) 2015, Tom Huang <huangtao117@gmail.com>
#

CC = gcc

CFLAGS = -Wall -g \
		 -Wno-unused-parameter \
		 -Isrc \
		 -Iinclude

INCLUDES = src/card.h \
		   src/card_player.h \
           src/card_sort.h

OBJS = src/card.o \
	   src/gp.o \
       src/card_sort.o

TEST_OBJS = test/test.o

# build library and test app
all: libqp.a test
.PHONY: all

libqp.a: $(OBJS)
	$(AR) crs $@ $^

test: $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $<
 
.PHONY: clean	
clean:
	-$(RM) $(OBJS) libqp.a
