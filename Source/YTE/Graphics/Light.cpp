#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/InstantiatedLight.hpp"
#include "YTE/Graphics/Light.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    UnusedArguments(aComponent);

    std::vector<std::string> result{ "Directional", "Point", "Spot"/*, "Area"*/ };
    return result;
  }



  YTEDefineType(Light)
  {
    RegisterType<Light>();
    TypeBuilder<Light> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    builder.Property<&Light::GetLightType, &Light::SetLightType>("Light Type")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    builder.Property<&Light::GetAmbient, &Light::SetAmbient>("Ambient")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();

    builder.Property<&Light::GetDiffuse, &Light::SetDiffuse>("Diffuse")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();

    builder.Property<&Light::GetSpecular, &Light::SetSpecular>("Specular")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();

    builder.Property<&Light::GetSpotLightCones, &Light::SetSpotLightCones>("Spot Light Cone Angles")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&Light::GetSpotLightFalloff, &Light::SetSpotLightFalloff>("Spot Light Falloff")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&Light::GetActive, &Light::SetActive>("Is Active")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&Light::GetIntensity, &Light::SetIntensity>("Intensity")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }



  Light::Light(Composition* aOwner, Space* aSpace)
    : Component(aOwner, aSpace)
    , mEngine(nullptr)
    , mRenderer(nullptr)
    , mGraphicsView(nullptr)
    , mTransform(nullptr)
    , mInstantiatedLight(nullptr)
    , mSetTransform(false)
    , mUseTemp(false)
  {
    mEngine = aSpace->GetEngine();
    mRenderer = mEngine->GetComponent<GraphicsSystem>()->GetRenderer();
    mGraphicsView = aSpace->GetComponent<GraphicsView>();
  }



  Light::~Light()
  {
    Destroy();
  }



  void Light::Initialize()
  {
    mOwner->RegisterEvent<&Light::TransformUpdate>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&Light::TransformUpdate>(Events::RotationChanged, this);
    mOwner->RegisterEvent<&Light::TransformUpdate>(Events::ScaleChanged, this);
    mEngine->RegisterEvent<&Light::Update>(Events::LogicUpdate, this);

    mTransform = mOwner->GetComponent<Transform>(); 

    Create();
  }

  static glm::vec3 GetDirectionFromTransform(Transform *aTransform)
  {
    glm::quat rotation = aTransform->GetWorldRotation();

    glm::vec3 forward{ 0.0, -1.0, 0.0f };

    return rotation * forward;
  }



  void Light::TransformUpdate(TransformChanged* aEvent)
  {
    UnusedArguments(aEvent);

    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetPosition(mTransform->GetWorldTranslation());
      mInstantiatedLight->SetDirection(GetDirectionFromTransform(mTransform));
    }
    else
    {
      mLightTemp.mPosition = mTransform->GetWorldTranslation();
      mLightTemp.mDirection = glm::vec4(GetDirectionFromTransform(mTransform), 0.0f);
      mUseTemp = true;
    }
  }



  void Light::Update(LogicUpdate* aEvent)
  {
    YTEProfileFunction();
    UnusedArguments(aEvent);

    if (mTransform && mSetTransform == false)
    {
      mEngine->DeregisterEvent<&Light::Update>(Events::LogicUpdate,  this);
      if (mInstantiatedLight)
      {
        mInstantiatedLight->SetDirection(GetDirectionFromTransform(mTransform));
        mInstantiatedLight->SetPosition(mTransform->GetWorldTranslation());
      }
      else
      {
        mLightTemp.mPosition = mTransform->GetWorldTranslation();
        mLightTemp.mDirection = glm::vec4(GetDirectionFromTransform(mTransform), 0.0f);
        mUseTemp = true;
      }
      mSetTransform = true;
    }
  }


  void Light::SetLightSourceInformation(UBOs::Light& aLight)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetLightSourceInformation(aLight);
    }
    else
    {
      mLightTemp = aLight;
      mUseTemp = true;
    }
  }



  void Light::SetPosition(glm::vec3& aPosition)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetPosition(aPosition);
    }
    else
    {
      mLightTemp.mPosition = aPosition;
      mUseTemp = true;
    }
  }



  void Light::SetDirection(glm::vec3& aDirection)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetDirection(aDirection);
    }
    else
    {
      mLightTemp.mDirection = glm::vec4(aDirection, 1.0f);
      mUseTemp = true;
    }
  }



  void Light::SetAmbient(glm::vec3& aColor)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetAmbient(aColor);
    }
    else
    {
      mLightTemp.mAmbient = glm::vec4(aColor, 1.0f);
      mUseTemp = true;
    }
  }



  void Light::SetDiffuse(glm::vec3& aColor)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetDiffuse(aColor);
    }
    else
    {
      mLightTemp.mDiffuse = glm::vec4(aColor, 1.0f);
      mUseTemp = true;
    }
  }



  void Light::SetSpecular(glm::vec3& aColor)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetSpecular(aColor);
    }
    else
    {
      mLightTemp.mSpecular = aColor;
      mUseTemp = true;
    }
  }



  void Light::SetSpotLightCones(glm::vec2& aCones)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetSpotLightCones(aCones);
    }
    else
    {
      mLightTemp.mSpotLightConeAngles = glm::radians(aCones);
      mUseTemp = true;
    }
  }



  void Light::SetLightType(std::string aLightType)
  {
    LightType lt;

    if (false == (aLightType != "Directional"))
    {
      lt = LightType::Directional;
    }
    else if (false == (aLightType != "Point"))
    {
      lt = LightType::Point;
    }
    else if (false == (aLightType != "Spot"))
    {
      lt = LightType::Spot;
    }
    //else if (false == (aLightType != "Area"))
    //{
    //  lt = LightType::Area;
    //}
    else
    {
      lt = LightType::None;
    }

    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetLightType(lt);
    }
    else
    {
      mLightTemp.mLightType = static_cast<unsigned int>(lt);
    }
  }



  void Light::SetSpotLightFalloff(float& aFalloff)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetSpotLightFalloff(aFalloff);
    }
    else
    {
      mLightTemp.mSpotLightFalloff = aFalloff;
      mUseTemp = true;
    }

  }



  void Light::SetActive(bool aValue)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetActive(aValue);
    }
    else
    {
      mLightTemp.mActive = aValue;
      mUseTemp = true;
    }
  }


  void Light::SetIntensity(float& aItensity)
  {
    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetIntensity(aItensity);
    }
    else
    {
      mLightTemp.mIntensity = aItensity;
      mUseTemp = true;
    }

  }


  glm::vec3 Light::GetPosition() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetPosition();
    }
    return glm::vec3(0,0,0);
  }



  glm::vec3 Light::GetDirection() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetDirection();
    }
    return glm::vec3(0,0,0);
  }



  glm::vec3 Light::GetAmbient() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetAmbient();
    }
    return glm::vec3(0,0,0);
  }



  glm::vec3 Light::GetDiffuse() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetDiffuse();
    }
    return glm::vec3(0,0,0);
  }



  glm::vec3 Light::GetSpecular() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetSpecular();
    }
    return glm::vec3(0,0,0);
  }



  glm::vec2 Light::GetSpotLightCones() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetSpotLightCones();
    }
    return glm::vec2(0,0);
  }



  std::string Light::GetLightType() const
  {
    if (mInstantiatedLight)
    {
      LightType type = mInstantiatedLight->GetLightType();

      switch(type)
      {
        case LightType::None:
        {
          return "None";
        }
        case LightType::Directional:
        {
          return "Directional";
        }
        case LightType::Point:
        {
          return "Point";
        }
        case LightType::Spot:
        {
          return "Spot";
        }
        //case LightType::Area:
        //{
        //  return "Area";
        //}
      }
    }

    return "None";
  }



  float Light::GetSpotLightFalloff() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetSpotLightFalloff();
    }

    return 0.0f;
  }



  bool Light::GetActive() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetActive();
    }
    return false;
  }


  float Light::GetIntensity() const
  {
    if (mInstantiatedLight)
    {
      return mInstantiatedLight->GetIntensity();
    }
    return 1.0f;
  }



  void Light::Create()
  {
    mInstantiatedLight = std::make_unique<InstantiatedLight>(mGraphicsView);

    if (mUseTemp)
    {
      mUseTemp = false;
      mInstantiatedLight->SetLightSourceInformation(mLightTemp);
    }
  }



  void Light::Destroy()
  {
    mInstantiatedLight.reset();
  }
}


