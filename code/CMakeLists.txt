CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

MESSAGE(STATUS ">>>>CMakeLists:" ${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE(${CMAKE_CURRENT_LIST_DIR}/../cmake/config.cmake)

### 工程名
PROJECT(library)

#添加源目录
ADD_SUBDIRECTORY(source)