#pragma once

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "glm/gtc/type_ptr.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"

namespace YTE::Tools 
{  
  inline
  YTE::SubmeshData::TextureType ToYTE(aiTextureType aType)
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

  inline
  glm::vec3 ToGlm(const aiVector3D *aVector)
  {
    return { aVector->x, aVector->y ,aVector->z };
  }

  inline
  glm::vec3 ToGlm(const aiColor3D *aVector)
  {
    return { aVector->r, aVector->g ,aVector->b };
  }

  inline
  glm::quat ToGlm(const aiQuaternion *aQuat)
  {
    glm::quat quaternion;

    quaternion.x = aQuat->x;
    quaternion.y = aQuat->y;
    quaternion.z = aQuat->z;
    quaternion.w = aQuat->w;

    return quaternion;
  }

  inline
  glm::mat4 ToGlm(const aiMatrix4x4 &aMatrix)
  {
    return glm::transpose(glm::make_mat4(&aMatrix.a1));
  }

  inline
  aiMatrix4x4 ToAssimp(const glm::mat4 &aMatrix)
  {
    auto transposed = glm::transpose(aMatrix);
    return *(reinterpret_cast<aiMatrix4x4*>(glm::value_ptr(transposed)));
  }
}