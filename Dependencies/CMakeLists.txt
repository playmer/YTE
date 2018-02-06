add_subdirectory(crunch)
add_subdirectory(fmt)
add_subdirectory(LuaJIT)
add_subdirectory(stb)

# Turn off/configure various parts of assimp.
option(BUILD_SHARED_LIBS "Build package with shared libraries." OFF)
option(ASSIMP_BUILD_ASSIMP_TOOLS "If the supplementary tools for Assimp are built in addition to the library." OFF)
option(ASSIMP_BUILD_TESTS "If the test suite for Assimp is built in addition to the library." OFF)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/VkHLF/3rdparty)

add_subdirectory(VkHLF) 

# Get WWise
FindWWise(WWise_SoundEngine)

set_target_properties(assimp                              PROPERTIES FOLDER "Dependencies/assimp") 
set_target_properties(IrrXML                              PROPERTIES FOLDER "Dependencies/assimp") 
set_target_properties(uninstall                           PROPERTIES FOLDER "Dependencies/assimp") 
set_target_properties(UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTIES FOLDER "Dependencies/assimp") 
set_target_properties(zlibstatic                          PROPERTIES FOLDER "Dependencies/assimp") 
set_target_properties(zlib                                PROPERTIES FOLDER "Dependencies/assimp") 
set_target_properties(Bullet2FileLoader                   PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(Bullet3Collision                    PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(Bullet3Common                       PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(Bullet3Dynamics                     PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(Bullet3Geometry                     PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(Bullet3OpenCL_clew                  PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(BulletCollision                     PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(BulletDynamics                      PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(BulletInverseDynamics               PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(BulletSoftBody                      PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(LinearMath                          PROPERTIES FOLDER "Dependencies/bullet3") 
set_target_properties(crunch                              PROPERTIES FOLDER "Dependencies") 
set_target_properties(fmt                                 PROPERTIES FOLDER "Dependencies") 
set_target_properties(LuaJIT                              PROPERTIES FOLDER "Dependencies") 
set_target_properties(STB                                 PROPERTIES FOLDER "Dependencies")
set_target_properties(VkHLF                               PROPERTIES FOLDER "Dependencies/VkHLF") 
set_target_properties(glslang                             PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(OGLCompiler                         PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(OSDependent                         PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(SPIRV                               PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(SPVRemapper                         PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
#set_target_properties(HLSL                                PROPERTIES FOLDER "Dependencies/VkHLF/hlsl")
set_target_properties(WWise_SoundEngine                   PROPERTIES FOLDER "Dependencies/YTE/Dependencies")


# Turn the warnings off on our dependencies.
target_compile_options(assimp                PRIVATE /W0)
target_compile_options(Bullet2FileLoader     PRIVATE /W0) 
target_compile_options(Bullet3Collision      PRIVATE /W0) 
target_compile_options(Bullet3Common         PRIVATE /W0) 
target_compile_options(Bullet3Dynamics       PRIVATE /W0) 
target_compile_options(Bullet3Geometry       PRIVATE /W0) 
target_compile_options(Bullet3OpenCL_clew    PRIVATE /W0) 
target_compile_options(BulletCollision       PRIVATE /W0) 
target_compile_options(BulletDynamics        PRIVATE /W0) 
target_compile_options(BulletInverseDynamics PRIVATE /W0) 
target_compile_options(BulletSoftBody        PRIVATE /W0) 
target_compile_options(LinearMath            PRIVATE /W0) 
target_compile_options(crunch                PRIVATE /W0)
target_compile_options(fmt                   PRIVATE /W0)
target_compile_options(LuaJIT                PRIVATE /W0)
target_compile_options(STB                   PRIVATE /W0)
target_compile_options(VkHLF                 PRIVATE /W0)
target_compile_options(glslang               PRIVATE /W0)
target_compile_options(OGLCompiler           PRIVATE /W0)
target_compile_options(OSDependent           PRIVATE /W0)
target_compile_options(SPIRV                 PRIVATE /W0)
target_compile_options(SPVRemapper           PRIVATE /W0)
#target_compile_options(HLSL                  PRIVATE /W0)

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
                      crunch
                      fmt
                      LuaJIT
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
