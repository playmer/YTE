get_filename_component(parentDirectory ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

add_library(assimp "")

include(code/CMakeLists.txt)
include(contrib/CMakeLists.txt)
include(include/CMakeLists.txt)

target_include_directories(assimp
  PUBLIC 
    ${parentDirectory} 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/openddlparser/include
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/gtest
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/gtest/include
    ${CMAKE_CURRENT_SOURCE_DIR}/contrib/rapidjson/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(assimp PRIVATE /bigobj)

target_compile_definitions(assimp 
  PUBLIC 
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    OPENDDL_STATIC_LIBARY 
    _SCL_SECURE_NO_WARNINGS 
    _CRT_SECURE_NO_WARNINGS 
    ASSIMP_BUILD_NO_C4D_IMPORTER
)

YTE_Source_Group(CMAKE_CURRENT_SOURCE_DIR assimp)
