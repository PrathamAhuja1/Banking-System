cmake_minimum_required(VERSION 3.16)

# Set toolchain BEFORE project() - this is critical
set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake")

project(Banking-System)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt6 automation
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)  # Changed to ON - you'll likely need this for .ui files
set(CMAKE_AUTORCC ON)  # Changed to ON - you'll likely need this for .qrc files

# Set Qt installation path for MinGW
set(CMAKE_PREFIX_PATH "C:/Qt/6.8.2/mingw_64")

# Find required packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)
find_package(OpenSSL REQUIRED)

# Verify packages were found
if(Qt6_FOUND)
    message(STATUS "Qt6 found successfully")
    message(STATUS "Qt6 version: ${Qt6_VERSION}")
else()
    message(FATAL_ERROR "Qt6 not found. Please check your Qt installation.")
endif()

if(OpenSSL_FOUND)
    message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
    message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
else()
    message(FATAL_ERROR "OpenSSL not found. Please check vcpkg installation.")
endif()

add_subdirectory(src/crypto)
add_subdirectory(src/compression)
add_subdirectory(src/gui)
add_subdirectory(src/core)
add_subdirectory(src/password)