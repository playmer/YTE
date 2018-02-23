/******************************************************************************/
/*!
\file   ImportMenu.cpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
Implementation of the import menu in the menu bar.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include <array>
#include <filesystem>
#include <set>
#include <iostream>
#include <fstream>
#include <thread>

#include <qprogressdialog.h>
#include <qfiledialog.h>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "crunch/inc/crnlib.h"
#include "stb/stb_image.h"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/StandardLibrary/Range.hpp"
#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/MainWindow/MainWindow.hpp"
#include "YTEditor/MenuBar/ImportMenu.hpp"
#include "YTEditor/OutputConsole/OutputConsole.hpp"


namespace fs = std::experimental::filesystem;

namespace YTEditor
{

  ImportMenu::ImportMenu(MainWindow * aMainWindow)
    : QMenu("Import"), mMainWindow(aMainWindow)
  {
    QAction *importAnimationAct = new QAction("Animation");
    addAction(importAnimationAct);
    connect(importAnimationAct, &QAction::triggered, this, &ImportMenu::ImportAnimation);

    QAction * importModelAct = new QAction("Model");
    addAction(importModelAct);
    connect(importModelAct, &QAction::triggered, this, &ImportMenu::ImportModel);


    QAction * importTextureAct = new QAction("Texture");
    addAction(importTextureAct);
    connect(importTextureAct, &QAction::triggered, this, &ImportMenu::ImportTexture);
  }

  ImportMenu::~ImportMenu()
  {
  }

  // CRN/DDS compression callback function.
  static crn_bool progress_callback_func(crn_uint32 phase_index,
    crn_uint32 total_phases,
    crn_uint32 subphase_index,
    crn_uint32 total_subphases,
    void *pUser_data_ptr)
  {
    int percentage_complete = (int)(.5f +
                              (phase_index + float(subphase_index) / total_subphases) *
                              100.0f) / total_phases;

    QProgressDialog *progress = static_cast<QProgressDialog*>(pUser_data_ptr);

    if (progress->wasCanceled())
    {
      return false;
    }

    // Sometimes the progress calculation is "wrong" and goes backwards.
    // Despite it possibly being correct, we want it to only ever move up,
    // hence us only setting the value if it's higher.
    if (progress->value() < percentage_complete)
    {
      progress->setValue(percentage_complete);
    }

    return true;
  }

  void ImportMenu::ImportAnimation()
  {
    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Import Animation"),
      QDir::homePath(),
      tr("Animations (*.fbx)"));

    if (fileName == "")
    {
      return;
    }

    namespace fs = std::experimental::filesystem;
    auto stdFileName = fileName.toStdString();
    fs::path animationFile{ stdFileName };
    animationFile = fs::canonical(animationFile);
    fs::path animationDirectory = animationFile.parent_path();
    std::string stdAnimationDirectory = animationDirectory.string();


    Assimp::Importer Importer;

    auto pScene = Importer.ReadFile(stdFileName, 0);

    if (nullptr == pScene)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Could not find a valid fbx named %s",
        stdAnimationDirectory.c_str());
      return;
    }

    if (pScene->mNumAnimations == 0)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "No animations are found in file %s",
        stdAnimationDirectory.c_str());
      return;
    }

    mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Green,
      "Importing Animation: %s",
      stdAnimationDirectory.c_str());

    for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
    {
      auto animation = pScene->mAnimations[i];
      std::string animationName = animation->mName.data;

      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Green,
        "Animation Name (%s), Animation Index (%i):",
        animationName.c_str(),
        i);
    }

    fs::path workingDir{ YTE::Path::GetGamePath().String() };
    fs::path assetsDir{ workingDir.parent_path() };
    fs::path animationDir{ assetsDir / L"Animations" };

    std::error_code code;

    fs::copy(animationFile,
      animationDir / animationFile.filename(),
      fs::copy_options::recursive |
      fs::copy_options::overwrite_existing,
      code);
  }

  void ImportMenu::ImportModel()
  {
    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import Model"),
                                                    QDir::homePath(),
                                                    tr("Models (*.fbx)"));

    if (fileName == "")
    {
      return;
    }

    auto stdFileName = fileName.toStdString();
    fs::path meshFile{ stdFileName };
    meshFile = fs::canonical(meshFile);
    fs::path meshDirectory = meshFile.parent_path();
    std::string stdMeshDirectory = meshDirectory.string();


    struct TextureType
    {
      aiTextureType mType;
      bool mShouldWarn;
      const char *mName;
    };

    Assimp::Importer Importer;

    auto pScene = Importer.ReadFile(stdFileName,
      aiProcess_FlipWindingOrder |
      aiProcess_Triangulate |
      aiProcess_PreTransformVertices |
      aiProcess_CalcTangentSpace |
      aiProcess_GenSmoothNormals);

    std::set<fs::path> textures;

    if (nullptr == pScene)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Could not find a valid fbx named %s",
        stdMeshDirectory.c_str());
      return;
    }

    if (pScene->mNumMeshes == 0)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "No meshes are found in file %s",
        stdMeshDirectory.c_str());
      return;
    }

    mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Green,
      "Importing Mesh: %s",
      stdMeshDirectory.c_str());

    for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
    {
      auto mesh = pScene->mMeshes[i];
      auto meshName = mesh->mName;

      auto material = pScene->mMaterials[mesh->mMaterialIndex];

      aiString aiMaterialName;
      material->Get(AI_MATKEY_NAME, aiMaterialName);

      std::string materialName{ aiMaterialName.C_Str() };
      std::string shaderName = materialName.substr(0, materialName.find_first_of('_'));


      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Green,
        "Material Name (%s), Shader Name(%s):",
        materialName.c_str(),
        shaderName.c_str());

      std::vector<TextureType> textureTypes{ { aiTextureType_NONE, true, "NONE" },
      { aiTextureType_DIFFUSE, false , "DIFFUSE" },
      { aiTextureType_SPECULAR, false, "SPECULAR" },
      { aiTextureType_AMBIENT, true, "AMBIENT" },
      { aiTextureType_EMISSIVE, true, "EMISSIVE" },
      { aiTextureType_HEIGHT, true, "HEIGHT" },
      { aiTextureType_NORMALS, false, "NORMALS" },
      { aiTextureType_SHININESS, true, "SHININESS" },
      { aiTextureType_OPACITY, true, "OPACITY" },
      { aiTextureType_DISPLACEMENT, true, "DISPLACEMENT" },
      { aiTextureType_LIGHTMAP, true, "LIGHTMAP" },
      { aiTextureType_REFLECTION, true, "REFLECTION" },
      { aiTextureType_UNKNOWN, true, "UNKNOWN" } };


      for (auto &type : textureTypes)
      {
        aiString texture;

        auto count = material->GetTextureCount(type.mType);

        if (0 == count)
        {
          continue;
        }

        if (0 != count && type.mShouldWarn)
        {
          mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Blue,
            "Texture types of type %s are not currently supported, "
            "but will be converted and copied.",
            type.mName);
        }

        if (1 != count)
        {
          mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
            "We will only process the first texture of type %s in "
            "material %s. (There are %d remaining)",
            type.mName,
            materialName.c_str(),
            count - 1);
        }

        for (unsigned j = 0; j < count; j++)
        {
          material->GetTexture(type.mType, j, &texture);

          fs::path texturePath{ texture.C_Str() };
          auto fullTexturePath = meshDirectory / texturePath.filename();

          std::string stdFullTexturePath{ fullTexturePath.string() };

          mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Green,
            "  Type: %s, Texture Path: %s",
            type.mName,
            stdFullTexturePath.c_str());

          textures.emplace(fullTexturePath);
        }

        mMainWindow->GetOutputConsole().PrintLn("");
      }
    }

    fs::path workingDir{ YTE::Path::GetGamePath().String() };
    fs::path assetsDir{ workingDir.parent_path() };
    fs::path modelDir{ assetsDir / L"Models" };
    fs::path textureDir{ assetsDir / L"Textures" };
    fs::path textureOriginalDir{ textureDir / L"Originals" };
    fs::path textureCrunchDir{ textureDir / L"Crunch" };

    // Check textures meeting our expectations.
    bool correctTextures = true;
    for (auto &texture : textures)
    {
      if (false == texture.has_extension())
      {
        mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                                 "Texture %s doesn't have "
                                                 "an extension.\n",
                                                 texture.c_str());
        correctTextures = false;
        continue;
      }
    }

    if (false == correctTextures)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                               "Aborting model load!\n");
    }

    std::error_code code;
    fs::create_directories(textureOriginalDir, code);
    fs::create_directories(textureCrunchDir, code);

    std::vector<TextureHelper> loadedTextures;

    // Process Texture files
    for (auto &texture : textures)
    {
      loadedTextures.emplace_back(TextureFileChecks(texture.string()));

      if (loadedTextures.back().mFailure)
      {
        return;
      }
    }

    for (auto [texture, i]: YTE::enumerate(loadedTextures))
    {
      if (false == CompressTextureFile(*texture))
      {
        for (size_t j = 0; j < i; ++j)
        {
          fs::remove(loadedTextures[j].mCrunchFilePath, code);
        }

        return;
      }
    }


    for (auto &texture : loadedTextures)
    {
      CopyTextureFile(texture);
    }

    fs::copy(meshFile,
             modelDir / meshFile.filename(),
             fs::copy_options::recursive |
             fs::copy_options::overwrite_existing,
             code);
  }

  void ImportMenu::ImportTexture()
  {
    // Construct a file dialog for selecting the correct file
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Import Texture"),
                                                    QDir::homePath(),
                                                    tr("Textures (*.bmp *.hdr *.png *.jpg)"));

    if (fileName == "")
    {
      return;
    }

    auto texture = TextureFileChecks(fileName.toUtf8().toStdString());
    
    if (CompressTextureFile(texture))
    {
      CopyTextureFile(texture);
    }
    else
    {
      std::error_code code;
      fs::remove(texture.mCrunchFilePath, code);
    }
  }

  ImportMenu::TextureHelper ImportMenu::TextureFileChecks(std::string aTexture)
  {
    TextureHelper helper;

    namespace fs = std::experimental::filesystem;
    fs::path textureFile{ aTexture };
    aTexture = fs::canonical(textureFile).string();

    helper.mTextureFile = aTexture;

    path workingDir{ YTE::Path::GetGamePath().String() };
    path assetsDir{ workingDir.parent_path() };

    helper.mTextureDirectory = assetsDir / L"Textures";

    std::string stdTextureDirectory = helper.mTextureDirectory.string();

    helper.mPixels = stbi_load(aTexture.c_str(),
                               &helper.mWidth, 
                               &helper.mHeight, 
                               &helper.mChannels, 
                               STBI_rgb_alpha);
    

    if (nullptr == helper.mPixels)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                               "Couldn't load texture file at: %s",
                                               aTexture.c_str());

      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                               "Aborting texture load!");

      helper.mFailure = true;
      return helper;
    }

    if ((3 != helper.mChannels) && (4 != helper.mChannels))
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                               "Texture file has %d channels instead of 3 (rgb) or 4 (rgba): %s",
                                               helper.mChannels,
                                               aTexture.c_str());

      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                               "Aborting texture load!");

      helper.mFailure = true;
      return helper;
    }

    helper.mHasAlpha = (helper.mChannels == 3) ? false : true;

    return helper;
  }

  bool ImportMenu::CompressTextureFile(TextureHelper &aTexture)
  {
    //std::string message{ "Compressing " };
    //message += aTexture.mTextureFile;
    //
    //path textureCrunchDir{ aTexture.mTextureDirectory / L"Crunch" };
    //path textureFile{ aTexture.mTextureFile };
    //
    //aTexture.mCrunchFilePath = textureCrunchDir / textureFile.filename().concat(L".crn");
    //
    //QProgressDialog compressProgress(message.c_str(), "cancel", 0, 100, this);
    //compressProgress.setWindowModality(Qt::WindowModal);
    //
    //
    //crn_uint32 *pixelSource = reinterpret_cast<crn_uint32*>(aTexture.mPixels);
    //
    //crn_comp_params comp_params;
    //comp_params.m_width = aTexture.mWidth;
    //comp_params.m_height = aTexture.mHeight;
    //comp_params.set_flag(cCRNCompFlagPerceptual, true);
    //comp_params.set_flag(cCRNCompFlagDXT1AForTransparency, false);
    //comp_params.set_flag(cCRNCompFlagHierarchical, false);
    //comp_params.m_file_type = cCRNFileTypeCRN;
    //
    //comp_params.m_format = (aTexture.mHasAlpha ? cCRNFmtDXT5 : cCRNFmtDXT1);
    //comp_params.m_pImages[0][0] = pixelSource;
    //comp_params.m_pProgress_func = progress_callback_func;
    //comp_params.m_pProgress_func_data = &compressProgress;
    //comp_params.m_num_helper_threads = std::thread::hardware_concurrency();
    //
    //auto check = comp_params.check();
    //
    //YTE::u32 filesize;
    //auto crnFileVoid = crn_compress(comp_params, filesize);
    //
    //if (nullptr == crnFileVoid)
    //{
    //  mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
    //                                           "Failed to compress Texture file: %s",
    //                                           aTexture.mTextureFile.c_str());
    //
    //  mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
    //                                           "Aborting texture load!");
    //
    //  return false;
    //}
    //
    //const char *crnFile = static_cast<const char*>(crnFileVoid);
    //std::ofstream ostrm(aTexture.mCrunchFilePath, std::ios::binary);
    //ostrm.write(crnFile, filesize);
    //
    //crn_free_block(crnFileVoid);
    //
    //compressProgress.setValue(100);

    return true;
  }


  bool ImportMenu::CopyTextureFile(TextureHelper &aTexture)
  {
    namespace fs = std::experimental::filesystem;
    path textureOriginalDir{ aTexture.mTextureDirectory / L"Originals" };
    path textureFile{ aTexture.mTextureFile };

    std::error_code code;
    fs::copy(textureFile,
             textureOriginalDir / textureFile.filename(),
             fs::copy_options::recursive |
             fs::copy_options::overwrite_existing,
             code);

    return true;
  }
}

