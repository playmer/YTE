#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "glm/gtc/type_ptr.hpp"

#include "YTE/StandardLibrary/File.hpp"

#include "YTETools/ImportMathTypeConverters.hpp"
#include "YTETools/YTEMesh.hpp"
#include "YTETools/YTESkeleton.hpp"
#include "YTETools/YTESkeletonInternal.hpp"

namespace YTE::Tools
{
  // We search for the correct path to the texture.
  static std::string FindTexturePath(std::string const& aPath, std::filesystem::path const& aMeshDirectory)
  {
    std::filesystem::path path{ aPath };

    // First just check the given path itself.
    if (std::filesystem::exists(path))
    {
      return path.u8string();
    }
    
    // Then check to see if it's in a path relative to the mesh.
    path = aMeshDirectory / path;

    if (std::filesystem::exists(path))
    {
      return path.u8string();
    }
    
    // Then check to see if it's in the same folder as the mesh.
    path = aMeshDirectory / path.filename();

    if (std::filesystem::exists(path))
    {
      return path.u8string();
    }

    // Then check if it's in the working directory.
    path = path.filename();

    if (std::filesystem::exists(path))
    {
      return path.u8string();
    }

    // If it's not in any of those, just return nothing.
    return std::string{};
  }

  // We change the extension to .basis since we're importing, and remove the extraneous pathing information.
  static std::string NormalizeTexturePath(std::string const& aPath)
  {
    std::filesystem::path path{ aPath };
    return path.filename().replace_extension(".basis").u8string();
  }

  void ImportSubMesh(YTE::Mesh* aYTEMesh, 
                     YTE::Submesh& aSubmesh, 
                     const aiScene* aScene, 
                     const aiMesh* aMesh, 
                     YTE::Skeleton* aSkeleton, 
                     uint32_t aBoneStartingVertexOffset,
                     std::filesystem::path const& aRootDirectory,
                     std::set<std::string>& aTextures)
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

      // The textures we look to import, we need the correct path of, we accept relative/absolute paths,
      // as well as searching for the file in the same directory as the model we're loading.
      aTextures.emplace(FindTexturePath(textureName, aRootDirectory));

      aSubmesh.mData.mTextureData.emplace_back(NormalizeTexturePath(textureName), YTE::TextureViewType::e2D, ToYTE(type));
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

  
  void ImportMesh(YTE::Mesh& aMesh, const std::string &aFile, std::set<std::string>& aTextures)
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
      if (false == HasBones(meshScene))
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
        SkeletonInitialize(aMesh.mSkeleton, meshScene);
      }

      std::filesystem::path filePath{aFile};
      std::filesystem::path directory{ "" };
      
      if (filePath.has_root_directory())
      {
        directory = filePath.root_directory();
      }

      aMesh.mParts.clear();
      aMesh.mParts.reserve(meshScene->mNumMeshes);
      aMesh.mParts.resize(static_cast<size_t>(meshScene->mNumMeshes));

      // Load Mesh
      YTE::u32 startingVertex = 0;
      for (size_t i = 0; i < aMesh.mParts.size(); i++)
      {
        ImportSubMesh(&aMesh, aMesh.mParts[i], meshScene, meshScene->mMeshes[i], &aMesh.mSkeleton, startingVertex, directory, aTextures);

        startingVertex += meshScene->mMeshes[i]->mNumVertices;
      }

      aMesh.mName = filePath.filename().u8string();
    }

    aMesh.mDimension = YTE::CalculateDimensions(aMesh.mParts);
  }


  
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

}