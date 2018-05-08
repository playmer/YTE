///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/InfluenceMap.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::vector<std::string> result{ "Linear", "Squared", "Cubic", "Logarithmic" };
    return result;
  }

  YTEDefineType(InfluenceMap)
  {
    RegisterType<InfluenceMap>();
    TypeBuilder<InfluenceMap> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    builder.Property<&InfluenceMap::GetRadius, &InfluenceMap::SetRadius>( "Radius")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&InfluenceMap::GetColor, &InfluenceMap::SetColor>( "Color")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();

    builder.Property<&InfluenceMap::GetDebugDraw, &InfluenceMap::SetDebugDraw>( "DebugDraw")
      .AddAttribute<EditorProperty>();

    builder.Property<&InfluenceMap::GetColorIntensity, &InfluenceMap::SetColorIntensity>( "ColorIntensity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&InfluenceMap::GetWaveIntensity, &InfluenceMap::SetWaveIntensity>( "Wave Intensity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&InfluenceMap::GetActive, &InfluenceMap::SetActive>( "Active")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&InfluenceMap::GetColorInfluenceFunction, &InfluenceMap::SetColorInfluenceFunction>( "ColorInfluenceFunction")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    builder.Property<&InfluenceMap::GetWaveInfluenceFunction, &InfluenceMap::SetWaveInfluenceFunction>( "WaveInfluenceFunction")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);
  }



  InfluenceMap::InfluenceMap(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mEngine(nullptr)
    , mRenderer(nullptr)
    , mGraphicsView(nullptr)
    , mTransform(nullptr)
    , mInstantiatedInfluenceMap(nullptr)
    , mConstructing(true)
    , mSetTransform(false)
    , mUseTemp(false)
    , mDebugDraw(false)
  {
    mEngine = aSpace->GetEngine();
    mRenderer = mEngine->GetComponent<GraphicsSystem>()->GetRenderer();
    mGraphicsView = aSpace->GetComponent<GraphicsView>();

    DeserializeByType(aProperties, this, GetStaticType());
  }



  InfluenceMap::~InfluenceMap()
  {
    Destroy();
  }



  void InfluenceMap::Initialize()
  {
    mOwner->RegisterEvent<&InfluenceMap::TransformUpdate>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&InfluenceMap::TransformUpdate>(Events::RotationChanged, this);
    mOwner->RegisterEvent<&InfluenceMap::TransformUpdate>(Events::ScaleChanged, this);
    mEngine->RegisterEvent<&InfluenceMap::Update>(Events::LogicUpdate, this);

    mTransform = mOwner->GetComponent<Transform>(); 
    mDrawer = std::make_unique<LineDrawer>(mOwner->GetGUID().ToIdentifierString(), mGraphicsView->GetRenderer(), mGraphicsView);
    mConstructing = false;
    Create();
  }



  void InfluenceMap::TransformUpdate(TransformChanged* aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetCenter(mTransform->GetTranslation());

      if (mDebugDraw)
      {
        // corners
        glm::vec3 bl, br, tl, tr;

        bl.x = mInstantiatedInfluenceMap->GetCenter().x - mInstantiatedInfluenceMap->GetRadius();
        br.x = mInstantiatedInfluenceMap->GetCenter().x + mInstantiatedInfluenceMap->GetRadius();
        tl.x = bl.x;
        tr.x = br.x;

        bl.z = mInstantiatedInfluenceMap->GetCenter().z - mInstantiatedInfluenceMap->GetRadius();
        tl.z = mInstantiatedInfluenceMap->GetCenter().z + mInstantiatedInfluenceMap->GetRadius();
        tr.z = tl.z;
        br.z = bl.z;

        tl.y = bl.y = tr.y = br.y = mInstantiatedInfluenceMap->GetCenter().y;

        mDrawer->Start();
        mDrawer->AddLine(tl, bl, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(bl, br, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(br, tr, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(tr, tl, mInstantiatedInfluenceMap->GetColor());
        mDrawer->End();
      }
    }
    else
    {
      mMapTemp.mCenter = mTransform->GetTranslation();
      mUseTemp = true;
    }
  }



  void InfluenceMap::Update(LogicUpdate* aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mTransform && mSetTransform == false)
    {
      mEngine->DeregisterEvent<&InfluenceMap::Update>(Events::LogicUpdate,  this);
      if (mInstantiatedInfluenceMap)
      {
        mInstantiatedInfluenceMap->SetCenter(mTransform->GetTranslation());
      }
      else
      {
        mMapTemp.mCenter = mTransform->GetTranslation();
        mUseTemp = true;
      }
      mSetTransform = true;
    }
  }


  void InfluenceMap::SetMapSourceInformation(UBOWaterInfluenceMap& aMap)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetInfluenceMapInformation(aMap);
    }
    else
    {
      mMapTemp = aMap;
      mUseTemp = true;
    }
  }



  void InfluenceMap::SetColor(glm::vec3& aColor)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetColor(aColor);
      if (mDebugDraw)
      {
        // corners
        glm::vec3 bl, br, tl, tr;

        bl.x = mInstantiatedInfluenceMap->GetCenter().x - mInstantiatedInfluenceMap->GetRadius();
        br.x = mInstantiatedInfluenceMap->GetCenter().x + mInstantiatedInfluenceMap->GetRadius();
        tl.x = bl.x;
        tr.x = br.x;

        bl.z = mInstantiatedInfluenceMap->GetCenter().z - mInstantiatedInfluenceMap->GetRadius();
        tl.z = mInstantiatedInfluenceMap->GetCenter().z + mInstantiatedInfluenceMap->GetRadius();
        tr.z = tl.z;
        br.z = bl.z;

        tl.y = bl.y = tr.y = br.y = mInstantiatedInfluenceMap->GetCenter().y;

        mDrawer->Start();
        mDrawer->AddLine(tl, bl, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(bl, br, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(br, tr, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(tr, tl, mInstantiatedInfluenceMap->GetColor());
        mDrawer->End();
      }
    }
    else
    {
      mMapTemp.mColor = aColor;
      mUseTemp = true;
    }
  }


 
  void InfluenceMap::SetColorIntensity(float aIntensity)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetColorIntensity(aIntensity);
    }
    else
    {
      mMapTemp.mColorIntensity = aIntensity;
      mUseTemp = true;
    }
  }


  void InfluenceMap::SetWaveIntensity(float aIntensity)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetWaveIntensity(aIntensity);
    }
    else
    {
      mMapTemp.mWaveIntensity = aIntensity;
      mUseTemp = true;
    }
  }



  void InfluenceMap::SetActive(bool aActive)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetActive(aActive);
    }
    else
    {
      mMapTemp.mActive = aActive;
      mUseTemp = true;
    }
  }



  void InfluenceMap::SetColorInfluenceFunction(std::string aFunction)
  {
    unsigned int fn = 0;

    if ("Linear" == aFunction)
    {
      fn = 0;
    }
    else if ("Squared" == aFunction)
    {
      fn = 1;
    }
    else if ("Cubic" == aFunction)
    {
      fn = 2;
    }
    else if ("Logarithmic" == aFunction)
    {
      fn = 3;
    }
    else
    {
      fn = 0;
    }

    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetColorInfluenceFunction(fn);
    }
    else
    {
      mMapTemp.mColorInfluenceFunction = fn;
      mUseTemp = true;
    }
  }



  void InfluenceMap::SetWaveInfluenceFunction(std::string aFunction)
  {
    unsigned int fn = 0;

    if ("Linear" == aFunction)
    {
      fn = 0;
    }
    else if ("Squared" == aFunction)
    {
      fn = 1;
    }
    else if ("Cubic" == aFunction)
    {
      fn = 2;
    }
    else if ("Logarithmic" == aFunction)
    {
      fn = 3;
    }
    else
    {
      fn = 0;
    }

    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetWaveInfluenceFunction(fn);
    }
    else
    {
      mMapTemp.mWaveInfluenceFunction = fn;
      mUseTemp = true;
    }
  }



  void InfluenceMap::SetRadius(float aRadius)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetRadius(aRadius);

      if (mDebugDraw)
      {
        // corners
        glm::vec3 bl, br, tl, tr;

        bl.x = mInstantiatedInfluenceMap->GetCenter().x - mInstantiatedInfluenceMap->GetRadius();
        br.x = mInstantiatedInfluenceMap->GetCenter().x + mInstantiatedInfluenceMap->GetRadius();
        tl.x = bl.x;
        tr.x = br.x;

        bl.z = mInstantiatedInfluenceMap->GetCenter().z - mInstantiatedInfluenceMap->GetRadius();
        tl.z = mInstantiatedInfluenceMap->GetCenter().z + mInstantiatedInfluenceMap->GetRadius();
        tr.z = tl.z;
        br.z = bl.z;

        tl.y = bl.y = tr.y = br.y = mInstantiatedInfluenceMap->GetCenter().y;

        mDrawer->Start();
        mDrawer->AddLine(tl, bl, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(bl, br, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(br, tr, mInstantiatedInfluenceMap->GetColor());
        mDrawer->AddLine(tr, tl, mInstantiatedInfluenceMap->GetColor());
        mDrawer->End();
      }
    }
    else
    {
      mMapTemp.mRadius = aRadius;
      mUseTemp = true;
    }
  }


  float InfluenceMap::GetColorIntensity() const
  {
    if (mInstantiatedInfluenceMap)
    {
      return mInstantiatedInfluenceMap->GetColorIntensity();
    }
    return 0.0f;
  }


  float InfluenceMap::GetWaveIntensity() const
  {
    if (mInstantiatedInfluenceMap)
    {
      return mInstantiatedInfluenceMap->GetWaveIntensity();
    }
    return  0.0f;
  }


  bool InfluenceMap::GetActive() const
  {
    if (mInstantiatedInfluenceMap)
    {
      return mInstantiatedInfluenceMap->GetActive();
    }
    return false;
  }


  std::string InfluenceMap::GetColorInfluenceFunction() const
  {
    if (mInstantiatedInfluenceMap)
    {
      unsigned int fn = mInstantiatedInfluenceMap->GetColorInfluenceFunction();

      switch (fn)
      {
        case 0:
        {
          return "Linear";
        }
        case 1:
        {
          return "Squared";
        }
        case 2:
        {
          return "Cubic";
        }
        case 3:
        {
          return "Logarithmic";
        }
        default:
        {
          return "Linear";
        }
      }
    }
    return "EMPTY";
  }


  std::string InfluenceMap::GetWaveInfluenceFunction() const
  {
    if (mInstantiatedInfluenceMap)
    {
      unsigned int fn = mInstantiatedInfluenceMap->GetWaveInfluenceFunction();

      switch (fn)
      {
        case 0:
        {
          return "Linear";
        }
        case 1:
        {
          return "Squared";
        }
        case 2:
        {
          return "Cubic";
        }
        case 3:
        {
          return "Logarithmic";
        }
        default:
        {
          return "Linear";
        }
      }
    }
    return "EMPTY";
  }



  glm::vec3 InfluenceMap::GetColor() const
  {
    if (mInstantiatedInfluenceMap)
    {
      return mInstantiatedInfluenceMap->GetColor();
    }
    return glm::vec3(0,0,0);
  }



  float InfluenceMap::GetRadius() const
  {
    if (mInstantiatedInfluenceMap)
    {
      return mInstantiatedInfluenceMap->GetRadius();
    }
    return 0.0f;
  }


  void InfluenceMap::Create()
  {
    mInstantiatedInfluenceMap = mRenderer->CreateWaterInfluenceMap(mGraphicsView);

    if (mUseTemp)
    {
      mUseTemp = false;
      mInstantiatedInfluenceMap->SetInfluenceMapInformation(mMapTemp);
    }
  }



  void InfluenceMap::Destroy()
  {
    mInstantiatedInfluenceMap.reset();
  }



  void InfluenceMap::SetDebugDraw(bool aDraw)
  {
    mDebugDraw = aDraw;
    if (mDebugDraw && mInstantiatedInfluenceMap)
    {
      // corners
      glm::vec3 bl, br, tl, tr;

      bl.x = mInstantiatedInfluenceMap->GetCenter().x - mInstantiatedInfluenceMap->GetRadius();
      br.x = mInstantiatedInfluenceMap->GetCenter().x + mInstantiatedInfluenceMap->GetRadius();
      tl.x = bl.x;
      tr.x = br.x;

      bl.z = mInstantiatedInfluenceMap->GetCenter().z - mInstantiatedInfluenceMap->GetRadius();
      tl.z = mInstantiatedInfluenceMap->GetCenter().z + mInstantiatedInfluenceMap->GetRadius();
      tr.z = tl.z;
      br.z = bl.z;

      tl.y = bl.y = tr.y = br.y = mInstantiatedInfluenceMap->GetCenter().y;

      mDrawer->Start();
      mDrawer->AddLine(tl, bl, mInstantiatedInfluenceMap->GetColor());
      mDrawer->AddLine(bl, br, mInstantiatedInfluenceMap->GetColor());
      mDrawer->AddLine(br, tr, mInstantiatedInfluenceMap->GetColor());
      mDrawer->AddLine(tr, tl, mInstantiatedInfluenceMap->GetColor());
      mDrawer->End();
    }
    else
    {
      mDrawer->Start();
      mDrawer->End();
    }
  }
}


