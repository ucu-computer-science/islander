cmake_minimum_required(VERSION 3.16)
project(client C CXX)

set(CMAKE_CXX_STANDARD 17)
#set(GCC_COVERAGE_LINK_FLAGS    "-D_GLIBCXX_USE_CXX11_ABI=0")
#set(GCC_COVERAGE_LINK_FLAGS    "-Wall -std=c++11")
#set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
#add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)

add_compile_options(-Wall -std=c++11)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/extra)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/inc HEADERS)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCES)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})

#! Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Readline)
include_directories(${Readline_INCLUDE_DIR})

find_package(OpenSSL)
include_directories(${OpenSSL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} Boost::program_options ${Readline_LIBRARY} ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES})
