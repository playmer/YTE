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
    YTEDeclareType(GraphicsView);
    GraphicsView(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void UpdateView(Camera *aCamera, UBOView &aView);

    Window* GetWindow()
    {
      return mWindow;
    }

    Camera* GetLastCamera() { return mLastCamera; }
    glm::vec4 GetClearColor();
    void SetClearColor(const glm::vec4 &aColor);

  private:
    Camera *mLastCamera;
    Window *mWindow;
    Renderer *mRenderer;
    glm::vec4 mClearColor;
    std::string mWindowName;
  };
}
#endif
