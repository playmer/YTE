#include "YTE/Core/AssetLoader.hpp"

#include "YTE/StandardLibrary/File.hpp"

#include "YTETools/ImportMathTypeConverters.hpp"
#include "YTETools/YTEMesh.hpp"
#include "YTETools/YTESkeleton.hpp"

namespace YTE::Tools
{
  bool HasBones(aiScene const* aScene)
  {
    u32 const numMeshes = aScene->mNumMeshes;

    for (u32 i = 0; i < numMeshes; ++i)
    {
      aiMesh const* mesh = aScene->mMeshes[i];

      if (0 < mesh->mNumBones)
      {
        return true;
      }
    }

    return false;
  }

  void VisitNodes(Skeleton& aSkeleton, const aiNode* aNode, glm::mat4 const& aParentTransform)
  {
    OPTICK_EVENT();

    std::string nodeName(aNode->mName.data);
    glm::mat4 globalTrans = aParentTransform * ToGlm(aNode->mTransformation);

    auto index = aSkeleton.mBones.find(nodeName);

    if (index != aSkeleton.mBones.end())
    {
      aSkeleton.mBoneData[index->second].mFinalTransformation = 
        aSkeleton.mGlobalInverseTransform *
        globalTrans *
        aSkeleton.mBoneData[index->second].mOffset;
    }

    for (uint32_t i = 0; i < aNode->mNumChildren; ++i)
    {
      VisitNodes(aSkeleton, aNode->mChildren[i], globalTrans);
    }
  }

  void SkeletonPreTransform(Skeleton& aSkeleton, const aiScene* aScene)
  {
    OPTICK_EVENT();

    // setup parent transforms
    glm::mat4 identity;

    // recursive step
    VisitNodes(aSkeleton, aScene->mRootNode, identity);

    for (auto& [boneData, i] : enumerate(aSkeleton.mBoneData))
    {
      aSkeleton.mDefaultOffsets.mBones[i] = boneData->mFinalTransformation;
    }
  }

  void LoadBoneData(Skeleton& aSkeleton, u32& mBonesSoFar, const aiMesh* aMesh, uint32_t aVertexStartingIndex)
  {
    OPTICK_EVENT();
    DebugObjection(aMesh->mNumBones >= BoneConstants::MaxBones,
                   "Animated models cannot have more than %d bones, %s mesh has %d bones.",
                   BoneConstants::MaxBones, aMesh->mName.C_Str(), aMesh->mNumBones);

    for (uint32_t i = 0; i < aMesh->mNumBones; ++i)
    {
      uint32_t index = 0;

      auto bone = aMesh->mBones[i];
      std::string boneName(bone->mName.data);

      // try to find bone by name
      auto boneIt = aSkeleton.mBones.find(boneName);
      if (boneIt == aSkeleton.mBones.end())
      {
        // insert new bone
        index = mBonesSoFar;
        ++mBonesSoFar;
        aSkeleton.mBones[boneName] = index;
        aSkeleton.mBoneData.emplace_back(ToGlm(bone->mOffsetMatrix));
      }
      else
      {
        // use existing bone
        index = boneIt->second;
      }

      // get the weights and vertex IDs from the bone data
      for (uint32_t b = 0; b < bone->mNumWeights; ++b)
      {
        auto weight = bone->mWeights[b];
        uint32_t id = aVertexStartingIndex + weight.mVertexId;
//#ifdef _DEBUG
//        if (mVertexSkeletonData[id].AddBone(index, weight.mWeight) == false)
//        {
//          mVertexErrorAdds[id] += 1;
//        }
//#else
        aSkeleton.mVertexSkeletonData[id].AddBone(index, weight.mWeight);
//#endif
      }
    }

    // loop errors
//#if defined(_DEBUG) && defined(BSTDULAF)
//    for (size_t i = 0; i < mVertexErrorAdds.size(); ++i)
//    {
//      if (mVertexErrorAdds[i] != 0)
//      {
//        std::cout << "Warning: Vertex " << i << " did not add " << mVertexErrorAdds[i] << " of extra bones to the skeleton." << std::endl;
//        std::cout << "\t\tVertex Bone Information: \n\t\t\tWeights: "
//                  << mVertexSkeletonData[i].mWeights[0] << ",\t"
//                  << mVertexSkeletonData[i].mWeights[1] << ",\t"
//                  << mVertexSkeletonData[i].mWeights[2] << ",\t"
//                  << mVertexSkeletonData[i].mWeights[3] << "\n\t\t\t     IDs: "
//                  << mVertexSkeletonData[i].mWeights[4] << "\n\t\t\t     IDs: "
//                  << mVertexSkeletonData[i].mIDs[0] << ",\t"
//                  << mVertexSkeletonData[i].mIDs[1] << ",\t"
//                  << mVertexSkeletonData[i].mIDs[2] << ",\t"
//                  << mVertexSkeletonData[i].mIDs[3] << "\t"
//                  << mVertexSkeletonData[i].mIDs[4] << "\n\n";
//
//      }
//    }
//#endif
  }
  
  void SkeletonInitialize(Skeleton& aSkeleton, aiScene const* aScene)
  {
    OPTICK_EVENT();
    // find number of vertices to initialize the skeleton
    u32 numMeshes = aScene->mNumMeshes;
    u32 vertCount = 0;
    for (u32 i = 0; i < numMeshes; ++i)
    {
      vertCount += aScene->mMeshes[i]->mNumVertices;
    }

    aSkeleton.mVertexSkeletonData.resize(vertCount);
//#ifdef _DEBUG
//    mVertexErrorAdds.resize(vertCount);
//#endif

    u32 numBones = 0;

    auto globalInverse = aScene->mRootNode->mTransformation;
    aSkeleton.mGlobalInverseTransform = ToGlm(globalInverse.Inverse());

    u32 startingVertex = 0;
    for (u32 i = 0; i < numMeshes; ++i)
    {
      aiMesh* mesh = aScene->mMeshes[i];

      if (0 < mesh->mNumBones)
      {
        LoadBoneData(aSkeleton, numBones, mesh, startingVertex);
      }

      startingVertex += mesh->mNumVertices;
    }

    SkeletonPreTransform(aSkeleton, aScene);
  }
  
  void WriteSkeletonToFile(std::string const& aName, Skeleton const& aSkeleton)
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

  void ImportSkelton(std::string const& aName, Skeleton & aSkeleton)
  {
  
  }
}