/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/01/19
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <fstream>

#include <stb/stb_image_write.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Graphics/SpriteText.hpp"

namespace YTE
{
  static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  {
    YTEUnusedArgument(aComponent);

      // TODO(Isaac): Consider cross-platform solution
    filesystem::path fontPath = "C:\\Windows\\Fonts";

    std::vector<std::string> result;

    for (auto &fonts : filesystem::directory_iterator(fontPath))
    {
      if (fonts.path().filename().extension().generic_string() == ".ttf")
      {
        std::string str = fonts.path().filename().generic_string();

        result.push_back(str);
      }
    }

    return result;
  }

  static std::vector<std::string> PopulateAlignXDropDownList(Component *)
  {
    std::vector<std::string> result;

    for (int i = 0; i < AlignmentX::COUNT; ++i)
    {
      result.push_back(AlignmentX::Names[i]);
    }

    return result;
  }

  static std::vector<std::string> PopulateAlignYDropDownList(Component *)
  {
    std::vector<std::string> result;

    for (int i = 0; i < AlignmentY::COUNT; ++i)
    {
      result.push_back(AlignmentY::Names[i]);
    }

    return result;
  }

  YTEDefineType(SpriteText)
  {
    YTERegisterType(SpriteText);
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    YTEBindProperty(&SpriteText::GetText, &SpriteText::SetText, "Text")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&SpriteText::GetFont, &SpriteText::SetFont, "Font")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    YTEBindProperty(&SpriteText::GetFontSize, &SpriteText::SetFontSize, "FontSize")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    YTEBindProperty(&SpriteText::GetAlignmentX, &SpriteText::SetAlignmentX, "AlignX")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateAlignXDropDownList);

    YTEBindProperty(&SpriteText::GetAlignmentY, &SpriteText::SetAlignmentY, "AlignY")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateAlignYDropDownList);
  }

  SpriteText::SpriteText(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : BaseModel{ aOwner, aSpace, aProperties }
    , mFontName("calibri.ttf")
    , mAlignX(AlignmentX::Center)
    , mAlignY(AlignmentY::Center)
    , mConstructing(true)
  {
    mFontSize = 1.f;

    DeserializeByType(aProperties, this, GetStaticType());

    mConstructing = false;
  }

  SpriteText::~SpriteText()
  {

  }

  void SpriteText::Initialize()
  {
    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    mTransform = mOwner->GetComponent<Transform>();

    mSpace->YTERegister(Events::LogicUpdate, this, &SpriteText::OnStart);

    mOwner->YTERegister(Events::PositionChanged, this, &SpriteText::TransformUpdate);
    mOwner->YTERegister(Events::RotationChanged, this, &SpriteText::TransformUpdate);
    //mOwner->YTERegister(Events::ScaleChanged, this, &SpriteText::TransformUpdate);

    PrepareFont();
    CreateSpriteText();
  }

  void SpriteText::CreateSpriteText()
  {
    if (nullptr != mInstantiatedSprite)
    {
      mInstantiatedSprite.reset();
    }

    if (mTextureName.empty())
    {
      return;
    }

    std::string meshName = "__SpriteText";
    meshName += mOwner->GetGUID().ToString();

    Submesh submesh;
    std::vector<Submesh> submeshes;

    u32 lastIndex = 0;
    float offsetX = 0, offsetY = 0;

    float sizeFactor = mFontSize / mFontInfo.mSize;

    submesh.mDiffuseMap = mTextureName;
    submesh.mDiffuseType = TextureViewType::e2D;
    submesh.mShaderSetName = "SpriteText";

    submesh.mCullBackFaces = false;

    float maxHeight = 0.0f;
    for (auto &c : mText)
    {
      Vertex vert0;
      Vertex vert1;
      Vertex vert2;
      Vertex vert3;

      stbtt_aligned_quad quad;

      stbtt_GetPackedQuad(mFontInfo.mCharInfo.get(), 
                          mFontInfo.mAtlasWidth, 
                          mFontInfo.mAtlasHeight, 
                          c - mFontInfo.mFirstChar, 
                          &offsetX, 
                          &offsetY, 
                          &quad, 
                          1);

      float height = sizeFactor * glm::abs(quad.y1 - quad.y0);
      maxHeight = height > maxHeight ? height : maxHeight;

        // Save vertex attributes
      vert0.mPosition = { sizeFactor * quad.x0, sizeFactor * -quad.y1, 0.0 };  // Bottom-left
      vert1.mPosition = { sizeFactor * quad.x1, sizeFactor * -quad.y1, 0.0 };  // Bottom-right
      vert2.mPosition = { sizeFactor * quad.x1, sizeFactor * -quad.y0, 0.0 };  // Top-right
      vert3.mPosition = { sizeFactor * quad.x0, sizeFactor * -quad.y0, 0.0 };  // Top-left
      vert0.mTextureCoordinates = { quad.s0, 1.0f - quad.t1, 0.0f };  // Bottom-left (UVs)
      vert1.mTextureCoordinates = { quad.s1, 1.0f - quad.t1, 0.0f };  // Bottom-right (UVs)
      vert2.mTextureCoordinates = { quad.s1, 1.0f - quad.t0, 0.0f };  // Top-right (UVs)
      vert3.mTextureCoordinates = { quad.s0, 1.0f - quad.t0, 0.0f };  // Top-left (UVs)

      submesh.mVertexBuffer.emplace_back(vert0);
      submesh.mVertexBuffer.emplace_back(vert1);
      submesh.mVertexBuffer.emplace_back(vert2);
      submesh.mVertexBuffer.emplace_back(vert3);

      submesh.mIndexBuffer.push_back(lastIndex);
      submesh.mIndexBuffer.push_back(lastIndex + 1);
      submesh.mIndexBuffer.push_back(lastIndex + 2);
      submesh.mIndexBuffer.push_back(lastIndex);
      submesh.mIndexBuffer.push_back(lastIndex + 2);
      submesh.mIndexBuffer.push_back(lastIndex + 3);

      lastIndex += 4;
    }

    float maxWidth = (submesh.mVertexBuffer.empty())
      ? 0.0f
      : (submesh.mVertexBuffer.end() - 2)->mPosition.x - submesh.mVertexBuffer.begin()->mPosition.x;
    
    for (auto &verts : submesh.mVertexBuffer)
    {
        // Without any modification, text is bottom-left aligned
      switch (mAlignX)
      {
        case AlignmentX::Center:
          verts.mPosition.x -= (maxWidth / 2.0f);
          break;
        case AlignmentX::Right:
          verts.mPosition.x -= maxWidth;
          break;
      }
      
      switch (mAlignY)
      {
        case AlignmentY::Center:
          verts.mPosition.y -= (maxHeight / 2.0f);
          break;
        case AlignmentY::Top:
          verts.mPosition.y -= maxHeight;
          break;
      }
      
    }

    submeshes.emplace_back(submesh);

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(meshName, submeshes, true);

    mInstantiatedSprite = mRenderer->CreateModel(view, mesh);
    CreateTransform();
    mUBOModel.mDiffuseColor = mInstantiatedSprite->GetUBOModelData().mDiffuseColor;
    mInstantiatedSprite->UpdateUBOModel(mUBOModel);
    mInstantiatedSprite->SetVisibility(mVisibility);

    mInstantiatedSprite->mType = ShaderType::AlphaBlendShader;
  }

  void SpriteText::CreateTransform()
  {
    if (mTransform == nullptr)
      return;

    mUBOModel.mModelMatrix = glm::translate(glm::mat4(1.0f), mTransform->GetWorldTranslation());
    mUBOModel.mModelMatrix = mUBOModel.mModelMatrix * glm::toMat4(mTransform->GetWorldRotation());
    mUBOModel.mModelMatrix = glm::scale(mUBOModel.mModelMatrix, glm::vec3(1.0f));
  }

  void SpriteText::OnStart(LogicUpdate *)
  {
    mTransform->SetWorldScale(glm::vec3(1.0f));
    mSpace->YTEDeregister(Events::LogicUpdate, this, &SpriteText::OnStart);
  }

  void SpriteText::TransformUpdate(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    CreateTransform();

    if (mInstantiatedSprite)
    {
      mInstantiatedSprite->UpdateUBOModel(mUBOModel);
    }
  }

  void SpriteText::PrepareFont()
  {
      // Build the font atlas
    std::string pathName = std::string("C:\\Windows\\Fonts\\") + mFontName;
    std::ifstream file(pathName, std::ios::binary | std::ios::ate);

    if (!file.is_open())
      std::cout << fmt::format("SpriteText: Failed to open file ", pathName) << std::endl;

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    auto bytes = std::vector<uint8_t>(size);
    file.read(reinterpret_cast<char*>(&bytes[0]), size);
    file.close();

    auto atlasData = std::make_unique<unsigned char[]>(mFontInfo.mAtlasWidth * mFontInfo.mAtlasHeight);
    mFontInfo.mCharInfo = std::make_unique<stbtt_packedchar[]>(mFontInfo.mCharCount);

    stbtt_pack_context context;
    if (!stbtt_PackBegin(&context, atlasData.get(), mFontInfo.mAtlasWidth, mFontInfo.mAtlasHeight, 0, 1, nullptr))
      std::cout << fmt::format("SpriteText: Failed to initialize font!") << std::endl;

    stbtt_PackSetOversampling(&context, 2, 2);
    if (!stbtt_PackFontRange(&context, bytes.data(), 0, mFontInfo.mSize, mFontInfo.mFirstChar, mFontInfo.mCharCount, mFontInfo.mCharInfo.get()))
      std::cout << fmt::format("SpriteText: Failed to pack font") << std::endl;

    stbtt_PackEnd(&context);

      // If the font texture does not already exist on disk, save it to disk for lookup
    size_t extensionPos = mFontName.size() - 4;
    std::string texName = mFontName;
    texName.replace(extensionPos, 6, "64.png");

    filesystem::path outPath = Path::GetGamePath().String();
    outPath = outPath.parent_path() / "Textures/Originals" / texName;

    if (!filesystem::exists(outPath))
    {
      stbi_write_png(outPath.string().c_str(), mFontInfo.mAtlasWidth, mFontInfo.mAtlasHeight, 1, atlasData.get(), mFontInfo.mAtlasWidth);
    }

      // Set our mTextureName (used to confirm we have a texture to read into)
    mTextureName = texName;
  }
}
