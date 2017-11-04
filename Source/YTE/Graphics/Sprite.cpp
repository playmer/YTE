
#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Sprite.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

    std::wstring wStrPath = YTE::cWorkingDirectory;

    filesystem::path fsPath = Path::GetGamePath().String();

    filesystem::path finalPath = fsPath.parent_path() / L"Textures/Originals";

    std::vector<std::string> result;

    for (auto & p : filesystem::directory_iterator(finalPath))
    {
      std::string str = p.path().filename().generic_string();

      result.push_back(str);
    }

    return result;
  }


  YTEDefineType(Sprite)
  {
    YTERegisterType(Sprite);

    YTEBindField(&Sprite::mTextureName, "TextureName", PropertyBinding::GetSet)
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);
  }

  Sprite::Sprite(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    auto renderer = aSpace->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    auto window = aSpace->GetComponent<GraphicsView>()->GetWindow();

    DeserializeByType<Sprite*>(aProperties, this, Sprite::GetStaticType());
  }
}