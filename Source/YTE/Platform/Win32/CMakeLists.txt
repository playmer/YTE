
target_sources(YTE
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/Win32/DialogBox_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/Gamepad_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/GamepadSystem_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/Keyboard_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/Mouse_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/OsKeys_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/OsMouseButtons_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/OsXboxButtons_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/SharedObject_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/WindowData_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/Window_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Win32/WindowsInclude_Win32.hpp
)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
  target_link_libraries(YTE PUBLIC wsock32.lib)
  target_link_libraries(YTE PUBLIC ws2_32.lib)
  target_link_libraries(YTE PUBLIC IPHLPAPI.lib)
  target_link_libraries(YTE PUBLIC Winmm.lib)
  target_link_libraries(YTE PUBLIC Shlwapi.lib)
  target_link_libraries(YTE PUBLIC Psapi.lib)
  target_link_libraries(YTE PUBLIC dxguid.lib)
  target_link_libraries(YTE PUBLIC XInput9_1_0.lib)
endif()