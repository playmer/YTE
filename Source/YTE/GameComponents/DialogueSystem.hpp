/******************************************************************************/
/*!
\file   DialogueSystem.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-02-23
\brief
DELAYED DEVELOPMENT, USE CHARACTER DIALOGUES

An interface on the space for running dialogue. This component will 
parse the dialogue texts,
store them for the game to access at runtime...maybe other?

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_DialogueSystem_hpp
#define YTE_Gameplay_DialogueSystem_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

namespace YTE
{
/******************************************************************************/
/*
    Classes for data types used by DialogueSystem component
*/
/******************************************************************************/
  class LineData
  {
  public:
    enum class LineType { Text, Input, Action };
    LineData(int aHierarchy, LineType aType, std::string aString);
  private:
    int mHierarchy;
    LineType mType;
    std::string mString;
  };

  class ConversationData
  {
  public:
    ConversationData();
  private:
    bool mIsCompleted;
      // Vector of Lines
    std::vector<LineData> mLinesVector;
  };

  class ConversationMap
  {
  public:
    ConversationMap();
  private:
      // Map of conversation name to ConversationData
    std::map<std::string, ConversationData> mConversationMap;
  };

/******************************************************************************/
/*
    DialogueSystem component class
*/
/******************************************************************************/
  class DialogueSystem : public Component
  {
  public:
    YTEDeclareType(DialogueSystem);
    DialogueSystem(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
  private:
    void ParseTextFiles();
      // Map of character name to ConversationMap
    std::unordered_map<std::string, ConversationMap> mCharacterMap;
  };
} // end yte

#endif
