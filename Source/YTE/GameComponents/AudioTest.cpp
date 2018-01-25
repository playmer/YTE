/******************************************************************************/
/*!
\file   AudioTest.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-24

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "AudioTest.hpp"

namespace YTE
{
    static std::vector<std::string> PopulateDropDownList(Component *aComponent)
    {
        auto banks = aComponent->GetOwner()->GetEngine()->GetComponent<WWiseSystem>()->GetBanks();
        std::vector<std::string> result;
        
        for (auto i : banks)
        {
            std::string str = i.first;
            result.push_back(str);
        }
        return result;
    }

    YTEDefineType(AudioTest)
    {
        YTERegisterType(AudioTest);

        std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

        AudioTest::GetStaticType()->AddAttribute<ComponentDependencies>(deps);

        YTEBindProperty(&AudioTest::GetSoundName, &AudioTest::SetSoundName, "SoundName")
            .AddAttribute<EditorProperty>()
            .AddAttribute<DropDownStrings>(PopulateDropDownList);
    }

    AudioTest::AudioTest(Composition *aOwner, Space *aSpace, RSValue *aProperties)
        : Component(aOwner, aSpace)
    {
        YTEUnusedArgument(aProperties);
    }

    void AudioTest::Initialize()
    {
        mOwner->YTERegister(Events::KeyPress, this, &AudioTest::Play);
        mKeyboard = &mOwner->GetEngine()->GetWindow()->mKeyboard;
    }

    void AudioTest::Play(KeyboardEvent *aEvent)
    {
        if (aEvent->Key == Keys::Space)
        {
            //play the specified sound
        }
    }

} // end yte namespace