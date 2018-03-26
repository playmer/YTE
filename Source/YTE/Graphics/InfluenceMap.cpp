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

    YTEBindProperty(&InfluenceMap::GetCenter, &InfluenceMap::SetCenter, "Center")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();
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
      mInstantiatedInfluenceMap->SetPosition(mTransform->GetTranslation());
      mInstantiatedInfluenceMap->SetDirection(GetDirectionFromTransform(mTransform));
    }
    else
    {
      mMapTemp.mPosition = mTransform->GetTranslation();
      mMapTemp.mDirection = glm::vec4(GetDirectionFromTransform(mTransform), 0.0f);
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
        mInstantiatedInfluenceMap->SetDirection(GetDirectionFromTransform(mTransform));
        mInstantiatedInfluenceMap->SetPosition(mTransform->GetTranslation());
      }
      else
      {
        mMapTemp.mPosition = mTransform->GetTranslation();
        mMapTemp.mDirection = glm::vec4(GetDirectionFromTransform(mTransform), 0.0f);
        mUseTemp = true;
      }
      mSetTransform = true;
    }
  }


  void InfluenceMap::SetMapSourceInformation(UBOWaterInfluenceMap& aMap)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetMapSourceInformation(aMap);
    }
    else
    {
      mMapTemp = aMap;
      mUseTemp = true;
    }
  }



  void InfluenceMap::SetCenter(glm::vec3& aCenter)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetCenter(aCenter);
    }
    else
    {
      mMapTemp.mCenter = aCenter;
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



  void InfluenceMap::SetRadius(float& aRadius)
  {
    if (mInstantiatedInfluenceMap)
    {
      mInstantiatedInfluenceMap->SetRadius(aRadius);
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
      return mInstantiatedInfluenceMap->GetPosition();
    }
    return glm::vec3(0,0,0);
  }



  glm::vec3 InfluenceMap::GetDirection() const
  {
    if (mInstantiatedInfluenceMap)
    {
      return mInstantiatedInfluenceMap->GetDirection();
    }
    return glm::vec3(0,0,0);
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
}


