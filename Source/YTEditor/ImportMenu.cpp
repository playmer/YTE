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

#include "ImportMenu.hpp"
#include "YTEditorMainWindow.hpp"
#include "OutputConsole.hpp"

#include "YTE/Core/Utilities.hpp"
#include "YTE/Utilities/Utilities.h"

#include "crunch/inc/crnlib.h"
#include <qprogressdialog.h>
#include <qfiledialog.h>

#include <array>
#include <filesystem>
#include <set>
#include <iostream>
#include <fstream>
#include <thread>

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "stb/stb_image.h"


ImportMenu::ImportMenu(YTEditorMainWindow * aMainWindow) : QMenu("Import"), mMainWindow(aMainWindow)
{
  QAction * importModelAct = new QAction("Model");
  addAction(importModelAct);
  connect(importModelAct, &QAction::triggered, this, &ImportMenu::ImportModel);

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



void ImportMenu::ImportModel()
{
  // Construct a file dialog for selecting the correct file
  QString fileName = QFileDialog::getOpenFileName(this,
    tr("Import Model"),
    QDir::currentPath() + "/../Models/",
    tr("Models (*.fbx)"));

  if (fileName == "")
  {
    return;
  }

  namespace fs = std::experimental::filesystem;
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
      "Could not find a valid mesh in file %s",
      stdMeshDirectory.c_str());
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

  fs::path workingDir{ YTE::cWorkingDirectory };
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

    if ("png" != texture.extension())
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
                                               "Texture %s isn't a png.\n",
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

  fs::copy(meshFile,
           modelDir / meshFile.filename(),
           fs::copy_options::recursive |
           fs::copy_options::overwrite_existing,
           code);


  // Process Texture files
  for (auto &texture : textures)
  {
    std::string textureFilePath = texture.string();
    fs::path crunchFilePath = textureCrunchDir / texture.filename().concat(L".crn");

    fs::copy(texture,
      textureOriginalDir / texture.filename(),
      fs::copy_options::recursive |
      fs::copy_options::overwrite_existing,
      code);

    int width, height, channels;

    auto pixels = stbi_load(textureFilePath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    crn_uint32 *pixelSource = reinterpret_cast<crn_uint32*>(pixels);

    if (nullptr == pixels)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Couldn't load texture file at: %s",
        textureFilePath.c_str());

      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Aborting model load!");
      return;
    }

    if ((3 != channels) && (4 != channels))
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Texture file has %d channels instead of 3 (rgb) or 4 (rgba): %s",
        channels,
        textureFilePath.c_str());

      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Aborting model load!");
      return;
    }

    bool hasAlpha = (channels == 3) ? false : true;

    std::string message{ "Compressing " };
    message += textureFilePath;

    QProgressDialog compressProgress(message.c_str(), "cancel", 0, 100, this);
    compressProgress.setWindowModality(Qt::WindowModal);

    crn_comp_params comp_params;
    comp_params.m_width = width;
    comp_params.m_height = height;
    comp_params.set_flag(cCRNCompFlagPerceptual, true);
    comp_params.set_flag(cCRNCompFlagDXT1AForTransparency, false);
    comp_params.set_flag(cCRNCompFlagHierarchical, false);
    comp_params.m_file_type = cCRNFileTypeCRN;
    comp_params.m_format = (hasAlpha ? cCRNFmtDXT5 : cCRNFmtDXT1);
    comp_params.m_pImages[0][0] = pixelSource;
    comp_params.m_pProgress_func = progress_callback_func;
    comp_params.m_pProgress_func_data = &compressProgress;
    comp_params.m_num_helper_threads = std::thread::hardware_concurrency();

    YTE::u32 filesize;
    auto crnFileVoid = crn_compress(comp_params, filesize);

    if (nullptr == crnFileVoid)
    {
      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Failed to compress Texture file: %s",
        textureFilePath.c_str());

      mMainWindow->GetOutputConsole().PrintLnC(OutputConsole::Color::Red,
        "Aborting model load!");

      return;
    }

    const char *crnFile = static_cast<const char*>(crnFileVoid);
    std::ofstream ostrm(crunchFilePath, std::ios::binary);
    ostrm.write(crnFile, filesize);

    crn_free_block(crnFileVoid);
    stbi_image_free(pixels);

    compressProgress.setValue(100);
  }
}
