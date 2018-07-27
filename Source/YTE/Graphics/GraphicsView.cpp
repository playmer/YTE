#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Camera.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
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

    builder.Field<&GraphicsView::mWindowName>( "WindowName", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&GraphicsView::GetOrder, &GraphicsView::SetOrder>( "Order")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The order to render the views. We render lowest to highest.");

    builder.Property<&GraphicsView::GetSuperSampling, &GraphicsView::SetSuperSampling>( "SuperSampling")
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Determines the Super Sampling rate of the view. Must be a power of 2.");

    builder.Property<&GraphicsView::GetClearColor, &GraphicsView::SetClearColor>( "ClearColor")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>()
      .SetDocumentation("The color the screen will be painted before rendering, defaults to gray.");

    builder.Property<&GraphicsView::GetDrawerType, &GraphicsView::SetDrawerType>( "DrawerType")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDrawerTypeDropDownList);
    builder.Property<&GraphicsView::GetDrawerCombinationType, &GraphicsView::SetDrawerCombinationType>( "DrawerCombination")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateCombinationTypeDropDownList);
  }

  GraphicsView::GraphicsView(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mActiveCamera(nullptr)
    , mDrawerCombination(DrawerTypeCombination::DefaultCombination)
    , mDrawerType(DrawerTypes::DefaultDrawer)
    , mWindow(nullptr)
    , mClearColor(0.22f, 0.22f, 0.22f, 1.0f)
    , mSuperSampling(1)
    , mOrder(0.0f)
    , mConstructing(true)
    , mInitialized(false)
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();

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

    mRenderer->RegisterView(this, mDrawerType, mDrawerCombination);

    mWindow->mKeyboard.RegisterEvent<&GraphicsView::KeyPressed>(Events::KeyPress, this);

    SetClearColor(mClearColor);
    mConstructing = false;
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
  }


  void GraphicsView::KeyPressed(KeyboardEvent *aUpdate)
  {
    if (aUpdate->Key == Keys::F1)
    {
      SetOrder(-mOrder);
    }
  }

  void GraphicsView::UpdateView(Camera *aCamera, UBOView &aView)
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

    mRenderer->UpdateWindowViewBuffer(this, aView);
  }
  
  void GraphicsView::UpdateIllumination(UBOIllumination& aIllumination)
  {
    mRenderer->UpdateWindowIlluminationBuffer(this, aIllumination);
  }

  glm::vec4 GraphicsView::GetClearColor()
  {
    if (nullptr == mWindow)
    {
      return mClearColor;
    }

    return mRenderer->GetClearColor(this);
  }

  void GraphicsView::ChangeWindow(const std::string &aWindowName)
  {
    ViewChanged event;
    event.View = this;
    event.Window = nullptr;

    if (false == mConstructing)
    {
      SendEvent(Events::SurfaceLost, &event);
      mRenderer->DeregisterView(this);
    }

    mWindowName = aWindowName;
    auto it = mSpace->GetEngine()->GetWindows().find(mWindowName);

    mWindow = it->second.get();

    event.Window = mWindow;

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
      event.Window = aWindow;
      mWindow = aWindow;
      NativeInitialize();
      SendEvent(Events::SurfaceGained, &event);
      return;
    }

    event.Window = nullptr;

    if (false == mConstructing)
    {
      SendEvent(Events::SurfaceLost, &event);
      mRenderer->DeregisterView(this);
    }

    mWindow = aWindow;
    mWindowName = aWindow->mName;
    event.Window = mWindow;

    if (false == mConstructing)
    {
      mRenderer->RegisterView(this);
      SendEvent(Events::SurfaceGained, &event);
    }
  }

  void GraphicsView::SetClearColor(const glm::vec4 &aColor)
  {
    mClearColor = aColor;

    if (nullptr == mWindow)
    {
      return;
    }

    mRenderer->SetClearColor(this, aColor);
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

    if (dt == mDrawerType)
    {
      return;
    }
    else
    {
      mDrawerType = dt;
    }

    if (mConstructing == false)
    {
      mRenderer->SetViewDrawingType(this, mDrawerType, mDrawerCombination);
    }
  }
}