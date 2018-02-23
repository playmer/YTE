function(YTE_Target_Folder_Recursive aTarget aFolderDelimiter)
  get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
  foreach(linkLibrary ${targetLinkLibraries})
    if (TARGET ${linkLibrary})
      get_target_property(targetType ${linkLibrary} TYPE)

      if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
        get_target_property(targetFolder ${linkLibrary} FOLDER)
        set_target_properties(${linkLibrary} PROPERTIES FOLDER ${aFolderDelimiter})
      endif ()

      YTE_Target_Folder(${linkLibrary} ${aFolderDelimiter}/${linkLibrary})
    endif()
  endforeach()
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

function(YTE_Target_Set_Outputs aTarget aLibraryDirectory aBinaryDirectory)
  set_target_properties(${aTarget}
                        PROPERTIES
                        ARCHIVE_OUTPUT_DIRECTORY ${aLibraryDirectory}
                        LIBRARY_OUTPUT_DIRECTORY ${aLibraryDirectory}
                        RUNTIME_OUTPUT_DIRECTORY ${aBinaryDirectory})
  
  get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
  foreach(linkLibrary ${targetLinkLibraries})
    if (TARGET ${linkLibrary})
      get_target_property(targetType ${linkLibrary} TYPE)

      if (NOT targetType STREQUAL "INTERFACE_LIBRARY")
        set_target_properties(${linkLibrary}
                              PROPERTIES
                              ARCHIVE_OUTPUT_DIRECTORY ${aLibraryDirectory}
                              LIBRARY_OUTPUT_DIRECTORY ${aLibraryDirectory}
                              RUNTIME_OUTPUT_DIRECTORY ${aBinaryDirectory})
      endif ()

      YTE_Target_Set_Outputs(${linkLibrary} ${aLibraryDirectory} ${aBinaryDirectory})
    endif()
  endforeach()
endfunction(YTE_Target_Set_Outputs)
