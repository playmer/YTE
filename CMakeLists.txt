################################################################################
# This source file is a part of LambPlanet.
# Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
# Author : Joshua T. Fisher
################################################################################
################################################################################
# Set up the required version of cmake and policies related to versions.
################################################################################
cmake_minimum_required(VERSION 3.16.1 FATAL_ERROR) 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)  

################################################################################
# Declaring our Project and it's used languages.
################################################################################
project(YTE CXX C)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

################################################################################
# Set up some options
################################################################################
option(YTE_BUILD_DOCUMENTATION
       "Determines if we should build documentation. Requires Doxygen and Sphinx."
       OFF)

################################################################################
# Set up some variables that we'll be able to access anywhere.
################################################################################
set(Repo_Root         ${CMAKE_CURRENT_SOURCE_DIR})
set(YTE_Repo_Root     ${CMAKE_CURRENT_SOURCE_DIR})
set(CMake_Include     ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(Dependencies_Root ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)
set(YTE_Binary_Dir    ${CMAKE_CURRENT_SOURCE_DIR}/Output/Binary)
set(YTE_Library_Dir   ${CMAKE_CURRENT_SOURCE_DIR}/Output/Library)
set(YTE_Assets_Root   ${CMAKE_CURRENT_SOURCE_DIR}/Assets/YTE)

message("Assets Dir: ${YTE_Assets_Root}")

################################################################################
# Path to modules for finding external libraries.
################################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Source_Root}/cmake/modules/")

################################################################################
## Includes
################################################################################
include(${CMake_Include}/AddFile.cmake)
#include(${CMake_Include}/FindDependency.cmake)
include(${CMake_Include}/YTE_Source_Group.cmake)
include(${CMake_Include}/YTE_Target_Precompiled_Headers.cmake)
include(${CMake_Include}/YTE_Target_Folder.cmake)


################################################################################
## Separate Projects into folders for IDEs.
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (CMAKE_SYSTEM_NAME STREQUAL Windows)
  # Find the QtWidgets library
  set(QtPath $ENV{Qt})
  if(NOT QtPath) 
    message(FATAL_ERROR "Please define the environment variable `Qt` with the path to Qt 5.9.0 - i.e. export Qt = C:/Qt/5.9")
  endif()

  set(CMAKE_PREFIX_PATH ${QtPath}/msvc2017_64/)
  message(STATUS "CMAKE_PREFIX_PATH: ${CMAKE_PREFIX_PATH}")
endif()

################################################################################
## Call the CMakeLists.txt in our engine folder, and game folder.
################################################################################
add_subdirectory(Dependencies)
add_subdirectory(Source)

if (YTE_BUILD_DOCUMENTATION)
    add_subdirectory(Documentation)
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YTEditor) 