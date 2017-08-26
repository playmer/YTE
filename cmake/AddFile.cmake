################################################################################
# Project: YTE Engine
# Legal  : All content (C) 2017 DigiPen (USA) Corporation, all rights reserved. 
# Author : Joshua T. Fisher (2017)
################################################################################
Macro(AddFile aSource aPreviousSource aFileName)
  Get_Filename_Component(DirectoryName ${CMAKE_CURRENT_SOURCE_DIR} NAME)
  
  Set(${aPreviousSource} ${${aPreviousSource}} ${DirectoryName}/${aFileName})
EndMacro()
