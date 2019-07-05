################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Nicholas Ammann
################################################################################

target_sources(YTEditor
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/GameWindowEventFilter.cpp
  ${CMAKE_CURRENT_LIST_DIR}/GameWindowEventFilter.hpp
  ${CMAKE_CURRENT_LIST_DIR}/YTEWindow.cpp
  ${CMAKE_CURRENT_LIST_DIR}/YTEWindow.hpp
)