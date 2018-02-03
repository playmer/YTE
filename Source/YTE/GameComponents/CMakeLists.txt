target_sources(YTE
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/BoatController.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AudioTest.cpp
    ${CMAKE_CURRENT_LIST_DIR}/InputInterpreter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/AddComponentToEngine.cpp
    ${CMAKE_CURRENT_LIST_DIR}/CameraController.cpp
#  PUBLIC    
    ${CMAKE_CURRENT_LIST_DIR}/BoatController.hpp
    ${CMAKE_CURRENT_LIST_DIR}/AudioTest.hpp
    ${CMAKE_CURRENT_LIST_DIR}/InputInterpreter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/AddComponentToEngine.hpp
    ${CMAKE_CURRENT_LIST_DIR}/CameraController.hpp
)
