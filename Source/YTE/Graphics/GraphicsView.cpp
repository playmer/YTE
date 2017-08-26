#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

namespace YTE
{
  DefineType(GraphicsView)
  {
    YTERegisterType(GraphicsView);

    YTEBindField(&GraphicsView::mWindowName, "WindowName", PropertyBinding::GetSet)->AddAttribute<EditorProperty>();
  }

  GraphicsView::GraphicsView(Composition *aOwner, 
                             Space *aSpace, 
                             RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    auto engine = aSpace->GetEngine();
    mRenderer = engine->GetComponent<GraphicsSystem>()->GetRenderer();
    DeserializeByType<GraphicsView*>(aProperties, this, GraphicsView::GetStaticType());

    auto it = engine->GetWindows().find(mWindowName);

    mWindow = it->second.get();
  }

  void GraphicsView::Initialize()
  {

  }

  void GraphicsView::UpdateView(UBOView &aView)
  {
    mRenderer->UpdateViewBuffer(mWindow, aView);
  }
}