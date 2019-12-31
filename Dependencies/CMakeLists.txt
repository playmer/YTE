

add_subdirectory(crunch)
add_subdirectory(fmt)
add_subdirectory(kiss_fft)
#add_subdirectory(LuaJIT)
add_subdirectory(stb)
add_subdirectory(imgui)
add_subdirectory(optick)

target_compile_definitions(OptickCore PUBLIC OPTICK_ENABLE_COMPRESSION=0)


##########################################################################
# Turn off/configure various parts of SDL
option(SDL_SHARED_ENABLED_BY_DEFAULT "We need the SDL dll" ON)

# See the following for info
# https://bugzilla.libsdl.org/show_bug.cgi?id=4759
# https://developercommunity.visualstudio.com/content/problem/711317/unexpected-implicit-memset-in-release-optimized-bu.html
#
# Note that despite these being resolved at time of this commit, this issue is not actually resolved,
option(LIBC "A bug in I guess MSVC causes us to need to use LIBC with SDL." ON)
add_subdirectory(SDL)

##########################################################################
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


##########################################################################
# Turn off/configure various parts of toolwindowmanager.
option (TWM_BUILD_EXAMPLE "Build example" OFF)

add_subdirectory(toolwindowmanager)

##########################################################################
# Turn off/configure various parts of basis_universal.
option(BUILD_SHARED_LIBS "Build Shared Libraries" OFF)
option(BUILD_TESTS "Build test applications" OFF)
option(BASISU_BUILD_TOOL "Build the tool" OFF)
option(BASISU_INSTALL "Installs basis universal" OFF)
option(BASISU_INSTALL_TOOL "Installs the tool" OFF)
option(BASISU_OPENMP "Allows OpenMP support" OFF)

add_subdirectory(basis_universal)

##########################################################################
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

##########################################################################
# Turn off/configure various parts of glm.
option(GLM_TEST_ENABLE "Build unit tests" OFF)
option(GLM_INSTALL_ENABLE "Allow installing of the library" OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)
option(BUILD_STATIC_LIBS "Build static library" OFF)

add_subdirectory(glm)

target_compile_definitions(glm 
  INTERFACE 
    GLM_FORCE_CXX17 
    GLM_FORCE_SIZE_T_LENGTH 
    GLM_FORCE_DEPTH_ZERO_TO_ONE 
    GLM_FORCE_CTOR_INIT 
    GLM_ENABLE_EXPERIMENTAL 
    GLM_FORCE_SILENT_WARNINGS
)

target_sources(glm INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/glm/util/glm.natvis
)

#target_include_directories(glm INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/glm)

##########################################################################
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

target_compile_definitions(glslang 
PUBLIC 
  AMD_EXTENSIONS
  NV_EXTENSIONS
)

##########################################################################
# Get WWise
#FindWWise(WWise_SoundEngine)

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
target_compile_options(basisu_encoder        PRIVATE ${disableWarning})
target_compile_options(basisu_transcoder     PRIVATE ${disableWarning})
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
target_compile_options(SDL2                  PRIVATE ${disableWarning})
target_compile_options(SDL2main              PRIVATE ${disableWarning})
target_compile_options(SDL2-static           PRIVATE ${disableWarning})

set_target_properties(SPVRemapper PROPERTIES FOLDER "Ignore")

set_target_properties(
  assimp
  basisu_encoder
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
  imgui
  KissFFT
  #LuaJIT
  STB
  VkHLF
  glslang
  OGLCompiler
  OSDependent
  SDL2
  SDL2main
  SDL2-static
  SPIRV
  SPVRemapper
  #HLSL
PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
  LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
  RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir}
)
