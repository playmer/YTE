################################################################################
# This source file is a part of LambPlanet.
# Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
# Author : Joshua T. Fisher
################################################################################
################################################################################
# Set up the required version of cmake and policies related to versions.
################################################################################
cmake_minimum_required(VERSION 3.9.1 FATAL_ERROR) 
set_property(GLOBAL PROPERTY USE_FOLDERS ON)  

################################################################################
# Declaring our Project and it's used languages.
################################################################################
project(YTE CXX C)

################################################################################
# Set up some variables that we'll be able to access anywhere.
################################################################################
set(CMake_Include          ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(Dependencies_Root      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)
set(YTE_Binary_Dir  ${CMAKE_CURRENT_SOURCE_DIR}/Output/Binary)
set(YTE_Library_Dir ${CMAKE_CURRENT_SOURCE_DIR}/Output/Library)

################################################################################
# Path to modules for finding external libraries.
################################################################################
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Source_Root}/cmake/modules/")

################################################################################
## Includes
################################################################################
include(${CMake_Include}/AddFile.cmake)
include(${CMake_Include}/FindDependency.cmake)
include(${CMake_Include}/YTE_Source_Group.cmake)
include(${CMake_Include}/YTE_Target_Precompiled_Headers.cmake)
include(${CMake_Include}/YTE_Target_Folder.cmake)


################################################################################
## Separate Projects into folders for IDEs.
################################################################################
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
## Call the CMakeLists.txt in our engine folder, and game folder.
################################################################################
add_subdirectory(Dependencies)
add_subdirectory(Source)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YTEditor) 