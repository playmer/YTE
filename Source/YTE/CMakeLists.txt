add_library(YTE SHARED)

get_filename_component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

add_subdirectory(Core)
add_subdirectory(Graphics)
add_subdirectory(Physics)
add_subdirectory(Math)
add_subdirectory(Meta)
add_subdirectory(Platform)
add_subdirectory(StandardLibrary)
add_subdirectory(Utilities)

target_include_directories(YTE 
  PUBLIC
    ${Dependencies_Root}
    ${Dependencies_Root}/assimp/include
    ${Dependencies_Root}/assimp/contrib/rapidjson/include
    ${Dependencies_Root}/basis_universal/src
    ${Dependencies_Root}/bullet3/src
    ${Source_Root}
    ${ParentDirectory}
)

option(YTE_PROFILER_ENABLED "Allows profiler to be used." ON)

if(YTE_PROFILER_ENABLED)
  target_link_libraries(YTE PUBLIC OptickCore)
  target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_CAN_PROFILE=1>)
  target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_CAN_PROFILE=0>)
else()
  target_compile_definitions(YTE PUBLIC YTE_CAN_PROFILE=0)
  target_compile_definitions(YTE PUBLIC USE_OPTICK=0)
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
    fmt
    glm
    imgui
    VkHLF
  PRIVATE
    assimp
    glslang
    KissFFT
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
      -permissive- 
      -std:c++17 
      -MP
      #-wd4127 # Abseil, expression is constant
      #-wd4324 # Abseil, structure was padded for alignment
    PRIVATE
      -WX- 
      -W4
  )
else()
  target_compile_options(YTE PRIVATE -std=c++17)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
  target_compile_options(YTE PRIVATE 
    PUBLIC
      -std=c++17
      -W0
      -Wno-everything
      #-Wno-inconsistent-missing-override
      #-Wno-unsequenced
  )
endif()

target_compile_definitions(YTE PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_definitions(YTE PUBLIC $<$<NOT:$<CONFIG:PUBLISH>>:YTE_ENGINE_CONFIG="../../../../../Assets/Bin/Config">)
target_compile_definitions(YTE PUBLIC $<$<CONFIG:PUBLISH>:YTE_ENGINE_CONFIG="./Config">)

# Set the defines needed for YTE::CompilerOptions
target_compile_definitions(YTE PUBLIC $<$<NOT:$<OR:$<CONFIG:PUBLISH>,$<CONFIG:RELEASE>>>:YTE_DEBUG=1>)
target_compile_definitions(YTE PUBLIC $<$<OR:$<CONFIG:PUBLISH>,$<CONFIG:RELEASE>>:YTE_DEBUG=0>)

target_compile_definitions(YTE PUBLIC $<$<NOT:$<OR:$<CONFIG:PUBLISH>,$<CONFIG:RELEASE>>>:YTE_RELEASE=0>)
target_compile_definitions(YTE PUBLIC $<$<OR:$<CONFIG:PUBLISH>,$<CONFIG:RELEASE>>:YTE_RELEASE=1>)

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
