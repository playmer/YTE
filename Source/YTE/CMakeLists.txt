Set(YTE_Source)

Get_Filename_Component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
Add_Subdirectory(Core)
Add_Subdirectory(Graphics)
Add_Subdirectory(Physics)
Add_Subdirectory(Meta)
Add_Subdirectory(Platform)
Add_Subdirectory(StandardLibrary)
Add_Subdirectory(Utilities)
Add_Subdirectory(WWise)

Include_Directories(${ParentDirectory}
                    ${Dependencies_Root}/assimp/include)

Add_Library(YTE ${YTE_Source})

YTE_Source_Group(YTE_Root YTE_Source)

Target_Link_Libraries(YTE assimp) 
Target_Link_Libraries(YTE Bullet) 
Target_Link_Libraries(YTE crunch)
Target_Link_Libraries(YTE glslang) 
Target_Link_Libraries(YTE LuaJIT)
Target_Link_Libraries(YTE STB_Image)
Target_Link_Libraries(YTE vkel)
Target_Link_Libraries(YTE VkHLF)
Target_Link_Libraries(YTE "$ENV{VK_SDK_PATH}/Lib/vulkan-1.lib" ) 

Set_Target_Properties(YTE
                      PROPERTIES
                      ARCHIVE_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      LIBRARY_OUTPUT_DIRECTORY ${YTE_Library_Dir}
                      RUNTIME_OUTPUT_DIRECTORY ${YTE_Binary_Dir})

ForEach(library ${StaticLibrariesRelease})
  Target_Link_Libraries(YTE optimized ${library})
EndForEach()

ForEach(library ${StaticLibrariesDebug})
  Target_Link_Libraries(YTE debug ${library})
EndForEach()

Target_Link_Libraries(YTE ${StaticLibraries})
