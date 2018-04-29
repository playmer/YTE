/******************************************************************************/
/*!
\file   BasilMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BasilMovement.hpp"
#include "YTE/GameComponents/BasilDialogue.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(BasilMovement)
  {
    RegisterType<BasilMovement>();

    // dock index
    YTEBindProperty(&BasilMovement::GetDockIndex, &BasilMovement::SetDockIndex, "Dock Index")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // character positions
    YTEBindProperty(&BasilMovement::GetDockOnePos, &BasilMovement::SetDockOnePos, "Dock One Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetDockTwoPos, &BasilMovement::SetDockTwoPos, "Dock Two Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetDockThreePos, &BasilMovement::SetDockThreePos, "Dock Three Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // character rotations
    YTEBindProperty(&BasilMovement::GetCharRotOne, &BasilMovement::SetCharRotOne, "Char Rotation One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetCharRotTwo, &BasilMovement::SetCharRotTwo, "Char Rotation Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetCharRotThree, &BasilMovement::SetCharRotThree, "Char Rotation Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // boat offsets
    YTEBindProperty(&BasilMovement::GetBoatOffsetOne, &BasilMovement::SetBoatOffsetOne, "Boat Offset One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetBoatOffsetTwo, &BasilMovement::SetBoatOffsetTwo, "Boat Offset Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetBoatOffsetThree, &BasilMovement::SetBoatOffsetThree, "Boat Offset Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // boat rotations
    YTEBindProperty(&BasilMovement::GetBoatRotOne, &BasilMovement::SetBoatRotOne, "Boat Rotation One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetBoatRotTwo, &BasilMovement::SetBoatRotTwo, "Boat Rotation Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&BasilMovement::GetBoatRotThree, &BasilMovement::SetBoatRotThree, "Boat Rotation Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  BasilMovement::BasilMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
    , mDockOnePos(glm::vec3(0))
    , mDockTwoPos(glm::vec3(0))
    , mDockThreePos(glm::vec3(0))
    , mCharRotationOne(glm::vec3(0))
    , mCharRotationTwo(glm::vec3(0))
    , mCharRotationThree(glm::vec3(0))
    , mBoatOffsetOne(glm::vec3(0))
    , mBoatOffsetTwo(glm::vec3(0))
    , mBoatOffsetThree(glm::vec3(0))
    , mBoatRotationOne(glm::vec3(0))
    , mBoatRotationTwo(glm::vec3(0))
    , mBoatRotationThree(glm::vec3(0))
    , mDockIndex(0)
  {
    DeserializeByType(aProperties, this, GetStaticType());

    mStartPos = glm::vec3(161, 0, 55);
  }

  void BasilMovement::Initialize()
  {
    mDocks.emplace_back(mDockOnePos);
    mDocks.emplace_back(mDockTwoPos);
    mDocks.emplace_back(mDockThreePos);

    mCharRotations.emplace_back(mCharRotationOne);
    mCharRotations.emplace_back(mCharRotationTwo);
    mCharRotations.emplace_back(mCharRotationThree);

    mBoatOffsets.emplace_back(mBoatOffsetOne);
    mBoatOffsets.emplace_back(mBoatOffsetTwo);
    mBoatOffsets.emplace_back(mBoatOffsetThree);

    mBoatRotations.emplace_back(mBoatRotationOne);
    mBoatRotations.emplace_back(mBoatRotationTwo);
    mBoatRotations.emplace_back(mBoatRotationThree);

    mSpace->YTERegister(Events::QuestStart, this, &BasilMovement::OnQuestStart);
  }

  void BasilMovement::MoveToNextDock()
  {
    mOwner->GetComponent<Transform>()->SetTranslation(mDocks[mDockIndex]);
    ++mDockIndex;
    if (mDockIndex > mDocks.size() - 1)
    {
      mDockIndex = 0;
    }
  }

  void BasilMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::Basil)
    {
      Transform *charTrans = mOwner->GetComponent<Transform>();

      // set position of character
      charTrans->SetWorldTranslation(mDocks[mDockIndex]);

      // set rotation of character
      charTrans->SetWorldRotation(mCharRotations[mDockIndex]);

      Composition *boat = mOwner->FindFirstCompositionByName("boat");
      Transform *boatTrans = boat->GetComponent<Transform>();

      // set offset of boat
      boatTrans->SetTranslation(mBoatOffsets[mDockIndex]);

      // set rotation of boat
      boatTrans->SetRotation(mBoatRotations[mDockIndex]);

      ++mDockIndex;
      if (mDockIndex > mDocks.size() - 1)
      {
        mDockIndex = 0;
      }
    }
  }

  int BasilMovement::GetDockIndex() const
  {
    return mDockIndex;
  }

  void BasilMovement::SetDockIndex(int index)
  {
    mDockIndex = index;
  }

  // character positions
  glm::vec3 BasilMovement::GetDockOnePos() const
  {
    return mDockOnePos;
  }

  void BasilMovement::SetDockOnePos(glm::vec3 aPos)
  {
    mDockOnePos = aPos;
  }

  glm::vec3 BasilMovement::GetDockTwoPos() const
  {
    return mDockTwoPos;
  }

  void BasilMovement::SetDockTwoPos(glm::vec3 aPos)
  {
    mDockTwoPos = aPos;
  }

  glm::vec3 BasilMovement::GetDockThreePos() const
  {
    return mDockThreePos;
  }

  void BasilMovement::SetDockThreePos(glm::vec3 aPos)
  {
    mDockThreePos = aPos;
  }

  // character rotations
  glm::vec3 BasilMovement::GetCharRotOne() const
  {
    return mCharRotationOne;
  }

  void BasilMovement::SetCharRotOne(glm::vec3 aRotation)
  {
    mCharRotationOne = aRotation;
  }

  glm::vec3 BasilMovement::GetCharRotTwo() const
  {
    return mCharRotationTwo;
  }

  void BasilMovement::SetCharRotTwo(glm::vec3 aRotation)
  {
    mCharRotationTwo = aRotation;
  }

  glm::vec3 BasilMovement::GetCharRotThree() const
  {
    return mCharRotationThree;
  }

  void BasilMovement::SetCharRotThree(glm::vec3 aRotation)
  {
    mCharRotationThree = aRotation;
  }

  // boat offsets
  glm::vec3 BasilMovement::GetBoatOffsetOne() const
  {
    return mBoatOffsetOne;
  }

  void BasilMovement::SetBoatOffsetOne(glm::vec3 aOffset)
  {
    mBoatOffsetOne = aOffset;
  }

  glm::vec3 BasilMovement::GetBoatOffsetTwo() const
  {
    return mBoatOffsetTwo;
  }

  void BasilMovement::SetBoatOffsetTwo(glm::vec3 aOffset)
  {
    mBoatOffsetTwo = aOffset;
  }

  glm::vec3 BasilMovement::GetBoatOffsetThree() const
  {
    return mBoatOffsetThree;
  }

  void BasilMovement::SetBoatOffsetThree(glm::vec3 aOffset)
  {
    mBoatOffsetThree = aOffset;
  }

  // boat rotations
  glm::vec3 BasilMovement::GetBoatRotOne() const
  {
    return mBoatRotationOne;
  }

  void BasilMovement::SetBoatRotOne(glm::vec3 aRotation)
  {
    mBoatRotationOne = aRotation;
  }

  glm::vec3 BasilMovement::GetBoatRotTwo() const
  {
    return mBoatRotationTwo;
  }

  void BasilMovement::SetBoatRotTwo(glm::vec3 aRotation)
  {
    mBoatRotationTwo = aRotation;
  }

  glm::vec3 BasilMovement::GetBoatRotThree() const
  {
    return mBoatRotationThree;
  }

  void BasilMovement::SetBoatRotThree(glm::vec3 aRotation)
  {
    mBoatRotationThree = aRotation;
  }

}//end yte