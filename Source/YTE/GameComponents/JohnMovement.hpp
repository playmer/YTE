/******************************************************************************/
/*!
\file   JohnMovement.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25
\brief
    This component handles the logic for moving john around the map

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once
#ifndef YTE_Gameplay_JohnMovement_hpp
#define YTE_Gameplay_JohnMovement_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/GameComponents/JohnDialogue.hpp"

namespace YTE
{
  class JohnMovement : public Component
  {
  public:
    YTEDeclareType(JohnMovement);
    JohnMovement(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    void Initialize() override;
    void Start() override;

    void OnQuestStart(QuestStart *aEvent);

    int GetDockIndex() const;
    void SetDockIndex(int index);

    // dock positions
    glm::vec3 GetDockOnePos() const;
    void SetDockOnePos(glm::vec3 aPos);

    glm::vec3 GetDockTwoPos() const;
    void SetDockTwoPos(glm::vec3 aPos);

    glm::vec3 GetDockThreePos() const;
    void SetDockThreePos(glm::vec3 aPos);

    // boat rotations
    glm::vec3 GetCharRotOne() const;
    void SetCharRotOne(glm::vec3 aRotation);

    glm::vec3 GetCharRotTwo() const;
    void SetCharRotTwo(glm::vec3 aRotation);

    glm::vec3 GetCharRotThree() const;
    void SetCharRotThree(glm::vec3 aRotation);

    // boat offsets
    glm::vec3 GetBoatOffsetOne() const;
    void SetBoatOffsetOne(glm::vec3 aOffset);

    glm::vec3 GetBoatOffsetTwo() const;
    void SetBoatOffsetTwo(glm::vec3 aOffset);

    glm::vec3 GetBoatOffsetThree() const;
    void SetBoatOffsetThree(glm::vec3 aOffset);

    // boat rotations
    glm::vec3 GetBoatRotOne() const;
    void SetBoatRotOne(glm::vec3 aRotation);

    glm::vec3 GetBoatRotTwo() const;
    void SetBoatRotTwo(glm::vec3 aRotation);

    glm::vec3 GetBoatRotThree() const;
    void SetBoatRotThree(glm::vec3 aRotation);

  private:
    std::vector<glm::vec3> mDocks;
    std::vector<glm::vec3> mCharRotations;
    std::vector<glm::vec3> mBoatOffsets;
    std::vector<glm::vec3> mBoatRotations;
    glm::vec3 mStartPos;
    int mDockIndex;

    // dock positions
    glm::vec3 mDockOnePos;
    glm::vec3 mDockTwoPos;
    glm::vec3 mDockThreePos;

    // character rotations
    glm::vec3 mCharRotationOne;
    glm::vec3 mCharRotationTwo;
    glm::vec3 mCharRotationThree;

    // boat offsets
    glm::vec3 mBoatOffsetOne;
    glm::vec3 mBoatOffsetTwo;
    glm::vec3 mBoatOffsetThree;

    // boat rotations
    glm::vec3 mBoatRotationOne;
    glm::vec3 mBoatRotationTwo;
    glm::vec3 mBoatRotationThree;

  };
}//end yte
#endif