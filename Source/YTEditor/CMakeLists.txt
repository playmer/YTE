add_library(YTEditor SHARED)

add_subdirectory(Framework)
add_subdirectory(YTELevelEditor)

target_sources(YTEditor
  PRIVATE
    ForwardDeclarations.hpp
    EditorPlugin.cpp
    EditorPlugin.hpp
    YTEditorMeta.hpp
)

target_include_directories(YTEditor
  PUBLIC
    ${Source_Root}
    ${ParentDirectory}
    ${Dependencies_Root}/assimp/include
)

set_target_properties(YTEditor
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})
     
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
  # Find the QtWidgets library
  set(QtPath $ENV{Qt})
  if(NOT QtPath) 
    message(FATAL_ERROR "Please define the environment variable `Qt` with the path to Qt 5.9.0 - i.e. export Qt = C:/Qt/5.9")
  endif()

  set(CMAKE_PREFIX_PATH ${QtPath}/msvc2017_64/)
endif()

find_package(Qt5Widgets)

YTE_Target_Set_LTCG(YTEditor)

target_link_libraries(YTEditor 
  PUBLIC 
    YTEMain
    YTETools
    Qt5::Widgets 
    toolwindowmanager 
    assimp
)

target_compile_definitions(YTEditor PRIVATE YTEditor_Internal=1)

YTE_Source_Group(YTEditor_Root YTEditor)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # Have to do /LTCG here because it appears as though setting this property resets it.
  set_target_properties(YTEditor PROPERTIES LINK_FLAGS_PUBLISH "/SUBSYSTEM:WINDOWS /LTCG")

  # I want to get clean of this warning, but don't want to fix it now. I think mostly can be fixed
  # by using C++/SDL file loading and fmt.
  target_compile_definitions(YTEditor PRIVATE _CRT_SECURE_NO_WARNINGS)

  target_compile_options(YTEditor PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
else()
  target_compile_options(YTEditor PRIVATE -std=c++17)
endif()
