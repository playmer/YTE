#pragma once

#ifndef YTE_Graphics_ForwardDeclarations_hpp
#define YTE_Graphics_ForwardDeclarations_hpp

namespace YTE
{
  class Animator;
  class Animation;
  class Camera;
  class GraphicsDataUpdate;
  class GraphicsSystem;
  class GraphicsView;
  class FFT_WaterSimulation;
  class InstantiatedModel;
  class InstantiatedLight;
  class LightManager;
  class Light;
  class Model;
  class ModelChanged;
  class Renderer;
  class ParticleEmitter;
  class SimpleModel;
  class Sprite;
  class ViewChanged;

  struct Instance;

  namespace UBOs
  {
    struct Material;
    struct Model;
    struct Light;
    struct View;
    struct Illumination;
  }

  class GPUBufferBase;
  struct Vertex;
}
#endif