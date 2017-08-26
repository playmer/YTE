################################################################################
# Project: YTE Engine
# Legal  : All content (C) 2017 DigiPen (USA) Corporation, all rights reserved. 
# Author : Joshua T. Fisher (2017)
################################################################################
Macro(FindWWise StaticLibrariesRelease StaticLibrariesDebug IncludeDirectory)
  Set(WWisePath $ENV{WWISESDK})
 
  Message(STATUS WWise)
  File(GLOB_RECURSE 
       StaticLibraryReleaseFiles 
       "${WWisePath}/x64_vc150/Release/lib"
       "${WWisePath}/x64_vc150/Release/lib/*.lib")
        
  File(GLOB_RECURSE 
       StaticLibraryDebugFiles 
       "${WWisePath}/x64_vc150/Debug/lib"
       "${WWisePath}/x64_vc150/Debug/lib/*.lib")

  Set(${StaticLibrariesRelease} ${StaticLibraryReleaseFiles})
  Set(${StaticLibrariesDebug} ${StaticLibraryDebugFiles})
  Set(${IncludeDirectory} ${WWisePath}/include)
EndMacro()
