/******************************************************************************/
/*!
\file   AudioTest.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-24

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

// TODO: find out why this breaks
// keep this order of includes or it will break
#include "AudioTest.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/WWise/WWiseSystem.hpp"



namespace YTE
{
    static std::vector<std::string> PopulateDropDownList(Component *aComponent)
    {
        auto banks = aComponent->GetOwner()->GetEngine()->GetComponent<WWiseSystem>()->GetBanks();
        std::vector<std::string> result;
        
        for (auto i : banks)
        {
            //std::string str = i.second.mName;
            auto events = i.second.mEvents;
            for (auto j : events)
            {
                result.push_back(j.mName);
            }
        }
        return result;
    }

    YTEDefineType(AudioTest)
    {
        YTERegisterType(AudioTest);

        std::vector<std::vector<Type*>> deps = 
        { 
            { TypeId<Transform>() }, 
            { TypeId<WWiseEmitter>() }
        };

        GetStaticType()->AddAttribute<ComponentDependencies>(deps);


        YTEBindProperty(&AudioTest::GetSoundName, &AudioTest::SetSoundName, "SoundName")
            .AddAttribute<Serializable>()
            .AddAttribute<EditorProperty>()
            .AddAttribute<DropDownStrings>(PopulateDropDownList);
    }

    AudioTest::AudioTest(Composition *aOwner, Space *aSpace, RSValue *aProperties)
        : Component(aOwner, aSpace)
    {
        YTEUnusedArgument(aProperties);
        DeserializeByType(aProperties, this, GetStaticType());
    }

    void AudioTest::Initialize()
    {
        mKeyboard = &mOwner->GetEngine()->GetWindow()->mKeyboard;
        mEmitter = mOwner->GetComponent<WWiseEmitter>();
        
        Space *space = mOwner->GetSpace();
        GraphicsView *view = space->GetComponent<GraphicsView>();
        Window *window = view->GetWindow();

        window->mKeyboard.YTERegister(Events::KeyPress, this, &AudioTest::Play);
    }

    void AudioTest::Play(KeyboardEvent *aEvent)
    {
        if (aEvent->Key == Keys::Space)
        {
            std::cout << "SPACEW\n";
            mEmitter->PlayEvent(mSound);
        }
    }


    void AudioTest::SetSoundName(std::string aName)
    { 
      auto banks = GetOwner()->GetEngine()->GetComponent<WWiseSystem>()->GetBanks();


      for (auto &bank : banks)
      {
        for (auto &event : bank.second.mEvents)
        {
          if (event.mName == aName)
          {
            mSound = aName;
            mSoundId = event.mId;
            return;
          }
        }
      }
    }

} // end yte namespace