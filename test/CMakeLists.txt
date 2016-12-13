include_directories(../src)

# 指定生成目标
add_executable(test_mjhz test_mjhz.c)
add_executable(test_gp test_gp.c)

# 添加链接库
target_link_libraries(test_mjhz qipai)
target_link_libraries(test_gp qipai)
