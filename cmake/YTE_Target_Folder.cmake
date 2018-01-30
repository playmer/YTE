function(YTE_Target_Folder aTarget aFolder)
  get_target_property(targetLinkLibraries ${aTarget} LINK_LIBRARIES)
  foreach(linkLibrary ${targetLinkLibraries})
    if (TARGET ${linkLibrary})
      get_target_property(targetFolder ${linkLibrary} FOLDER)


      if (targetFolder STREQUAL "targetFolder-NOTFOUND")
        set(newFolder ${aFolder})
      else()
        #set(newFolder ${aFolder}/${targetFolder})
        set(newFolder ${aFolder})
      endif()

      Message(STATUS "${linkLibrary} : ${targetFolder} : ${newFolder}")

      set_target_properties(${linkLibrary} PROPERTIES FOLDER ${newFolder}) 

      YTE_Target_Folder(${linkLibrary} ${newFolder}/${linkLibrary})
    endif()
  endforeach()
endfunction(YTE_Target_Folder)