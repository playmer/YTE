#include "YTE/Graphics/Model.hpp"
#include "YTE/Graphics/Material.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"

namespace YTE
{
  YTEDefineType(MaterialRepresentation)
  {
    YTERegisterType(MaterialRepresentation);
  }


  void MaterialRepresentation::UpdateUBO()
  {

  }


  YTEDefineType(Material)
  {
    YTERegisterType(Material);

    GetStaticType()->AddAttribute<EditorHeaderList>(&Material::Lister, "Animations");

    std::vector<std::vector<Type*>> deps = { { TypeId<Model>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  Material::Material(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    DeserializeByType<Material*>(aProperties, this, GetStaticType());
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
}