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
else()
  target_compile_options(YTE PRIVATE -std=c++17)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  target_compile_options(YTE PRIVATE 
    PUBLIC
      /std:c++17
  )
endif()

# Set these to false first.
set(YTE_Windows "YTE_Windows=0")
set(YTE_Linux "YTE_Linux=0")
set(YTE_Darwin "YTE_Darwin=0")
set(YTE_Android "YTE_Android=0")
set(YTE_x64 "YTE_x64=0")
set(YTE_x86 "YTE_x86=0")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(YTE_Windows "YTE_Windows=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(YTE_Linux "YTE_Linux=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(YTE_Darwin "YTE_Darwin=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(YTE_Android "YTE_Android=1")
endif()

# This might fail on Apple, but also maybe not? Not sure they have any 
# x86 bit targets left
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(YTE_x86 YTE_x86=1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(YTE_x64 YTE_x64=1)
endif()


target_compile_definitions(YTE PUBLIC ${YTE_Windows})
target_compile_definitions(YTE PUBLIC ${YTE_Linux})
target_compile_definitions(YTE PUBLIC ${YTE_Darwin})
target_compile_definitions(YTE PUBLIC ${YTE_Android})
target_compile_definitions(YTE PUBLIC ${YTE_x64})
target_compile_definitions(YTE PUBLIC ${YTE_x86})

target_compile_definitions(YTE PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_definitions(YTE PUBLIC GLM_FORCE_CXX14 GLM_FORCE_SIZE_T_LENGTH GLM_FORCE_DEPTH_ZERO_TO_ONE)
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
