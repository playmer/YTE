target_sources(YTE
PRIVATE  
  DialogBox_SDL.cpp
  Gamepad_SDL.cpp
  Gamepad_SDL.hpp
  GamepadSystem_SDL.cpp
  GamepadSystem_SDL.hpp
  Keyboard_SDL.cpp
  Keyboard_SDL.hpp
  Mouse_SDL.cpp
  Mouse_SDL.hpp
  OsKeys_SDL.hpp
  OsMouseButtons_SDL.hpp
  OsGamepadButtons_SDL.hpp
  PlatformManager_SDL.cpp
  PlatformManager_SDL.hpp
  SharedObject_SDL.cpp
  Window_SDL.cpp
  Window_SDL.hpp
  Window_Secluded_SDL.cpp
)

target_link_libraries(YTE PRIVATE SDL2)