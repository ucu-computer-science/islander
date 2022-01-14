cmake_minimum_required(VERSION 3.15)

#! Check every comment after the "#!"

#! CHANGE YOUR PROJECT NAME
set(PROJECT_NAME islander-server)
project(${PROJECT_NAME} C CXX) # project(${PROJECT_NAME} C CXX ASM)

set(CMAKE_CXX_STANDARD 17)

#! Options
set(WARNINGS_AS_ERRORS ON)
set(ENABLE_PVS_STUDIO OFF)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#! ENABLE_SANITIZERS is the option for the test builds!
#  Definitely enable it while developing.
#  Disable it for the production builds and before submitting for grading!
set(ENABLE_SANITIZERS OFF)

#! Set output directories
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

#! Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif ()

#! Include headers
include_directories(include)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/include HEADERS)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCES)

# copy certificate file in binary dir for encrypted communication
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/mycert.pem
		DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Include fixed CMake configuration
#include(cmake/config.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
		${CMAKE_CURRENT_SOURCE_DIR}/cmake/extra)

#! Project source compilation
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

#! Add external packages
find_package(Boost 1.71.0 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(OpenSSL)
include_directories(${OpenSSL_INCLUDE_DIR})

target_link_libraries(${PROJECT_NAME} Boost::program_options ${OPENSSL_CRYPTO_LIBRARIES} ${OPENSSL_SSL_LIBRARIES})

#####################################
# Define ALL_TARGETS variable to use in PVS and Sanitizers
set(ALL_TARGETS ${PROJECT_NAME})

if (ENABLE_PVS_STUDIO)
	set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	include(cmake/extra/PVS-Studio.cmake)
	pvs_studio_add_target(TARGET ${TARGET_NAME}.analyze ALL
			OUTPUT FORMAT errorfile
			ANALYZE ${TARGET_NAME}
			MODE GA:1,2,3 OP:1,2,3 GA:1,2,3 OP:1,2,3 64:1,2,3 CS:1,2,3 MISRA:1,2,3
			LOG target.err)
else ()
	message(NOTICE "\n Consider using PVS-Studio with -DENABLE_PVS_STUDIO=ON flag or Windows GUI application.\n")
endif ()

# bin
set_target_properties(${PROJECT_NAME}
		PROPERTIES
		RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../bin)
