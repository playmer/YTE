function(YTE_Source_Group aRoot aTarget) 
  get_target_property(targetBinaryDir ${aTarget} BINARY_DIR)
  get_target_property(targetSourceDir ${aTarget} SOURCE_DIR)
  get_target_property(targetSources ${aTarget} SOURCES)

  # This will determine if the given files are in a folder or not and separate 
  # them on that alone. 
  foreach(aFile ${targetSources}) 
    file(TO_CMAKE_PATH ${aFile} resultFile) 
    get_filename_component(nameComponent ${resultFile} NAME) 
    get_filename_component(fullPath ${nameComponent} ABSOLUTE) 
    
    if(IS_ABSOLUTE ${aFile})
      # It's only safe to call RELATIVE_PATH if the path begins with our "root" dir.
      string(FIND "${aFile}" "${targetSourceDir}" checkIfRelativeToSourceDir)
      string(FIND "${aFile}" "${targetBinaryDir}" checkIfRelativeToBinaryDir)

      if ("${checkIfRelativeToSourceDir}" EQUAL 0)
        file(RELATIVE_PATH relativePath ${targetSourceDir} ${aFile})
      elseif ("${checkIfRelativeToBinaryDir}" EQUAL 0)
        file(RELATIVE_PATH relativePath ${targetBinaryDir} ${aFile})
        set(fullPath ${targetBinaryDir}/${nameComponent})
      endif()
    else()
      set(relativePath ${aFile})
    endif()

    if(EXISTS ${fullPath}) 
      set(notInAFolder ${notInAFolder} ${relativePath}) 
    else()
      set(inAFolder ${inAFolder} ${relativePath}) 
    endif() 
  endforeach() 

  # We use a no space prefix with files from folders, otherwise the filters  
  # don't get made. 
  source_group(TREE ${${aRoot}}  
               PREFIX "" 
               FILES ${inAFolder}) 

  # We use a one space prefix with files not in folders, otherwise the files 
  # are put into "Source Files" and "Header Files" filters. 
  source_group(TREE ${${aRoot}}  
               PREFIX " " 
               FILES ${notInAFolder}) 
endfunction() 
