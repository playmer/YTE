#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Actions/Action.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"

#include "YTE/GameComponents/MakeLoadingScreen.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/SpriteText.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  YTEDefineType(MakeLoadingScreen)
  {
    YTERegisterType(MakeLoadingScreen);

    std::vector<std::vector<Type*>> deps = { { TypeId<GraphicsView>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  MakeLoadingScreen::MakeLoadingScreen(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void MakeLoadingScreen::NativeInitialize()
  {
    String hud{ "MSR_Loading" };
    mChild = mSpace->AddChildSpace(hud, true);
    mSpace->GetComponent<GraphicsView>()->SetOrder(-1000.f);
    mSpace->YTERegister(Events::FrameUpdate, this, &MakeLoadingScreen::Update);
  }

  void MakeLoadingScreen::Update(LogicUpdate *aUpdate)
  {
    if (mSpace->GetFinishedLoading())
    {
      mSpace->RemoveComposition(mChild);

      Remove();
    }
  }
}