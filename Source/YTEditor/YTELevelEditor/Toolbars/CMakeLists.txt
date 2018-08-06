################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Nicholas Ammann
################################################################################

target_sources(YTEditor
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/GameToolbar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/GizmoToolbar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Toolbar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ToolbarButton.cpp
#  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/GameToolbar.hpp
  ${CMAKE_CURRENT_LIST_DIR}/GizmoToolbar.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Toolbar.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ToolbarButton.hpp
)