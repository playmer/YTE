#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"
#include "YTE/Core/Actions/Action.hpp"
#include "YTE/Core/Actions/ActionManager.hpp"

#include "YTE/GameComponents/Credits.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/SpriteText.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Platform/Window.hpp"

namespace YTE
{
  YTEDefineType(Credits)
  {
    YTERegisterType(Credits);

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  Credits::Credits(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDelay{0.5f}
    , mY{0.f}
    , mCurrentNumber{0}
    , mDone{false}
    , mFadeFinished(false)
    , mFadeValue(0.0f)
    , mFirstLine(nullptr)
    , mLastLine{nullptr}
  {
    YTEUnusedArgument(aProperties);
  }

  void Credits::Start()
  {
    float initialMovement = 400.f;
    auto view = mSpace->GetComponent<GraphicsView>();

    if (view)
    {
      auto window = view->GetWindow();

      if (window)
      {
        initialMovement = static_cast<float>(window->GetHeight());
      }
    }

    
    CreateLineInternal("LambPlanet", 800.f);
    mY -= initialMovement;


    CreateHeader("President of the School");
    CreateLine("Claude Comair");

    CreateHeader("Game Professors");
    CreateLine("Christopher Orth");
    CreateLine("Jo Cronk");
    CreateLine("Brigitte Samson");
    CreateLine("Zahra Haghiri");
    CreateLine("Lawrence Schwedler");
    CreateLine("Bryan Schmidt");
    CreateLine("Steven Saulls");

    CreateHeader("Producer");
    CreateLine("Nicholas Ammann");

    CreateHeader("Art Director");
    CreateLine("Trevor Barrett");

    CreateHeader("Game Director");
    CreateLine("Jonathan Ackerman");

    CreateHeader("Technical Director");
    CreateLine("Joshua T. Fisher");

    CreateHeader("Art Team");
    CreateLine("Trevor Barrett");
    CreateLine("Amanda Krum");
    CreateLine("Candice Kang");
    CreateLine("Tabatha Tipton");

    CreateHeader("Design Team");
    CreateLine("Jonathan Ackerman");
    CreateLine("Jonathan Frederick Williams");

    CreateHeader("Engine Team");
    CreateLine("Nicholas Ammann");
    CreateLine("Gabryelle Chamberlin");
    CreateLine("Evan Collier");
    CreateLine("Isaac Dayton");
    CreateLine("Joshua T. Fisher");
    CreateLine("Andrew Griffin");

    CreateHeader("Composer and Sound Designer");
    CreateLine("Kathy Strebel");

    CreateHeader("Voice Actors");
    CreateLine("Farmer Daisy - Brooke Robertson");
    CreateLine("Basil Bouillon - Charlie Wells");
    CreateLine("Chef John - Dustin Williams");

    CreateHeader("Instrumentalists");
    CreateLine("Guitar - Dustin Williams");
    CreateLine("Tenor Saxophone - Drake Parker");
    CreateLine("Flute - Kathy Strebel");


    CreateHeader("Thanks To The Following Programs/Libraries");
    CreateLine("Assimp");
    CreateLine("Bullet Physics");
    CreateLine("crunch");
    CreateLine("Cubase");
    CreateLine("easy_profiler");
    CreateLine("fmt");
    CreateLine("glm");
    CreateLine("ImGui");
    CreateLine("ImGuizmo");
    CreateLine("KissFFT");
    CreateLine("Native Instruments");
    CreateLine("Qt");
    CreateLine("RenderDoc");
    CreateLine("WWise");
    CreateLine("Visual Studio");
    CreateLine("VkHLF");

    CreateHeader("Special Thanks");
    CreateLine("Nathan Carlson");
    CreateLine("Aaron Kitchen");
    CreateLine("Austin Jensen");
    CreateLine("Matthew Oakes");
    CreateLine("Miles Marchant");
    CreateLine("Trevor Sundberg");

    CreateHeader("In Memory Of");
    CreateLine("Thomas C. Fisher");

    mY -= initialMovement;

    CreateLine("All content (c) 2018 DigiPen (USA) Corporation, all rights reserved.");

    mFirstLine->GetComponent<SpriteText>()->SetVisibility(false);
    mSpace->YTERegister(Events::StartCredits, this, &Credits::OnStartCredits);
  }

  void Credits::OnStartCredits(StartCredits *)
  {
    mOwner->GetComponent<Transform>()->SetWorldTranslation(0.0f, 0.0f, 0.0f);
    mDone = false;
    mDelay = 0.5f;

    mFadeFinished = false;
    mSpace->GetParent()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Disabled);
    mSpace->YTERegister(Events::LogicUpdate, this, &Credits::Update);

    auto actionManager = mSpace->GetComponent<ActionManager>();
    ActionSequence fadeOutSeq;
    
    fadeOutSeq.Add<Quad::easeInOut>(mFadeValue, 1.1f, 0.5f);

    fadeOutSeq.Call([this]() {
      mFirstLine->GetComponent<SpriteText>()->SetVisibility(true);
      mFadeFinished = true;
    });

    actionManager->AddSequence(mOwner, fadeOutSeq);
  }

  void Credits::Update(LogicUpdate *aUpdate)
  {
    if (mFadeFinished)
    {
      mDelay -= static_cast<float>(aUpdate->Dt);

      if (0.f < mDelay)
      {
        return;
      }

      auto transform = mOwner->GetComponent<Transform>();
      auto translation = transform->GetWorldTranslation();
      translation.y += static_cast<float>(200.0000 * aUpdate->Dt);

      transform->SetWorldTranslation(translation);

      if (mLastLine)
      {
        float height = 1080.f;
        auto view = mSpace->GetComponent<GraphicsView>();

        if (view)
        {
          auto window = view->GetWindow();

          if (window)
          {
            height = static_cast<float>(window->GetHeight());
          }
        }

        auto spriteTransform = mLastLine->GetComponent<Transform>();
        auto spriteTranslation = spriteTransform->GetWorldTranslation();

        if (height < spriteTranslation.y)
        {
          mDone = true;

          auto actionManager = mSpace->GetComponent<ActionManager>();
          ActionSequence fadeInSeq;
          mFadeFinished = false;

          fadeInSeq.Add<Quad::easeInOut>(mFadeValue, -0.1f, 0.5f);

          fadeInSeq.Call([this]() {
            mSpace->GetParent()->GetComponent<InputInterpreter>()->SetInputContext(InputInterpreter::InputContext::Menu);
            mSpace->YTEDeregister(Events::LogicUpdate, this, &Credits::Update);
          });

          actionManager->AddSequence(mOwner, fadeInSeq);
        }
      }
    }

    else
    {
      auto view = mSpace->GetComponent<GraphicsView>();

      if (view)
      {
        auto clearColor = view->GetClearColor();

        view->SetClearColor(glm::vec4(clearColor.x, clearColor.y, clearColor.z, mFadeValue));
      }
    }
  }

  static const std::string cAlignment{ "Center" };

  void Credits::CreateHeader(char const *aString)
  {
    mY -= 100.f;
    CreateLineInternal(aString, 650.0f);
    mY -= 100.f;
  }

  void Credits::CreateLine(char const *aString)
  {
    CreateLineInternal(aString, 400.0f);
  }

  void Credits::CreateLineInternal(char const *aString, float aFontSize)
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
    transform->SetTranslation({ 0.f, mY, 0.f });

    mY -= 50.0000;

    auto spriteText = textComposition->GetComponent<SpriteText>();

    spriteText->SetAlignmentX(cAlignment);
    spriteText->SetFontSize(aFontSize);
    spriteText->SetText(text);

    if (!mFirstLine)
    {
      mFirstLine = textComposition;
    }

    mLastLine = textComposition;
  }
}