function(YTE_Target_Folder aTarget aFolderDelimiter)
  set_target_properties(${aTarget} PROPERTIES FOLDER ${aFolder})

  get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
  foreach(linkLibrary ${targetLinkLibraries})
    if (TARGET ${linkLibrary})
      get_target_property(targetFolder ${linkLibrary} FOLDER)

      #if (targetFolder STREQUAL "targetFolder-NOTFOUND")
      #  set(newFolder ${aFolder})
      #else()
      #  #set(newFolder ${aFolder}/${targetFolder})
      #  set(newFolder ${aFolder})
      #endif()

      Message(STATUS "${linkLibrary} : ${targetFolder} : ${aFolderDelimiter}")

      set_target_properties(${linkLibrary} PROPERTIES FOLDER ${aFolderDelimiter})

      YTE_Target_Folder(${linkLibrary} ${aFolderDelimiter}/${linkLibrary})
    endif()
  endforeach()
endfunction(YTE_Target_Folder)