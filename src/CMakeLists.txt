#
# A library for poker and majang application.
# It's provide common poker and majang algorithm.
#
# Copyright (C) 2013 Huang Tao <huangtao117@gmail.com>
#

# Set projectname
project(libqipai)

# CMake policies
cmake_minimum_required(VERSION 2.8)

# define all source files
set(sources
    qp_common.c
    mj.c
    mj_algo.c
    mjhz.c
    card.c
    card_algo.c
    gp.c
)

# defile all header files
set(headers
    ht_lch.h
    card.h
    hand.h
    qipai.h
    deck.h
    hand_type.h
    card_player.h
    sort_card.h
    ddz.h
    gp.h
    texas.h
)

# add all .h files to a group(visual studio)
source_group("Header Files" FILES ${headers})

#libqipai.a for release
#libqipai-d.a for debug
#set(CMAKE_DEBUG_POSTFIX -d)

add_library(libqp ${sources} ${headers})
#add_library(libqipai SHARED ${sources} ${headers})
#add_library(libqipai-s STATIC ${sources} ${headers})
#set_target_properties(libqipai-s PROPERTIES OUTPUT_NAME "libqipai" PREFIX "lib")

# add the install targets
install(TARGETS libqp DESTINATION lib)
install(FILES ${headers} DESTINATION include)
