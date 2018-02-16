#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDrawerTypeDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::vector<std::string> result
    {
      "DefaultDrawer",
      "GameForwardDrawer"
    };
    return result;
  }

  static std::vector<std::string> PopulateCombinationTypeDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

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

  YTEDefineType(GraphicsView)
  {
    YTERegisterType(GraphicsView);

    GetStaticType()->AddAttribute<RunInEditor>();

    YTEBindField(&GraphicsView::mWindowName, "WindowName", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&GraphicsView::GetOrder, &GraphicsView::SetOrder, "Order")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The order to render the views. We render lowest to highest.");

    YTEBindProperty(&GraphicsView::GetClearColor, &GraphicsView::SetClearColor, "ClearColor")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The color the screen will be painted before rendering, defaults to gray.");

    YTEBindProperty(&GraphicsView::GetDrawerType, &GraphicsView::SetDrawerType, "DrawerType")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDrawerTypeDropDownList);
    YTEBindProperty(&GraphicsView::GetDrawerCombinationType, &GraphicsView::SetDrawerCombinationType, "DrawerCombination")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateCombinationTypeDropDownList);
  }

  GraphicsView::GraphicsView(Composition *aOwner, 
                             Space *aSpace, 
                             RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mLastCamera(nullptr)
    , mWindow(nullptr)
    , mClearColor(0.22f, 0.22f, 0.22f, 1.0f)
    , mConstructing(true)
    , mOrder(0.0f)
    , mInitialized(false)
    , mDrawerCombination(YTEDrawerTypeCombination::DefaultCombination)
    , mDrawerType(YTEDrawerTypes::DefaultDrawer)
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();

    DeserializeByType(aProperties, this, GetStaticType());

    auto it = engine->GetWindows().find(mWindowName);

    if (it != engine->GetWindows().end())
    {
      mWindow = it->second.get();
    }
  }

  GraphicsView::~GraphicsView()
  {
    mRenderer->DeregisterView(this);
  }

  void GraphicsView::NativeInitialize()
  {
    if (mWindow == nullptr)
    {
      return;
    }

    mRenderer->RegisterView(this, mDrawerType, mDrawerCombination);

    auto engine = mSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();

    auto it = engine->GetWindows().find(mWindowName);

    if (it != engine->GetWindows().end())
    {
      mWindow = it->second.get();
    }

    mConstructing = false;
    mWindow->mKeyboard.YTERegister(Events::KeyPress, this, &GraphicsView::KeyPressed);

    SetClearColor(mClearColor);
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
    mLastCamera = aCamera;
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
    if (false == mConstructing)
    {
      mRenderer->DeregisterView(this);
    }

    mWindowName = aWindowName;
    auto it = mSpace->GetEngine()->GetWindows().find(mWindowName);

    mWindow = it->second.get();

    if (false == mConstructing)
    {
      mRenderer->RegisterView(this);
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
    if (mDrawerCombination == YTEDrawerTypeCombination::AdditiveBlend)
    {
      return "AdditiveBlend";
    }
    else if (mDrawerCombination == YTEDrawerTypeCombination::AlphaBlend)
    {
      return "AlphaBlend";
    }
    else if (mDrawerCombination == YTEDrawerTypeCombination::Opaque)
    {
      return "Opaque";
    }
    else if (mDrawerCombination == YTEDrawerTypeCombination::MultiplicativeBlend)
    {
      return "MultiplicativeBlend";
    }
    else if (mDrawerCombination == YTEDrawerTypeCombination::DefaultCombination)
    {
      return "DefaultCombination";
    }
    else if (mDrawerCombination == YTEDrawerTypeCombination::DoNotInclude)
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
    if (mDrawerType == YTEDrawerTypes::GameForwardDrawer)
    {
      return "GameForwardDrawer";
    }
    else if (mDrawerType == YTEDrawerTypes::DefaultDrawer)
    {
      return "DefaultDrawer";
    }
    else
    {
      return "DefaultDrawer";
    }
  }

  void GraphicsView::SetDrawerCombinationType(std::string aCombination)
  {
    YTEDrawerTypeCombination dc;

    if (false == (aCombination != "AdditiveBlend"))
    {
      dc = YTEDrawerTypeCombination::AdditiveBlend;
    }
    else if (false == (aCombination != "AlphaBlend"))
    {
      dc = YTEDrawerTypeCombination::AlphaBlend;
    }
    else if (false == (aCombination != "Opaque"))
    {
      dc = YTEDrawerTypeCombination::Opaque;
    }
    else if (false == (aCombination != "MultiplicativeBlend"))
    {
      dc = YTEDrawerTypeCombination::MultiplicativeBlend;
    }
    else if (false == (aCombination != "DefaultCombination"))
    {
      dc = YTEDrawerTypeCombination::DefaultCombination;
    }
    else if (false == (aCombination != "DoNotInclude"))
    {
      dc = YTEDrawerTypeCombination::DoNotInclude;
    }
    else
    {
      dc = YTEDrawerTypeCombination::AlphaBlend;
    }

    if (dc == mDrawerCombination)
    {
      return;
    }
    else
    {
      mDrawerCombination = dc;
    }

    if (mConstructing == false)
    {
      mRenderer->SetViewCombinationType(this, mDrawerCombination);
    }
  }

  void GraphicsView::SetDrawerType(std::string aType)
  {
    YTEDrawerTypes dt;

    if (false == (aType != "GameForwardDrawer"))
    {
      dt = YTEDrawerTypes::GameForwardDrawer;
    }
    else if (false == (aType != "DefaultDrawer"))
    {
      dt = YTEDrawerTypes::GameForwardDrawer;
    }
    else
    {
      dt = YTEDrawerTypes::GameForwardDrawer;
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