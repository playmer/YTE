function(YTE_Target_Folder_Recursive aTarget aFolderDelimiter)

  get_target_property(targetType ${aTarget} TYPE)

  if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
    get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
    foreach(linkLibrary ${targetLinkLibraries})
      if (TARGET ${linkLibrary})
        get_target_property(targetType ${linkLibrary} TYPE)

        if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
              
          get_target_property(aliasedLibrary ${linkLibrary} ALIASED_TARGET)
                
          if (aliasedLibrary STREQUAL "aliasedLibrary-NOTFOUND")
            set_target_properties(${linkLibrary} PROPERTIES FOLDER ${aFolderDelimiter})
          endif()
        endif ()

        YTE_Target_Folder_Recursive(${linkLibrary} ${aFolderDelimiter}/${linkLibrary}_Dependencies)
      endif()
    endforeach()
  endif()
endfunction(YTE_Target_Folder_Recursive)

function(YTE_Target_Folder aTarget aFolderDelimiter)
  set_target_properties(${aTarget} PROPERTIES FOLDER ${aFolderDelimiter})
  YTE_Target_Folder_Recursive(${aTarget} ${aFolderDelimiter})
endfunction(YTE_Target_Folder)

# Adapted from: https://stackoverflow.com/a/45176243
function(YTE_Get_Targets_From_Directory aVariable aDirectory)
  get_property(subdirectories DIRECTORY "${aDirectory}" PROPERTY SUBDIRECTORIES)
  
  foreach(subdirectory IN LISTS subdirectories)
    YTE_Get_Targets_From_Directory(${aVariable} "${subdirectory}")
  endforeach()
  
  get_property(sub_targets DIRECTORY "${aDirectory}" PROPERTY BUILDSYSTEM_TARGETS)
  set(${aVariable} ${${aVariable}} ${sub_targets} PARENT_SCOPE)
endfunction()

macro(YTE_Cleanup_Orphan_Targets aFolder)
  YTE_Get_Targets_From_Directory(allTargets ${CMAKE_CURRENT_SOURCE_DIR})
  
  foreach(linkLibrary ${allTargets})
    get_target_property(targetType ${linkLibrary} TYPE)

    if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
      get_target_property(targetFolder ${linkLibrary} FOLDER)
      
      if (targetFolder STREQUAL "targetFolder-NOTFOUND")
        set_target_properties(${linkLibrary} PROPERTIES FOLDER ${aFolder})
      endif ()
    endif()
  endforeach()
endmacro(YTE_Cleanup_Orphan_Targets)

function(YTE_Is_Not_Interface_Alias_Or_Imported aTarget aReturn)
  if (TARGET ${aTarget})
        
    get_target_property(libraryType ${aTarget} TYPE)

    if (NOT libraryType STREQUAL "INTERFACE_LIBRARY")
      get_target_property(importedTarget ${aTarget} IMPORTED)

      if (NOT importedTarget)
        
        get_target_property(aliasedTarget ${aTarget} ALIASED_TARGET)
      
        if (aliasedTarget STREQUAL "aliasedTarget-NOTFOUND")
          set(${aReturn} TRUE)
        endif ()
      endif()
    endif ()
  endif()
endfunction()


function(YTE_Target_Set_LTCG aTarget)
  set(targetLinkLibraries "")
  get_target_property(libraryType ${aTarget} TYPE)
  get_target_property(aliasedTarget ${aTarget} ALIASED_TARGET)

  if (NOT libraryType STREQUAL "INTERFACE_LIBRARY")
    if (aliasedTarget STREQUAL "aliasedTarget-NOTFOUND")
      target_compile_options(${aTarget} PRIVATE $<$<CONFIG:Publish>:/GL>)
      set_target_properties(${aTarget} PROPERTIES LINK_FLAGS_PUBLISH /LTCG)
      get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
    endif()
  endif()

  foreach(linkLibrary ${targetLinkLibraries})
    if (TARGET ${linkLibrary})
    
      get_target_property(libraryType ${linkLibrary} TYPE)

      if (NOT libraryType STREQUAL "INTERFACE_LIBRARY")
        get_target_property(importedTarget ${linkLibrary} IMPORTED)

        if (NOT importedTarget)
          get_target_property(aliasedLibrary ${linkLibrary} ALIASED_TARGET)
          if (aliasedLibrary STREQUAL "aliasedLibrary-NOTFOUND")
            if (MSVC)
              target_compile_options(${linkLibrary} PRIVATE $<$<CONFIG:Publish>:/GL>)
              set_target_properties(${linkLibrary} PROPERTIES STATIC_LIBRARY_FLAGS_PUBLISH /LTCG)
            else()
              target_compile_options(${linkLibrary} PRIVATE -fPIC)
            endif()
          endif()
        endif()
      endif ()

      YTE_Target_Set_LTCG(${linkLibrary})
    endif()
  endforeach()
endfunction(YTE_Target_Set_LTCG)

function(YTE_Target_Set_Outputs aTarget aLibraryDirectory aBinaryDirectory)
  set(targetLinkLibraries "")
  get_target_property(targetType ${aTarget} TYPE)

  if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
        
    get_target_property(aliasedTarget ${aTarget} ALIASED_TARGET)
          
    if (aliasedTarget STREQUAL "aliasedTarget-NOTFOUND")
      set_target_properties(${aTarget}
                            PROPERTIES
                            ARCHIVE_OUTPUT_DIRECTORY ${aLibraryDirectory}
                            LIBRARY_OUTPUT_DIRECTORY ${aLibraryDirectory}
                            RUNTIME_OUTPUT_DIRECTORY ${aBinaryDirectory})
    endif()
    
    get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
  else()
    get_target_property(targetLinkLibraries ${aTarget} INTERFACE_LINK_LIBRARIES)
  endif()

  foreach(linkLibrary ${targetLinkLibraries})
    if (TARGET ${linkLibrary})
      get_target_property(libraryType ${linkLibrary} TYPE)

      if (NOT libraryType STREQUAL "INTERFACE_LIBRARY")
        
        get_target_property(aliasedLibrary ${linkLibrary} ALIASED_TARGET)
        
        if (aliasedLibrary STREQUAL "aliasedLibrary-NOTFOUND")
          set_target_properties(${linkLibrary}
                                PROPERTIES
                                ARCHIVE_OUTPUT_DIRECTORY ${aLibraryDirectory}
                                LIBRARY_OUTPUT_DIRECTORY ${aLibraryDirectory}
                                RUNTIME_OUTPUT_DIRECTORY ${aBinaryDirectory})
        endif()
      endif()
      
      YTE_Target_Set_Outputs(${linkLibrary} ${aLibraryDirectory} ${aBinaryDirectory})
    endif()
  endforeach()
endfunction(YTE_Target_Set_Outputs)
