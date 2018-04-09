add_library(STB 
  stb_image.cpp 
  stb_image.h
  stb_image_write.cpp
  stb_image_write.h
  stb_textedit.h
  stb_truetype.cpp
  stb_truetype.h
  stb_rect_pack.h
  stb_rect_pack.cpp
)

get_filename_component(ParentDirectory ${CMAKE_CURRENT_LIST_DIR} DIRECTORY)
target_include_directories(STB PUBLIC ${ParentDirectory} ${CMAKE_CURRENT_LIST_DIR})