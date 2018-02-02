add_library(YTE "")

get_filename_component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

include(Core/CMakeLists.txt)
include(GameComponents/CMakeLists.txt)
include(Graphics/CMakeLists.txt)
include(Physics/CMakeLists.txt)
include(Meta/CMakeLists.txt)
include(Platform/CMakeLists.txt)
include(StandardLibrary/CMakeLists.txt)
include(Utilities/CMakeLists.txt)
include(WWise/CMakeLists.txt)

target_include_directories(YTE 
  PUBLIC
    ${Dependencies_Root}
    ${Dependencies_Root}/assimp/include
    ${Dependencies_Root}/assimp/contrib/rapidjson/include
    ${Dependencies_Root}/bullet3/src
    ${Source_Root}
    ${ParentDirectory}
)


target_link_libraries(YTE
  PUBLIC
    assimp
    Bullet2FileLoader
    Bullet3Collision
    Bullet3Common
    Bullet3Dynamics
    Bullet3Geometry
    Bullet3OpenCL_clew
    BulletCollision
    BulletDynamics
    BulletInverseDynamics
    BulletSoftBody
    LinearMath
    crunch
    glslang
    fmt    
    LuaJIT
    STB
    VkHLF
    XInput9_1_0.lib
) 

set_target_properties(YTE
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

YTE_Target_Precompiled_Headers(YTE
                               PUBLIC 
                                 <unordered_map>
                                 <vector>
                               PRIVATE
                                 YTE/Meta/Type.hpp)

YTE_Source_Group(YTE_Root YTE)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_compile_options(YTE PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
endif()

target_compile_definitions(YTE PUBLIC GLM_FORCE_CXX14 GLM_FORCE_SIZE_T_LENGTH GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_ENGINE_CONFIG="../../../../../Assets/Bin/Config">)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_ENGINE_CONFIG="./Config">)

