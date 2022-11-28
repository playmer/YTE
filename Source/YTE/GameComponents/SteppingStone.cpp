
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Sprite.hpp"

#include "YTE/GameComponents/SteppingStone.hpp"

namespace YTE
{
  YTEDefineType(SteppingStone)
  {
    RegisterType<SteppingStone>();
    TypeBuilder<SteppingStone> builder;
  }

  static inline float RandomFloat(float a, float b, std::default_random_engine &e)
  {
    std::uniform_real_distribution<float> dist{ a, b };

    return dist(e);
  }

  static inline size_t randomSizeT(size_t a, size_t b, std::default_random_engine &e)
  {
    std::uniform_int_distribution<size_t> dist{ a, b };

    return dist(e);
  }

  SteppingStone::SteppingStone(Composition *aOwner, Space *aSpace)
    : Component{ aOwner, aSpace }
    , mSpriteOwner{ nullptr }
    , mLayer{ nullptr }
    , mK{ 4 }
    , mN{ 2 }
    , mTimeToChange{ 0.75 }
    , mTimeTracker{ 0.75 }
    , mRandomEngine{ mRandomDevice() }
  {
  }

  void SteppingStone::Start()
  {
    // Add the imgui layer to the level.
    YTE::String imguiName{ "ImguiEditorLayer" };
    auto layer = mSpace->AddComposition<YTE::Composition>(imguiName,
                                                          mOwner->GetEngine(),
                                                          imguiName,
                                                          mSpace);

    if (layer->ShouldSerialize())
    {
      layer->ToggleSerialize();
    }

    layer->AddComponent(YTE::GraphicsView::GetStaticType());
    auto view = layer->GetComponent<YTE::GraphicsView>();
    view->SetOrder(100.f);
    view->SetClearColor(glm::vec4{ 0.f, 0.f, 0.f, 0.f });
    view->ChangeWindow(mSpace->GetComponent<GraphicsView>()->GetWindow());

    mLayer = static_cast<ImguiLayer*>(layer->AddComponent(TypeId<ImguiLayer>()));

    view->SetDrawerType("ImguiDrawer");

    mSpace->RegisterEvent<&SteppingStone::Update>(Events::LogicUpdate, this);
  }

  static i64 Mod(i64 a, i64 b)
  {
    return ((a % b) + b) % b;
  }
  
  void SteppingStone::ResetColors()
  {
    for (auto &spriteVec : mSprites)
    {
      for (auto &sprite : spriteVec)
      {
        auto material = sprite->GetComponent<Material>();

        auto colorIndex = randomSizeT(0, mColors.size() - 1, mRandomEngine);
        material->GetModelMaterial().SetDiffuse(mColors[colorIndex]);
      }
    }
  }

  void SteppingStone::Update(LogicUpdate *aUpdate)
  {
    UnusedArguments(aUpdate);

    auto previousTimeToChange = mTimeToChange;
    mLayer->Begin("Stepping Stone");
    mLayer->InputInt("K", &mK);
    mLayer->InputInt("N", &mN);
    mLayer->InputFloat("Seconds to Change", &mTimeToChange);
    auto reset = mLayer->Button("Reset");
    mLayer->End();

    if (previousTimeToChange != mTimeToChange)
    {
      mTimeTracker = 0.0f;
    }

    if ((mColors.size() != mK) ||
        (mSprites.size() != mN))
    {
      AllocateObjects();
    }

    if (reset)
    {
      ResetColors();
    }

    mTimeTracker -= static_cast<float>(aUpdate->Dt);

    if (0 > mTimeTracker)
    {
      mTimeTracker = mTimeToChange;
    }
    else
    {
      return;
    }

    auto x = randomSizeT(0, mN - 1, mRandomEngine);
    auto y = randomSizeT(0, mN - 1, mRandomEngine);

    i64 iOffset;
    i64 jOffset;

    bool weGood = false;
    do
    {
      iOffset = 0;
      jOffset = 0;

      auto xAxis = randomSizeT(0, 2, mRandomEngine);
      auto yAxis = randomSizeT(0, 2, mRandomEngine);

      if (0 == xAxis)
      {
        --iOffset;
      }
      else if (2 == xAxis)
      {
        ++iOffset;
      }

      if (0 == yAxis)
      {
        --jOffset;
      }
      else if (2 == yAxis)
      {
        ++jOffset;
      }

      if (iOffset != 0 || jOffset != 0)
      {
        weGood = true;
      }

    } while (false == weGood);

    iOffset += x;
    jOffset += y;

    auto i = Mod(iOffset, mN);
    auto j = Mod(jOffset, mN);

    auto diffuse = mSprites[j][i]->GetComponent<Material>()->GetModelMaterial().GetDiffuse();

    mSprites[y][x]->GetComponent<Material>()->GetModelMaterial().SetDiffuse(diffuse);

    //static int blah = 0;
    //printf("%d, px: %d, py: %d, nx`: %d, ny`: %d, nx: %d, ny: %d\n",
    //       ++blah,
    //       x,
    //       y,
    //       iOffset,
    //       jOffset,
    //       i,
    //       j);
  }

  static std::string cTexture{ "white.png" };

  void SteppingStone::AllocateObjects()
  {
    for (auto& vec : mSprites)
    {
      for (auto& composition : vec)
      {
        composition->Remove();
      }
    }

    mSprites.clear();
    mColors.clear();

    if (nullptr != mSpriteOwner)
    {
      mSpriteOwner->Remove();
    }

    for (size_t i = 0; i < mK; ++i)
    {
      auto r = RandomFloat(0.0f, 1.0f, mRandomEngine);
      auto g = RandomFloat(0.0f, 1.0f, mRandomEngine);
      auto b = RandomFloat(0.0f, 1.0f, mRandomEngine);
      mColors.emplace_back(glm::vec3{ r, g, b });
    }

    YTE::String ownerName{ fmt::format("SpriteOwner") };

    mSpriteOwner = mOwner->AddComposition<YTE::Composition>(mOwner,
                                                            ownerName,
                                                            mOwner->GetEngine(),
                                                            ownerName,
                                                            mSpace);

    for (size_t i = 0; i < mN; ++i)
    {
      mSprites.emplace_back(std::vector<Composition*>{});

      auto &sprites = mSprites.back();

      for (size_t j = 0; j < mN; ++j)
      {
        YTE::String compositionName{ fmt::format("Sprites_{}_{}", i, j) };

        auto textComposition = mSpriteOwner->AddComposition<YTE::Composition>(mSpriteOwner,
                                                                              compositionName,
                                                                              mSpriteOwner->GetEngine(),
                                                                              compositionName,
                                                                              mSpace);

        // add the camera component to the camera object
        textComposition->AddComponent(TypeId<Transform>());
        textComposition->AddComponent(TypeId<Sprite>());

        auto transform = textComposition->GetComponent<Transform>();
        transform->SetTranslation({ static_cast<float>(j - (mN / 2.0f)), 
                                    static_cast<float>(i - (mN / 2.0f)), 
                                    0.f });

        auto sprite = textComposition->GetComponent<Sprite>();

        sprite->SetTexture(cTexture);

        auto material = static_cast<Material*>(textComposition->AddComponent(TypeId<Material>()));

        auto colorIndex = randomSizeT(0, mColors.size() - 1, mRandomEngine);
        material->GetModelMaterial().SetDiffuse(mColors[colorIndex]);

        sprites.emplace_back(textComposition);
      }
    }
  }
}