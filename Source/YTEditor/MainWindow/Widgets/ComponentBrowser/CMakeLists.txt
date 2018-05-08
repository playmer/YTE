################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Nicholas Ammann
################################################################################

target_sources(YTEditor
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/ArchetypeTools.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ColorPicker.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentBrowser.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentProperty.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentSearchBar.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentTools.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentTree.cpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentWidget.cpp
  ${CMAKE_CURRENT_LIST_DIR}/HeaderListProperty.cpp
  ${CMAKE_CURRENT_LIST_DIR}/HeaderListWidget.cpp
  ${CMAKE_CURRENT_LIST_DIR}/SearchBarEventFilter.cpp
#  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/ArchetypeTools.hpp
  ${CMAKE_CURRENT_LIST_DIR}/CheckBox.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ColorPicker.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentBrowser.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentProperty.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentSearchBar.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentTools.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentTree.hpp
  ${CMAKE_CURRENT_LIST_DIR}/ComponentWidget.hpp
  ${CMAKE_CURRENT_LIST_DIR}/HeaderListProperty.hpp
  ${CMAKE_CURRENT_LIST_DIR}/HeaderListWidget.hpp
  ${CMAKE_CURRENT_LIST_DIR}/LineEdit.hpp
  ${CMAKE_CURRENT_LIST_DIR}/PropertyWidget.hpp
  ${CMAKE_CURRENT_LIST_DIR}/PropertyWidgetBase.hpp
  ${CMAKE_CURRENT_LIST_DIR}/SearchBarEventFilter.hpp
)