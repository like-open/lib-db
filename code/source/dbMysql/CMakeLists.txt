CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

MESSAGE(STATUS ">>>>CMakeLists:" ${CMAKE_CURRENT_SOURCE_DIR})

# 装载文件列表
INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../macros_build.cmake)

# add_definitions(-DMYSQLLIB = ${MYSQL_LIBRARY})
		
INCLUDE_DIRECTORIES(${system_include_dir})

macro_build_library(dbMysql ${CMAKE_CURRENT_LIST_DIR} "")