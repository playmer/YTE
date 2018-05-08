#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/TempDiffuseColoringComponent.hpp"
#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Vulkan/VKInstantiatedModel.hpp"

namespace YTE
{
  YTEDefineType(TempDiffuseColoringComponent)
  {
    RegisterType<TempDiffuseColoringComponent>();
    TypeBuilder<TempDiffuseColoringComponent> builder;

    builder.Property<&TempDiffuseColoringComponent::GetDiffuseColor, &TempDiffuseColoringComponent::SetDiffuseColor>( "Diffuse Color")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Change the diffuse color");
  }

  TempDiffuseColoringComponent::TempDiffuseColoringComponent(Composition *aOwner,
    Space *aSpace,
    RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mModel(nullptr)
    , update(false)
  {
    mDiffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
    
    DeserializeByType(aProperties, this, GetStaticType());
  }

  TempDiffuseColoringComponent::~TempDiffuseColoringComponent()
  {
    //if (mModel)
    //{
    //  auto m = static_cast<VkInstantiatedModel*>(mModel->GetInstantiatedModel());
    //  auto buff = m->GetUBOModelData();
    //  buff.mDiffuseColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //  m->UpdateUBOModel(buff);
    //}
  }


  void TempDiffuseColoringComponent::Initialize()
  {
    mModel = mOwner->GetComponent<Model>();
    if (update == true)
    {
      auto m = static_cast<VkInstantiatedModel*>(mModel->GetInstantiatedModel()[0]);
      auto buff = m->GetUBOModelData();
      buff.mDiffuseColor = glm::vec4(mDiffuseColor, 1.0f);
      m->UpdateUBOModel(buff);
      update = false;
    }
  }

  void TempDiffuseColoringComponent::SetDiffuseColor(glm::vec3 aColor)
  {
    mDiffuseColor = aColor;

    glm::clamp(mDiffuseColor, 0.0f, 1.0f);

    if (mModel)
    {
      auto m = static_cast<VkInstantiatedModel*>(mModel->GetInstantiatedModel()[0]);
      auto buff = m->GetUBOModelData();
      buff.mDiffuseColor = glm::vec4(mDiffuseColor, 1.0f);
      m->UpdateUBOModel(buff);
      update = false;
    }
    else
    {
      update = true;
    }
  }
}

