################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Nicholas Ammann
################################################################################

include(${CMAKE_CURRENT_LIST_DIR}/MenuBar/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Physics/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Toolbars/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/UndoRedo/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Widgets/CMakeLists.txt)

target_sources(YTEditor
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/ComponentFactoryInit.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentFactoryInit.cpp

  ${CMAKE_CURRENT_LIST_DIR}/Gizmo.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Gizmo.cpp

  ${CMAKE_CURRENT_LIST_DIR}/YTEditorMainWindow.hpp
  ${CMAKE_CURRENT_LIST_DIR}/YTEditorMainWindow.cpp

  ${CMAKE_CURRENT_LIST_DIR}/ScriptBind.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ScriptBind.cpp

  ${CMAKE_CURRENT_LIST_DIR}/YTELevelEditor.hpp
  ${CMAKE_CURRENT_LIST_DIR}/YTELevelEditor.cpp
)
