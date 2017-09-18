#pragma once

#include "YTE/Core/Utilities.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/InstantiatedMesh.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class Renderer
  {
  public:
    virtual ~Renderer() {};

    virtual std::unique_ptr<InstantiatedMesh> AddModel(Window *aWindow,
                                                       std::string &aMeshFile)
    {
      (void)aWindow;
      (void)aMeshFile;

      return nullptr;
    }

    virtual void RemoveMeshId(Window *aWindow, u64 aId) 
    {
      (void)aWindow;
      (void)aId;
    };

    virtual void UpdateModelTransformation(Model *aModel) 
    {
      (void)aModel;
    };

    virtual Texture *AddTexture(Window *aWindow,
                                const char *aTextureFile) 
    {
      (void)aWindow;
      (void)aTextureFile;
      return nullptr;
    };

     virtual void UpdateViewBuffer(Window *aWindow, 
                                   UBOView &aView) 
    { 
      (void)aWindow; 
      (void)aView; 
    }

     virtual glm::vec4 GetClearColor(Window *aWindow)
     {
       return glm::vec4{};
     }

     virtual void SetClearColor(Window *aWindow, const glm::vec4 &aColor)
     {
       (void)aWindow;
       (void)aColor;
     }
  };
}