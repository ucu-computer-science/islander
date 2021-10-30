cmake_minimum_required(VERSION 3.15)
project(namespaces C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# Project source compilation
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/inc HEADERS)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCES)

add_executable(namespaces ${SOURCES} ${HEADERS})
