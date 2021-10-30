if (ENABLE_SANITIZERS)
if(CMAKE_C_COMPILER_ID STREQUAL "MSVC" OR CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(WARNING "Sanitizers for the MSVC are not yet supported")
elseif(MINGW) #  OR CYGWIN?
    message(WARNING "Sanitizers for the MINGW are not yet supported")
else()
    set(SANITIZE_UNDEFINED ON)

    # Only one of Memory, Address, or Thread sanitizers is applicable at the time
if(CMAKE_C_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(SANITIZE_MEMORY ON)
    # set(SANITIZE_ADDRESS ON)
elseif(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(SANITIZE_ADDRESS ON) # GCC does not have a SANITIZE_MEMORY.
endif()
#    set(SANITIZE_THREAD ON)

    set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/extra/sanitizers" ${CMAKE_MODULE_PATH})
    find_package(Sanitizers)

    add_sanitizers(${ALL_TARGETS})
endif ()

endif () # For MSVC

