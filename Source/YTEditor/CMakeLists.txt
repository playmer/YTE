################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Joshua T. Fisher
################################################################################
set(YTEditorSource main.cpp
                   WinMain.cpp
                   YTEditor.rc
                   YTEditorMeta.hpp
                   ForwardDeclarations.hpp
                   ComponentBrowser/ArchetypeTools.hpp
                   ComponentBrowser/ArchetypeTools.cpp
                   ComponentBrowser/ColorPicker.cpp
                   ComponentBrowser/ColorPicker.hpp
                   ComponentBrowser/ComponentBrowser.hpp
                   ComponentBrowser/ComponentBrowser.cpp
                   ComponentBrowser/ComponentProperty.hpp
                   ComponentBrowser/ComponentProperty.cpp
                   ComponentBrowser/ComponentSearchBar.hpp
                   ComponentBrowser/ComponentSearchBar.cpp
                   ComponentBrowser/ComponentTools.hpp
                   ComponentBrowser/ComponentTools.cpp
                   ComponentBrowser/ComponentTree.hpp
                   ComponentBrowser/ComponentTree.cpp
                   ComponentBrowser/ComponentWidget.hpp
                   ComponentBrowser/ComponentWidget.cpp
                   ComponentBrowser/HeaderListProperty.hpp
                   ComponentBrowser/HeaderListProperty.cpp
                   ComponentBrowser/HeaderListWidget.hpp
                   ComponentBrowser/HeaderListWidget.cpp
                   ComponentBrowser/PropertyWidget.hpp
                   ComponentBrowser/PropertyWidgetBase.hpp
                   ComponentBrowser/SearchBarEventFilter.hpp
                   ComponentBrowser/SearchBarEventFilter.cpp
                   ComponentBrowser/CheckBox.hpp
                   ComponentBrowser/LineEdit.hpp
                   FileViewer/FileViewer.hpp
                   FileViewer/FileViewer.cpp
                   GameWindow/GameWindow.hpp
                   GameWindow/GameWindow.cpp
                   GameWindow/GameWindowEventFilter.hpp
                   GameWindow/GameWindowEventFilter.cpp
                   GameWindow/GameToolbar.hpp
                   GameWindow/GameToolbar.cpp
                   Gizmos/Gizmo.hpp
                   Gizmos/Gizmo.cpp
                   Gizmos/GizmoToolbar.hpp
                   Gizmos/GizmoToolbar.cpp
                   Gizmos/Translate.hpp
                   Gizmos/Translate.cpp
                   Gizmos/Scale.hpp
                   Gizmos/Scale.cpp
                   Gizmos/Rotate.hpp
                   Gizmos/Rotate.cpp
                   MainWindow/MainWindow.hpp
                   MainWindow/MainWindow.cpp
                   MainWindow/ScriptBind.hpp
                   MainWindow/ScriptBind.cpp
                   MainWindow/Toolbar.hpp
                   MainWindow/Toolbar.cpp
                   MainWindow/ToolbarButton.hpp
                   MainWindow/ToolbarButton.cpp
                   MainWindow/ComponentFactoryInit.hpp
                   MainWindow/ComponentFactoryInit.cpp
                   MaterialViewer/MaterialViewer.hpp
                   MaterialViewer/MaterialViewer.cpp
                   MenuBar/EditMenu.hpp
                   MenuBar/EditMenu.cpp
                   MenuBar/FileMenu.hpp
                   MenuBar/FileMenu.cpp
                   MenuBar/GameObjectMenu.hpp
                   MenuBar/GameObjectMenu.cpp
                   MenuBar/ImportMenu.hpp
                   MenuBar/ImportMenu.cpp
                   MenuBar/LevelMenu.hpp
                   MenuBar/LevelMenu.cpp
                   MenuBar/WindowsMenu.hpp
                   MenuBar/WindowsMenu.cpp
                   ObjectBrowser/ObjectBrowser.hpp
                   ObjectBrowser/ObjectBrowser.cpp
                   ObjectBrowser/ObjectItem.hpp
                   ObjectBrowser/ObjectItem.cpp
                   OutputConsole/OutputConsole.hpp
                   OutputConsole/OutputConsole.cpp
                   Physics/PhysicsHandler.hpp
                   Physics/PhysicsHandler.cpp
                   UndoRedo/Commands.hpp
                   UndoRedo/Commands.cpp
                   UndoRedo/UndoRedo.hpp
                   UndoRedo/UndoRedo.cpp
                   WWiseViewer/WWiseWidget.hpp
                   WWiseViewer/WWiseWidget.cpp)
                   
add_executable(YTEditor ${YTEditorSource})

target_include_directories(YTEditor 
  PRIVATE
    ${Source_Root}
    ${ParentDirectory}
    ${Dependencies_Root}/assimp/include
)

set_target_properties(YTEditor
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})
     
                          
# Find the QtWidgets library
set(QtPath $ENV{Qt})
if(NOT QtPath) 
  message(FATAL_ERROR "Please define the environment variable `Qt` with the path to Qt 5.9.0 - i.e. export Qt = C:/Qt/5.9")
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  set(CMAKE_PREFIX_PATH ${QtPath}/msvc2017_64/)
endif()

find_package(Qt5Widgets)

target_link_libraries(YTEditor YTE Qt5::Widgets)

target_compile_options(YTEditor PRIVATE -permissive- -std:c++17 -WX- -W3)

target_compile_definitions(YTEditor PRIVATE YTEditor_Internal=1)

YTE_Source_Group(YTEditor_Root YTEditor) 

YTE_Target_Set_LTCG(YTEditor)

# Have to do /LTCG here because it appears as though setting this property resets it.
set_target_properties(YTEditor PROPERTIES LINK_FLAGS_PUBLISH "/SUBSYSTEM:WINDOWS /LTCG")

if ("${INSTALL_YTEDITOR}" STREQUAL "1")
  message(STATUS "YTEditor Install Target Enabled")
  Install(TARGETS YTEditor
  RUNTIME DESTINATION BIN
  LIBRARY DESTINATION BIN
  ARCHIVE DESTINATION BIN
  COMPONENT applications)

  add_custom_command(
      TARGET YTEditor POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:Qt5::Widgets>
          $<TARGET_FILE_DIR:YTEditor>)

  Install(PROGRAMS "$<TARGET_FILE_DIR:YTEditor>/Qt5Widgets.dll" 
  DESTINATION BIN)
endif()
