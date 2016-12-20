# 版本号
set(qipai_VERSION_MAJOR 1)
set(qipai_VERSION_MINOR 0)

# 源文件
set(sources
    qp_common.c
    mj.c
    mj_algo.c
    mjhz.c
    card.c
    card_algo.c
    gp.c
)

# 头文件
set(headers
    qipai.h
    qp_common.h
    mj.h
    mj_algo.h
    mjhz.h
    card.h
    card_algo.h
    gp.h
)

# 目录下的源文件保存到变量
aux_source_directory(. MY_SRCS)

# add all .h files to a group(visual studio)
source_group("Header Files" FILES ${headers})

# 调试版本加_d
set(CMAKE_DEBUG_POSTFIX _d)

# 生成静态库
add_library(qipai STATIC ${sources} ${headers})
#add_library(libqipai SHARED ${sources} ${headers})
#add_library(libqipai-s STATIC ${sources} ${headers})
#set_target_properties(libqipai-s PROPERTIES OUTPUT_NAME "libqipai" PREFIX "lib")

# 链接库
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
target_link_libraries(qipai m)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

# 设置输出目录
if(ANDROID)
message(STATUS "android doesn't need ARCHIVE_OUTPUT_DIRECTORY")
else(ANDROID)
set_target_properties(qipai
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        )
endif(ANDROID)

# add the install targets
install(TARGETS qipai DESTINATION lib)
install(FILES ${headers} DESTINATION include)
