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

target_compile_definitions(YTE PUBLIC ${YTE_Windows})
target_compile_definitions(YTE PUBLIC ${YTE_Linux})
target_compile_definitions(YTE PUBLIC ${YTE_Darwin})
target_compile_definitions(YTE PUBLIC ${YTE_Android})

# This might fail on Apple, but also maybe not? Not sure they have any 
# x86 bit targets left
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(YTE_x86 YTE_x86=1)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(YTE_x64 YTE_x64=1)
endif()

target_compile_definitions(YTE PUBLIC ${YTE_x64})
target_compile_definitions(YTE PUBLIC ${YTE_x86})

add_subdirectory(SDL)

# Now add in the files per platform.
#if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#  add_subdirectory(Win32)
#elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#  add_subdirectory(Linux)
#elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#  set(YTE_Darwin "YTE_Darwin=1")
#elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
#  set(YTE_Android "YTE_Android=1")
#endif()

target_sources(YTE
  PRIVATE  
    DeviceEnums.cpp
    DeviceEnums.hpp
    DialogBox.hpp
    ForwardDeclarations.hpp
    Gamepad.cpp
    Gamepad.hpp
    GamepadSystem.cpp
    GamepadSystem.hpp
    Keyboard.cpp
    Keyboard.hpp
    Mouse.cpp
    Mouse.hpp
    PlatformManager.hpp
    SharedObject.hpp
    TargetDefinitions.hpp
    Window.cpp
    Window.hpp
)
