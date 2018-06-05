################################################################################
## This source file is a part of YTEPlayer.
## Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
## Author : Joshua T. Fisher
################################################################################
add_executable(YTEPlayer main.cpp
                          WinMain.cpp
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

YTE_Source_Group(YTEPlayer_Root YTEPlayer)

target_link_libraries(YTEPlayer PRIVATE YTE)

#Set the working directory and startup project. 
message("Working Dir: ${YTE_Assets_Root}/Bin")
set_target_properties(YTEPlayer PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${YTE_Assets_Root}/Bin)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT YTEPlayer) 

target_compile_options(YTEPlayer PRIVATE -permissive- -std:c++17 -WX- -W4)

YTE_Target_Set_LTCG(YTEPlayer)

#Have to do /LTCG again here because it appears as though setting this property resets it.
set_target_properties(YTEPlayer PROPERTIES LINK_FLAGS_PUBLISH "/SUBSYSTEM:WINDOWS /LTCG")

install(TARGETS YTEPlayer 
            RUNTIME DESTINATION BIN
            LIBRARY DESTINATION BIN
            ARCHIVE DESTINATION BIN
            COMPONENT applications)
