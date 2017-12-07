///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/Light.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::vector<std::string> result{ "Directional", "Point", "Spot"/*, "Area"*/ };
    return result;
  }



  YTEDefineType(Light)
  {
    YTERegisterType(Light);

    YTEBindProperty(&Light::GetLightType, &Light::SetLightType, "Light Type")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&Light::GetAmbient, &Light::SetAmbient, "Ambient")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Light::GetDiffuse, &Light::SetDiffuse, "Diffuse")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Light::GetSpecular, &Light::SetSpecular, "Specular")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Light::GetSpotLightCones, &Light::SetSpotLightCones, "Spot Light Cone Angles")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&Light::GetSpotLightFalloff, &Light::SetSpotLightFalloff, "Spot Light Falloff")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }



  Light::Light(Composition* aOwner, Space* aSpace, RSValue* aProperties)
    : Component(aOwner, aSpace)
    , mEngine(nullptr)
    , mRenderer(nullptr)
    , mGraphicsView(nullptr)
    , mTransform(nullptr)
    , mInstantiatedLight(nullptr)
    , mConstructing(true)
    , mSetTransform(false)
    , mUseTemp(false)
  {
    mRenderer = aSpace->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mGraphicsView = aSpace->GetComponent<GraphicsView>();
    mEngine = aSpace->GetEngine();

    DeserializeByType<Light*>(aProperties, this, Light::GetStaticType());
  }



  Light::~Light()
  {
    Destroy();
  }



  void Light::Initialize()
  {
    mOwner->YTERegister(Events::PositionChanged, this, &Light::TransformUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &Light::TransformUpdate);
    mOwner->YTERegister(Events::ScaleChanged, this, &Light::TransformUpdate);
    mEngine->YTERegister(Events::LogicUpdate, this, &Light::Update);

    mTransform = mOwner->GetComponent<Transform>(); 

    mConstructing = false;
    Create();
  }



  void Light::TransformUpdate(TransformChanged* aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mInstantiatedLight)
    {
      mInstantiatedLight->SetPosition(mTransform->GetTranslation());
      mInstantiatedLight->SetDirection(mTransform->GetRotationAsEulerRadians());
    }
    else
    {
      mLightTemp.mPosition = glm::vec4(mTransform->GetTranslation(), 1.0f);
      mLightTemp.mDirection = glm::vec4(mTransform->GetRotationAsEulerRadians(), 0.0f);
      mUseTemp = true;
    }
  }



  void Light::Update(LogicUpdate* aEvent)
  {
    YTEUnusedArgument(aEvent);

    if (mTransform && mSetTransform == false)
    {
      mEngine->YTEDeregister(Events::LogicUpdate, this, &Light::Update);
      if (mInstantiatedLight)
      {
        mInstantiatedLight->SetDirection(mTransform->GetRotationAsEulerRadians());
        mInstantiatedLight->SetPosition(mTransform->GetTranslation());
      }
      else
      {
        mLightTemp.mPosition = glm::vec4(mTransform->GetTranslation(), 1.0f);
        mLightTemp.mDirection = glm::vec4(mTransform->GetRotationAsEulerRadians(), 0.0f);
        mUseTemp = true;
      }
      mSetTransform = true;
    }
  }


  void Light::SetLightSourceInformation(UBOLight& aLight)
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
      mLightTemp.mPosition = glm::vec4(aPosition, 1.0f);
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
      mLightTemp.mSpecular = glm::vec4(aColor, 1.0f);
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
      mLightTemp.mSpotLightConeAngles = aCones;
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
      mInstantiatedLight->GetSpotLightFalloff();
    }

    return 0.0f;
  }



  void Light::Create()
  {
    mInstantiatedLight = mRenderer->CreateLight(mGraphicsView);

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

