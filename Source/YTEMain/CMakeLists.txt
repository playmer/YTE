add_library(YTEMain STATIC)

target_sources(YTEMain 
  PRIVATE
    YTEMain.cpp
    YTEMain.hpp
)

target_include_directories(YTEMain 
  PRIVATE
    ${Source_Root}
)

set_target_properties(YTEMain
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

YTE_Source_Group(YTEMain_Root YTEMain)

YTE_Target_Set_LTCG(YTEMain)

target_link_libraries(YTEMain PUBLIC YTE SDL2main SDL2)

