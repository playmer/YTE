target_sources(YTE
  PRIVATE  
    ${CMAKE_CURRENT_LIST_DIR}/WWiseSystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/MemoryImplementation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WWiseEmitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WWiseListener.cpp
    ${CMAKE_CURRENT_LIST_DIR}/WWiseView.cpp
#  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/WWiseSystem.hpp
    ${CMAKE_CURRENT_LIST_DIR}/WWiseEmitter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/WWiseListener.hpp
    ${CMAKE_CURRENT_LIST_DIR}/WWiseView.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Utilities.hpp
)

target_link_libraries(YTE PUBLIC WWise_SoundEngine)
