add_subdirectory(crunch)
add_subdirectory(fmt)
add_subdirectory(kiss_fft)
#add_subdirectory(LuaJIT)
add_subdirectory(stb)
add_subdirectory(imgui)


# Turn off/configure various parts of Assimp.
option(ASSIMP_BUILD_ASSIMP_TOOLS
       "If the supplementary tools for Assimp are built in addition to the library."
       OFF)

option(ASSIMP_BUILD_TESTS
       "If the test suite for Assimp is built in addition to the library."
       OFF)

add_subdirectory(assimp)

get_target_property(assimpBinaryDir assimp BINARY_DIR)

target_include_directories(assimp BEFORE PUBLIC 
                            ${assimpBinaryDir}/../include)

# TODO: Is this really needed? Doesn't Assimp do this itself?
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_compile_definitions(assimp PUBLIC _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
  target_compile_options(assimp PRIVATE /bigobj)
endif()

# Turn off/configure various parts of bullet3.
option(BUILD_PYBULLET "Set when you want to build pybullet (Python bindings for Bullet)" OFF)
option(BUILD_UNIT_TESTS "Build Unit Tests"	OFF)
option(USE_GRAPHICAL_BENCHMARK "Use Graphical Benchmark" OFF)
option(BUILD_BULLET2_DEMOS "Set when you want to build the Bullet 2 demos" OFF)
option(BUILD_EXTRAS "Set when you want to build the extras" OFF)
option(INSTALL_LIBS "Set when you want to install libraries" OFF)
option(BUILD_CPU_DEMOS "Build original Bullet CPU examples" OFF)
option(BUILD_ENET "Set when you want to build apps with enet UDP networking support" OFF)
option(BUILD_CLSOCKET "Set when you want to build apps with enet TCP networking support" OFF)
option(USE_MSVC_RUNTIME_LIBRARY_DLL "Use MSVC Runtime Library DLL (/MD or /MDd)" ON)

add_subdirectory(bullet3)

# Turn off/configure various parts of VkHLF.
set(BUILD_VKCPP_SAMPLES OFF CACHE BOOL "Build the VkCpp based samples" )
option(BUILD_VKCPP_GLFW "Build GLFW" OFF)
option(ENABLE_TESTING "Allow building of tests and looking for gmock" OFF)
option(ENABLE_HLSL "Enables HLSL input support" OFF)
option(BUILD_TESTING "Enables testing" OFF)

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
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  set(disableWarning /W0)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Clang OR
        ${CMAKE_CXX_COMPILER_ID} STREQUAL AppleClang)
  # Also, this is an easy place to force color output for me in this moment
  # Future maintainers of this file may judge me, but I will do this thing.
  set(disableWarning -Wno-everything -fcolor-diagnostics)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  set(disableWarning -w)
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
  set(disableWarning -w0)
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
target_compile_options(imgui                 PRIVATE ${disableWarning})
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
