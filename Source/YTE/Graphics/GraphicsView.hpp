#pragma once
#ifndef YTE_Graphics_GraphicsView_hpp 
#define YTE_Graphics_GraphicsView_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"

namespace YTE
{
  class GraphicsView : public Component
  {
  public:
    YTEDeclareType(GraphicsView);
    GraphicsView(Composition *aOwner, Space *aSpace, RSValue *aProperties);

    void Initialize() override;
    void UpdateView(Camera *aCamera, UBOView &aView);

    // Gettors / Settors
    glm::vec4 GetClearColor();

    Camera* GetLastCamera()
    {
      return mLastCamera;
    }

    Window* GetWindow()
    {
      return mWindow;
    }



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
