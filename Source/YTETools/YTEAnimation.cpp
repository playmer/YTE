#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "assimp/types.h"
#include "assimp/vector3.h"

#include "YTE/Graphics/Animation.hpp"

#include "YTE/StandardLibrary/File.hpp"

#include "YTETools/ImportMathTypeConverters.hpp"
#include "YTETools/YTEAnimation.hpp"

namespace YTE::Tools
{
  template <typename tContainer, typename tKey>
  void InsertKeys(tContainer& aContainer, tKey const* aBegin, tKey const* aEnd)
  {
    for (auto it = aBegin; it < aEnd; ++it)
    {
      aContainer.emplace_back(it->mTime, ToGlm(&it->mValue));
    }
  }

  static inline
  const aiNodeAnim* FindNodeAnimation(aiAnimation *aAnimation, const char *aName)
  {
    for (uint32_t i = 0; i < aAnimation->mNumChannels; ++i)
    {
      const aiNodeAnim *anim = aAnimation->mChannels[i];

      if (!strcmp(anim->mNodeName.data, aName))
      {
        return anim;
      }
    }
    return nullptr;
  }

  static inline
  void MakeNodes(aiAnimation *aAssimpAnimation,
                 aiNode *aAssimpNode,
                 AnimationData& aData,
                 size_t aNodeIndex)
  {
    AnimationData::Node& node = aData.mNodes[aNodeIndex];

    node.mTransformationOffset = aData.mTransformations.size();
    aData.mTransformations.emplace_back(ToGlm(aAssimpNode->mTransformation));

    size_t nameLength = aAssimpNode->mName.length;

    node.mNameOffset = aData.mNames.size();
    node.mNameSize = nameLength;
    for (size_t i = 0; i < aAssimpNode->mName.length; ++i)
    {
      aData.mNames.emplace_back(*(aAssimpNode->mName.C_Str() + i));
    }
    
    // Null terminate the string.
    aData.mNames.emplace_back('\0');

    aiNodeAnim const* animKeys = FindNodeAnimation(aAssimpAnimation, aAssimpNode->mName.data);

    if (animKeys)
    {
      node.mTranslationKeyOffset = aData.mTranslationKeys.size();
      node.mTranslationKeySize = animKeys->mNumPositionKeys;
      InsertKeys(aData.mTranslationKeys, animKeys->mPositionKeys, animKeys->mPositionKeys + animKeys->mNumPositionKeys);

      node.mScaleKeyOffset = aData.mScaleKeys.size();
      node.mScaleKeySize = animKeys->mNumScalingKeys;
      InsertKeys(aData.mScaleKeys, animKeys->mScalingKeys, animKeys->mScalingKeys + animKeys->mNumScalingKeys);

      node.mRotationKeyOffset = aData.mRotationKeys.size();
      node.mRotationKeySize = animKeys->mNumRotationKeys;
      InsertKeys(aData.mRotationKeys, animKeys->mRotationKeys, animKeys->mRotationKeys + animKeys->mNumRotationKeys);
    }

    node.mChildrenOffset = aData.mChildren.size();
    node.mChildrenSize = aAssimpNode->mNumChildren;

    // Add the children nodes to the AnimationData.
    // NOTE: node can now be potentially invalidated, so we no longer use it.
    for (uint32_t i = 0; i < aAssimpNode->mNumChildren; ++i)
    {
      aData.mChildren.emplace_back(aData.mNodes.size());
      aData.mNodes.emplace_back(AnimationData::Node{});
    }

    // Process the children added
    for (size_t i = 0; i < aAssimpNode->mNumChildren; ++i)
    {
      MakeNodes(aAssimpAnimation, 
                aAssimpNode->mChildren[i], 
                aData, 
                aData.mChildren[i + aData.mNodes[aNodeIndex].mChildrenOffset]);
    }
  }

  bool Equal(AnimationData const& aLeft, AnimationData const& aRight)
  {
    auto nodes = aLeft.mNodes == aRight.mNodes;
    auto transformations = aLeft.mTransformations == aRight.mTransformations;
    auto children = aLeft.mChildren == aRight.mChildren;
    auto translations = aLeft.mTranslationKeys == aRight.mTranslationKeys;
    auto scales = aLeft.mScaleKeys == aRight.mScaleKeys;
    auto rotations = aLeft.mRotationKeys == aRight.mRotationKeys;
    auto names = aLeft.mNames == aRight.mNames;
    auto durations = aLeft.mDuration == aRight.mDuration;
    auto ticks = aLeft.mTicksPerSecond == aRight.mTicksPerSecond;

    return nodes && transformations && children && translations && scales && rotations && names && durations && ticks;
  }

  AnimationData TestWritingAndReading(std::string const& aFile, AnimationData const& aOriginalData)
  {
    auto aniFileYTE = aFile + ".YTEAnimation";
  
    auto writtenFileData = WriteAnimationDataToFile(aFile, aOriginalData);
  
    auto [readFile, readFileData] = ReadAnimationDataFromFile(aniFileYTE);
  
    if (writtenFileData != readFileData)
    {
      #if YTE_Windows
        __debugbreak();
      #else
        __builtin_trap();
      #endif
    }
  
    if (false == Equal(aOriginalData, readFile))
    {
      #if YTE_Windows
        __debugbreak();
      #else
        __builtin_trap();
      #endif
    }
  
    return readFile;
  }

  std::vector<char> WriteAnimationDataToFile(std::string const& aName, AnimationData const& aData)
  {
    OPTICK_EVENT();
    std::string fileName = aName;
    fileName += ".YTEAnimation";

    FileWriter file{ fileName };

    if (file.mOpened)
    {
      AnimationFileHeader header;

      header.mNodeSize = aData.mNodes.size();
      header.mTransformationsSize = aData.mTransformations.size();
      header.mChildrenSize = aData.mChildren.size();
      header.mTranslationKeysSize = aData.mTranslationKeys.size();
      header.mScaleKeysSize = aData.mScaleKeys.size();
      header.mRotationKeysSize = aData.mRotationKeys.size();
      header.mNamesSize = aData.mNames.size();
      header.mDuration = aData.mDuration;
      header.mTicksPerSecond = aData.mTicksPerSecond;

      // The "header" of the file.
      file.Write(header);

      file.Write(aData.mNodes.data(), aData.mNodes.size());
      file.Write(aData.mTransformations.data(), aData.mTransformations.size());
      file.Write(aData.mChildren.data(), aData.mChildren.size());
      file.Write(aData.mTranslationKeys.data(), aData.mTranslationKeys.size());
      file.Write(aData.mScaleKeys.data(), aData.mScaleKeys.size());
      file.Write(aData.mRotationKeys.data(), aData.mRotationKeys.size());
      file.Write(aData.mNames.data(), aData.mNames.size());
    }

    return file.mData;
  }
  
  AnimationData ImportAnimation(std::string const& aFile)
  {
    OPTICK_EVENT();
    auto aniFile = Path::GetAnimationPath(Path::GetGamePath(), aFile);

    Assimp::Importer importer;
    
    auto scene = importer.ReadFile(aniFile.c_str(),
                                    aiProcess_Triangulate |
                                    aiProcess_CalcTangentSpace |
                                    aiProcess_GenSmoothNormals);
    
    UnusedArguments(scene);
    
    DebugObjection(scene == nullptr,
                    "Failed to load animation file %s from assimp",
                    aniFile.c_str());
    
    DebugObjection(scene->HasAnimations() == false,
                    "Failed to find animations in scene loaded from %s",
                    aniFile.c_str());
    
    auto animation = scene->mAnimations[0];
    
    AnimationData data;
    AnimationData::Node node;
    
    data.mNodes.emplace_back(node);
    
    MakeNodes(animation, scene->mRootNode, data, 0);
    
    data.mTicksPerSecond = animation->mTicksPerSecond;
    data.mDuration = animation->mDuration;
    
    return std::move(TestWritingAndReading(aniFile, data));
  }
}