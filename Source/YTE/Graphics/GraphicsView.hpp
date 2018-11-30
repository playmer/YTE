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
    YTE_Shared GraphicsView(Composition *aOwner, Space *aSpace);
    YTE_Shared ~GraphicsView();

    YTE_Shared void NativeInitialize() override;
    YTE_Shared void UpdateView(Camera *aCamera, UBOs::View &aView);
    YTE_Shared void UpdateIllumination(UBOs::Illumination &aIllumination);

    // Getters / Setters
    YTE_Shared glm::vec4 GetClearColor();
    YTE_Shared void SetClearColor(const glm::vec4 &aColor);

    Camera* GetActiveCamera()
    {
      return mActiveCamera;
    }

    void SetActiveCamera(Camera *aCamera)
    {
      mActiveCamera = aCamera;
    }

    YTE_Shared void ChangeWindow(const std::string& aWindowName);
    YTE_Shared void ChangeWindow(Window *aWindow);

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

    YTE_Shared void SetOrder(float aOrder);

    YTE_Shared void KeyPressed(KeyboardEvent *aUpdate);

    YTE_Shared std::string GetDrawerCombinationType();
    YTE_Shared void SetDrawerCombinationType(std::string aCombination);
    YTE_Shared std::string GetDrawerType();
    YTE_Shared void SetDrawerType(std::string aType);


    YTE_Shared void SetSuperSampling(i32 aSuperSampling);

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
