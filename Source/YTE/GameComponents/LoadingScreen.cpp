#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Actions/Action.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"

#include "YTE/GameComponents/LoadingScreen.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/SpriteText.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  YTEDefineType(LoadingScreen)
  {
    YTERegisterType(LoadingScreen);

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  LoadingScreen::LoadingScreen(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mCurrentNumber{0}
    , mLastLine{nullptr}
  {
    YTEUnusedArgument(aProperties);
  }

  void LoadingScreen::Start()
  {
    CreateLineInternal("Loading..", 800.f);

    mSpace->YTERegister(Events::FrameUpdate, this, &LoadingScreen::Update);
  }
  
  void LoadingScreen::Update(LogicUpdate *aUpdate)
  {
    auto transform = mLastLine->GetComponent<Transform>();
    auto rotation = transform->GetRotation();

    auto delta = AroundAxis({ 0.f, 0.f, 1.f }, static_cast<float>(aUpdate->Dt * 3.14 / 2.f));

    transform->SetRotation(delta * rotation);
  }

  static const std::string cAlignment{ "Center" };
  
  void LoadingScreen::CreateLineInternal(char const *aString, float aFontSize)
  {
    std::string text = aString;
    YTE::String compositionName{ fmt::format("CreditsScroll_{}", mCurrentNumber++) };

    auto textComposition = mOwner->AddComposition<YTE::Composition>(mOwner,
                                                                    compositionName,
                                                                    mOwner->GetEngine(),
                                                                    compositionName,
                                                                    mSpace);

    // add the camera component to the camera object
    textComposition->AddComponent(TypeId<Transform>());
    textComposition->AddComponent(TypeId<SpriteText>());

    auto transform = textComposition->GetComponent<Transform>();
    transform->SetTranslation({ 0.f, 0.f, 0.f });

    auto spriteText = textComposition->GetComponent<SpriteText>();

    spriteText->SetAlignmentX(cAlignment);
    spriteText->SetFontSize(aFontSize);
    spriteText->SetText(text);

    mLastLine = textComposition;
  }
}