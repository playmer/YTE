Set(STB_Source)

Get_Filename_Component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
Include_Directories(${ParentDirectory} ${CMAKE_CURRENT_SOURCE_DIR})

Set(STB_Source )

add_library(STB 
  stb_image.cpp 
  stb_image.h
  stb_image_write.cpp
  stb_image_write.h
  stb_truetype.cpp
  stb_truetype.h
  stb_rect_pack.h
  stb_rect_pack.cpp
)
