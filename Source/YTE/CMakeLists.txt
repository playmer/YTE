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
#include(WWise/CMakeLists.txt)

target_include_directories(YTE 
  PUBLIC
    ${Dependencies_Root}
    ${Dependencies_Root}/assimp/include
    ${Dependencies_Root}/assimp/contrib/rapidjson/include
    ${Dependencies_Root}/basis_universal/src
    ${Dependencies_Root}/bullet3/src
    #${Dependencies_Root}/glm
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
  message(used: ${EasyProfilerBeingUsed})
endif()

if(EasyProfilerBeingUsed)
  target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_CAN_PROFILE=1>)
  target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_CAN_PROFILE=0>)
else()
  target_compile_definitions(YTE PUBLIC YTE_CAN_PROFILE=0)
endif()

target_link_libraries(YTE
  PUBLIC
    basisu_transcoder
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
    glm
    imgui
    VkHLF
  PRIVATE
    assimp
    glslang
    KissFFT
    #LuaJIT
    STB
) 

target_sources(YTE INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/YTE.natvis
)

set_target_properties(YTE
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

YTE_Source_Group(YTE_Root YTE)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_compile_definitions(YTE PUBLIC _ENABLE_EXTENDED_ALIGNED_STORAGE)
  target_compile_options(YTE PRIVATE 
    PUBLIC
      -permissive- -std:c++17 -MP
    PRIVATE
      -WX- -W4
  )
else()
  target_compile_options(YTE PRIVATE -std=c++17)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  target_compile_options(YTE PRIVATE 
    PUBLIC
      /std:c++17
  )
endif()

target_compile_definitions(YTE PUBLIC ${YTE_Windows})
target_compile_definitions(YTE PUBLIC ${YTE_Linux})
target_compile_definitions(YTE PUBLIC ${YTE_Darwin})
target_compile_definitions(YTE PUBLIC ${YTE_Android})
target_compile_definitions(YTE PUBLIC ${YTE_x64})
target_compile_definitions(YTE PUBLIC ${YTE_x86})

target_compile_definitions(YTE PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_ENGINE_CONFIG="../../../../../Assets/Bin/Config">)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_ENGINE_CONFIG="./Config">)

# Set the defines needed for YTE::CompilerOptions
target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_DEBUG=1>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_DEBUG=0>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_RELEASE=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_RELEASE=1>)

# Set the defines needed for YTE::CompilerConfiguration
target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:DEBUG>>:YTE_CONFIG_DEBUG=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:DEBUG>:YTE_CONFIG_DEBUG=1>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:MINSIZEREL>>:YTE_CONFIG_MINSIZEREL=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:MINSIZEREL>:YTE_CONFIG_MINSIZEREL=1>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:RELWITHDEBINFO>>:YTE_CONFIG_RELWITHDEBINFO=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:RELWITHDEBINFO>:YTE_CONFIG_RELWITHDEBINFO=1>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:RELEASE>>:YTE_CONFIG_RELEASE=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:RELEASE>:YTE_CONFIG_RELEASE=1>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_CONFIG_PUBLISH=0>)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_CONFIG_PUBLISH=1>)

# Set the define needed for proper DLL import/export.
target_compile_definitions(YTE PRIVATE YTE_Internal=1)
