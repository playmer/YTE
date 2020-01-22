#pragma once

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "YTE/Graphics/Generics/Mesh.hpp"

namespace YTE::Tools
{
  bool HasBones(aiScene const* aScene);
  void SkeletonPreTransform(Skeleton& aSkeleton, const aiScene* aScene);
  void SkeletonInitialize(Skeleton& aSkeleton, aiScene const* aScene);
  void VisitNodes(Skeleton& aSkeleton, const aiNode* aNode, glm::mat4 const& aParentTransform);
}