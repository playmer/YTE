add_subdirectory(crunch)
add_subdirectory(fmt)
add_subdirectory(kiss_fft)
#add_subdirectory(LuaJIT)
add_subdirectory(stb)
add_subdirectory(imgui)

add_subdirectory(assimp)

get_target_property(assimpBinaryDir assimp BINARY_DIR)

target_include_directories(assimp PUBLIC 
                            ${assimpBinaryDir}/../include 
                            ${assimpBinaryDir}../)

target_compile_definitions(assimp PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
target_compile_options(assimp PRIVATE /bigobj)

# Turn off/configure various parts of bullet3.
OPTION(BUILD_PYBULLET "Set when you want to build pybullet (Python bindings for Bullet)" OFF)
OPTION(BUILD_UNIT_TESTS "Build Unit Tests"	OFF)
OPTION(USE_GRAPHICAL_BENCHMARK "Use Graphical Benchmark" OFF)
OPTION(BUILD_BULLET2_DEMOS "Set when you want to build the Bullet 2 demos" OFF)
OPTION(BUILD_EXTRAS "Set when you want to build the extras" OFF)
OPTION(INSTALL_LIBS "Set when you want to install libraries" OFF)
OPTION(BUILD_CPU_DEMOS "Build original Bullet CPU examples" OFF)
OPTION(BUILD_ENET "Set when you want to build apps with enet UDP networking support" OFF)
OPTION(BUILD_CLSOCKET "Set when you want to build apps with enet TCP networking support" OFF)
OPTION(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)

add_subdirectory(bullet3)

# Turn off/configure various parts of VkHLF.
set(BUILD_VKCPP_SAMPLES OFF CACHE BOOL "Build the VkCpp based samples" )
option(BUILD_VKCPP_GLFW "Build GLFW" OFF)
option(ENABLE_TESTING "Allow building of tests and looking for gmock" OFF)
option(ENABLE_HLSL "Enables HLSL input support" OFF)
option(BUILD_TESTING "Enables testing" OFF)

set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)

set(LLVM_USE_CRT_DEBUG "MDd")
set(LLVM_USE_CRT_MINSIZEREL "MD")
set(LLVM_USE_CRT_RELEASE "MD")
set(LLVM_USE_CRT_RELWITHDEBINFO "MD")

set(SKIP_GLSLANG_INSTALL ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/VkHLF/3rdparty)

add_subdirectory(VkHLF) 

# Get WWise
FindWWise(WWise_SoundEngine)

target_compile_options(crn                   PRIVATE ${disableWarning})
target_compile_options(fmt                   PRIVATE ${disableWarning})
# Turn the warnings off on our dependencies.
if (CMAKE_SYSTEM_NAME STREQUAL Windows)
  if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(disableWarning /W0)
  elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang OR
          ${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang OR
          ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
    set(disableWarning -w)
  elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
    set(disableWarning -w0)
  endif()
endif()

target_compile_options(assimp                PRIVATE ${disableWarning})
target_compile_options(Bullet2FileLoader     PRIVATE ${disableWarning}) 
target_compile_options(Bullet3Collision      PRIVATE ${disableWarning}) 
target_compile_options(Bullet3Common         PRIVATE ${disableWarning}) 
target_compile_options(Bullet3Dynamics       PRIVATE ${disableWarning}) 
target_compile_options(Bullet3Geometry       PRIVATE ${disableWarning}) 
target_compile_options(Bullet3OpenCL_clew    PRIVATE ${disableWarning}) 
target_compile_options(BulletCollision       PRIVATE ${disableWarning}) 
target_compile_options(BulletDynamics        PRIVATE ${disableWarning}) 
target_compile_options(BulletInverseDynamics PRIVATE ${disableWarning}) 
target_compile_options(BulletSoftBody        PRIVATE ${disableWarning}) 
target_compile_options(LinearMath            PRIVATE ${disableWarning}) 
target_compile_options(crn                   PRIVATE ${disableWarning}) 
target_compile_options(imgui                   PRIVATE ${disableWarning})
target_compile_options(fmt                   PRIVATE ${disableWarning})
target_compile_options(STB                   PRIVATE ${disableWarning})
target_compile_options(VkHLF                 PRIVATE ${disableWarning})
target_compile_options(glslang               PRIVATE ${disableWarning})
target_compile_options(OGLCompiler           PRIVATE ${disableWarning})
target_compile_options(OSDependent           PRIVATE ${disableWarning})
target_compile_options(SPIRV                 PRIVATE ${disableWarning})
target_compile_options(SPVRemapper           PRIVATE ${disableWarning})
target_compile_options(KissFFT               PRIVATE ${disableWarning})

set_target_properties(SPVRemapper PROPERTIES FOLDER "Ignore")

set_target_properties(assimp
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
                      KissFFT
                      #LuaJIT
                      STB
                      VkHLF
                      glslang
                      OGLCompiler
                      OSDependent
                      SPIRV
                      SPVRemapper
                      #HLSL
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})
