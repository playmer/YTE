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
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aMeshFile);

      return nullptr;
    }

    virtual void RemoveMeshId(Window *aWindow, u64 aId) 
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aId);
    };

    virtual void UpdateModelTransformation(Model *aModel) 
    {
      YTEUnusedArgument(aModel);
    };

    virtual Texture *AddTexture(Window *aWindow,
                                const char *aTextureFile) 
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aTextureFile);
      return nullptr;
    };

     virtual void UpdateViewBuffer(Window *aWindow, 
                                   UBOView &aView) 
    {
      YTEUnusedArgument(aWindow);
      YTEUnusedArgument(aView);
    }

     virtual glm::vec4 GetClearColor(Window *aWindow)
     {
       YTEUnusedArgument(aWindow);
       return glm::vec4{};
     }

     virtual void SetClearColor(Window *aWindow, const glm::vec4 &aColor)
     {
       YTEUnusedArgument(aWindow);
       YTEUnusedArgument(aColor);
     }
  };
}