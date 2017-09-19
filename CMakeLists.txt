################################################################################
# This source file is a part of LambPlanet.
# Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
# Author : Joshua T. Fisher
################################################################################
################################################################################
# Set up the required version of cmake and policies related to versions.
################################################################################
CMake_Minimum_Required(VERSION 3.9.1 FATAL_ERROR) 
Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)  

################################################################################
# Declaring our Project and it's used languages.
################################################################################
Project(YTE CXX C)

################################################################################
# Set up some variables that we'll be able to access anywhere.
################################################################################
Set(CMake_Include          ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
Set(Dependencies_Root      ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies)
Set(YTE_Binary_Dir  ${CMAKE_CURRENT_SOURCE_DIR}/Output/Binary)
Set(YTE_Library_Dir ${CMAKE_CURRENT_SOURCE_DIR}/Output/Library)

################################################################################
# Path to modules for finding external libraries.
################################################################################
Set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${Source_Root}/cmake/modules/")

################################################################################
## Includes
################################################################################
Include(${CMake_Include}/AddFile.cmake)
Include(${CMake_Include}/FindDependency.cmake)
Include(${CMake_Include}/YTE_Source_Group.cmake) 

################################################################################
## Setting compiler warnings based on platform.
################################################################################
If (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
  Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
  Set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /debug:FASTLINK")
EndIf()

Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DGLM_FORCE_CXX14 ")

################################################################################
## Separate Projects into folders for IDEs.
################################################################################
Set_Property(GLOBAL PROPERTY USE_FOLDERS ON)

################################################################################
## Sets the folder to check for dependencies in, based on architecture.
################################################################################
Set(QtPath $ENV{Qt})
If(NOT QtPath) 
  Message(FATAL_ERROR "Please define the environment variable `Qt` with the path to Qt 5.9.0 - i.e. export Qt = C:/Qt/5.9")
EndIf()

If (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  Set(CMAKE_PREFIX_PATH ${QtPath}/msvc2017_64/)
EndIf()

################################################################################
## Find the external dependencies we'll link to.
################################################################################
FindWWise(Wwise_Static_Libraries_Release
          Wwise_Static_Libraries_Debug
          Wwise_Include_Directory)

Include_Directories(${Wwise_Include_Directory})
Include_Directories(${Dependencies_Root})
Include_Directories(${Dependencies_Root}/Bullet)
Include_Directories(${Dependencies_Root}/assimp/contrib/rapidjson/include/)

#TODO: Move this to the platform part of the YTE probably?
If (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  Set(StaticLibraries ${StaticLibraries} wsock32.lib)
  Set(StaticLibraries ${StaticLibraries} ws2_32.lib)
  Set(StaticLibraries ${StaticLibraries} IPHLPAPI.lib)
  Set(StaticLibraries ${StaticLibraries} Winmm.lib)
  Set(StaticLibraries ${StaticLibraries} Shlwapi.lib)
  Set(StaticLibraries ${StaticLibraries} Psapi.lib)
  Set(StaticLibraries ${StaticLibraries} dxguid.lib)
  Set(StaticLibraries ${StaticLibraries} XInput9_1_0.lib)
EndIf()

Set(StaticLibrariesRelease ${StaticLibrariesRelease} ${Wwise_Static_Libraries_Release})
Set(StaticLibrariesDebug ${StaticLibrariesDebug} ${Wwise_Static_Libraries_Debug})

################################################################################
## Call the CMakeLists.txt in our engine folder, and game folder.
################################################################################
Add_Subdirectory(Dependencies)
Add_Subdirectory(Source)

Set_Property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YTEditor) 