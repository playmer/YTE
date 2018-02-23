/******************************************************************************/
/*!
\file   DialogueSystem.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-23

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DialogueSystem.hpp"
#include "YTE/Core/AssetLoader.hpp"
#include "YTE/Utilities/Utilities.hpp"

namespace fs = std::experimental::filesystem;

namespace YTE
{
/******************************************************************************/
/*
    Component Definitions and Registrations
*/
/******************************************************************************/
  YTEDefineType(DialogueSystem)
  {
    YTERegisterType(DialogueSystem);
  }

/******************************************************************************/
/*
    LineData
*/
/******************************************************************************/
  LineData::LineData(int aHierarchy, LineType aType, std::string aString)
    : mHierarchy(aHierarchy), mType(aType), mString(aString)
  {
  }

/******************************************************************************/
/*
    ConversationData
*/
/******************************************************************************/
  ConversationData::ConversationData()
    : mIsCompleted(false)
  {  
    mLinesVector = *(new std::vector<LineData>());
  };

/******************************************************************************/
/*
    ConversationMap
*/
/******************************************************************************/
  ConversationMap::ConversationMap()
  {
    mConversationMap = *(new std::map<std::string, ConversationData>());
  }

/******************************************************************************/
/*
    DialogueSystem
*/
/******************************************************************************/
  DialogueSystem::DialogueSystem(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
    mCharacterMap = *(new std::unordered_map<std::string, ConversationMap>());
  }

  void DialogueSystem::Initialize()
  {
      // Vector of file names
    std::vector<std::string> filenames;
      // Get directory
    auto textDir = Path::GetTextsDirectory(Path::GetGamePath());
    fs::directory_iterator end{};
      // Loop through directory for text files
    for (fs::directory_iterator iter{ textDir }; iter != end; ++iter)
    {
      filenames.push_back(iter->path().string());
      auto charIter = iter->path().string().begin();
        // Name Format: CharName_ConvoName_ConvoType (eg: Basil_Breakfast_Begin)
        // Loop through filenames, sort by CharName & ConvoName, add to data structure
      while (charIter != iter->path().string().end())
      {
        if (*charIter == '_')
        {
          // word is done, store it somewhere
          ++charIter;
          break;
        }
        // hold onto the character
      }
    }

  }

  void DialogueSystem::ParseTextFiles()
  {

  }

} // end yte