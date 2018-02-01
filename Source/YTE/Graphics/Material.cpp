#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

namespace YTE
{
  YTEDefineType(MaterialRepresentation)
  {
    YTERegisterType(MaterialRepresentation);

    YTEBindProperty(&GetDiffuse, &SetDiffuse, "Diffuse")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindProperty(&GetAmbient, &SetAmbient, "Ambient")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindProperty(&GetSpecular, &SetSpecular, "Specular")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindProperty(&GetEmissive, &SetEmissive, "Emissive")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindProperty(&GetTransparent, &SetTransparent, "Transparent")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    //YTEBindProperty(&GetReflective, &SetReflective, "Reflective")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //YTEBindProperty(&GetOpacity, &SetOpacity, "Opacity")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    YTEBindProperty(&GetShininess, &SetShininess, "Shininess")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    YTEBindProperty(&GetShininessStrength, &SetShininessStrength, "ShininessStrength")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
    //YTEBindProperty(&GetReflectivity, &SetReflectivity, "Reflectivity")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //YTEBindProperty(&GetReflectiveIndex, &SetReflectiveIndex, "ReflectiveIndex")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    //YTEBindProperty(&GetBumpScaling, &SetBumpScaling, "BumpScaling")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<Serializable>();
    YTEBindProperty(&GetIsEditorObject, &SetIsEditorObject, "IsEditorObject")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();
  }


  void MaterialRepresentation::UpdateUBO()
  {
    auto model = mMaterialComponent->GetOwner()->GetComponent<Model>();

    if (nullptr != model)
    {
      auto instantiatedModel = model->GetInstantiatedModel();

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


  YTEDefineType(Material)
  {
    YTERegisterType(Material);

    GetStaticType()->AddAttribute<EditorHeaderList>(&Material::Deserializer,
                                                    &Material::Serializer,
                                                    &Material::Lister, 
                                                    "Materials");

    std::vector<std::vector<Type*>> deps = { { TypeId<Model>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindField(&Material::mName, "Name", PropertyBinding::GetSet)
      .AddAttribute<Serializable>();
  }

  Material::Material(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mModelMaterial(this)
  {
    DeserializeByType(aProperties, this, GetStaticType());
  }

  void Material::Create(ModelChanged *aEvent)
  {
    auto model = aEvent->Object->GetComponent<Model>();

    if (nullptr != model)
    {
      auto instantiatedModel = model->GetInstantiatedModel();

      if (nullptr != instantiatedModel)
      {
        if (instantiatedModel->GetMesh()->mName != mName)
        {
          for (auto[submesh, i] : enumerate(instantiatedModel->GetMesh()->mParts))
          {
            auto materialRep = std::make_unique<MaterialRepresentation>(submesh->mUBOMaterial,
                                                                        this,
                                                                        i,
                                                                        &(*submesh));
            mSubmeshMaterials.emplace_back(std::move(materialRep));
          }
        }
        else
        {
          for (auto[submesh, i] : enumerate(instantiatedModel->GetMesh()->mParts))
          {
            mSubmeshMaterials[i]->SetSubmesh(&(*submesh));
            mSubmeshMaterials[i]->SetIndex(i);
            mSubmeshMaterials[i]->UpdateUBO();
          }
        }

        mName = instantiatedModel->GetMesh()->mName;
      }
    }
  }

  void Material::Initialize()
  {
    mModel = mOwner->GetComponent<Model>();

    mModel->YTERegister(Events::ModelChanged, this, &Material::Create);

    ModelChanged event;
    event.Object = mOwner;

    Create(&event);
  }

  Material::~Material()
  {

  }

  std::vector<std::pair<YTE::Object*, std::string>> Material::Lister(YTE::Object *aSelf)
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

  void Material::Deserializer(RSValue &aValue, 
                              Object *aOwner)
  {
    auto owner = static_cast<Material*>(aOwner);
    owner->mSubmeshMaterials.clear();

    for (auto valueIt = aValue.MemberBegin(); valueIt < aValue.MemberEnd(); ++valueIt)
    {
      owner->mSubmeshMaterials.emplace_back(std::make_unique<MaterialRepresentation>(owner, &(valueIt->value)));
    }
  }


  RSValue Material::Serializer(RSAllocator &aAllocator, 
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

      auto &name = submeshMaterial->GetSubmesh()->mMaterialName;
      materialName.SetString(name.c_str(),
                             static_cast<RSSizeType>(name.size()),
                             aAllocator);

      materials.AddMember(materialName, materialSerialized, aAllocator);
    }

    return materials;
  }
}