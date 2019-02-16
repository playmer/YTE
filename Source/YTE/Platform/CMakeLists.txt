# Set these to false first.
set(YTE_Windows "YTE_Windows=0")
set(YTE_Linux "YTE_Linux=0")
set(YTE_Darwin "YTE_Darwin=0")
set(YTE_Android "YTE_Android=0")
set(YTE_x64 "YTE_x64=0")
set(YTE_x86 "YTE_x86=0")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(YTE_Windows "YTE_Windows=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(YTE_Linux "YTE_Linux=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(YTE_Darwin "YTE_Darwin=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(YTE_Android "YTE_Android=1")
endif()

# This might fail on Apple, but also maybe not? Not sure they have any 
# x86 bit targets left
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(YTE_x86 YTE_x86=1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(YTE_x64 YTE_x64=1)
endif()


include(${CMAKE_CURRENT_LIST_DIR}/SDL/CMakeLists.txt)

# Now add in the files per platform.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  include(${CMAKE_CURRENT_LIST_DIR}/Win32/CMakeLists.txt)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  include(${CMAKE_CURRENT_LIST_DIR}/Linux/CMakeLists.txt)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(YTE_Darwin "YTE_Darwin=1")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(YTE_Android "YTE_Android=1")
endif()

target_sources(YTE
  PRIVATE  
    ${CMAKE_CURRENT_LIST_DIR}/DeviceEnums.cpp
    ${CMAKE_CURRENT_LIST_DIR}/DeviceEnums.hpp
    ${CMAKE_CURRENT_LIST_DIR}/DialogBox.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ForwardDeclarations.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Gamepad.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Gamepad.hpp
    ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GamepadSystem.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Keyboard.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Keyboard.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Mouse.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Mouse.hpp
    ${CMAKE_CURRENT_LIST_DIR}/PlatformManager.hpp
    ${CMAKE_CURRENT_LIST_DIR}/SharedObject.hpp
    ${CMAKE_CURRENT_LIST_DIR}/TargetDefinitions.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Window.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Window.hpp
)
