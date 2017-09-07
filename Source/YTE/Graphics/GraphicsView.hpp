#pragma once
#ifndef YTE_Graphics_GraphicsView_hpp 
#define YTE_Graphics_GraphicsView_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  struct UBOView
  {
    glm::mat4 mProjectionMatrix;
    glm::mat4 mViewMatrix;
  };

  class GraphicsView : public Component
  {
  public:
    DeclareType(GraphicsView);
    GraphicsView(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void UpdateView(UBOView &aView);

    Window* GetWindow()
    {
      return mWindow;
    }

  private:
    Window *mWindow;
    Renderer *mRenderer;
    std::string mWindowName;
  };
}
#endif
