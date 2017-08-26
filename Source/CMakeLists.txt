################################################################################
# This source file is a part of LambPlanet.
# Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
# Author : Joshua T. Fisher
################################################################################
Set(Source_Root ${CMAKE_CURRENT_SOURCE_DIR})
Set(YTE_Root ${Source_Root}/YTE)
Set(YTEditor_Root ${Source_Root}/YTEditor)
Set(CMake_Include ${Source_Root}/cmake)

Add_Subdirectory(YTE)
Add_Subdirectory(YTEditor)
