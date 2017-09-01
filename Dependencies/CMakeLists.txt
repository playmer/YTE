Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")

Add_Subdirectory(assimp)
Add_Subdirectory(Bullet)
Add_Subdirectory(crunch)
Add_Subdirectory(LuaJIT)
Add_Subdirectory(stb)
Add_Subdirectory(vkel)

Set(BUILD_VKCPP_SAMPLES OFF CACHE BOOL "Build the VkCpp based samples" )
option(BUILD_VKCPP_GLFW "Build GLFW" OFF)

Set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF)

Set(LLVM_USE_CRT_DEBUG "MDd")
Set(LLVM_USE_CRT_MINSIZEREL "MD")
Set(LLVM_USE_CRT_RELEASE "MD")
Set(LLVM_USE_CRT_RELWITHDEBINFO "MD")

Include_Directories(${CMAKE_CURRENT_SOURCE_DIR}/VkHLF/3rdparty)

Add_Subdirectory(VkHLF) 

Set_Target_Properties(assimp      PROPERTIES FOLDER "Dependencies") 
Set_Target_Properties(Bullet      PROPERTIES FOLDER "Dependencies") 
Set_Target_Properties(crunch      PROPERTIES FOLDER "Dependencies") 
Set_Target_Properties(STB_Image   PROPERTIES FOLDER "Dependencies") 
Set_Target_Properties(LuaJIT      PROPERTIES FOLDER "Dependencies") 
Set_Target_Properties(vkel        PROPERTIES FOLDER "Dependencies") 
Set_Target_Properties(VkHLF       PROPERTIES FOLDER "Dependencies/VkHLF") 
Set_Target_Properties(glslang     PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
Set_Target_Properties(OGLCompiler PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
Set_Target_Properties(OSDependent PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
Set_Target_Properties(SPIRV       PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
Set_Target_Properties(SPVRemapper PROPERTIES FOLDER "Dependencies/VkHLF/glslang") 
Set_Target_Properties(HLSL        PROPERTIES FOLDER "Dependencies/VkHLF/hlsl")

Set_Target_Properties(assimp
                      Bullet
                      crunch
                      LuaJIT
                      STB_Image
                      vkel
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
