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

#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
	struct Font
	{
		const float mSize = 40;
		const int mAtlasWidth = 512;
		const int mAtlasHeight = 512;
		const int mFirstChar = ' ';
		const int mCharCount = '~' - ' ';
		std::unique_ptr<stbtt_packedchar[]> mCharInfo;
	};

  class SpriteText : public Component
  {
  public:
    YTEDeclareType(SpriteText);
    SpriteText(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~SpriteText();

    void Initialize() override;
    void CreateSpriteText();

		// PROPERTIES /////////////////////////////////////////
		std::string GetText()
		{
			return mText;
		}

		void SetText(std::string &aText)
		{
			mText = aText;

			if (!mConstructing)
				CreateSpriteText();
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
			}
		}
		////////////////////////////////////////////////////////

    void CreateTransform();
    void TransformUpdate(TransformChanged *aEvent);

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;

		std::string mText;
		std::string mFontName;
		std::string mTextureName;

		Font mFontInfo;

		std::unique_ptr<InstantiatedModel> mInstantiatedSprite;
    bool mConstructing;

		void PrepareFont();
  };
}
