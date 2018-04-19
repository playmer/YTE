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
    , mY{200.f}
    , mCurrentNumber{0}
    , mDone{false}
    , mFadeFinished(false)
    , mFadeValue(0.0f)
    , mBlackoutView(nullptr)
    , mFirstLine(nullptr)
    , mLastLine{nullptr}
  {
    YTEUnusedArgument(aProperties);
  }

  void Credits::Start()
  {
    mBlackoutView = mSpace->AddChildSpace("MSR_Blackout")->GetComponent<GraphicsView>();
    mMyView = mSpace->GetComponent<GraphicsView>();

    if (mMyView)
    {
      mMyView->SetOrder(-1.0f);
    }

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


    CreateLineInternal("DigiPen Institute of Technology", 800.f);
    mY -= 25;

    CreateLineInternal("Presents", 400.f);
    CreateLineInternal("A Josh's Farm Production", 600.f);
    mY -= 25;

    CreateLineInternal("LambPlanet", 800.f);
    mY -= 50;

    CreateLineInternal("www.digipen.edu", 200.f);
    CreateLine("All content (c) 2018 DigiPen (USA) Corporation, all rights reserved.");
    mY -= initialMovement;


    CreateHeader("President of the School");
    CreateLine("Claude Comair");

    CreateHeader("Executives");
    CreateLine("John Bauer");
    CreateLine("Jason Chu");
    CreateLine("Michele Comair");
    CreateLine("Prasanna Ghali");
    CreateLine("Melvin Gonsalvez");
    CreateLine("Angela Kugler");
    CreateLine("Xin Li");
    CreateLine("Meighan McKelvey");
    CreateLine("Samir Abu Samra");
    CreateLine("Raymond Yan");

    CreateHeader("Game Professors");
    CreateLine("Jo Cronk");
    CreateLine("Zahra Haghiri");;
    CreateLine("Christopher Orth");
    CreateLine("Brigitte Samson");
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
    CreateLine("Audacity");
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
    CreateLine("Lars Aarhus");
    CreateLine("Barnabas Bede");
    CreateLine("Nathan Carlson");
    CreateLine("Dearest Floom");
    CreateLine("Connor Deakin");
    CreateLine("Aaron Kitchen");
    CreateLine("Austin Jensen");
    CreateLine("Matthew Oakes");
    CreateLine("Miles Marchant");
    CreateLine("Trevor Sundberg");
    CreateLine("Chris Underwood");

    CreateHeader("In Memory Of");
    CreateLine("Thomas C. Fisher");

    mY -= initialMovement;

    CreateLine("Powered by Wwise (c) 2006 - 2018 Audiokinetic Inc. All rights reserved.");

    CreateHeader("Additional Copyrights");
    CreateLine("Assimp Copyright (c) 2006-2018, assimp team. All rights reserved.");
    CreateLine("fmt Copyright (c) 2012 - 2018, Victor Zverovich. All rights reserved.");
    CreateLine("crunch Copyright (c) 2010-2018 Richard Geldreich, Jr. and Binomial LLC");
    CreateLine("glm Copyright (c) 2005 - 2018 G-Truc Creation");
    CreateLine("ImGui Copyright (c) 2014-2018 Omar Cornut");
    CreateLine("ImGuizmo Copyright(c) 2018 Cedric Guillemet");
    CreateLine("KissFFT Copyright (c) 2003-2018 Mark Borgerding");
    CreateLine("VkHLF Copyright (c) 2016-2018, NVIDIA CORPORATION. All rights reserved.");

    //mFirstLine->GetComponent<SpriteText>()->SetVisibility(false);
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
      //mFirstLine->GetComponent<SpriteText>()->SetVisibility(true);
      mMyView->SetOrder(2.0f);
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
      if (mBlackoutView)
      {
        auto clearColor = mBlackoutView->GetClearColor();

        mBlackoutView->SetClearColor(glm::vec4(clearColor.x, clearColor.y, clearColor.z, mFadeValue));
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

  bool Credits::GetDone()
  {
    return mDone;
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