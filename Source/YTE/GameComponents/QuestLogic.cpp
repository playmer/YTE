/******************************************************************************/
/*!
\file   QuestLogic.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-25

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/QuestLogic.hpp"
#include "YTE/GameComponents/QuestProgressionTrigger.hpp"
#include "YTE/GameComponents/StarMovement.hpp"

namespace YTE
{
  YTEDefineEvent(ProgressionItemEvent);
  YTEDefineEvent(ProgressionLocationEvent);
  YTEDefineEvent(ProgressionDialogueEvent);

  YTEDefineType(ProgressionItemEvent) { RegisterType<ProgressionItemEvent>(); }
  YTEDefineType(ProgressionLocationEvent) { RegisterType<ProgressionLocationEvent>(); }
  YTEDefineType(ProgressionDialogueEvent) { RegisterType<ProgressionDialogueEvent>(); }

  YTEDefineType(QuestLogic) { RegisterType<QuestLogic>(); }

  QuestLogic::QuestLogic(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
    YTEUnusedArgument(aProperties);
  }

  void QuestLogic::Initialize()
  {
    mSpace->RegisterEvent<&QuestLogic::OnSpawnProgressionItem>(Events::SpawnProgressionItem, this);
    mSpace->RegisterEvent<&QuestLogic::OnSpawnProgressionLocation>(Events::SpawnProgressionLocation, this);
    mSpace->RegisterEvent<&QuestLogic::OnSpawnProgressionDialogue>(Events::SpawnProgressionDialogue, this);

    mSpace->RegisterEvent<&QuestLogic::OnProgressionItemEvent>(Events::ProgressionItemEvent, this);
    mSpace->RegisterEvent<&QuestLogic::OnProgressionLocationEvent>(Events::ProgressionLocationEvent, this);
    mSpace->RegisterEvent<&QuestLogic::OnProgressionDialogueEvent>(Events::ProgressionDialogueEvent, this);
  }

  void QuestLogic::OnProgressionItemEvent(ProgressionItemEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);
    Quest::CharacterName name = (*mPostcardHandle)->GetCharacter();

    if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      switch (name)
      {
        case Quest::CharacterName::John:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::John);
          break;
        }

        case Quest::CharacterName::Daisy:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Daisy);
          break;
        }

        case Quest::CharacterName::Basil:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Basil);
          break;
        }
      }
    }

    UpdateActiveQuestState update(name, Quest::State::Accomplished);
    mSpace->SendEvent(Events::UpdateActiveQuestState, &update);
  }

  void QuestLogic::OnProgressionLocationEvent(ProgressionLocationEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);
    Quest::CharacterName name = (*mPostcardHandle)->GetCharacter();

    if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      switch (name)
      {
        case Quest::CharacterName::John:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::John);
          break;
        }

        case Quest::CharacterName::Daisy:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Daisy);
          break;
        }

        case Quest::CharacterName::Basil:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Basil);
          break;
        }
      }
    }

    UpdateActiveQuestState update(name, Quest::State::Accomplished);
    mSpace->SendEvent(Events::UpdateActiveQuestState, &update);
  }

  void QuestLogic::OnProgressionDialogueEvent(ProgressionDialogueEvent *aEvent)
  {
    YTEUnusedArgument(aEvent);
    Quest::CharacterName name = (*mPostcardHandle)->GetCharacter();

    if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      switch (name)
      {
        case Quest::CharacterName::John:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::John);
          break;
        }

        case Quest::CharacterName::Daisy:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Daisy);
          break;
        }

        case Quest::CharacterName::Basil:
        {
          star->GetComponent<StarMovement>()->SetAnchor(StarMovement::CurrentAnchor::Basil);
          break;
        }
      }
    }

    UpdateActiveQuestState update(name, Quest::State::Accomplished);
    mSpace->SendEvent(Events::UpdateActiveQuestState, &update);
  }

  void QuestLogic::OnSpawnProgressionItem(SpawnProgressionItem *aEvent)
  {
    YTEUnusedArgument(aEvent);

    glm::vec3 itemPos;

    switch (aEvent->character)
    {
      case Quest::CharacterName::John:
      {
        itemPos = glm::vec3(481, 3, 540);
        break;
      }
      case Quest::CharacterName::Daisy:
      {
        itemPos = glm::vec3(232, 3, 262);
        break;
      }
      case Quest::CharacterName::Basil:
      {
        itemPos = glm::vec3(292, 3, 667);
        break;
      }
    }

    mSpace->AddCompositionAtPosition("ProgressionItem", "item", itemPos);
    
    if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      star->GetComponent<StarMovement>()->SetActive(itemPos + glm::vec3(0, 8, 0));
    }
  }

  void QuestLogic::OnSpawnProgressionLocation(SpawnProgressionLocation *aEvent)
  {
    YTEUnusedArgument(aEvent);

    glm::vec3 itemPos;

    switch (aEvent->character)
    {
      case Quest::CharacterName::John:
      {
        itemPos = glm::vec3(293, 3, 271);
        break;
      }
      case Quest::CharacterName::Daisy:
      {
        itemPos = glm::vec3(630, 3, 72);
        break;
      }
      case Quest::CharacterName::Basil:
      {
        itemPos = glm::vec3(502, 3, 911);
        break;
      }
    }

    mSpace->AddCompositionAtPosition("ProgressionItem", "item", itemPos);

    if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      star->GetComponent<StarMovement>()->SetActive(itemPos + glm::vec3(0, 8, 0));
    }
  }

  void QuestLogic::OnSpawnProgressionDialogue(SpawnProgressionDialogue *aEvent)
  {
    YTEUnusedArgument(aEvent);

    glm::vec3 itemPos;

    switch (aEvent->character)
    {
      case Quest::CharacterName::John:
      {
        itemPos = glm::vec3(-60, 3, 436);
        break;
      }
      case Quest::CharacterName::Daisy:
      {
        itemPos = glm::vec3(710, 3, 612);
        break;
      }
      case Quest::CharacterName::Basil:
      {
        itemPos = glm::vec3(-10, 3, 303);
        break;
      }
    }

    mSpace->AddCompositionAtPosition("ProgressionItem", "item", itemPos);

    if (auto star = mSpace->FindFirstCompositionByName("FeedbackStar"))
    {
      star->GetComponent<StarMovement>()->SetActive(itemPos + glm::vec3(0, 8, 0));
    }
  }


}//end yte