
target_sources(YTE
PRIVATE  
  ${CMAKE_CURRENT_LIST_DIR}/DialogBox_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Gamepad_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Keyboard_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/Mouse_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/OsKeys_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/OsMouseButtons_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/OsXboxButtons_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/SharedObject_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WindowData_Win32.hpp
  ${CMAKE_CURRENT_LIST_DIR}/Window_Win32.cpp
  ${CMAKE_CURRENT_LIST_DIR}/WindowsInclude_Win32.hpp
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