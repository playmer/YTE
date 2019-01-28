################################################################################
# Project: YTE Engine
# Legal  : All content (C) 2017 DigiPen (USA) Corporation, all rights reserved. 
# Author : Joshua T. Fisher (2017)
################################################################################
################################################################################
# Find WWise
################################################################################
Function(FindWWise aTarget)
  # Find the WWise library
  set(WWisePath $ENV{WWISESDK})
  if(NOT WWisePath)
    string(CONCAT errorString
       "Please define the environment variable `WWISESDK` with the path to WWise "
       "2017.1.0.6302 - i.e. export WWISESDK = "
       "C:/Program Files (x86)/Audiokinetic/Wwise 2017.2.4.6590/SDK")
    message(FATAL_ERROR ${errorString})
  endif()

  get_filename_component(WWisePath $ENV{WWISESDK} ABSOLUTE)
 
  file(GLOB_RECURSE 
       CommonFiles 
       "${WWisePath}/samples/SoundEngine/Common/*.inl"
       "${WWisePath}/samples/SoundEngine/Common/*.cpp"
       "${WWisePath}/samples/SoundEngine/Common/*.h"
       "${WWisePath}/samples/SoundEngine/Common/*.hpp")

  if (CMAKE_SYSTEM_NAME STREQUAL Windows)
    set(PlatformIncludeDirectory ${WWisePath}/samples/SoundEngine/Win32/)
    set(PlatformLibraryDirectory x64_vc150)
    
    file(GLOB_RECURSE 
         PlatformFiles 
         "${WWisePath}/samples/SoundEngine/Win32/*.inl"
         "${WWisePath}/samples/SoundEngine/Win32/*.cpp"
         "${WWisePath}/samples/SoundEngine/Win32/*.h"
         "${WWisePath}/samples/SoundEngine/Win32/*.hpp")
  else()
    set(PlatformIncludeDirectory ${WWisePath}/samples/SoundEngine/POSIX/)
    set(PlatformLibraryDirectory Linux_x64)

    file(GLOB_RECURSE 
         PlatformFiles 
         "${WWisePath}/samples/SoundEngine/POSIX/*.inl"
         "${WWisePath}/samples/SoundEngine/POSIX/*.cpp"
         "${WWisePath}/samples/SoundEngine/POSIX/*.h"
         "${WWisePath}/samples/SoundEngine/POSIX/*.hpp")
  endif()
  
  set(WWiseTargetFiles ${CommonFiles} ${PlatformFiles})

  add_library(${aTarget} ${WWiseTargetFiles})

  target_compile_definitions(${aTarget} PRIVATE UNICODE)
  target_include_directories(${aTarget} PRIVATE ${PlatformIncludeDirectory})
  target_include_directories(${aTarget} PUBLIC ${WWisePath}/include)
  target_include_directories(${aTarget} PUBLIC ${WWisePath}/samples)

  file(GLOB_RECURSE 
       staticLibraryRelease
       "${WWisePath}/${PlatformLibraryDirectory}/Release/lib"
       "${WWisePath}/${PlatformLibraryDirectory}/Release/lib/*.lib")
        
  file(GLOB_RECURSE 
       staticLibraryProfile
       "${WWisePath}/${PlatformLibraryDirectory}/Profile/lib"
       "${WWisePath}/${PlatformLibraryDirectory}/Profile/lib/*.lib")
        
  file(GLOB_RECURSE 
       staticLibraryDebug
       "${WWisePath}/${PlatformLibraryDirectory}/Debug/lib"
       "${WWisePath}/${PlatformLibraryDirectory}/Debug/lib/*.lib")

  foreach(library ${staticLibraryRelease})
    get_filename_component(libraryName ${library} NAME_WE)

    if (NOT ${libraryName} MATCHES "^AkMotionSink$")
      target_link_libraries(${aTarget} $<$<CONFIG:PUBLISH>:${library}>)
      target_link_libraries(${aTarget} $<$<CONFIG:RELEASE>:${library}>)
    endif()
  endforeach()

  foreach(library ${staticLibraryProfile})
    get_filename_component(libraryName ${library} NAME_WE)

    if (NOT ${libraryName} MATCHES "^AkMotionSink$")
      target_link_libraries(${aTarget} $<$<CONFIG:RELWITHDEBINFO>:${library}>)
      target_link_libraries(${aTarget} $<$<CONFIG:MINSIZEREL>:${library}>)
    endif()
  endforeach()
    
  foreach(library ${staticLibraryDebug})
    get_filename_component(libraryName ${library} NAME_WE)

    if (NOT ${libraryName} MATCHES "^AkMotionSink$")
      target_link_libraries(${aTarget} debug ${library})
    endif()
  endforeach()
endfunction()
