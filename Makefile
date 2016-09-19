#
# Copyright (c) 2015, Tom Huang <huangtao117@gmail.com>
#

CC = gcc

CFLAGS = -Wall -g \
         -Isrc \
         -Iinclude

INCLUDES = src/card.h \
           src/card_algo.h \
           src/gp.h \
           src/mj.h \
           src/mj_algo.h \
           src/mjhz.h

OBJS = src/card.o \
       src/gp.o \
       src/texas.o \
       src/card_algo.o \
       src/mj.o \
       src/mj_algo.o \
       src/mjhz.o

TEST_GP_OBJS = test/test_gp.o
TEST_MJHZ_OBJS = test/test_mjhz.o

# build library and test app
all: libqipai.a test_gp test_mjhz
.PHONY: all

libqipai.a: $(OBJS)
	$(AR) crs $@ $^

test_gp: $(TEST_GP_OBJS) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

test_mjhz: $(TEST_MJHZ_OBJS) $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c -o $@ $<
 
.PHONY: clean	
clean:
	-$(RM) $(OBJS) libqp.a
