/******************************************************************************/
/*!
\file   AudioTest.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-24
\brief
Plays the specified sound when you press the spacebar

All content (c) 2018 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_AudioTest_hpp
#define YTE_Gameplay_AudioTest_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Core/Engine.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/DeviceEnums.hpp"

#include "YTE/WWise/WWiseSystem.hpp"

#include "YTE/Physics/Transform.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
    class AudioTest : public Component
    {
    public:
        YTEDeclareType(AudioTest);
        AudioTest(Composition *aOwner, Space *aSpace, RSValue *aProperties);
        void Initialize() override;
        void Play(KeyboardEvent *aEvent);
            // Getter for binding
        std::string GetSoundName() { return mSound; }
            // full disclosure i have no idea, bind property requires a setter for serializing
        void SetSoundName(std::string aName) { mSound = aName; }
    private:
        Keyboard *mKeyboard;
        std::string mSound;
    };

} // end yte namespace

#endif