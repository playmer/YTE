Set(vkel_Source)

Get_Filename_Component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
Include_Directories(${ParentDirectory} ${CMAKE_CURRENT_SOURCE_DIR})

Set(vkel_Source vkel.c vkel.h)

Add_Library(vkel ${vkel_Source})