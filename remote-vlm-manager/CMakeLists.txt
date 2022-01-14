cmake_minimum_required(VERSION 3.16)
project(remote-vlm-manager)

set(CMAKE_CXX_STANDARD 17)

# Project source compilation
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/inc HEADERS)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCES)

add_executable(remote-vlm-manager ${SOURCES} ${HEADERS})