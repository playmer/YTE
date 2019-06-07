################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Nicholas Ammann
################################################################################

target_sources(YTEditor
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/EditMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/EditMenu.hpp
  ${CMAKE_CURRENT_LIST_DIR}/FileMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/FileMenu.hpp
  ${CMAKE_CURRENT_LIST_DIR}/GameObjectMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/GameObjectMenu.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ImportMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ImportMenu.hpp
  ${CMAKE_CURRENT_LIST_DIR}/LevelMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/LevelMenu.hpp
  ${CMAKE_CURRENT_LIST_DIR}/WindowsMenu.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WindowsMenu.hpp
)