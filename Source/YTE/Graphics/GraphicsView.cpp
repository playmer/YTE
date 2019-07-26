#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/Drawers.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
  void Frustum::Update(UBOs::View const& aView)
  {
    //, glm::vec4 const& aCameraPosition;
    glm::mat4 const& clipSpace = aView.mProjectionMatrix * aView.mViewMatrix;

    mCameraPosition = glm::vec3(aView.mCameraPosition);

    mPlanes[LEFT].x = clipSpace[0].w + clipSpace[0].x;
    mPlanes[LEFT].y = clipSpace[1].w + clipSpace[1].x;
    mPlanes[LEFT].z = clipSpace[2].w + clipSpace[2].x;
    mPlanes[LEFT].w = clipSpace[3].w + clipSpace[3].x;

    mPlanes[RIGHT].x = clipSpace[0].w - clipSpace[0].x;
    mPlanes[RIGHT].y = clipSpace[1].w - clipSpace[1].x;
    mPlanes[RIGHT].z = clipSpace[2].w - clipSpace[2].x;
    mPlanes[RIGHT].w = clipSpace[3].w - clipSpace[3].x;

    mPlanes[TOP].x = clipSpace[0].w - clipSpace[0].y;
    mPlanes[TOP].y = clipSpace[1].w - clipSpace[1].y;
    mPlanes[TOP].z = clipSpace[2].w - clipSpace[2].y;
    mPlanes[TOP].w = clipSpace[3].w - clipSpace[3].y;

    mPlanes[BOTTOM].x = clipSpace[0].w + clipSpace[0].y;
    mPlanes[BOTTOM].y = clipSpace[1].w + clipSpace[1].y;
    mPlanes[BOTTOM].z = clipSpace[2].w + clipSpace[2].y;
    mPlanes[BOTTOM].w = clipSpace[3].w + clipSpace[3].y;

    //mPlanes[BACK].x = /*clipSpace[0].w +*/ clipSpace[0].z;
    //mPlanes[BACK].y = /*clipSpace[1].w +*/ clipSpace[1].z;
    //mPlanes[BACK].z = /*clipSpace[2].w +*/ clipSpace[2].z;
    //mPlanes[BACK].w = /*clipSpace[3].w +*/ clipSpace[3].z;

    mPlanes[BACK].x = clipSpace[0].w + clipSpace[0].z;
    mPlanes[BACK].y = clipSpace[1].w + clipSpace[1].z;
    mPlanes[BACK].z = clipSpace[2].w + clipSpace[2].z;
    mPlanes[BACK].w = clipSpace[3].w + clipSpace[3].z;

    mPlanes[FRONT].x = clipSpace[0].w - clipSpace[0].z;
    mPlanes[FRONT].y = clipSpace[1].w - clipSpace[1].z;
    mPlanes[FRONT].z = clipSpace[2].w - clipSpace[2].z;
    mPlanes[FRONT].w = clipSpace[3].w - clipSpace[3].z;

    for (auto i = 0; i < mPlanes.size(); i++)
    {
      //glm::vec3 this_normal(mPlanes[i].x, mPlanes[i].y, mPlanes[i].z);
      //float d = mPlanes[i].w;
      //float length_normal = glm::length(this_normal);
      //this_normal /= -length_normal;
      //d /= length_normal;
      //
      //const glm::vec4 this_plane(this_normal, d);
      //mPlanes[i] = this_plane;

      float length = sqrtf(mPlanes[i].x * mPlanes[i].x + mPlanes[i].y * mPlanes[i].y + mPlanes[i].z * mPlanes[i].z);
      mPlanes[i] /= length;
    }
  }

  bool Frustum::CheckSphere(glm::vec3 aPosition, float aRadius)
  {
    YTEProfileFunction();

    // Check to see if Camera is within the sphere, if so, just draw it.
    auto distance = glm::length(mCameraPosition - aPosition);

    if (distance < aRadius)
    {
      return true;
    }

    // Next check to see if the sphere is within the planes.
    for (auto i = 0; i < mPlanes.size(); i++)
    {
      if ((mPlanes[i].x * aPosition.x) + (mPlanes[i].y * aPosition.y) + (mPlanes[i].z * aPosition.z) + mPlanes[i].w <= -aRadius)
      {
        return false;
      }
    }
    return true;
  }

  static std::vector<std::string> PopulateDrawerTypeDropDownList(Component *aComponent)
  {
    UnusedArguments(aComponent);

    std::vector<std::string> result
    {
      "DefaultDrawer",
      "GameForwardDrawer",
      "ImguiDrawer"
    };
    return result;
  }

  static std::vector<std::string> PopulateCombinationTypeDropDownList(Component *aComponent)
  {
    UnusedArguments(aComponent);

    std::vector<std::string> result
    {
      "DefaultCombination",
      "AlphaBlend",
      "AdditiveBlend",
      "Opaque",
      "MultiplicativeBlend",
      "DoNotInclude"
    };
    return result;
  }

  YTEDefineEvent(SurfaceLost);
  YTEDefineEvent(SurfaceGained);

  YTEDefineType(GraphicsView)
  {
    RegisterType<GraphicsView>();
    TypeBuilder<GraphicsView> builder;

    GetStaticType()->AddAttribute<RunInEditor>();

    builder.Field<&GraphicsView::mWindowName>("WindowName", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&GraphicsView::GetOrder, &GraphicsView::SetOrder>("Order")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The order to render the views. We render lowest to highest.");

    builder.Property<&GraphicsView::GetSuperSampling, &GraphicsView::SetSuperSampling>("SuperSampling")
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Determines the Super Sampling rate of the view. Must be a power of 2.");

    builder.Property<&GraphicsView::GetClearColor, &GraphicsView::SetClearColor>("ClearColor")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>()
      .SetDocumentation("The color the screen will be painted before rendering, defaults to gray.");

    builder.Property<&GraphicsView::GetDrawerType, &GraphicsView::SetDrawerType>("DrawerType")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDrawerTypeDropDownList);
    builder.Property<&GraphicsView::GetDrawerCombinationType, &GraphicsView::SetDrawerCombinationType>("DrawerCombination")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateCombinationTypeDropDownList);
  }

  GraphicsView::GraphicsView(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mActiveCamera{ nullptr }
    , mDrawerCombination{ DrawerTypeCombination::DefaultCombination }
    , mDrawerType{ DrawerTypes::DefaultDrawer }
    , mWindow{ nullptr }
    , mClearColor{ 0.22f, 0.22f, 0.22f, 1.0f }
    , mSuperSampling{ 1 }
    , mOrder{ 0.0f }
    , mConstructing{ true }
    , mInitialized{ false }
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();

    mLightManager.emplace(this);
    mWaterInfluenceMapManager.emplace(this);

    auto it = engine->GetWindows().find(mWindowName);

    if (it != engine->GetWindows().end())
    {
      mWindow = it->second.get();
    }
    else
    {
      mWindow = mOwner->GetEngine()->GetWindow();
    }

    if (mWindow == nullptr)
    {
      return;
    }

    auto uboAllocator = mRenderer->GetAllocator(AllocatorTypes::UniformBufferObject);
      
    mViewUBO = uboAllocator->CreateBuffer<UBOs::View>(1,
                                                      GPUAllocation::BufferUsage::TransferDst |
                                                      GPUAllocation::BufferUsage::UniformBuffer,
                                                      GPUAllocation::MemoryProperty::DeviceLocal);

    mIlluminationUBO = uboAllocator->CreateBuffer<UBOs::Illumination>(1,
                                                                      GPUAllocation::BufferUsage::TransferDst |
                                                                      GPUAllocation::BufferUsage::UniformBuffer,
                                                                      GPUAllocation::MemoryProperty::DeviceLocal);

    mRenderer->RegisterView(this, mDrawerType, mDrawerCombination);

    mWindow->mKeyboard.RegisterEvent<&GraphicsView::KeyPressed>(Events::KeyPress, this);

    SetClearColor(mClearColor);
  }

  GraphicsView::~GraphicsView()
  {
    mRenderer->DeregisterView(this);
  }

  // https://stackoverflow.com/a/108340
  static bool IsPowerOf2(i32 aValue)
  {
    return (aValue > 0 && !(aValue & (aValue - 1)));
  }

  void GraphicsView::SetSuperSampling(i32 aSuperSampling)
  {
    if (!IsPowerOf2(aSuperSampling))
    {
      return;
    }

    mSuperSampling = aSuperSampling;

    mRenderer->ResetView(this);
  }

  void GraphicsView::NativeInitialize()
  {
    mConstructing = false;
  }


  void GraphicsView::KeyPressed(KeyboardEvent *aUpdate)
  {
    if (aUpdate->Key == Keys::F1)
    {
      SetOrder(-mOrder);
    }
  }

  void GraphicsView::UpdateView(Camera const* aCamera, UBOs::View const& aView)
  {
    if (aCamera != mActiveCamera)
    {
      mOwner->GetEngine()->Log(LogType::Warning,
                               fmt::format("A camera by the name of {} is attempting to update "
                                           "the GraphicsView despite it not being the active "
                                           "camera. The current active camera is named {}",
                                           aCamera->GetOwner()->GetName().c_str(),
                                           mActiveCamera->GetOwner()->GetName().c_str()));
      return;
    }

    mViewUBOData = aView;
    mViewUBO.Update(mViewUBOData);
    mFrustum.Update(aView);

    // Draw frustum planes.
    if (mDebugDrawer)
    {

    }
  }
  
  void GraphicsView::UpdateIllumination(UBOs::Illumination& aIllumination)
  {
    mIlluminationUBOData = aIllumination;
    mIlluminationUBO.Update(mIlluminationUBOData);
  }

  void GraphicsView::ChangeWindow(const std::string &aWindowName)
  {
    ViewChanged event;
    event.View = this;
    event.ChangingWindow = nullptr;

    if (false == mConstructing)
    {
      SendEvent(Events::SurfaceLost, &event);
      mRenderer->DeregisterView(this);
    }

    mWindowName = aWindowName;
    auto it = mSpace->GetEngine()->GetWindows().find(mWindowName);

    mWindow = it->second.get();

    event.ChangingWindow = mWindow;

    if (false == mConstructing)
    {
      mRenderer->RegisterView(this);
      SendEvent(Events::SurfaceGained, &event);
    }
  }


  void GraphicsView::ChangeWindow(Window *aWindow)
  {
    ViewChanged event;
    event.View = this;

    if (mConstructing && nullptr == mWindow)
    {
      event.ChangingWindow = aWindow;
      mWindow = aWindow;
      NativeInitialize();
      SendEvent(Events::SurfaceGained, &event);
      return;
    }

    event.ChangingWindow = nullptr;

    if (false == mConstructing)
    {
      SendEvent(Events::SurfaceLost, &event);
      mRenderer->DeregisterView(this);
    }

    mWindow = aWindow;
    mWindowName = aWindow->mName;
    event.ChangingWindow = mWindow;

    if (false == mConstructing)
    {
      mRenderer->RegisterView(this);
      SendEvent(Events::SurfaceGained, &event);
    }
  }

  void GraphicsView::SetOrder(float aOrder)
  {
    if (false == mConstructing)
    {
      mRenderer->ViewOrderChanged(this, aOrder);
    }

    mOrder = aOrder;
  }

  std::string GraphicsView::GetDrawerCombinationType()
  {
    if (mDrawerCombination == DrawerTypeCombination::AdditiveBlend)
    {
      return "AdditiveBlend";
    }
    else if (mDrawerCombination == DrawerTypeCombination::AlphaBlend)
    {
      return "AlphaBlend";
    }
    else if (mDrawerCombination == DrawerTypeCombination::Opaque)
    {
      return "Opaque";
    }
    else if (mDrawerCombination == DrawerTypeCombination::MultiplicativeBlend)
    {
      return "MultiplicativeBlend";
    }
    else if (mDrawerCombination == DrawerTypeCombination::DefaultCombination)
    {
      return "DefaultCombination";
    }
    else if (mDrawerCombination == DrawerTypeCombination::DoNotInclude)
    {
      return "DoNotInclude";
    }
    else
    {
      return "DefaultCombination";
    }
  }

  std::string GraphicsView::GetDrawerType()
  {
    if (mDrawerType == DrawerTypes::GameForwardDrawer)
    {
      return "GameForwardDrawer";
    }
    else if (mDrawerType == DrawerTypes::DefaultDrawer)
    {
      return "DefaultDrawer";
    }
    else if (mDrawerType == DrawerTypes::ImguiDrawer)
    {
      return "ImguiDrawer";
    }
    else
    {
      return "DefaultDrawer";
    }
  }

  void GraphicsView::SetDrawerCombinationType(std::string aCombination)
  {
    DrawerTypeCombination dc;

    if ("AdditiveBlend" == aCombination)
    {
      dc = DrawerTypeCombination::AdditiveBlend;
    }
    else if ("AlphaBlend" == aCombination)
    {
      dc = DrawerTypeCombination::AlphaBlend;
    }
    else if ("Opaque" == aCombination)
    {
      dc = DrawerTypeCombination::Opaque;
    }
    else if ("MultiplicativeBlend" == aCombination)
    {
      dc = DrawerTypeCombination::MultiplicativeBlend;
    }
    else if ("DefaultCombination" == aCombination)
    {
      dc = DrawerTypeCombination::DefaultCombination;
    }
    else if ("DoNotInclude" == aCombination)
    {
      dc = DrawerTypeCombination::DoNotInclude;
    }
    else
    {
      dc = DrawerTypeCombination::AlphaBlend;
    }

    if (dc == mDrawerCombination)
    {
      return;
    }
    else
    {
      mDrawerCombination = dc;
    }

    if (mConstructing == false && mSpace->GetIsEditorSpace() == false)
    {
      mRenderer->SetViewCombinationType(this, mDrawerCombination);
    }
  }

  void GraphicsView::SetDrawerType(std::string aType)
  {
    DrawerTypes dt;

    if ("GameForwardDrawer" == aType)
    {
      dt = DrawerTypes::GameForwardDrawer;
    }
    else if ("DefaultDrawer" == aType)
    {
      dt = DrawerTypes::GameForwardDrawer;
    }
    else if ("ImguiDrawer" == aType)
    {
      dt = DrawerTypes::ImguiDrawer;
    }
    else
    {
      dt = DrawerTypes::GameForwardDrawer;
    }

    mDrawerType = dt;

    if (mConstructing == false)
    {
      mRenderer->SetViewDrawingType(this, mDrawerType, mDrawerCombination);
    }
  }
}