add_library(YTE SHARED "")

get_filename_component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

include(Core/CMakeLists.txt)
include(Graphics/CMakeLists.txt)
include(Physics/CMakeLists.txt)
include(Math/CMakeLists.txt)
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

# Find the EasyProfiler library
set(EasyProfilerSDKPath $ENV{EasyProfilerSDK})
if(EasyProfilerSDKPath)
  set(CMAKE_PREFIX_PATH ${EasyProfilerSDKPath}/lib/cmake/easy_profiler/)
  find_package(easy_profiler REQUIRED)
  target_link_libraries(YTE PUBLIC easy_profiler)
  target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_CAN_PROFILE=1>)
  target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_CAN_PROFILE=0>)

  set(EasyProfilerBeingUsed TRUE)
endif()

if(EasyProfilerBeingUsed)
  target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_CAN_PROFILE=1>)
  target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_CAN_PROFILE=0>)
else()
  target_compile_definitions(YTE PUBLIC YTE_CAN_PROFILE=0)
endif()

target_link_libraries(YTE
  PUBLIC
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
    crn
    fmt
    imgui
    VkHLF
  PRIVATE
    assimp
    glslang
    KissFFT
    #LuaJIT
    STB
) 



set_target_properties(YTE
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

YTE_Source_Group(YTE_Root YTE)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_compile_options(YTE PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
endif()

target_compile_definitions(YTE PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_definitions(YTE PUBLIC GLM_FORCE_CXX14 GLM_FORCE_SIZE_T_LENGTH GLM_FORCE_DEPTH_ZERO_TO_ONE)
target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_ENGINE_CONFIG="../../../../../Assets/Bin/Config">)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_ENGINE_CONFIG="./Config">)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_DEBUG=1>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_DEBUG=0>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_RELEASE=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_RELEASE=1>)

target_compile_definitions(YTE PRIVATE YTE_Internal=1)