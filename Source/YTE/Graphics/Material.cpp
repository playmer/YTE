#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/SpriteText.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedHeightmap.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

namespace YTE
{
  YTEDefineType(MaterialRepresentation)
  {
    RegisterType<MaterialRepresentation>();
    TypeBuilder<MaterialRepresentation> builder;

    builder.Property<&GetDiffuse, &SetDiffuse>("Diffuse")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();
    builder.Property<&GetAmbient, &SetAmbient>("Ambient")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();
    builder.Property<&GetSpecular, &SetSpecular>("Specular")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();
    builder.Property<&GetEmissive, &SetEmissive>("Emissive")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();
    builder.Property<&GetTransparent, &SetTransparent>("Transparent")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<EditableColor>();
    //builder.Property<&GetReflective, &SetReflective>("Reflective")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //builder.Property<&GetOpacity, &SetOpacity>("Opacity")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    builder.Property<&GetShininess, &SetShininess>("Shininess")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    builder.Property<&GetShininessStrength, &SetShininessStrength>("ShininessStrength")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    //builder.Property<&GetReflectivity, &SetReflectivity>("Reflectivity")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //builder.Property<&GetReflectiveIndex, &SetReflectiveIndex>("ReflectiveIndex")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //builder.Property<&GetBumpScaling, &SetBumpScaling>("BumpScaling")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //builder.Property<&GetIsEditorObject, &SetIsEditorObject>("IsEditorObject")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
  }


  void MaterialRepresentation::UpdateUBO()
  {
    auto model = mMaterialComponent->GetOwner()->GetDerivedComponent<BaseModel>();

    if (nullptr != model)
    {
      auto instantiatedModels = model->GetInstantiatedModel();

      for (size_t i = 0; i < instantiatedModels.size(); ++i)
      {
        auto instantiatedModel = instantiatedModels[i];
        if (nullptr != instantiatedModel)
        {
          // If we have submesh pointer, we're a submesh material, otherwise we're a 
          // Model, or "Universal" material.
          if (nullptr != mSubmesh)
          {
            instantiatedModel->UpdateUBOSubmeshMaterial(&mMaterial, mIndex);
          }
          else
          {
            instantiatedModel->UpdateUBOMaterial(&mMaterial);
          }
        }
      }
    }
  }


  YTEDefineType(Material)
  {
    RegisterType<Material>();
    TypeBuilder<Material> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    GetStaticType()->AddAttribute<EditorHeaderList>(&Material::SubmeshMaterialDeserializer,
                                                    &Material::SubmeshMaterialSerializer,
                                                    &Material::SubmeshMaterialLister, 
                                                    "Materials");

    std::vector<std::vector<Type*>> deps = { 
      { TypeId<Transform>() }, 
      { TypeId<Model>(),
        TypeId<FFT_WaterSimulation>(),
        TypeId<Sprite>(),
        TypeId<SpriteText>()
      } 
    };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Field<&Material::mName>("Name", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();

    builder.Field<&Material::mModelMaterial>("ModelMaterial", PropertyBinding::GetSet)
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .AddAttribute<RedirectObject>(&Material::ModelMaterialDeserializer,
                                    &Material::ModelMaterialSerializer,
                                    &Material::ModelMaterialGetObject,
                                    "ModelMaterial");
  }

  Material::Material(Composition *aOwner, Space *aSpace)
    : Component(aOwner, aSpace)
    , mModelMaterial(this)
  {
  }

  void Material::Create(ModelChanged *aEvent)
  {
    auto model = aEvent->Object->GetDerivedComponent<BaseModel>();

    bool isModel{ false };

    if (model->GetType()->IsA<Model>())
    {
      isModel = true;
    }

    if (nullptr != model)
    {
      InstantiatedModel *instantiatedModel{ nullptr };

      auto models = model->GetInstantiatedModel();

      if (false == models.empty())
      {
        instantiatedModel = models.front();
      }

      auto mesh = instantiatedModel->GetMesh();

      if (nullptr != instantiatedModel)
      {
        if ((isModel && (mesh->mName != mName)) ||
            (mSubmeshMaterials.size() != mesh->mParts.size()))
        {
          mSubmeshMaterials.clear();

          for (auto[submesh, i] : enumerate(mesh->mParts))
          {
            auto materialRep = std::make_unique<MaterialRepresentation>(submesh->mUBOMaterial,
                                                                        this,
                                                                        i,
                                                                        &(*submesh));
            mSubmeshMaterials.emplace_back(std::move(materialRep));
          }

          mModelMaterial = MaterialRepresentation{ instantiatedModel->GetUBOMaterialData(),
                                                  this,
                                                  0,
                                                  nullptr };
        }
        else
        {
          for (auto[submesh, i] : enumerate(mesh->mParts))
          {
            mSubmeshMaterials[i]->SetSubmesh(&(*submesh));
            mSubmeshMaterials[i]->SetIndex(i);
            mSubmeshMaterials[i]->UpdateUBO();
          }

          mModelMaterial.UpdateUBO();
        }

        mName = mesh->mName;
      }
    }
  }

  void Material::Initialize()
  {
    mModel = mOwner->GetComponent<Model>();
    mFFTWater = mOwner->GetComponent<FFT_WaterSimulation>();

    mOwner->RegisterEvent<&Material::Create>(Events::ModelChanged, this);

    ModelChanged event;
    event.Object = mOwner;

    Create(&event);
  }

  Material::~Material()
  {

  }

  std::vector<std::pair<YTE::Object*, std::string>> Material::SubmeshMaterialLister(YTE::Object *aSelf)
  {
    auto self = static_cast<Material*>(aSelf);

    std::vector<std::pair<YTE::Object*, std::string>> materials;

    for (auto &materialIt : self->mSubmeshMaterials)
    {
      std::string name = materialIt->GetSubmesh() ? 
                          materialIt->GetSubmesh()->mMaterialName :
                          "";

      materials.emplace_back(std::make_pair(materialIt.get(), name));
    }

    return materials;
  }

  void Material::SubmeshMaterialDeserializer(RSValue &aValue,
                                             Object *aOwner)
  {
    auto owner = static_cast<Material*>(aOwner);
    owner->mSubmeshMaterials.clear();

    for (auto valueIt = aValue.MemberBegin(); valueIt < aValue.MemberEnd(); ++valueIt)
    {
      owner->mSubmeshMaterials.emplace_back(std::make_unique<MaterialRepresentation>(owner, &(valueIt->value)));
    }
  }


  RSValue Material::SubmeshMaterialSerializer(RSAllocator &aAllocator,
                                              Object *aOwner)
  {
    auto owner = static_cast<Material*>(aOwner);

    RSValue materials;
    materials.SetObject();
    for (auto &submeshMaterial : owner->mSubmeshMaterials)
    {
      auto materialSerialized  = SerializeByType(aAllocator, 
                                                 submeshMaterial.get(), 
                                                 TypeId<MaterialRepresentation>());

      RSValue materialName;

      std::string name;

      if (submeshMaterial->GetSubmesh())
      {
        name = submeshMaterial->GetSubmesh()->mMaterialName;
      }

      materialName.SetString(name.c_str(),
                             static_cast<RSSizeType>(name.size()),
                             aAllocator);

      materials.AddMember(materialName, materialSerialized, aAllocator);
    }

    return materials;
  }

  std::pair<YTE::Object*, std::string> Material::ModelMaterialGetObject(YTE::Object *aSelf)
  {
    auto self = static_cast<Material*>(aSelf);

    return {&(self->mModelMaterial), "ModelMaterial"};
  }

  void Material::ModelMaterialDeserializer(RSValue &aValue,
                                           Object *aOwner)
  {
    auto owner = static_cast<Material*>(aOwner);
    
    owner->mModelMaterial = MaterialRepresentation{ owner, &aValue };
  }


  RSValue Material::ModelMaterialSerializer(RSAllocator &aAllocator,
                                            Object *aOwner)
  {
    auto owner = static_cast<Material*>(aOwner);

    RSValue materials;
    materials.SetObject();
    return SerializeByType(aAllocator, 
                           &owner->mModelMaterial,
                           TypeId<MaterialRepresentation>());
  }
}