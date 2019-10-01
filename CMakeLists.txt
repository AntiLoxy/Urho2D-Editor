# Set project name
project (Geode)
# Define target name
set (TARGET_NAME Geode)

######################################

# Set CMake minimum version and CMake policy required by Urho3D-CMake-common module
if (WIN32)
    cmake_minimum_required (VERSION 3.2.3)      # Going forward all platforms will use this as minimum version
else ()
    cmake_minimum_required (VERSION 2.8.6)
endif ()
if (COMMAND cmake_policy)
    cmake_policy (SET CMP0003 NEW)
    if (CMAKE_VERSION VERSION_GREATER 2.8.12 OR CMAKE_VERSION VERSION_EQUAL 2.8.12)
        # INTERFACE_LINK_LIBRARIES defines the link interface
        cmake_policy (SET CMP0022 NEW)
    endif ()
    if (CMAKE_VERSION VERSION_GREATER 3.0.0 OR CMAKE_VERSION VERSION_EQUAL 3.0.0)
        # Disallow use of the LOCATION target property - so we set to OLD as we still need it
        cmake_policy (SET CMP0026 OLD)
        # MACOSX_RPATH is enabled by default
        cmake_policy (SET CMP0042 NEW)
    endif ()
endif ()

#####################################
##  Import all librairies sources
#####################################
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)
include (UrhoCommon)

#####################################
##  Import application sources
#####################################

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY bin)

file (GLOB_RECURSE SRC_CPP_FILES ${CMAKE_SOURCE_DIR}/Sources/*.cpp)
file (GLOB_RECURSE SRC_H_FILES ${CMAKE_SOURCE_DIR}/Sources/*.h)
define_source_files (GROUP EXTRA_CPP_FILES ${SRC_CPP_FILES} EXTRA_H_FILES ${SRC_H_FILES})

#####################################
##  Resources management
#####################################

set(RESOURCE_DIRS ${CMAKE_SOURCE_DIR}/Resources/Data ${CMAKE_SOURCE_DIR}/Resources/CoreData)
foreach(DIR ${RESOURCE_DIRS})
    file(COPY ${DIR} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/bin)
endforeach(DIR)

#####################################
##  Exe & link libraries
#####################################

setup_main_executable()
