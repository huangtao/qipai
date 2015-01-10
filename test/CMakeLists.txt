# Create unit test "test"

project(test)

# The version number
set (test_VERSION_MAJOR 1)
set (test_VERSION_MINOR 0)

# search path for include files
include_directories("${CMAKE_SOURCE_DIR}/src")

# search path for lib files
link_directories("${CMAKE_SOURCE_DIR}/lib")

# boost path
#set(BOOST_ROOT "d:/boost_1_53_0/")
#set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "d:/boost_1_53_0/")
#set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "d:/boost_1_53_0/stage/lib")
#set(Boost_USE_STATIC_LIBS ON)
#set(Boost_USE_MULTITHREADED ON)
#set(Boost_USE_STATIC_RUNTIME OFF)
#set(Boost_DEBUG TRUE)
#set(Boost_INCLUDE_DIR "d:/boost_1_53_0")
#set(Boost_LIBRARY_DIR "d:/boost_1_53_0/stage/lib")
#find_package(boost 1.53.0 COMPONENTS thread filesystem system REQUIRED)
#include_directories(${Boost_INCLUDE_DIRS})
#link_directories(${Boost_LIBRARY_DIR})

add_executable(test ht_log.cc test.cpp)
#target_link_libraries(test libqipai ${Boost_LIBRARIES})
target_link_libraries(test libqipai)

install(TARGETS test DESTINATION bin)
