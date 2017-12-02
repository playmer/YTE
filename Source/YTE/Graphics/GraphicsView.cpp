#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
  YTEDefineType(GraphicsView)
  {
    YTERegisterType(GraphicsView);

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
      .SetDocumentation("The color the screen will be painted before rendering, defaults to grey.");
  }



  GraphicsView::GraphicsView(Composition *aOwner, 
                             Space *aSpace, 
                             RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mLastCamera(nullptr)
    , mWindow(nullptr)
    , mClearColor(0.44f, 0.44f, 0.44f, 1.0f)
    , mConstructing(true)
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();

    DeserializeByType<GraphicsView*>(aProperties, this, GraphicsView::GetStaticType());

    auto it = engine->GetWindows().find(mWindowName);

    if (it != engine->GetWindows().end()) 
    {
      mWindow = it->second.get();
    }

    mConstructing = false;
  }

  GraphicsView::~GraphicsView()
  {
    mRenderer->DeregisterView(this);
  }

  void GraphicsView::Initialize()
  {
    mRenderer->RegisterView(this);
    SetClearColor(mClearColor);
    mSpace->YTERegister(Events::LogicUpdate, this, &GraphicsView::Update);
  }


  void GraphicsView::Update(LogicUpdate *aUpdate)
  {
    if (mWindow->mKeyboard.IsKeyDown(Keys::F2))
    {
      mOrder = -mOrder;
    }
  }

  void GraphicsView::UpdateView(Camera *aCamera, UBOView &aView)
  {
    mLastCamera = aCamera;
    mRenderer->UpdateWindowViewBuffer(this, aView);
  }

  glm::vec4 GraphicsView::GetClearColor()
  {
    if (nullptr == mWindow)
    {
      return mClearColor;
    }

    return mRenderer->GetClearColor(this);
  }

  void GraphicsView::ChangeWindow(const std::string & aWindowName)
  {
    mWindowName = aWindowName;
    auto it = mSpace->GetEngine()->GetWindows().find(mWindowName);

    mWindow = it->second.get();
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
    mOrder = aOrder;

    if (false == mConstructing)
    {
      mRenderer->ViewOrderChanged(this, mOrder, aOrder);
    }
  }
}