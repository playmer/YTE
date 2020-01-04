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

//#include "assimp/Importer.hpp"
//#include "assimp/postprocess.h"
//#include "assimp/scene.h"
#include "stb/stb_image.h"

#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Core/Utilities.hpp"

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
    //AddAction<ImportMenu>("Animation", &ImportMenu::ImportAnimation, this);
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
    //QFileDialog dialog(nullptr);
    //dialog.setFileMode(QFileDialog::ExistingFiles);
    //dialog.setNameFilter(tr("Animations (*.fbx)"));
    //dialog.setDirectory(QDir::homePath());
    //
    //QStringList files;
    //
    //if (dialog.exec())
    //{
    //  files = dialog.selectedFiles();
    //}
    //
    //if (files.isEmpty())
    //{
    //  return;
    //}
    //
    //for (auto filename : files)
    //{
    //
    //  namespace fs = std::filesystem;
    //  auto stdFileName = filename.toStdString();
    //  fs::path animationFile{ stdFileName };
    //  animationFile = fs::canonical(animationFile);
    //  fs::path animationDirectory = animationFile.parent_path();
    //  std::string stdAnimationDirectory = animationDirectory.string();
    //
    //  Assimp::Importer Importer;
    //
    //  auto pScene = Importer.ReadFile(stdFileName, 0);
    //
    //  if (nullptr == pScene)
    //  {
    //    mWorkspace->GetWidget<OutputConsole>()->PrintLnC(OutputConsole::Color::Red,
    //                                                     "Could not find a valid fbx named %s",
    //                                                     stdAnimationDirectory.c_str());
    //    return;
    //  }
    //
    //  if (pScene->mNumAnimations == 0)
    //  {
    //    mConsole->PrintLnC(OutputConsole::Color::Red,
    //                       "No animations are found in file %s",
    //                       stdAnimationDirectory.c_str());
    //    return;
    //  }
    //
    //  mConsole->PrintLnC(OutputConsole::Color::Green,
    //                     "Importing Animation: %s",
    //                     stdAnimationDirectory.c_str());
    //
    //  for (unsigned int i = 0; i < pScene->mNumAnimations; i++)
    //  {
    //    auto animation = pScene->mAnimations[i];
    //    std::string animationName = animation->mName.data;
    //
    //    mConsole->PrintLnC(OutputConsole::Color::Green,
    //                       "Animation Name (%s), Animation Index (%i):",
    //                       animationName.c_str(),
    //                       i);
    //  }
    //
    //  fs::path workingDir{ YTE::Path::GetGamePath().String() };
    //  fs::path assetsDir{ workingDir.parent_path() };
    //  fs::path animationDir{ assetsDir / L"Animations" };
    //
    //  std::error_code code;
    //
    //  fs::copy(animationFile,
    //           animationDir / animationFile.filename(),
    //           fs::copy_options::recursive |
    //           fs::copy_options::overwrite_existing,
    //           code);
    //
    //
    //  // copy the face animation files if they exist
    //  fs::path animStem = animationFile;
    //  animStem.replace_extension("");
    //
    //  fs::path eyePath(animStem.string() + "_EyeAnim.txt");
    //  std::ifstream eyeFile(eyePath);
    //
    //  if (eyeFile.good())
    //  {
    //    fs::copy(eyePath,
    //             animationDir / eyePath.filename(),
    //             fs::copy_options::recursive |
    //             fs::copy_options::overwrite_existing,
    //             code);
    //  }
    //  eyeFile.close();
    //
    //  fs::path mouthPath(animStem.string() + "_MouthAnim.txt");
    //  std::ifstream mouthFile(mouthPath);
    //
    //  if (mouthFile.good())
    //  {
    //    fs::copy(mouthPath,
    //             animationDir / mouthPath.filename(),
    //             fs::copy_options::recursive |
    //             fs::copy_options::overwrite_existing,
    //             code);
    //  }
    //  mouthFile.close();
    //}
  }

  
  void WriteMeshToFile(std::string const& aName, YTE::Mesh const& aMesh);
  void ImportMesh(YTE::Mesh& aMesh, const std::string& aFile);

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

      ImportMesh(mesh, file);

      fs::path pathToFile = file;
      WriteMeshToFile(pathToFile.stem().u8string(), mesh);
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

















#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "glm/gtc/type_ptr.hpp"

#include "YTE/StandardLibrary/File.hpp"

namespace YTEditor
{
  static YTE::SubmeshData::TextureType ToYTE(aiTextureType aType)
  {
    switch (aType)
    {
      case aiTextureType_DIFFUSE: return YTE::SubmeshData::TextureType::Diffuse;
      case aiTextureType_SPECULAR: return YTE::SubmeshData::TextureType::Specular;
      case aiTextureType_AMBIENT: return YTE::SubmeshData::TextureType::Ambient;
      case aiTextureType_EMISSIVE: return YTE::SubmeshData::TextureType::Emissive;
      case aiTextureType_HEIGHT: return YTE::SubmeshData::TextureType::Height;
      case aiTextureType_NORMALS: return YTE::SubmeshData::TextureType::Normal;
      case aiTextureType_SHININESS: return YTE::SubmeshData::TextureType::Shininess;
      case aiTextureType_OPACITY: return YTE::SubmeshData::TextureType::Opacity;
      case aiTextureType_DISPLACEMENT: return YTE::SubmeshData::TextureType::Displacment;
      case aiTextureType_LIGHTMAP: return YTE::SubmeshData::TextureType::Lightmap;
      case aiTextureType_REFLECTION: return YTE::SubmeshData::TextureType::Reflection;
    }

    return YTE::SubmeshData::TextureType::Unknown;
  }

  static inline
  glm::vec3 ToGlm(const aiVector3D *aVector)
  {
    return { aVector->x, aVector->y ,aVector->z };
  }

  static inline
  glm::vec3 ToGlm(const aiColor3D *aVector)
  {
    return { aVector->r, aVector->g ,aVector->b };
  }

  static inline
  glm::quat ToGlm(const aiQuaternion *aQuat)
  {
    glm::quat quaternion;

    quaternion.x = aQuat->x;
    quaternion.y = aQuat->y;
    quaternion.z = aQuat->z;
    quaternion.w = aQuat->w;

    return quaternion;
  }

  static inline
  glm::mat4 ToGlm(const aiMatrix4x4 &aMatrix)
  {
    return glm::transpose(glm::make_mat4(&aMatrix.a1));
  }

  static inline
  aiMatrix4x4 ToAssimp(const glm::mat4 &aMatrix)
  {
    auto transposed = glm::transpose(aMatrix);
    return *(reinterpret_cast<aiMatrix4x4*>(glm::value_ptr(transposed)));
  }

  struct SkeletonHeader
  {
    YTE::u64 mBoneDataSize;
    YTE::u64 mVertexSkeletonDataSize;
    YTE::u64 mBoneMappingSize;
  };
  
  void WriteSkeletonToFile(std::string const& aName, YTE::Skeleton const& aSkeleton)
  {
    std::string fileName = aName + ".YTESkeleton";
    fileName = YTE::Path::GetModelPath(YTE::Path::GetGamePath(), fileName);

    YTE::FileWriter file{ fileName };

    if (file.mOpened)
    {
      SkeletonHeader skeletonHeader;
        
      skeletonHeader.mBoneMappingSize = static_cast<size_t>(aSkeleton.mBones.size());;
      skeletonHeader.mBoneDataSize = static_cast<size_t>(aSkeleton.mBoneData.size());
      skeletonHeader.mVertexSkeletonDataSize = static_cast<size_t>(aSkeleton.mVertexSkeletonData.size());

      // The "header" of the file.
      file.Write(skeletonHeader);
      file.Write(aSkeleton.mGlobalInverseTransform);
      file.Write(aSkeleton.mDefaultOffsets);

      file.Write(aSkeleton.mBoneData.data(), aSkeleton.mBoneData.size());
      file.Write(aSkeleton.mVertexSkeletonData.data(), aSkeleton.mVertexSkeletonData.size());

      for (auto& [key, bone] : aSkeleton.mBones)
      {
        file.Write(static_cast<YTE::u64>(bone));
        file.Write(static_cast<YTE::u64>(key.size()));
        file.Write(key.data(), key.size());
      }
    }
  }
  
  struct SubmeshHeader
  {
    YTE::u64 mNumberOfPositions;
    YTE::u64 mNumberOfTextureCoordinates;
    YTE::u64 mNumberOfNormals;
    YTE::u64 mNumberOfColors;
    YTE::u64 mNumberOfTangents;
    YTE::u64 mNumberOfBinormals;
    YTE::u64 mNumberOfBitangents;
    YTE::u64 mNumberOfBoneWeights;
    YTE::u64 mNumberOfBoneWeights2;
    YTE::u64 mNumberOfBoneIds;
    YTE::u64 mNumberOfBoneIds2;
    YTE::u64 mNumberOfIndices;
    YTE::u64 mNameSize;
    YTE::u64 mMaterialNameSize;
    YTE::u64 mShaderSetNameSize;
    YTE::u64 mNumberOfTextures;
  };

  struct MeshHeader
  {
    YTE::u64 mNumberOfSubmeshes;
    YTE::u64 mHasSkeleton;
  };

  struct TextureDataHeader
  {
    YTE::u64 mStringSize;
    YTE::TextureViewType mViewType = YTE::TextureViewType::e2D;
    YTE::SubmeshData::TextureType mSamplerType;
  };


  
  void WriteMeshToFile(std::string const& aName, YTE::Mesh const& aMesh)
  {
    OPTICK_EVENT();
    std::string fileName = aName + ".YTEMesh";

    fileName = YTE::Path::GetModelPath(YTE::Path::GetGamePath(), fileName);

    if (aMesh.mSkeleton.HasBones())
    {
      WriteSkeletonToFile(aName, aMesh.mSkeleton);
    }

    YTE::FileWriter file{ fileName };

    if (file.mOpened)
    {
      auto& submeshes = aMesh.mParts;

      MeshHeader meshHeader;
      meshHeader.mNumberOfSubmeshes = submeshes.size();
      meshHeader.mHasSkeleton = aMesh.mSkeleton.HasBones();

      // The "header" of the file.
      file.Write(meshHeader);
      file.Write(aMesh.mDimension);

      // Write submesh data, submesh data always starts with a submesh header.
      for (auto& submesh : submeshes)
      {
        auto& submeshData = submesh.mData;
        auto& vertexData = submeshData.mVertexData;

        SubmeshHeader submeshHeader;
        
        submeshHeader.mNumberOfPositions = static_cast<YTE::u64>(vertexData.mPositionData.size());
        submeshHeader.mNumberOfTextureCoordinates = static_cast<YTE::u64>(vertexData.mTextureCoordinatesData.size());
        submeshHeader.mNumberOfNormals = static_cast<YTE::u64>(vertexData.mNormalData.size());
        submeshHeader.mNumberOfColors = static_cast<YTE::u64>(vertexData.mColorData.size());
        submeshHeader.mNumberOfTangents = static_cast<YTE::u64>(vertexData.mTangentData.size());
        submeshHeader.mNumberOfBinormals = static_cast<YTE::u64>(vertexData.mBinormalData.size());
        submeshHeader.mNumberOfBitangents = static_cast<YTE::u64>(vertexData.mBitangentData.size());
        submeshHeader.mNumberOfBoneWeights = static_cast<YTE::u64>(vertexData.mBoneWeightsData.size());
        submeshHeader.mNumberOfBoneWeights2 = static_cast<YTE::u64>(vertexData.mBoneWeights2Data.size());
        submeshHeader.mNumberOfBoneIds = static_cast<YTE::u64>(vertexData.mBoneIDsData.size());
        submeshHeader.mNumberOfBoneIds2 = static_cast<YTE::u64>(vertexData.mBoneIDs2Data.size());
        submeshHeader.mNumberOfIndices = static_cast<YTE::u64>(submeshData.mIndexData.size());
        
        submeshHeader.mNameSize = submeshData.mName.size();
        submeshHeader.mMaterialNameSize = submeshData.mMaterialName.size();
        submeshHeader.mShaderSetNameSize = submeshData.mShaderSetName.size();
        submeshHeader.mNumberOfTextures = submeshData.mTextureData.size();

        file.Write(submeshHeader);

        file.Write(submeshData.mName.data(), submeshData.mName.size());
        file.Write(submeshData.mMaterialName.data(), submeshData.mMaterialName.size());
        file.Write(submeshData.mShaderSetName.data(), submeshData.mShaderSetName.size());
        
        file.Write(vertexData.mPositionData.data(), vertexData.mPositionData.size());
        file.Write(vertexData.mTextureCoordinatesData.data(), vertexData.mTextureCoordinatesData.size());
        file.Write(vertexData.mNormalData.data(), vertexData.mNormalData.size());
        file.Write(vertexData.mColorData.data(), vertexData.mColorData.size());
        file.Write(vertexData.mTangentData.data(), vertexData.mTangentData.size());
        file.Write(vertexData.mBinormalData.data(), vertexData.mBinormalData.size());
        file.Write(vertexData.mBitangentData.data(), vertexData.mBitangentData.size());
        file.Write(vertexData.mBoneWeightsData.data(), vertexData.mBoneWeightsData.size());
        file.Write(vertexData.mBoneWeights2Data.data(), vertexData.mBoneWeights2Data.size());
        file.Write(vertexData.mBoneIDsData.data(), vertexData.mBoneIDsData.size());
        file.Write(vertexData.mBoneIDs2Data.data(), vertexData.mBoneIDs2Data.size());
        file.Write(submeshData.mIndexData.data(), submeshData.mIndexData.size());

        file.Write(submeshData.mUBOMaterial);
        file.Write(submeshData.mDimension);

        for (auto& texture : submeshData.mTextureData)
        {
          file.Write(TextureDataHeader{ texture.mName.size(), texture.mViewType, texture.mSamplerType });

          file.Write(texture.mName.data(), texture.mName.size());
        }
      }
    }
  }

  void ImportSubMesh(YTE::Mesh* aYTEMesh, 
                     YTE::Submesh& aSubmesh, 
                     const aiScene* aScene, 
                     const aiMesh* aMesh, 
                     YTE::Skeleton* aSkeleton, 
                     uint32_t aBoneStartingVertexOffset)
  {
    OPTICK_EVENT();

    aSubmesh.mData.mMesh = aYTEMesh;

    aiColor3D pColor(0.f, 0.f, 0.f);
    aScene->mMaterials[aMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE,
      pColor);

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    auto material = aScene->mMaterials[aMesh->mMaterialIndex];

    aiString name;

    aiColor3D Diffuse;
    aiColor3D Ambient;
    aiColor3D Specular;
    aiColor3D Emissive;
    aiColor3D Transparent;
    aiColor3D Reflective;

    material->Get(AI_MATKEY_COLOR_DIFFUSE, Diffuse);
    material->Get(AI_MATKEY_COLOR_AMBIENT, Ambient);
    material->Get(AI_MATKEY_COLOR_SPECULAR, Specular);
    material->Get(AI_MATKEY_COLOR_EMISSIVE, Emissive);
    material->Get(AI_MATKEY_COLOR_TRANSPARENT, Transparent);
    material->Get(AI_MATKEY_COLOR_REFLECTIVE, Reflective);

    auto& uboMaterial = aSubmesh.mData.mUBOMaterial;

    uboMaterial.mDiffuse = glm::vec4(ToGlm(&Diffuse), 1.0f);
    uboMaterial.mAmbient = glm::vec4(ToGlm(&Ambient), 1.0f);
    uboMaterial.mSpecular = glm::vec4(ToGlm(&Specular), 1.0f);
    uboMaterial.mEmissive = glm::vec4(ToGlm(&Emissive), 1.0f);
    uboMaterial.mTransparent = glm::vec4(ToGlm(&Transparent), 1.0f);
    uboMaterial.mReflective = glm::vec4(ToGlm(&Reflective), 1.0f);
    uboMaterial.mFlags = 0;

    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_OPACITY, uboMaterial.mOpacity);
    material->Get(AI_MATKEY_SHININESS, uboMaterial.mShininess);
    material->Get(AI_MATKEY_SHININESS_STRENGTH, uboMaterial.mShininessStrength);
    material->Get(AI_MATKEY_REFLECTIVITY, uboMaterial.mReflectivity);
    material->Get(AI_MATKEY_REFRACTI, uboMaterial.mReflectiveIndex);
    material->Get(AI_MATKEY_BUMPSCALING, uboMaterial.mBumpScaling);

    aSubmesh.mData.mName = aMesh->mName.C_Str();
    aSubmesh.mData.mMaterialName = name.C_Str();

    std::array textureTypesSupported{
      // The texture is combined with the result of the diffuse lighting equation.
      aiTextureType_DIFFUSE,

      // The texture is combined with the result of the specular lighting equation.
      aiTextureType_SPECULAR,

      //// The texture is combined with the result of the ambient lighting equation.
      //aiTextureType_AMBIENT,

      //// The texture is added to the result of the lighting calculation. It isn't 
      //// influenced by incoming light.
      //aiTextureType_EMISSIVE,

      //// Height Map: By convention, higher gray-scale values stand for higher elevations 
      //// from the base height.
      //aiTextureType_HEIGHT,

      // Normal Map: (Tangent Space) Again, there are several conventions for tangent-space 
      // normal maps. Assimp does (intentionally) not distinguish here.
      aiTextureType_NORMALS,

      //// The texture defines the glossiness of the material.
      //// The glossiness is in fact the exponent of the specular (phong) lighting 
      //// equation. Usually there is a conversion function defined to map the linear
      //// color values in the texture to a suitable exponent. Have fun.
      //aiTextureType_SHININESS,

      //// The texture defines per-pixel opacity. Usually 'white' means opaque and 
      //// 'black' means 'transparency'. Or quite the opposite. Have fun.
      //aiTextureType_OPACITY,

      //// Displacement texture: The exact purpose and format is application-dependent. 
      //// Higher color values stand for higher vertex displacements.
      //aiTextureType_DISPLACEMENT,

      //// Lightmap texture: (aka Ambient Occlusion) Both 'Lightmaps' and dedicated 
      //// 'ambient occlusion maps' are covered by this material property. The texture contains 
      //// a scaling value for the final color value of a pixel. Its intensity is not affected by 
      //// incoming light.
      //aiTextureType_LIGHTMAP,

      //// Reflection texture: Contains the color of a perfect mirror reflection. 
      //// Rarely used, almost never for real-time applications.
      //aiTextureType_REFLECTION
    };


    std::string defaultTexture{ "white.png" };

    for (auto type : textureTypesSupported)
    {
      aiString aiTextureName;
      material->GetTexture(type, 0, &aiTextureName);

      std::string textureName;

      if (0 != aiTextureName.length)
      {
        textureName = aiTextureName.C_Str();

        if (aiTextureType_NORMALS == type)
        {
          uboMaterial.mUsesNormalTexture = 1;
        }
      }
      else
      {
        textureName = defaultTexture;
      }

      aSubmesh.mData.mTextureData.emplace_back(textureName, YTE::TextureViewType::e2D, ToYTE(type));
    }

    if (aSkeleton->HasBones())
    {
      aSubmesh.mData.mShaderSetName = "AnimatedPhong";
    }
    else
    {
      aSubmesh.mData.mShaderSetName = "Phong";
    }

    // get the vertex data with bones (if provided)
    for (unsigned int j = 0; j < aMesh->mNumVertices; j++)
    {
      const aiVector3D *pPos = aMesh->mVertices + j;
      const aiVector3D *pNormal = aMesh->mNormals + j;
      const aiVector3D *pTexCoord = &Zero3D;
      const aiVector3D *pTangent = &Zero3D;
      const aiVector3D *pBiTangent = &Zero3D;

      if (aMesh->HasTextureCoords(0))
      {
        pTexCoord = aMesh->mTextureCoords[0] + j;
      }

      if (aMesh->HasTangentsAndBitangents())
      {
        pTangent = aMesh->mTangents + j;
        pBiTangent = aMesh->mBitangents + j;
      }

      auto position = ToGlm(pPos);

      // NOTE: We do this to invert the uvs to what the texture would expect.
      auto textureCoordinates = glm::vec3{ pTexCoord->x,
                                           1.0f - pTexCoord->y,
                                           pTexCoord->z };

      auto normal = ToGlm(pNormal);
      auto color = glm::vec4{ ToGlm(&pColor), 1.0f };
      auto tangent = ToGlm(pTangent);
      auto binormal = glm::cross(tangent, normal);
      auto bitangent = ToGlm(pBiTangent);

      glm::vec3 boneWeights;
      glm::vec2 boneWeights2;
      glm::ivec3 boneIDs;
      glm::ivec2 boneIDs2;

      if (aSkeleton->HasBones())
      {
        auto vertexData = aSkeleton->GetVertexBoneData()[aBoneStartingVertexOffset + j];

        // has bones, now we find the weights for this vertex
        for (uint32_t i = 0; i < YTE::BoneConstants::MaxBonesPerVertex1; ++i)
        {
          boneWeights[i] = vertexData.mWeights[i];
          boneIDs[i] = vertexData.mIDs[i];
        }

        for (uint32_t i = 0; i < YTE::BoneConstants::MaxBonesPerVertex2; ++i)
        {
          boneWeights2[i] = vertexData.mWeights[YTE::BoneConstants::MaxBonesPerVertex1 + i];
          boneIDs2[i] = vertexData.mIDs[YTE::BoneConstants::MaxBonesPerVertex1 + i];
        }
      }
      else
      {
        // no bones, so default weights
        boneWeights = glm::vec3(0.0f, 0.0f, 0.0f);
        boneWeights2 = glm::vec2(0.0f, 0.0f);
        boneIDs = glm::ivec3(0, 0, 0);
        boneIDs2 = glm::ivec2(0, 0);
      }
      
      aSubmesh.mData.mVertexData.mPositionData.emplace_back(position);
      aSubmesh.mData.mVertexData.mTextureCoordinatesData.emplace_back(textureCoordinates);
      aSubmesh.mData.mVertexData.mNormalData.emplace_back(normal);
      aSubmesh.mData.mVertexData.mColorData.emplace_back(color);
      aSubmesh.mData.mVertexData.mTangentData.emplace_back(tangent);
      aSubmesh.mData.mVertexData.mBinormalData.emplace_back(binormal);
      aSubmesh.mData.mVertexData.mBitangentData.emplace_back(bitangent);
      aSubmesh.mData.mVertexData.mBoneWeightsData.emplace_back(boneWeights);
      aSubmesh.mData.mVertexData.mBoneWeights2Data.emplace_back(boneWeights2);
      aSubmesh.mData.mVertexData.mBoneIDsData.emplace_back(boneIDs);
      aSubmesh.mData.mVertexData.mBoneIDs2Data.emplace_back(boneIDs2);

      aSubmesh.mData.mInitialTextureCoordinates.emplace_back(textureCoordinates);
    }

    uint32_t indexBase = static_cast<uint32_t>(aSubmesh.mData.mIndexData.size());

    for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
    {
      const aiFace &Face = aMesh->mFaces[j];

      if (Face.mNumIndices != 3)
      {
        continue;
      }

      aSubmesh.mData.mIndexData.push_back(indexBase + Face.mIndices[0]);
      aSubmesh.mData.mIndexData.push_back(indexBase + Face.mIndices[1]);
      aSubmesh.mData.mIndexData.push_back(indexBase + Face.mIndices[2]);
    }

    YTE::CalculateSubMeshDimensions(aSubmesh);

    DebugAssert((aSubmesh.mData.mIndexData.size() % 3) == 0, "Index buffer must be divisible by 3.");
  }

  
  void ImportMesh(YTE::Mesh& aMesh, const std::string &aFile)
  {
    OPTICK_EVENT();

    Assimp::Importer Importer;

    aiScene const* meshScene = nullptr;

    {
      OPTICK_EVENT("Importing Mesh");

      meshScene = Importer.ReadFile(aFile.c_str(),
        aiProcess_Triangulate |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals);
    }

    if (meshScene)
    {
      // If this scene has bones, we need to reload it without pre-transforming the
      // vertices.
      if (false == YTE::Skeleton::HasBones(meshScene))
      {
        OPTICK_EVENT("Applying aiProcess_PreTransformVertices");

        Importer.ApplyPostProcessing(
          //aiProcess_Triangulate |
          //aiProcess_GenSmoothNormals |
          //aiProcess_CalcTangentSpace |
          aiProcess_PreTransformVertices
        );
      }
      else
      {
        // Load bone data
        aMesh.mSkeleton.Initialize(meshScene);
      }

      aMesh.mParts.clear();
      aMesh.mParts.reserve(meshScene->mNumMeshes);

      //printf("Mesh FileName: %s\n", aFile.c_str());

      aMesh.mParts.resize(static_cast<size_t>(meshScene->mNumMeshes));

      // Load Mesh
      YTE::u32 startingVertex = 0;
      for (size_t i = 0; i < aMesh.mParts.size(); i++)
      {
        ImportSubMesh(&aMesh, aMesh.mParts[i], meshScene, meshScene->mMeshes[i], &aMesh.mSkeleton, startingVertex);

        startingVertex += meshScene->mMeshes[i]->mNumVertices;
      }

      aMesh.mName = aFile;
    }

    aMesh.mDimension = YTE::CalculateDimensions(aMesh.mParts);
  }
}
