################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Joshua T. Fisher
################################################################################
Include_Directories(${Source_Root})
Include_Directories(${ParentDirectory}
                    ${Dependencies_Root}/assimp/include)

Set(YTEditorSource main.cpp
                   WinMain.cpp
                   YTEditor.rc
                   YTEditorMainWindow.hpp
                   YTEditorMainWindow.cpp
                   ArchetypeTools.hpp
                   ArchetypeTools.cpp
                   ObjectBrowser.hpp
                   ObjectBrowser.cpp
                   OutputConsole.hpp
                   OutputConsole.cpp
                   GameWindow.hpp
                   GameWindow.cpp					
                   ComponentBrowser.hpp
                   ComponentBrowser.cpp
                   ComponentWidget.hpp
                   ComponentWidget.cpp
                   ComponentProperty.hpp
                   PropertyWidget.hpp
                   PropertyWidgetBase.hpp
                   ComponentTree.hpp
                   ComponentTree.cpp
                   ComponentTools.hpp
                   ComponentTools.cpp
                   MaterialViewer.hpp
                   MaterialViewer.cpp
                   ComponentSearchBar.hpp
                   ComponentSearchBar.cpp
                   SearchBarEventFilter.hpp
                   SearchBarEventFilter.cpp
                   FileMenu.hpp
                   FileMenu.cpp
                   EditMenu.hpp
                   EditMenu.cpp
                   WindowsMenu.hpp
                   WindowsMenu.cpp
                   GameObjectMenu.hpp
                   GameObjectMenu.cpp
                   ImportMenu.hpp
                   ImportMenu.cpp
                   LevelMenu.hpp
                   LevelMenu.cpp
                   ObjectItem.hpp
                   ObjectItem.cpp
                   UndoRedo.hpp
                   UndoRedo.cpp
                   Commands.hpp
                   Commands.cpp
                   YTELineEdit.hpp
                   YTECheckBox.hpp
                   Physics/PhysicsHandler.hpp
                   Physics/PhysicsHandler.cpp
                   YTECheckBox.hpp
                   WWiseWidget.hpp
                   WWiseWidget.cpp)

Add_Executable(YTEditor ${YTEditorSource})

Set_Target_Properties(YTEditor
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})
	 
YTE_Source_Group(YTEditor_Root YTEditorSource) 
                          
# Find the QtWidgets library
Find_Package(Qt5Widgets)

Target_Link_Libraries(YTEditor YTE ${StaticLibraries} Qt5::Widgets)

Target_Compile_Options(YTEditor PRIVATE -permissive- -std:c++17 -WX- -W4)
