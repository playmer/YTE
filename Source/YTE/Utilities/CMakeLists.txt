target_sources(YTE
  PRIVATE  
    ${CMAKE_CURRENT_LIST_DIR}/Utilities.cpp
    ${CMAKE_CURRENT_LIST_DIR}/String/String.cpp
#  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/JsonHelpers.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Utilities.hpp
    ${CMAKE_CURRENT_LIST_DIR}/DebugChecking/Assert.hpp
    ${CMAKE_CURRENT_LIST_DIR}/String/String.hpp
)
