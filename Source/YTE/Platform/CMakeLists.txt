target_sources(YTE
  PRIVATE  
    ${CMAKE_CURRENT_LIST_DIR}/DeviceEnums.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Gamepad.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Keyboard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Mouse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Window.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/DialogBox_Windows.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/Gamepad_Windows.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/GamepadSystem_Windows.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/Keyboard_Windows.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/Mouse_Windows.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/Window_Windows.cpp
#  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/DeviceEnums.hpp
    ${CMAKE_CURRENT_LIST_DIR}/DialogBox.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ForwardDeclarations.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Gamepad.hpp
    ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Keyboard.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Mouse.hpp
    ${CMAKE_CURRENT_LIST_DIR}/TargetDefinitions.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Window.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/OsKeys_Windows.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/OsMouseButtons_Windows.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/OsXboxButtons_Windows.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/WindowData_Windows.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Windows/WindowsInclude_Windows.hpp
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