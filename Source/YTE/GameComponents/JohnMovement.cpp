/******************************************************************************/
/*!
\file   JohnMovement.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/JohnMovement.hpp"
#include "YTE/Physics/Transform.hpp"

namespace YTE
{
  YTEDefineType(JohnMovement) 
  { 
    RegisterType<JohnMovement>();
    TypeBuilder<JohnMovement> builder;

    // dock index
    builder.Property<&JohnMovement::GetDockIndex, &JohnMovement::SetDockIndex>( "Dock Index")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // character positions
    builder.Property<&JohnMovement::GetDockOnePos, &JohnMovement::SetDockOnePos>( "Dock One Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetDockTwoPos, &JohnMovement::SetDockTwoPos>( "Dock Two Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetDockThreePos, &JohnMovement::SetDockThreePos>( "Dock Three Position")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // character rotations
    builder.Property<&JohnMovement::GetCharRotOne, &JohnMovement::SetCharRotOne>( "Char Rotation One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetCharRotTwo, &JohnMovement::SetCharRotTwo>( "Char Rotation Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetCharRotThree, &JohnMovement::SetCharRotThree>( "Char Rotation Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // boat offsets
    builder.Property<&JohnMovement::GetBoatOffsetOne, &JohnMovement::SetBoatOffsetOne>( "Boat Offset One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetBoatOffsetTwo, &JohnMovement::SetBoatOffsetTwo>( "Boat Offset Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetBoatOffsetThree, &JohnMovement::SetBoatOffsetThree>( "Boat Offset Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    // boat rotations
    builder.Property<&JohnMovement::GetBoatRotOne, &JohnMovement::SetBoatRotOne>( "Boat Rotation One")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetBoatRotTwo, &JohnMovement::SetBoatRotTwo>( "Boat Rotation Two")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();

    builder.Property<&JohnMovement::GetBoatRotThree, &JohnMovement::SetBoatRotThree>( "Boat Rotation Three")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>();
  }

  JohnMovement::JohnMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties)
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

  void JohnMovement::Initialize()
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

    mSpace->RegisterEvent<&JohnMovement::OnQuestStart>(Events::QuestStart, this);
  }

  void JohnMovement::Start()
  {
   // mOwner->GetComponent<Transform>()->SetTranslation(mStartPos);
    //mSpace->RegisterEvent<&JohnMovement::OnQuestStart>(Events::QuestStart, this);
  }

  void JohnMovement::OnQuestStart(QuestStart *aEvent)
  {
    if (aEvent->mCharacter == Quest::CharacterName::John)
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

  int JohnMovement::GetDockIndex() const
  {
    return mDockIndex;
  }

  void JohnMovement::SetDockIndex(int index)
  {
    mDockIndex = index;
  }

  // character positions
  glm::vec3 JohnMovement::GetDockOnePos() const
  {
    return mDockOnePos;
  }

  void JohnMovement::SetDockOnePos(glm::vec3 aPos)
  {
    mDockOnePos = aPos;
  }

  glm::vec3 JohnMovement::GetDockTwoPos() const
  {
    return mDockTwoPos;
  }

  void JohnMovement::SetDockTwoPos(glm::vec3 aPos)
  {
    mDockTwoPos = aPos;
  }

  glm::vec3 JohnMovement::GetDockThreePos() const
  {
    return mDockThreePos;
  }

  void JohnMovement::SetDockThreePos(glm::vec3 aPos)
  {
    mDockThreePos = aPos;
  }

  // character rotations
  glm::vec3 JohnMovement::GetCharRotOne() const
  {
    return mCharRotationOne;
  }

  void JohnMovement::SetCharRotOne(glm::vec3 aRotation)
  {
    mCharRotationOne = aRotation;
  }

  glm::vec3 JohnMovement::GetCharRotTwo() const
  {
    return mCharRotationTwo;
  }

  void JohnMovement::SetCharRotTwo(glm::vec3 aRotation)
  {
    mCharRotationTwo = aRotation;
  }

  glm::vec3 JohnMovement::GetCharRotThree() const
  {
    return mCharRotationThree;
  }

  void JohnMovement::SetCharRotThree(glm::vec3 aRotation)
  {
    mCharRotationThree = aRotation;
  }

  // boat offsets
  glm::vec3 JohnMovement::GetBoatOffsetOne() const
  {
    return mBoatOffsetOne;
  }

  void JohnMovement::SetBoatOffsetOne(glm::vec3 aOffset)
  {
    mBoatOffsetOne = aOffset;
  }

  glm::vec3 JohnMovement::GetBoatOffsetTwo() const
  {
    return mBoatOffsetTwo;
  }

  void JohnMovement::SetBoatOffsetTwo(glm::vec3 aOffset)
  {
    mBoatOffsetTwo = aOffset;
  }

  glm::vec3 JohnMovement::GetBoatOffsetThree() const
  {
    return mBoatOffsetThree;
  }

  void JohnMovement::SetBoatOffsetThree(glm::vec3 aOffset)
  {
    mBoatOffsetThree = aOffset;
  }

  // boat rotations
  glm::vec3 JohnMovement::GetBoatRotOne() const
  {
    return mBoatRotationOne;
  }

  void JohnMovement::SetBoatRotOne(glm::vec3 aRotation)
  {
    mBoatRotationOne = aRotation;
  }

  glm::vec3 JohnMovement::GetBoatRotTwo() const
  {
    return mBoatRotationTwo;
  }

  void JohnMovement::SetBoatRotTwo(glm::vec3 aRotation)
  {
    mBoatRotationTwo = aRotation;
  }

  glm::vec3 JohnMovement::GetBoatRotThree() const
  {
    return mBoatRotationThree;
  }

  void JohnMovement::SetBoatRotThree(glm::vec3 aRotation)
  {
    mBoatRotationThree = aRotation;
  }


}//end yte