add_executable(YTEPlayer main.cpp
                         YTEPlayer.rc)

target_include_directories(YTEPlayer 
  PRIVATE
    ${Source_Root}
    ${ParentDirectory}
    ${Dependencies_Root}/assimp/include
)

set_target_properties(YTEPlayer
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

message(YTEPlayer runtime dir: ${YTE_Binary_Dir})

YTE_Source_Group(YTEPlayer_Root YTEPlayer)

target_link_libraries(YTEPlayer PRIVATE YTEMain)

#Set the working directory and startup project. 
set_target_properties(YTEPlayer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${YTE_Assets_Root}/Bin)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YTEPlayer) 

YTE_Target_Set_LTCG(YTEPlayer)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  # Have to do /LTCG here because it appears as though setting this property resets it.
  #set_target_properties(YTEPlayer PROPERTIES LINK_FLAGS_PUBLISH "/SUBSYSTEM:WINDOWS /LTCG")

  target_compile_options(YTEPlayer PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
else()
  target_compile_options(YTEPlayer PRIVATE -std=c++17)
endif()

install(TARGETS YTEPlayer 
            RUNTIME DESTINATION BIN
            LIBRARY DESTINATION BIN
            ARCHIVE DESTINATION BIN
            COMPONENT applications)
