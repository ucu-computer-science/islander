cmake_minimum_required(VERSION 3.15)

set(PROJECT_NAME ps)
project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 14)

#! Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

#! Project source compilation
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/inc HEADERS)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCES)

add_executable(ps ${SOURCES} ${HEADERS})

#! Add external packages
target_link_libraries(ps Boost::boost)
find_package(Boost 1.71.0)
include_directories(${Boost_INCLUDE_DIR})

#! set output directory (bin)
set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
