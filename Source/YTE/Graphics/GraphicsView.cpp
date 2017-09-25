#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
  YTEDefineType(GraphicsView)
  {
    YTERegisterType(GraphicsView);

    YTEBindField(&GraphicsView::mWindowName, "WindowName", PropertyBinding::GetSet).AddAttribute<EditorProperty>();

    YTEBindProperty(&GraphicsView::GetClearColor, &GraphicsView::SetClearColor, "ClearColor").AddAttribute<EditorProperty>();
  }

  GraphicsView::GraphicsView(Composition *aOwner, 
                             Space *aSpace, 
                             RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mLastCamera(nullptr)
    , mWindow(nullptr)
    , mClearColor(0.44f, 0.44f, 0.44f, 1.0f)
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();

    DeserializeByType<GraphicsView*>(aProperties, this, GraphicsView::GetStaticType());

    auto it = engine->GetWindows().find(mWindowName);

    mWindow = it->second.get();
  }

  void GraphicsView::Initialize()
  {
    SetClearColor(mClearColor);
  }

  void GraphicsView::UpdateView(Camera *aCamera, UBOView &aView)
  {
    mLastCamera = aCamera;
    mRenderer->UpdateViewBuffer(mWindow, aView);
  }


  glm::vec4 GraphicsView::GetClearColor()
  {
    if (nullptr == mWindow)
    {
      return glm::vec4{};
    }

    return mRenderer->GetClearColor(mWindow);
  }

  void GraphicsView::SetClearColor(const glm::vec4 &aColor)
  {
    if (nullptr == mWindow)
    {
      mClearColor = aColor;
      return;
    }

    mRenderer->SetClearColor(mWindow, aColor);
  }

}