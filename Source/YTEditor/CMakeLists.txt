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
                   ComponentBrowser/ArchetypeTools.hpp
                   ComponentBrowser/ArchetypeTools.cpp
                   ComponentBrowser/ComponentBrowser.hpp
                   ComponentBrowser/ComponentBrowser.cpp
                   ComponentBrowser/ComponentProperty.hpp
                   ComponentBrowser/ComponentSearchBar.hpp
                   ComponentBrowser/ComponentSearchBar.cpp
                   ComponentBrowser/ComponentTools.hpp
                   ComponentBrowser/ComponentTools.cpp
                   ComponentBrowser/ComponentTree.hpp
                   ComponentBrowser/ComponentTree.cpp
                   ComponentBrowser/ComponentWidget.hpp
                   ComponentBrowser/ComponentWidget.cpp
                   ComponentBrowser/PropertyWidget.hpp
                   ComponentBrowser/PropertyWidgetBase.hpp
                   ComponentBrowser/SearchBarEventFilter.hpp
                   ComponentBrowser/SearchBarEventFilter.cpp
                   ComponentBrowser/YTECheckBox.hpp
                   ComponentBrowser/YTELineEdit.hpp
                   GameWindow/GameWindow.hpp
                   GameWindow/GameWindow.cpp					
                   MainWindow/YTEditorMainWindow.hpp
                   MainWindow/YTEditorMainWindow.cpp
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
