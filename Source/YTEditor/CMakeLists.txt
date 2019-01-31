################################################################################
## This source file is a part of YTEditor.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Joshua T. Fisher
################################################################################
add_executable(YTEditor "")

include(Framework/CMakeLists.txt)
include(YTELevelEditor/CMakeLists.txt)

target_sources(YTEditor
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WinMain.cpp
    ${CMAKE_CURRENT_LIST_DIR}/YTEditor.rc
    ${CMAKE_CURRENT_LIST_DIR}/ForwardDeclarations.hpp
    ${CMAKE_CURRENT_LIST_DIR}/YTEditorMeta.hpp
)

target_include_directories(YTEditor
  PRIVATE
    ${Source_Root}
    ${ParentDirectory}
    ${Dependencies_Root}/assimp/include
)

set_target_properties(YTEditor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${YTE_Assets_Root}/Bin)

set_target_properties(YTEditor
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir}
)

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
