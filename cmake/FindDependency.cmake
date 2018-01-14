################################################################################
# Project: YTE Engine
# Legal  : All content (C) 2017 DigiPen (USA) Corporation, all rights reserved. 
# Author : Joshua T. Fisher (2017)
################################################################################
Function(FindWWise aTarget)
  Get_Filename_Component(WWisePath $ENV{WWISESDK} ABSOLUTE) 
 
  File(GLOB_RECURSE 
       CommonFiles 
       "${WWisePath}/samples/SoundEngine/Common/*.inl"
       "${WWisePath}/samples/SoundEngine/Common/*.cpp"
       "${WWisePath}/samples/SoundEngine/Common/*.h"
       "${WWisePath}/samples/SoundEngine/Common/*.hpp")

  If (${WIN32})
    Set(PlatformIncludeDirectory ${WWisePath}/samples/SoundEngine/Win32/)
    
    File(GLOB_RECURSE 
         PlatformFiles 
         "${WWisePath}/samples/SoundEngine/Win32/*.inl"
         "${WWisePath}/samples/SoundEngine/Win32/*.cpp"
         "${WWisePath}/samples/SoundEngine/Win32/*.h"
         "${WWisePath}/samples/SoundEngine/Win32/*.hpp")
  Else()
    Set(PlatformIncludeDirectory ${WWisePath}/samples/SoundEngine/POSIX/)

    File(GLOB_RECURSE 
         PlatformFiles 
         "${WWisePath}/samples/SoundEngine/POSIX/*.inl"
         "${WWisePath}/samples/SoundEngine/POSIX/*.cpp"
         "${WWisePath}/samples/SoundEngine/POSIX/*.h"
         "${WWisePath}/samples/SoundEngine/POSIX/*.hpp")
  EndIf()
  
  Set(WWiseTargetFiles ${CommonFiles} ${PlatformFiles})

  Add_Library(${aTarget} ${WWiseTargetFiles})

  Target_Compile_Definitions(${aTarget} PRIVATE UNICODE)
  Target_Include_Directories(${aTarget} PRIVATE ${PlatformIncludeDirectory})
  Target_Include_Directories(${aTarget} PUBLIC ${WWisePath}/include)
  Target_Include_Directories(${aTarget} PUBLIC ${WWisePath}/samples)

  File(GLOB_RECURSE 
       staticLibraryRelease
       "${WWisePath}/x64_vc150/Release/lib"
       "${WWisePath}/x64_vc150/Release/lib/*.lib")
        
  File(GLOB_RECURSE 
       staticLibraryDebug
       "${WWisePath}/x64_vc150/Debug/lib"
       "${WWisePath}/x64_vc150/Debug/lib/*.lib")

  ForEach(library ${staticLibraryRelease})
    Target_Link_Libraries(${aTarget} optimized ${library})
  EndForEach()
    
  ForEach(library ${staticLibraryDebug})
    Target_Link_Libraries(${aTarget} debug ${library})
  EndForEach()
EndFunction()
