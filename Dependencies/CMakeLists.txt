add_subdirectory(assimp)
add_subdirectory(Bullet)
add_subdirectory(crunch)
add_subdirectory(fmt)
add_subdirectory(LuaJIT)
add_subdirectory(stb)

# Turn off/configure various parts of VkHLF.
set(BUILD_VKCPP_SAMPLES OFF CACHE BOOL "Build the VkCpp based samples" )
option(BUILD_VKCPP_GLFW "Build GLFW" OFF)
option(ENABLE_TESTING "Allow building of tests and looking for gmock" OFF)

set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)

set(LLVM_USE_CRT_DEBUG "MDd")
set(LLVM_USE_CRT_MINSIZEREL "MD")
set(LLVM_USE_CRT_RELEASE "MD")
set(LLVM_USE_CRT_RELWITHDEBINFO "MD")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/VkHLF/3rdparty)

add_subdirectory(VkHLF) 

set_target_properties(assimp      PROPERTIES FOLDER "Dependencies") 
set_target_properties(Bullet      PROPERTIES FOLDER "Dependencies") 
set_target_properties(crunch      PROPERTIES FOLDER "Dependencies") 
set_target_properties(fmt         PROPERTIES FOLDER "Dependencies") 
set_target_properties(LuaJIT      PROPERTIES FOLDER "Dependencies") 
set_target_properties(STB_Image   PROPERTIES FOLDER "Dependencies") 
set_target_properties(VkHLF       PROPERTIES FOLDER "Dependencies/VkHLF") 
set_target_properties(glslang     PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(OGLCompiler PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(OSDependent PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(SPIRV       PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(SPVRemapper PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
set_target_properties(HLSL        PROPERTIES FOLDER "Dependencies/VkHLF/hlsl")

# Turn the warnings off on our dependencies.
target_compile_options(assimp      PRIVATE /W0)
target_compile_options(Bullet      PRIVATE /W0)
target_compile_options(crunch      PRIVATE /W0)
target_compile_options(fmt         PRIVATE /W0)
target_compile_options(LuaJIT      PRIVATE /W0)
target_compile_options(STB_Image   PRIVATE /W0)
target_compile_options(VkHLF       PRIVATE /W0)
target_compile_options(glslang     PRIVATE /W0)
target_compile_options(OGLCompiler PRIVATE /W0)
target_compile_options(OSDependent PRIVATE /W0)
target_compile_options(SPIRV       PRIVATE /W0)
target_compile_options(SPVRemapper PRIVATE /W0)
target_compile_options(HLSL        PRIVATE /W0)

set_target_properties(assimp
                      Bullet
                      crunch
                      fmt
                      LuaJIT
                      STB_Image
                      VkHLF
                      glslang
                      OGLCompiler
                      OSDependent
                      SPIRV
                      SPVRemapper
                      HLSL
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})
