#pragma once
#ifndef YTE_Graphics_GraphicsView_hpp 
#define YTE_Graphics_GraphicsView_hpp 

#include <optional>

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/GPUBuffer.hpp"
#include "YTE/Graphics/LightManager.hpp"
#include "YTE/Graphics/WaterInfluenceMapManager.hpp"

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
    Window *ChangingWindow;
  };


  // Adapted From Sascha Willems
  // https://github.com/SaschaWillems/Vulkan/blob/master/base/frustum.hpp
  class Frustum
  {
  public:
    enum side { LEFT = 0, RIGHT = 1, TOP = 2, BOTTOM = 3, BACK = 4, FRONT = 5 };
    std::array<glm::vec4, 6> mPlanes;
    glm::vec3 mCameraPosition;


    void Update(UBOs::View const& aView);
    bool CheckSphere(glm::vec3 aPosition, float aRadius);
  };

  class GraphicsView : public Component
  {
  public:
    YTEDeclareType(GraphicsView);
    YTE_Shared GraphicsView(Composition *aOwner, Space *aSpace);
    YTE_Shared ~GraphicsView();

    YTE_Shared void NativeInitialize() override;
    YTE_Shared void UpdateView(Camera const* aCamera, UBOs::View const& aView);
    YTE_Shared void UpdateIllumination(UBOs::Illumination &aIllumination);

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

    Frustum& GetFrustum()
    {
      return mFrustum;
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

    LightManager* GetLightManager()
    {
      return &(*mLightManager);
    }

    WaterInfluenceMapManager* GetWaterInfluenceMapManager()
    {
      return &(*mWaterInfluenceMapManager);
    }


    GPUBuffer<UBOs::View>& GetViewUBO()
    {
      return mViewUBO;
    }

    GPUBuffer<UBOs::Illumination>& GetIlluminationUBO()
    {
      return mIlluminationUBO;
    }


    UBOs::View const& GetViewUBOData()
    {
      return mViewUBOData;
    }
    UBOs::Illumination const& GetIlluminationUBOData()
    {
      return mIlluminationUBOData;
    }

    void SetClearColor(glm::vec4 aClearColor)
    {
      mClearColor = aClearColor;
    }

    glm::vec4 GetClearColor()
    {
      return mClearColor;
    }

  private:
    bool DetermineDefaultWindow();

    void ParentSurfaceLost(ViewChanged* aEvent);
    void ParentSurfaceGained(ViewChanged* aEvent);

    std::optional<LightManager> mLightManager;
    std::optional<WaterInfluenceMapManager> mWaterInfluenceMapManager;

    GPUBuffer<UBOs::View> mViewUBO;
    GPUBuffer<UBOs::Illumination> mIlluminationUBO;

    UBOs::View mViewUBOData;
    UBOs::Illumination mIlluminationUBOData;
    std::unique_ptr<LineDrawer> mDebugDrawer;
    Frustum mFrustum;
    Camera* mActiveCamera;
    DrawerTypeCombination mDrawerCombination;
    DrawerTypes mDrawerType;
    Window* mWindow;
    Renderer* mRenderer;
    std::string mWindowName;
    glm::vec4 mClearColor;
    i32 mSuperSampling;
    float mOrder;
    bool mConstructing;
    bool mInitialized;
  };
}
#endif
