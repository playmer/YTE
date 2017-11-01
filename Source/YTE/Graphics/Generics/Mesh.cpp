///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "YTE/Core/AssetLoader.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"

namespace YTE
{
  YTEDefineType(Mesh)
  {
    YTERegisterType(Mesh);
  }
  
  

  Submesh::Submesh(Window *aWindow, const aiScene *aScene, const aiMesh *aMesh)
  {
    YTEUnusedArgument(aWindow);

    aiColor3D pColor(0.f, 0.f, 0.f);
    aScene->mMaterials[aMesh->mMaterialIndex]->Get(AI_MATKEY_COLOR_DIFFUSE,
      pColor);

    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

    mName = aMesh->mName.C_Str();

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

    mUBOMaterial.mDiffuse = AssimpToGLM(&Diffuse);
    mUBOMaterial.mAmbient = AssimpToGLM(&Ambient);
    mUBOMaterial.mSpecular = AssimpToGLM(&Specular);
    mUBOMaterial.mEmissive = AssimpToGLM(&Emissive);
    mUBOMaterial.mTransparent = AssimpToGLM(&Transparent);
    mUBOMaterial.mReflective = AssimpToGLM(&Reflective);

    material->Get(AI_MATKEY_NAME, name);
    material->Get(AI_MATKEY_OPACITY, mUBOMaterial.mOpacity);
    material->Get(AI_MATKEY_SHININESS, mUBOMaterial.mShininess);
    material->Get(AI_MATKEY_SHININESS_STRENGTH, mUBOMaterial.mShininessStrength);
    material->Get(AI_MATKEY_REFLECTIVITY, mUBOMaterial.mReflectivity);
    material->Get(AI_MATKEY_REFRACTI, mUBOMaterial.mReflectiveIndex);
    material->Get(AI_MATKEY_BUMPSCALING, mUBOMaterial.mBumpScaling);

    mName = aMesh->mName.C_Str();
    mMaterialName = name.C_Str();
    mShaderSetName = mMaterialName.substr(0, mMaterialName.find_first_of('_'));

    aiString diffuse;
    aiString specular;
    aiString normals;

    material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse);
    material->GetTexture(aiTextureType_SPECULAR, 0, &specular);
    material->GetTexture(aiTextureType_NORMALS, 0, &normals);

    if (0 != diffuse.length)
    {
      mDiffuseMap = diffuse.C_Str();
    }

    if (0 != specular.length)
    {
      mSpecularMap = specular.C_Str();
    }

    if (0 != normals.length)
    {
      mNormalMap = normals.C_Str();
    }

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

      auto position = AssimpToGLM(pPos);

      // NOTE: We do this to invert the uvs to what the texture would expect.
      auto textureCoordinates = glm::vec3{ pTexCoord->x,
        1.0f - pTexCoord->y,
        pTexCoord->z };

      auto normal = AssimpToGLM(pNormal);
      auto color = AssimpToGLM(&pColor);
      auto tangent = AssimpToGLM(pTangent);
      auto binormal = glm::cross(tangent, normal);
      auto bitangent = AssimpToGLM(pBiTangent);

      mVertexBuffer.emplace_back(position,
        textureCoordinates,
        normal,
        color,
        tangent,
        binormal,
        bitangent);

      mDimension.mMax.x = fmax(pPos->x, mDimension.mMax.x);
      mDimension.mMax.y = fmax(pPos->y, mDimension.mMax.y);
      mDimension.mMax.z = fmax(pPos->z, mDimension.mMax.z);

      mDimension.mMin.x = fmin(pPos->x, mDimension.mMin.x);
      mDimension.mMin.y = fmin(pPos->y, mDimension.mMin.y);
      mDimension.mMin.z = fmin(pPos->z, mDimension.mMin.z);
    }

    mDimension.mSize = mDimension.mMax - mDimension.mMin;

    uint32_t indexBase = static_cast<uint32_t>(mIndexBuffer.size());

    for (unsigned int j = 0; j < aMesh->mNumFaces; j++)
    {
      const aiFace &Face = aMesh->mFaces[j];
      if (Face.mNumIndices != 3)
        continue;

      mIndexBuffer.push_back(indexBase + Face.mIndices[0]);
      mIndexBuffer.push_back(indexBase + Face.mIndices[1]);
      mIndexBuffer.push_back(indexBase + Face.mIndices[2]);
    }

    mVertexBufferSize = mVertexBuffer.size() * sizeof(Vertex);
    mIndexBufferSize = mIndexBuffer.size() * sizeof(u32);
  }



  Mesh::Mesh(Window *aWindow, std::string &aFile, CreateInfo *aCreateInfo)
  {
    Assimp::Importer Importer;

    // TODO: Are meshes always in the game's asset path?
    auto meshFile = Path::GetModelPath(Path::GetGamePath(), aFile);

    auto pScene = Importer.ReadFile(meshFile.c_str(),
      aiProcess_Triangulate |
      aiProcess_PreTransformVertices |
      aiProcess_CalcTangentSpace |
      aiProcess_GenSmoothNormals);

    if (pScene)
    {
      mParts.clear();
      mParts.reserve(pScene->mNumMeshes);

      glm::vec3 scale(1.0f);
      glm::vec2 uvscale(1.0f);
      glm::vec3 center(0.0f);

      if (aCreateInfo)
      {
        scale = aCreateInfo->mScale;
        uvscale = aCreateInfo->mUVscale;
        center = aCreateInfo->mCenter;
      }

      printf("Mesh FileName: %s\n", aFile.c_str());

      // Load meshes
      for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
      {
        mParts.emplace_back(aWindow, pScene, pScene->mMeshes[i]);
      }

      mName = aFile;
    }
  }



  Mesh::~Mesh()
  {

  }
}
