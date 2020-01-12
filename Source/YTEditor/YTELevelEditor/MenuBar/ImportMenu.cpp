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

#include "YTE/StandardLibrary/FileSystem.hpp"

#include <set>
#include <iostream>
#include <fstream>
#include <thread>

#include <qprogressdialog.h>
#include <qfiledialog.h>
#include <QSlider>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QCoreApplication>

#include "stb/stb_image.h"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTETools/YTEAnimation.hpp"
#include "YTETools/YTEMesh.hpp"
#include "YTETools/YTESkeleton.hpp"
#include "YTETools/YTETexture.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTE/StandardLibrary/Range.hpp"

#include "YTE/Utilities/Utilities.hpp"

#include "YTEditor/Framework/MainWindow.hpp"
#include "YTEditor/Framework/Workspace.hpp"

#include "YTEditor/YTELevelEditor/MenuBar/ImportMenu.hpp"

#include "YTEditor/YTELevelEditor/Widgets/OutputConsole/OutputConsole.hpp"

#include "YTEditor/YTELevelEditor/YTELevelEditor.hpp"

namespace fs = std::filesystem;

namespace YTEditor
{
  ImportMenu::ImportMenu(Framework::MainWindow* aMainWindow)
    : Framework::Menu("Import", aMainWindow->GetWorkspace<YTELevelEditor>())
    , mConsole(mWorkspace->GetWidget<OutputConsole>())
  {
    AddAction<ImportMenu>("Animation", &ImportMenu::ImportAnimation, this);
    AddAction<ImportMenu>("Model", &ImportMenu::ImportModel, this);
    //AddAction<ImportMenu>("Texture", &ImportMenu::ImportTexture, this);
  }

  // CRN/DDS compression callback function.
//  static crn_bool CrunchCallbackFunction(crn_uint32 phase_index,
//                                         crn_uint32 total_phases,
//                                         crn_uint32 subphase_index,
//                                         crn_uint32 total_subphases,
//                                         void *aUserData)
//  {
//    int percentage_complete = (int)(.5f +
//                              (phase_index + float(subphase_index) / total_subphases) *
//                              100.0f) / total_phases;
//
//    QProgressDialog *progress = static_cast<QProgressDialog*>(aUserData);
//
//    QCoreApplication::processEvents();
//
//    if (progress->wasCanceled())
//    {
//      return false;
//    }
//
//    // Sometimes the progress calculation is "wrong" and goes backwards.
//    // Despite it possibly being correct, we want it to only ever move up,
//    // hence us only setting the value if it's higher.
//    if (progress->value() < percentage_complete)
//    {
//      progress->setValue(percentage_complete);
//    }
//
//    return true;
//  }

  void ImportMenu::ImportAnimation()
  {
    
    // Construct a file dialog for selecting the correct file
    
    QFileDialog dialog(nullptr);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Models (*.fbx)"));
    dialog.setDirectory(QDir::homePath());
    
    QStringList files;
    
    if (dialog.exec())
    {
      files = dialog.selectedFiles();
    }
    
    if (files.isEmpty())
    {
      return;
    }
    
    for (auto filename : files)
    {
      auto stdFileName = filename.toStdString();
      fs::path animationFile{ stdFileName };
      animationFile = fs::canonical(animationFile);

      auto file = animationFile.u8string();

      YTE::AnimationData animation = YTE::Tools::ImportAnimation(file);
      
      fs::path workingDir{ YTE::Path::GetGamePath().String() };
      fs::path assetsDir{ workingDir.parent_path() };
      fs::path animationDir{ assetsDir / L"Animations" };

      fs::path pathToFile = file;
      YTE::Tools::WriteAnimationDataToFile((animationDir / pathToFile.filename().replace_extension("").stem()).u8string(), animation);

      // copy the face animation files if they exist
      fs::path animStem = animationFile;
      animStem.replace_extension("");
      
      fs::path eyePath(animStem.string() + "_EyeAnim.txt");
      std::ifstream eyeFile(eyePath);
      
      std::error_code code;
      
      if (eyeFile.good())
      {
        fs::copy(eyePath,
                 animationDir / eyePath.filename(),
                 fs::copy_options::recursive |
                 fs::copy_options::overwrite_existing,
                 code);
      }
      eyeFile.close();
      
      fs::path mouthPath(animStem.string() + "_MouthAnim.txt");
      std::ifstream mouthFile(mouthPath);
      
      if (mouthFile.good())
      {
        fs::copy(mouthPath,
                 animationDir / mouthPath.filename(),
                 fs::copy_options::recursive |
                 fs::copy_options::overwrite_existing,
                 code);
      }
      mouthFile.close();
    }
  }

  void ImportMenu::ImportModel()
  {
    // Construct a file dialog for selecting the correct file
    
    QFileDialog dialog(nullptr);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("Models (*.fbx)"));
    dialog.setDirectory(QDir::homePath());
    
    QStringList files;
    
    if (dialog.exec())
    {
      files = dialog.selectedFiles();
    }
    
    if (files.isEmpty())
    {
      return;
    }
    
    for (auto filename : files)
    {
      auto stdFileName = filename.toStdString();
      fs::path meshFile{ stdFileName };
      meshFile = fs::canonical(meshFile);
      //fs::path meshDirectory = meshFile.parent_path();
      //std::string stdMeshDirectory = meshDirectory.string();

      YTE::Mesh mesh;

      auto file = meshFile.u8string();

      YTE::Tools::ImportMesh(mesh, file);

      fs::path pathToFile = file;
      YTE::Tools::WriteMeshToFile(pathToFile.stem().u8string(), mesh);
    }
  }

//  void ImportMenu::ImportTexture()
//  {
//    // Construct a file dialog for selecting the correct file
//    QString fileName = QFileDialog::getOpenFileName(this,
//                                                    tr("Import Texture"),
//                                                    QDir::homePath(),
//                                                    tr("Textures (*.bmp *.hdr *.png *.jpg)"));
//
//    if (fileName == "")
//    {
//      return;
//    }
//
//    auto texture = TextureFileChecks(fileName.toUtf8().toStdString());
//    
//    if (CompressTextureFile(texture))
//    {
//      CopyTextureFile(texture);
//    }
//    else
//    {
//      std::error_code code;
//      fs::remove(texture.mCrunchFilePath, code);
//    }
//  }
//
//  ImportMenu::TextureHelper ImportMenu::TextureFileChecks(std::string aTexture)
//  {
//    TextureHelper helper;
//
//    namespace fs = std::filesystem;
//    fs::path textureFile{ aTexture };
//    aTexture = fs::canonical(textureFile).string();
//
//    helper.mTextureFile = aTexture;
//
//    path workingDir{ YTE::Path::GetGamePath().String() };
//    path assetsDir{ workingDir.parent_path() };
//
//    helper.mTextureDirectory = assetsDir / L"Textures";
//
//    std::string stdTextureDirectory = helper.mTextureDirectory.string();
//
//    helper.mPixels = stbi_load(aTexture.c_str(),
//                               &helper.mWidth, 
//                               &helper.mHeight, 
//                               &helper.mChannels, 
//                               STBI_rgb_alpha);
//    
//
//    if (nullptr == helper.mPixels)
//    {
//      mConsole->PrintLnC(OutputConsole::Color::Red,
//                                               "Couldn't load texture file at: %s",
//                                               aTexture.c_str());
//
//      mConsole->PrintLnC(OutputConsole::Color::Red,
//                                               "Aborting texture load!");
//
//      helper.mFailure = true;
//      return helper;
//    }
//
//    if ((3 != helper.mChannels) && (4 != helper.mChannels))
//    {
//      mConsole->PrintLnC(OutputConsole::Color::Red,
//                                               "Texture file has %d channels instead of 3 (rgb) or 4 (rgba): %s",
//                                               helper.mChannels,
//                                               aTexture.c_str());
//
//      mConsole->PrintLnC(OutputConsole::Color::Red,
//                                               "Aborting texture load!");
//
//      helper.mFailure = true;
//      return helper;
//    }
//
//    helper.mHasAlpha = (helper.mChannels == 3) ? false : true;
//
//    return helper;
//  }
//
//  struct TextureImportData
//  {
//    crn_comp_params mParameters;
//    bool mCompress;
//  };
//
//  class ImportTextureDialog : public QDialog
//  {
//  public:
//    ImportTextureDialog(TextureImportData &aData, QWidget *aParent = nullptr)
//      : QDialog{aParent}
//      , mData(aData)
//    {
//      mSlider = new QSlider{ Qt::Orientation::Horizontal ,this };
//      mSlider->setMinimum(0);
//      mSlider->setMaximum(255);
//      mSlider->setValue(255 / 2);
//      mSlider->setFocusPolicy(Qt::StrongFocus);
//      mSlider->setTickPosition(QSlider::TicksBothSides);
//      mSlider->setTickInterval(10);
//      mSlider->setSingleStep(1);
//      mSlider->setTracking(true);
//
//      mCheckBox = new QCheckBox{ "Compress", this };
//      mCheckBox->setCheckState(Qt::Unchecked);
//      mCheckBox->setCheckable(true);
//
//      this->setModal(true);
//
//      mButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok
//                                      | QDialogButtonBox::Cancel);
//
//      this->connect(mButtonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
//      this->connect(mButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
//
//      QVBoxLayout *mainLayout = new QVBoxLayout{};
//      mainLayout->addWidget(mSlider);
//      mainLayout->addWidget(mCheckBox);
//      mainLayout->addWidget(mButtonBox);
//      setLayout(mainLayout);
//    }
//
//    ~ImportTextureDialog()
//    {
//      mData.mParameters.m_quality_level = mSlider->value();
//      mData.mCompress = mCheckBox->isChecked();
//    }
//
//
//  private:
//    QSlider *mSlider;
//    QCheckBox *mCheckBox;
//    QDialogButtonBox *mButtonBox;
//    TextureImportData &mData;
//  };
//
//  bool ImportMenu::CompressTextureFile(TextureHelper &aTexture)
//  {
//    std::string message{ "Compressing..." };
//    message += aTexture.mTextureFile;
//    
//    path textureCrunchDir{ aTexture.mTextureDirectory / L"Crunch" };
//    path textureFile{ aTexture.mTextureFile };
//    
//    aTexture.mCrunchFilePath = textureCrunchDir / textureFile.filename().concat(L".crn");
//    
//    crn_uint32 *pixelSource = reinterpret_cast<crn_uint32*>(aTexture.mPixels);
//
//    TextureImportData textureData;
//    
//    textureData.mParameters.m_width = aTexture.mWidth;
//    textureData.mParameters.m_height = aTexture.mHeight;
//    textureData.mParameters.set_flag(cCRNCompFlagPerceptual, true);
//    textureData.mParameters.set_flag(cCRNCompFlagDXT1AForTransparency, false);
//    textureData.mParameters.set_flag(cCRNCompFlagHierarchical, false);
//    textureData.mParameters.m_file_type = cCRNFileTypeCRN;
//    
//    textureData.mParameters.m_format = (aTexture.mHasAlpha ? cCRNFmtDXT5 : cCRNFmtDXT1);
//    textureData.mParameters.m_pImages[0][0] = pixelSource;
//    textureData.mParameters.m_pProgress_func = CrunchCallbackFunction;
//
//    if (std::thread::hardware_concurrency() > 1)
//    {
//      textureData.mParameters.m_num_helper_threads = std::thread::hardware_concurrency() - 1;
//    }
//
//    textureData.mParameters.m_quality_level = 255;
//    
//    { 
//      ImportTextureDialog import{ textureData, this };
//      import.exec();
//    }
//
//    if (false == textureData.mCompress)
//    {
//      return true;
//    }
//
//    QProgressDialog compressProgress(message.c_str(), "cancel", 0, 100, this);
//    compressProgress.setWindowModality(Qt::WindowModal);
//    compressProgress.setValue(0);
//
//    textureData.mParameters.m_pProgress_func_data = &compressProgress;
//    YTE::u32 filesize;
//
//    compressProgress.show();
//    QCoreApplication::processEvents();
//
//    void *crnFileVoid = crn_compress(textureData.mParameters, filesize);
//
//    if (nullptr == crnFileVoid)
//    {
//      mConsole->PrintLnC(OutputConsole::Color::Red,
//                                               "Failed to compress Texture file: %s",
//                                               aTexture.mTextureFile.c_str());
//    
//      mConsole->PrintLnC(OutputConsole::Color::Red,
//                                               "Aborting texture load!");
//    
//      return false;
//    }
//    
//    const char *crnFile = static_cast<const char*>(crnFileVoid);
//    std::ofstream ostrm(aTexture.mCrunchFilePath, std::ios::binary);
//    ostrm.write(crnFile, filesize);
//    
//    crn_free_block(crnFileVoid);
//    
//    compressProgress.setValue(100);
//
//    return true;
//  }
//
//
//  bool ImportMenu::CopyTextureFile(TextureHelper &aTexture)
//  {
//    namespace fs = std::filesystem;
//    path textureOriginalDir{ aTexture.mTextureDirectory / L"Originals" };
//    path textureFile{ aTexture.mTextureFile };
//
//    std::error_code code;
//    fs::copy(textureFile,
//             textureOriginalDir / textureFile.filename(),
//             fs::copy_options::recursive |
//             fs::copy_options::overwrite_existing,
//             code);
//
//    return true;
//  }
}
