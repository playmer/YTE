/******************************************************************************/
/*!
* \author Isaac Dayton
* \date   2018/01/19
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <stb/stb_truetype.h>

#include "fmt/format.h"

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/BaseModel.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  struct Font
  {
    const float mSize = 64;
    const int mAtlasWidth = 1024;
    const int mAtlasHeight = 1024;
    const int mFirstChar = ' ';
    const int mCharCount = '~' - ' ';
    std::unique_ptr<stbtt_packedchar[]> mCharInfo;
  };

  namespace AlignmentX
  {
    enum Type { Left, Center, Right, COUNT };
    static std::string Names[] = { "Left", "Center", "Right" };
  }

  namespace AlignmentY
  {
    enum Type { Top, Center, Bottom, COUNT };
    static std::string Names[] = { "Top", "Center", "Bottom" };
  }

  class SpriteText : public BaseModel
  {
    struct BoundingBox
    {
      float xMin = FLT_MAX;
      float xMax = -FLT_MAX;
      float yMin = FLT_MAX;
      float yMax = -FLT_MAX;
    };

  public:
    YTEDeclareType(SpriteText);
    YTE_Shared SpriteText(Composition *aOwner, Space *aSpace);
    YTE_Shared ~SpriteText();

    YTE_Shared void AssetInitialize() override;
    YTE_Shared void NativeInitialize() override;
    
    // PROPERTIES /////////////////////////////////////////
    std::string GetText()
    {
      return mText;
    }

    void SetText(std::string &aText)
    {
      mText = aText;

      if (!mConstructing)
      {
        CreateSpriteText();
      }
    }

    std::string GetFont()
    {
      return mFontName;
    }

    void SetFont(std::string &aFont)
    {
      if (aFont != mFontName && aFont.size() != 0)
      {
        mFontName = aFont;

        PrepareFont();

        if (!mConstructing)
        {
          CreateSpriteText();
        }
      }
    }

    float GetFontSize()
    {
      return mFontSize * 10.f;
    }

    void SetFontSize(float& aFontSize)
    {
      mFontSize = aFontSize / 10.f;

      if (!mConstructing)
      {
        CreateSpriteText();
      }
    }

    const std::string& GetAlignmentX() const { return AlignmentX::Names[mAlignX]; }
    void SetAlignmentX(const std::string& aAlignment)
    {
      for (int i = 0; i < AlignmentX::COUNT; ++i)
      {
        if (AlignmentX::Names[i].compare(aAlignment) == 0 && mAlignX != AlignmentX::Type(i))
        {
          mAlignX = AlignmentX::Type(i);
          CreateSpriteText();
        }
      }
    }

    const std::string& GetAlignmentY() const { return AlignmentY::Names[mAlignY]; }
    void SetAlignmentY(const std::string& aAlignment)
    {
      for (int i = 0; i < AlignmentY::COUNT; ++i)
      {
        if (AlignmentY::Names[i].compare(aAlignment) == 0 && mAlignY != AlignmentY::Type(i))
        {
          mAlignY = AlignmentY::Type(i);
          CreateSpriteText();
        }
      }
    }

    float GetLineLength() const { return mLineLength; }
    void SetLineLength(float aLineLength) 
    { 
      mLineLength = aLineLength;
      CreateSpriteText();
    }
    ////////////////////////////////////////////////////////

    YTE_Shared void CreateSpriteText();
    YTE_Shared void CreateTransform();

    YTE_Shared void OnStart(LogicUpdate *);
    YTE_Shared void TransformUpdate(TransformChanged *aEvent);

    std::vector<InstantiatedModel*> GetInstantiatedModel() override
    {
      std::vector<InstantiatedModel*> toReturn;

      if (mInstantiatedSprite)
      {
          toReturn.emplace_back(mInstantiatedSprite.get());
      }
      
      return toReturn;
    }

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::string mText;
    std::string mFontName;
    std::string mTextureName;
    AlignmentX::Type mAlignX;
    AlignmentY::Type mAlignY;
    float mFontSize;
    float mLineLength;

    Font mFontInfo;
    BoundingBox mBoundingBox;

    std::unique_ptr<InstantiatedModel> mInstantiatedSprite;
    bool mConstructing;

    YTE_Shared void PrepareFont();
  };
}
