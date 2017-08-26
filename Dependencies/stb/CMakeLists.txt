Set(STB_Source)

Get_Filename_Component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
Include_Directories(${ParentDirectory} ${CMAKE_CURRENT_SOURCE_DIR})

Set(STB_Source stb_image.cpp stb_image.h)

Add_Library(STB_Image ${STB_Source})