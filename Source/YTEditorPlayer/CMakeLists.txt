add_executable(YTEditorPlayer)

target_sources(YTEditorPlayer
  PRIVATE
    main.cpp
    YTEditorPlayer.rc
)

target_include_directories(YTEditorPlayer
  PRIVATE
    ${Source_Root}
    ${ParentDirectory}
    ${Dependencies_Root}/assimp/include
)

set_target_properties(YTEditorPlayer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${YTE_Assets_Root}/Bin)

set_target_properties(YTEditorPlayer
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})


message(YTEditorPlayer runtime dir: ${YTE_Binary_Dir})

YTE_Target_Set_LTCG(YTEditorPlayer)

target_link_libraries(YTEditorPlayer PRIVATE YTEMain YTEditor)

YTE_Source_Group(YTEditor_Root YTEditorPlayer)

if ("${INSTALL_YTEDITORPLAYER}" STREQUAL "1")
  message(STATUS "YTEditorPlayer Install Target Enabled")
  Install(TARGETS YTEditorPlayer
  RUNTIME DESTINATION BIN
  LIBRARY DESTINATION BIN
  ARCHIVE DESTINATION BIN
  COMPONENT applications)

  add_custom_command(
      TARGET YTEditorPlayer POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:Qt5::Widgets>
          $<TARGET_FILE_DIR:YTEditorPlayer>)

  Install(PROGRAMS "$<TARGET_FILE_DIR:YTEditorPlayer>/Qt5Widgets.dll" 
  DESTINATION BIN)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # Have to do /LTCG here because it appears as though setting this property resets it.
  set_target_properties(YTEditorPlayer PROPERTIES LINK_FLAGS_PUBLISH "/SUBSYSTEM:WINDOWS /LTCG")

  # I want to get clean of this warning, but don't want to fix it now. I think mostly can be fixed
  # by using C++/SDL file loading and fmt.
  target_compile_definitions(YTEditorPlayer PRIVATE _CRT_SECURE_NO_WARNINGS)

  target_compile_options(YTEditorPlayer PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
else()
  target_compile_options(YTEditorPlayer PRIVATE -std=c++17)
endif()
