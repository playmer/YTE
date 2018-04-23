
#include "YTE/Core/Composition.hpp"
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Sprite.hpp"

#include "YTE/GameComponents/SteppingStone.hpp"

namespace YTE
{
  YTEDefineType(SteppingStone)
  {
    YTERegisterType(SteppingStone);
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

  SteppingStone::SteppingStone(Composition *aOwner, 
                                Space *aSpace, 
                                RSValue *aProperties)
    : Component{ aOwner, aSpace }
    , mSpriteOwner{ nullptr }
    , mLayer{ nullptr }
    , mK{ 4 }
    , mN{ 4 }
    , e{ r() }
  {
    YTEUnusedArgument(aProperties);
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

    mSpace->YTERegister(Events::LogicUpdate, this, &SteppingStone::Update);
  }

  static i64 Mod(i64 a, i64 b)
  {
    return ((a % b) + b) % b;
  }

  void SteppingStone::Update(LogicUpdate *aUpdate)
  {
    YTEUnusedArgument(aUpdate);
    mLayer->Begin("Stepping Stone");
    mLayer->InputInt("K", &mK);
    mLayer->InputInt("N", &mN);
    mLayer->End();

    if ((mColors.size() != mK) ||
        (mSprites.size() != mN))
    {
      AllocateObjects();
    }

    auto x = randomSizeT(0, mN - 1, e);
    auto y = randomSizeT(0, mN - 1, e);

    i64 i;
    i64 j;

    bool weGood = false;
    do
    {
      i = 0;
      j = 0;

      auto xAxis = randomSizeT(0, 2, e);
      auto yAxis = randomSizeT(0, 2, e);

      if (0 == xAxis)
      {
        --i;
      }
      else if (2 == xAxis)
      {
        ++i;
      }

      if (0 == yAxis)
      {
        --j;
      }
      else if (2 == yAxis)
      {
        ++j;
      }

      if (i != 0 || j != 0)
      {
        weGood = true;
      }

    } while (false == weGood);

    i += x;
    j += y;

    i = Mod(i, mN);
    j = Mod(j, mN);

    auto diffuse = mSprites[j][i]->GetComponent<Material>()->GetModelMaterial().GetDiffuse();

    mSprites[y][x]->GetComponent<Material>()->GetModelMaterial().SetDiffuse(diffuse);
  }

  static std::string cTexture{ "white.png" };

  void SteppingStone::AllocateObjects()
  {
    mSprites.clear();
    mColors.clear();

    if (nullptr != mSpriteOwner)
    {
      mSpriteOwner->Remove();
    }

    for (size_t i = 0; i < mK; ++i)
    {
      auto r = RandomFloat(0.0f, 1.0f, e);
      auto g = RandomFloat(0.0f, 1.0f, e);
      auto b = RandomFloat(0.0f, 1.0f, e);
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

        auto colorIndex = randomSizeT(0, mColors.size() - 1, e);
        material->GetModelMaterial().SetDiffuse(mColors[colorIndex]);

        sprites.emplace_back(textComposition);
      }
    }
  }
}