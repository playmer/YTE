add_library(YTETools SHARED)

target_sources(YTETools 
PRIVATE
  ImportMathTypeConverters.hpp
  YTEAnimation.cpp
  YTEAnimation.hpp
  YTEMesh.cpp
  YTEMesh.hpp
  YTESkeleton.cpp
  YTESkeleton.hpp
  YTESkeletonInternal.hpp
  YTETexture.cpp
  YTETexture.hpp
  YTEToolsMeta.hpp
)

target_include_directories(YTETools 
  PRIVATE
    ${Source_Root}
    ${Dependencies_Root}/assimp/include
)

target_compile_definitions(YTETools PRIVATE YTETools_Internal=1)

set_target_properties(YTETools
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

YTE_Source_Group(YTETools_Root YTETools)

YTE_Target_Set_LTCG(YTETools)

target_link_libraries(YTETools 
PUBLIC 
  YTE
PRIVATE
  assimp
)
