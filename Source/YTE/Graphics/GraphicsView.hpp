#pragma once
#ifndef YTE_Graphics_GraphicsView_hpp 
#define YTE_Graphics_GraphicsView_hpp 

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  enum class DrawerTypes
  {
    DefaultDrawer,
    GameForwardDrawer,
    ImguiDrawer
  };

  enum class DrawerTypeCombination
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
    Window *Window;
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

    Camera* GetActiveCamera()
    {
      return mActiveCamera;
    }

    void SetActiveCamera(Camera *aCamera)
    {
      mActiveCamera = aCamera;
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
    void SetDrawerCombinationType(std::string aCombination);
    std::string GetDrawerType();
    void SetDrawerType(std::string aType);


    void SetSuperSampling(i32 aSuperSampling);

    i32 GetSuperSampling()
    {
      return mSuperSampling;
    }

  private:
    Camera *mActiveCamera;
    DrawerTypeCombination mDrawerCombination;
    DrawerTypes mDrawerType;
    Window *mWindow;
    Renderer *mRenderer;
    std::string mWindowName;
    glm::vec4 mClearColor;
    i32 mSuperSampling;
    float mOrder;
    bool mConstructing;
    bool mInitialized;
  };
}
#endif
