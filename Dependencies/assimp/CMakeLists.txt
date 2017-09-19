Set(assimp_Source)

Get_Filename_Component(ParentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
Add_Subdirectory(code)
Add_Subdirectory(contrib)
Add_Subdirectory(include)
Include_Directories(${ParentDirectory} 
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/contrib
                    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/openddlparser/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/gtest
                    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/gtest/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/rapidjson/include
                    ${CMAKE_CURRENT_SOURCE_DIR}/include)

Set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj /DOPENDDL_STATIC_LIBARY /D_SCL_SECURE_NO_WARNINGS /D_CRT_SECURE_NO_WARNINGS /DASSIMP_BUILD_NO_C4D_IMPORTER")

Add_Library(assimp ${assimp_Source}) 

YTE_Source_Group(CMAKE_CURRENT_SOURCE_DIR assimp_Source) 
