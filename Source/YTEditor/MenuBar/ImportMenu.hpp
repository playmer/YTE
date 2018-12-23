/******************************************************************************/
/*!
\file   ImportMenu.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The menu bar drop down for importing models.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "YTE/StandardLibrary/FileSystem.hpp"

#include <qmenu.h>

namespace YTEditor
{

  class MainWindow;

  class ImportMenu : public QMenu
  {
  public:

    ImportMenu(MainWindow *aMainWindow);
    ~ImportMenu();

  private:

    using path = std::filesystem::path;

    struct TextureHelper
    {
      TextureHelper()
        : mPixels(nullptr)
        , mWidth(0)
        , mHeight(0)
        , mChannels(0)
        , mHasAlpha(false)
        , mFailure(false)
      {

      }

      TextureHelper(TextureHelper &&aOther)
        : mTextureFile(aOther.mTextureFile)
        , mTextureDirectory(aOther.mTextureDirectory)
        , mCrunchFilePath(aOther.mTextureDirectory)
        , mPixels(aOther.mPixels)
        , mWidth(aOther.mWidth)
        , mHeight(aOther.mHeight)
        , mChannels(aOther.mChannels)
        , mHasAlpha(aOther.mHasAlpha)
        , mFailure(aOther.mFailure)
      {
        aOther.mPixels = nullptr;
      }
      
      TextureHelper& operator=(TextureHelper &&aOther)
      {
        mTextureFile = aOther.mTextureFile;
        mTextureDirectory = aOther.mTextureDirectory;
        mCrunchFilePath = aOther.mTextureDirectory;
        mPixels = aOther.mPixels;
        mWidth = aOther.mWidth;
        mHeight = aOther.mHeight;
        mChannels = aOther.mChannels;
        mHasAlpha = aOther.mHasAlpha;
        mFailure = aOther.mFailure;
        aOther.mPixels = nullptr;

        return *this;
      }

      ~TextureHelper()
      {
        if (nullptr != mPixels)
        {
          stbi_image_free(mPixels);
        }
      }

      std::string mTextureFile;
      path mTextureDirectory;
      path mCrunchFilePath;
      stbi_uc *mPixels;
      int mWidth;
      int mHeight;
      int mChannels;
      bool mHasAlpha;
      bool mFailure;
    };

    TextureHelper TextureFileChecks(std::string aTexture);
    bool CompressTextureFile(TextureHelper &aTexture);
    bool CopyTextureFile(TextureHelper &aTexture);
    void ImportTexture();
    void ImportAnimation();
    void ImportModel();

    MainWindow * mMainWindow;

  };

}