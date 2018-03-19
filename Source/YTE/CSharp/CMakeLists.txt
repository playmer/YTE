target_sources(YTE
  PRIVATE

  #  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/CSharpSystem.cpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharpSystem.hpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharp_CodeGenerator.cpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharp_CodeGenerator.hpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharpComponent.hpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharpComponent.cpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharpComponentFactory.hpp
  ${CMAKE_CURRENT_LIST_DIR}/CSharpComponentFactory.cpp
)

FindMono(Mono_Static_Library 
         Mono_Include_Directory)

target_link_libraries(YTE PUBLIC ${Mono_Static_Library})
target_include_directories(YTE PRIVATE ${Mono_Include_Directory})