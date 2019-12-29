target_sources(YTE
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/DialogBox_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Gamepad_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Gamepad_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Keyboard_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Keyboard_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Mouse_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Mouse_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/OsKeys_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/OsMouseButtons_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/OsGamepadButtons_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/PlatformManager_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/PlatformManager_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/SharedObject_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Window_SDL.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Window_SDL.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Window_Secluded_SDL.cpp
)

target_link_libraries(YTE PRIVATE SDL2)