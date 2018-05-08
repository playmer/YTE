/******************************************************************************/
/*!
\file   DaisyMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DaisyMovement.hpp"
#include "YTE/GameComponents/DaisyDialogue.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(DaisyMovement)
  {
    RegisterType<DaisyMovement>();

    // dock index
    YTEBindProperty(&DaisyMovement::GetDockIndex, &DaisyMovement::SetDockIndex, "Dock Index")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // character positions
    YTEBindProperty(&DaisyMovement::GetDockOnePos, &DaisyMovement::SetDockOnePos, "Dock One Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetDockTwoPos, &DaisyMovement::SetDockTwoPos, "Dock Two Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetDockThreePos, &DaisyMovement::SetDockThreePos, "Dock Three Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // character rotations
    YTEBindProperty(&DaisyMovement::GetCharRotOne, &DaisyMovement::SetCharRotOne, "Char Rotation One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetCharRotTwo, &DaisyMovement::SetCharRotTwo, "Char Rotation Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetCharRotThree, &DaisyMovement::SetCharRotThree, "Char Rotation Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // boat offsets
    YTEBindProperty(&DaisyMovement::GetBoatOffsetOne, &DaisyMovement::SetBoatOffsetOne, "Boat Offset One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetBoatOffsetTwo, &DaisyMovement::SetBoatOffsetTwo, "Boat Offset Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetBoatOffsetThree, &DaisyMovement::SetBoatOffsetThree, "Boat Offset Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // boat rotations
    YTEBindProperty(&DaisyMovement::GetBoatRotOne, &DaisyMovement::SetBoatRotOne, "Boat Rotation One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetBoatRotTwo, &DaisyMovement::SetBoatRotTwo, "Boat Rotation Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    YTEBindProperty(&DaisyMovement::GetBoatRotThree, &DaisyMovement::SetBoatRotThree, "Boat Rotation Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  DaisyMovement::DaisyMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
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

  void DaisyMovement::Initialize()
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

    mSpace->RegisterEvent<&DaisyMovement::OnQuestStart>(Events::QuestStart, this);
  }

  void DaisyMovement::MoveToNextDock()
  {
    mOwner->GetComponent<Transform>()->SetTranslation(mDocks[mDockIndex]);
    ++mDockIndex;
    if (mDockIndex > mDocks.size() - 1)
    {
      mDockIndex = 0;
    }
  }

  void DaisyMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::Daisy)
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

  int DaisyMovement::GetDockIndex() const
  {
    return mDockIndex;
  }

  void DaisyMovement::SetDockIndex(int index)
  {
    mDockIndex = index;
  }

  // character positions
  glm::vec3 DaisyMovement::GetDockOnePos() const
  {
    return mDockOnePos;
  }

  void DaisyMovement::SetDockOnePos(glm::vec3 aPos)
  {
    mDockOnePos = aPos;
  }

  glm::vec3 DaisyMovement::GetDockTwoPos() const
  {
    return mDockTwoPos;
  }

  void DaisyMovement::SetDockTwoPos(glm::vec3 aPos)
  {
    mDockTwoPos = aPos;
  }

  glm::vec3 DaisyMovement::GetDockThreePos() const
  {
    return mDockThreePos;
  }

  void DaisyMovement::SetDockThreePos(glm::vec3 aPos)
  {
    mDockThreePos = aPos;
  }

  // character rotations
  glm::vec3 DaisyMovement::GetCharRotOne() const
  {
    return mCharRotationOne;
  }

  void DaisyMovement::SetCharRotOne(glm::vec3 aRotation)
  {
    mCharRotationOne = aRotation;
  }

  glm::vec3 DaisyMovement::GetCharRotTwo() const
  {
    return mCharRotationTwo;
  }

  void DaisyMovement::SetCharRotTwo(glm::vec3 aRotation)
  {
    mCharRotationTwo = aRotation;
  }

  glm::vec3 DaisyMovement::GetCharRotThree() const
  {
    return mCharRotationThree;
  }

  void DaisyMovement::SetCharRotThree(glm::vec3 aRotation)
  {
    mCharRotationThree = aRotation;
  }

  // boat offsets
  glm::vec3 DaisyMovement::GetBoatOffsetOne() const
  {
    return mBoatOffsetOne;
  }

  void DaisyMovement::SetBoatOffsetOne(glm::vec3 aOffset)
  {
    mBoatOffsetOne = aOffset;
  }

  glm::vec3 DaisyMovement::GetBoatOffsetTwo() const
  {
    return mBoatOffsetTwo;
  }

  void DaisyMovement::SetBoatOffsetTwo(glm::vec3 aOffset)
  {
    mBoatOffsetTwo = aOffset;
  }

  glm::vec3 DaisyMovement::GetBoatOffsetThree() const
  {
    return mBoatOffsetThree;
  }

  void DaisyMovement::SetBoatOffsetThree(glm::vec3 aOffset)
  {
    mBoatOffsetThree = aOffset;
  }

  // boat rotations
  glm::vec3 DaisyMovement::GetBoatRotOne() const
  {
    return mBoatRotationOne;
  }

  void DaisyMovement::SetBoatRotOne(glm::vec3 aRotation)
  {
    mBoatRotationOne = aRotation;
  }

  glm::vec3 DaisyMovement::GetBoatRotTwo() const
  {
    return mBoatRotationTwo;
  }

  void DaisyMovement::SetBoatRotTwo(glm::vec3 aRotation)
  {
    mBoatRotationTwo = aRotation;
  }

  glm::vec3 DaisyMovement::GetBoatRotThree() const
  {
    return mBoatRotationThree;
  }

  void DaisyMovement::SetBoatRotThree(glm::vec3 aRotation)
  {
    mBoatRotationThree = aRotation;
  }
}//end yte