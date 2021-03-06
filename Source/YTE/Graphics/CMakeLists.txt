include(${CMAKE_CURRENT_LIST_DIR}/DirectX12/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Generics/CMakeLists.txt)
include(${CMAKE_CURRENT_LIST_DIR}/Vulkan/CMakeLists.txt)

target_sources(YTE
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/Animation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Animation.hpp
    ${CMAKE_CURRENT_LIST_DIR}/BaseModel.cpp
    ${CMAKE_CURRENT_LIST_DIR}/BaseModel.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Camera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Camera.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Drawers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Drawers.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ForwardDeclarations.hpp
    ${CMAKE_CURRENT_LIST_DIR}/FacialAnimator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FacialAnimator.hpp
    ${CMAKE_CURRENT_LIST_DIR}/FFT_WaterSimulation.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FFT_WaterSimulation.hpp
    ${CMAKE_CURRENT_LIST_DIR}/FlybyCamera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/FlybyCamera.hpp
    ${CMAKE_CURRENT_LIST_DIR}/GameCamera.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GameCamera.hpp
    ${CMAKE_CURRENT_LIST_DIR}/GPUBuffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GPUBuffer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/GraphicsSystem.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GraphicsSystem.hpp
    ${CMAKE_CURRENT_LIST_DIR}/GraphicsView.cpp
    ${CMAKE_CURRENT_LIST_DIR}/GraphicsView.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ImguiLayer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ImguiLayer.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Light.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Light.hpp
    ${CMAKE_CURRENT_LIST_DIR}/InfluenceMap.cpp
    ${CMAKE_CURRENT_LIST_DIR}/InfluenceMap.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Material.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Material.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Model.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Model.hpp
    ${CMAKE_CURRENT_LIST_DIR}/ParticleEmitter.cpp
    ${CMAKE_CURRENT_LIST_DIR}/ParticleEmitter.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Sprite.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Sprite.hpp
    ${CMAKE_CURRENT_LIST_DIR}/SpriteText.cpp
    ${CMAKE_CURRENT_LIST_DIR}/SpriteText.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Skybox.cpp
    ${CMAKE_CURRENT_LIST_DIR}/Skybox.hpp
    ${CMAKE_CURRENT_LIST_DIR}/TempDiffuseColoringComponent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/TempDiffuseColoringComponent.hpp
    ${CMAKE_CURRENT_LIST_DIR}/UBOs.hpp
    ${CMAKE_CURRENT_LIST_DIR}/Vertex.hpp
)
