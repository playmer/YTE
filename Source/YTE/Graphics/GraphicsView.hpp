#pragma once
#ifndef YTE_Graphics_GraphicsView_hpp 
#define YTE_Graphics_GraphicsView_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  enum class YTEDrawerTypes
  {
    DefaultDrawer,
    GameForwardDrawer
  };

  enum class YTEDrawerTypeCombination
  {
    DoNotInclude,
    DefaultCombination,
    AlphaBlend,
    AdditiveBlend,
    Opaque,
    MultiplicativeBlend
  };

  YTEDeclareEvent(SurfaceLost);
  YTEDeclareEvent(SurfaceGained);

  class ViewChanged : public Event
  {
  public:
    GraphicsView *View;
  };

  class GraphicsView : public Component
  {
  public:
    YTEDeclareType(GraphicsView);
    GraphicsView(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~GraphicsView();

    void NativeInitialize() override;
    void UpdateView(Camera *aCamera, UBOView &aView);
    void UpdateIllumination(UBOIllumination &aIllumination);

    // Getters / Setters
    glm::vec4 GetClearColor();
    void SetClearColor(const glm::vec4 &aColor);

    Camera* GetLastCamera()
    {
      return mLastCamera;
    }

    void ChangeWindow(const std::string& aWindowName);
    void ChangeWindow(Window *aWindow);

    Window* GetWindow()
    {
      return mWindow;
    }

    Renderer* GetRenderer()
    {
      return mRenderer;
    }

    float GetOrder()
    {
      return mOrder;
    }

    void SetOrder(float aOrder);

    void KeyPressed(KeyboardEvent *aUpdate);

    std::string GetDrawerCombinationType();
    std::string GetDrawerType();
    void SetDrawerCombinationType(std::string aCombination);
    void SetDrawerType(std::string aType);

  private:
    Camera *mLastCamera;
    YTEDrawerTypeCombination mDrawerCombination;
    YTEDrawerTypes mDrawerType;
    Window *mWindow;
    Renderer *mRenderer;
    std::string mWindowName;
    glm::vec4 mClearColor;
    float mOrder;
    bool mConstructing;
    bool mInitialized;
  };
}
#endif
