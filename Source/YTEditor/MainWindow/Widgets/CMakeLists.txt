################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Nicholas Ammann
################################################################################

include(${CMAKE_CURRENT_LIST_DIR}/ComponentBrowser/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/FileViewer/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/GameWindow/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/MaterialViewer/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/ObjectBrowser/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/OutputConsole/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/WWiseViewer/CMakeLists.txt)

target_sources(YTEditor
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/Widget.cpp
#  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/Widget.hpp
)
