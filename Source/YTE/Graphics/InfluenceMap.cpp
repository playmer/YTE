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
  YTEDefineType(InfluenceMap)
  {
    YTERegisterType(InfluenceMap);
    GetStaticType()->AddAttribute<RunInEditor>();

    YTEBindProperty(&InfluenceMap::GetRadius, &InfluenceMap::SetRadius, "Radius")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&InfluenceMap::GetColor, &InfluenceMap::SetColor, "Color")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();

    YTEBindProperty(&InfluenceMap::GetDebugDraw, &InfluenceMap::SetDebugDraw, "DebugDraw")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
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
    mOwner->YTERegister(Events::PositionChanged, this, &InfluenceMap::TransformUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &InfluenceMap::TransformUpdate);
    mOwner->YTERegister(Events::ScaleChanged, this, &InfluenceMap::TransformUpdate);
    mEngine->YTERegister(Events::LogicUpdate, this, &InfluenceMap::Update);

    mTransform = mOwner->GetComponent<Transform>(); 
    mDrawer = std::make_unique<LineDrawer>(mOwner->GetGUID().ToIdentifierString(), mGraphicsView->GetRenderer(), mGraphicsView);
    mConstructing = false;
    Create();
  }

  static glm::vec3 GetDirectionFromTransform(Transform *aTransform)
  {
    glm::quat rotation = aTransform->GetWorldRotation();

    glm::vec3 forward{ 0.0, -1.0, 0.0f };

    return rotation * forward;
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
      mEngine->YTEDeregister(Events::LogicUpdate, this, &InfluenceMap::Update);
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
    }
    else
    {
      mMapTemp.mColor = glm::vec4(aColor, 1.0f);
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


