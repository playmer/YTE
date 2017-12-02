#pragma once
#ifndef YTE_Graphics_GraphicsView_hpp 
#define YTE_Graphics_GraphicsView_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class GraphicsView : public Component
  {
  public:
    YTEDeclareType(GraphicsView);
    GraphicsView(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~GraphicsView();

    void Initialize() override;
    void UpdateView(Camera *aCamera, UBOView &aView);

    // Getters / Setters
    glm::vec4 GetClearColor();
    void SetClearColor(const glm::vec4 &aColor);

    Camera* GetLastCamera()
    {
      return mLastCamera;
    }

    void ChangeWindow(const std::string& aWindowName);

    Window* GetWindow()
    {
      return mWindow;
    }

    float GetOrder()
    {
      return mOrder;
    }

    void SetOrder(float aOrder);

    void KeyPressed(KeyboardEvent *aUpdate);

  private:
    Camera *mLastCamera;
    Window *mWindow;
    Renderer *mRenderer;
    std::string mWindowName;
    glm::vec4 mClearColor;
    float mOrder;
    bool mConstructing;
  };
}
#endif
