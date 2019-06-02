#include <fstream>

#include <stb/stb_image_write.h>

#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/Model.hpp"
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
    UnusedArguments(aComponent);

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
    RegisterType<SpriteText>();
    TypeBuilder<SpriteText> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Property<&SpriteText::GetText, &SpriteText::SetText>("Text")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&SpriteText::GetFont, &SpriteText::SetFont>("Font")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateDropDownList);

    builder.Property<&SpriteText::GetFontSize, &SpriteText::SetFontSize>("FontSize")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>();

    builder.Property<&SpriteText::GetAlignmentX, &SpriteText::SetAlignmentX>("AlignX")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateAlignXDropDownList);

    builder.Property<&SpriteText::GetAlignmentY, &SpriteText::SetAlignmentY>("AlignY")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .AddAttribute<DropDownStrings>(PopulateAlignYDropDownList);

    builder.Property<&SpriteText::GetLineLength, &SpriteText::SetLineLength>("MaxLineLength")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The max length of a single line (in world units) for word-wrapping. 0.0 disables word-wrapping");
  }

  SpriteText::SpriteText(Composition *aOwner, Space *aSpace)
    : BaseModel{ aOwner, aSpace }
    , mText("")
    , mFontName("calibri.ttf")
    , mAlignX(AlignmentX::Center)
    , mAlignY(AlignmentY::Center)
    , mLineLength(0.0f)
    , mConstructing(true)
  {
    mFontSize = 1.f;
  }

  SpriteText::~SpriteText()
  {

  }

  void SpriteText::AssetInitialize()
  {
    mRenderer = mOwner->GetEngine()->GetComponent<GraphicsSystem>()->GetRenderer();

    if (mTextureName.empty())
    {
      return;
    }

    mRenderer->RequestTexture(mTextureName);

    mConstructing = false;
  }

  void SpriteText::NativeInitialize()
  {
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    mTransform = mOwner->GetComponent<Transform>();

    mSpace->RegisterEvent<&SpriteText::OnStart>(Events::LogicUpdate, this);

    mOwner->RegisterEvent<&SpriteText::TransformUpdate>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&SpriteText::TransformUpdate>(Events::RotationChanged, this);
    //mOwner->RegisterEvent<&SpriteText::TransformUpdate>(Events::ScaleChanged, this);

    PrepareFont();
    CreateSpriteText();
  }

  void SpriteText::CreateSpriteText()
  {
    YTEProfileFunction();
    if (nullptr != mInstantiatedSprite)
    {
      mInstantiatedSprite.reset();
    }

    if (mTextureName.empty())
    {
      return;
    }

    if (mText.empty())
    {
      return;
    }

    std::string meshName = "__SpriteText";
    meshName += mOwner->GetGUID().ToString();

    SubmeshData submesh;

    u32 lastIndex = 0;
    float offsetX = 0, offsetY = 0;

    float sizeFactor = mFontSize / mFontInfo.mSize;

    submesh.mShaderSetName = "SpriteText";

    submesh.mTextureData.emplace_back(mTextureName, TextureViewType::e2D, SubmeshData::TextureType::Diffuse);
    //submesh.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Specular);
    //submesh.mTextureData.emplace_back("white.png", TextureViewType::e2D, SubmeshData::TextureType::Normal);

    submesh.mCullBackFaces = false;

      // Reset our bounding volume to initial values when new text is created
    mBoundingBox.xMin = FLT_MAX;
    mBoundingBox.xMax = -FLT_MAX;
    mBoundingBox.yMin = FLT_MAX;
    mBoundingBox.yMax = -FLT_MAX;

    float maxCharHeight = 0.0f;
    float currMaxX = -FLT_MAX;  // The xMax for the current line

    int i = 0;
    int cursor = 0;
    while (mText[i])
    {
      Vertex vert0;
      Vertex vert1;
      Vertex vert2;
      Vertex vert3;

      stbtt_aligned_quad quad;

      stbtt_GetPackedQuad(mFontInfo.mCharInfo.get(), 
                          mFontInfo.mAtlasWidth, 
                          mFontInfo.mAtlasHeight, 
                          mText[i] - mFontInfo.mFirstChar, 
                          &offsetX, 
                          &offsetY, 
                          &quad, 
                          1);

        // Mark the last ' ' we found (for word-wrap)
      if (mText[i] == ' ')
      {
        cursor = i + 1;
      }

      float left = sizeFactor * quad.x0;
      float right = sizeFactor * quad.x1;
      float top = sizeFactor * -quad.y0;
      float bottom = sizeFactor * -quad.y1;

      if (offsetY == 0.0f && (top - bottom) > maxCharHeight)
      {
        maxCharHeight = (top - bottom);
      }

        // Update text bounding volume (used for alignment)
      if (left < mBoundingBox.xMin)
      {
        mBoundingBox.xMin = left;
      }
      if (right > mBoundingBox.xMax)
      {
        mBoundingBox.xMax = right;
      }
      if (bottom < mBoundingBox.yMin)
      {
        mBoundingBox.yMin = bottom;
      }
      if (top > mBoundingBox.yMax)
      {
        mBoundingBox.yMax = top;
      }

        // Update current line maximum (used for wrapping)
      if (right > currMaxX)
      {
        currMaxX = right;
      }

        // Save vertex attributes
      vert0.mPosition = { left, bottom , 0.0 };  // Bottom-left
      vert1.mPosition = { right, bottom, 0.0 };  // Bottom-right
      vert2.mPosition = { right, top, 0.0 };  // Top-right
      vert3.mPosition = { left, top, 0.0 };  // Top-left
      vert0.mTextureCoordinates = { quad.s0, 1.0f - quad.t1, 0.0f };  // Bottom-left (UVs)
      vert1.mTextureCoordinates = { quad.s1, 1.0f - quad.t1, 0.0f };  // Bottom-right (UVs)
      vert2.mTextureCoordinates = { quad.s1, 1.0f - quad.t0, 0.0f };  // Top-right (UVs)
      vert3.mTextureCoordinates = { quad.s0, 1.0f - quad.t0, 0.0f };  // Top-left (UVs)

      submesh.mVertexData.emplace_back(vert0);
      submesh.mVertexData.emplace_back(vert1);
      submesh.mVertexData.emplace_back(vert2);
      submesh.mVertexData.emplace_back(vert3);

      submesh.mIndexData.push_back(lastIndex);
      submesh.mIndexData.push_back(lastIndex + 1);
      submesh.mIndexData.push_back(lastIndex + 2);
      submesh.mIndexData.push_back(lastIndex);
      submesh.mIndexData.push_back(lastIndex + 2);
      submesh.mIndexData.push_back(lastIndex + 3);

      lastIndex += 4;
      ++i;

        // Word wrap after characters have been placed (graphically)
      if (mLineLength != 0.0f && (currMaxX - mBoundingBox.xMin) >= mLineLength)
      {       
          // If we happen to land exactly on a space (i.e., there are no overlapping characters)
          // Just carriage return
        if (cursor == i)
        {
          offsetX = 0.0f;
          currMaxX = -FLT_MAX;
        }

          // Otherwise, move any dangling characters to the next line (so the line ends with a ' ')
        else
        {
          int vertCursor = lastIndex - (4 * (i - cursor));
          float shiftAmount = (submesh.mVertexData[vertCursor].mPosition.x - mBoundingBox.xMin);

            // Shrink our bounding box to fit the wrapped text
          mBoundingBox.xMax = submesh.mVertexData[vertCursor - 2].mPosition.x;

          while (cursor < i)
          {
            submesh.mVertexData[vertCursor].mPosition -= glm::vec3(shiftAmount, mFontSize, 0.0f);
            submesh.mVertexData[vertCursor + 1].mPosition -= glm::vec3(shiftAmount, mFontSize, 0.0f);
            submesh.mVertexData[vertCursor + 2].mPosition -= glm::vec3(shiftAmount, mFontSize, 0.0f);
            submesh.mVertexData[vertCursor + 3].mPosition -= glm::vec3(shiftAmount, mFontSize, 0.0f);

            vertCursor += 4;
            ++cursor;
          }

          offsetX = (submesh.mVertexData[vertCursor - 2].mPosition.x / sizeFactor) - sizeFactor;
          currMaxX = submesh.mVertexData[vertCursor - 2].mPosition.x;
        }

          // Line feed
        offsetY += mFontInfo.mSize;
      }
    }

      // Apply alignment
    for (auto &verts : submesh.mVertexData)
    {
        // Offsets the text to account for leaving the anchor at the bottom-left of the first row
      verts.mPosition.y -= maxCharHeight;

        // Before this switch, text is top-left aligned
      switch (mAlignX)
      {
        case AlignmentX::Center:
        {
          verts.mPosition.x -= ((mBoundingBox.xMax - mBoundingBox.xMin) / 2.0f);
          break;
        }
        case AlignmentX::Right:
        {
          verts.mPosition.x -= (mBoundingBox.xMax - mBoundingBox.xMin);
          break;
        }
      }
      
      switch (mAlignY)
      {
        case AlignmentY::Center:
        {
          verts.mPosition.y += ((mBoundingBox.yMax - mBoundingBox.yMin) / 2.0f);
          break;
        }
        case AlignmentY::Bottom:
        {
          verts.mPosition.y += (mBoundingBox.yMax - mBoundingBox.yMin);
          break;
        }
      }
      
    }

    auto view = mSpace->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(meshName, submesh, true);

    mInstantiatedSprite = mRenderer->CreateModel(view, mesh);

    ModelChanged modChange;
    modChange.Object = mOwner;
    mOwner->SendEvent(Events::ModelChanged, &modChange);

    CreateTransform();
    mUBOModel.mDiffuseColor = mInstantiatedSprite->GetUBOMaterialData().mDiffuse;
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
    mSpace->DeregisterEvent<&SpriteText::OnStart>(Events::LogicUpdate,  this);
  }

  void SpriteText::TransformUpdate(TransformChanged *aEvent)
  {
    UnusedArguments(aEvent);

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
