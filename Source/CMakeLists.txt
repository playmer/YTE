################################################################################
# This source file is a part of LambPlanet.
# Legal  : All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
# Author : Joshua T. Fisher
################################################################################
set(Source_Root ${CMAKE_CURRENT_SOURCE_DIR})
set(YTE_Root ${Source_Root}/YTE)
set(YTEditor_Root ${Source_Root}/YTEditor)
set(YTEPlayer_Root ${Source_Root}/YTEPlayer)
set(YTEMain_Root ${Source_Root}/YTEMain)
set(CMake_Include ${Source_Root}/cmake)

add_subdirectory(YTE)
add_subdirectory(YTEditor)
add_subdirectory(YTEPlayer)
add_subdirectory(YTEMain)
