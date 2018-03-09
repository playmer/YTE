/******************************************************************************/
/*!
\file   DialogueRunner.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-03-07
\brief
This component will perform the logic to fill dialogue boxes with text.

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#ifndef YTE_Gameplay_DialogueRunner_hpp
#define YTE_Gameplay_DialogueRunner_hpp

#include "YTE/Core/Composition.hpp"
#include "YTE/GameComponents/DialogueGraph.hpp"

namespace YTE
{
/////////////////////////////////////////////////////////////////////////////////////
// Events
/////////////////////////////////////////////////////////////////////////////////////
	YTEDeclareEvent(FillDialogueBox);

	class FillDialogueBox : public Event
	{
	public:
		YTEDeclareType(FillDialogueBox);
		FillDialogueBox(Conversation *aConvo) { mConvo = aConvo; };
		Conversation *mConvo;
	};
/////////////////////////////////////////////////////////////////////////////////////
// Class
/////////////////////////////////////////////////////////////////////////////////////
	class DialogueRunner : public Component
	{
	public:
			YTEDeclareType(DialogueRunner);
			DialogueRunner(Composition *aOwner, Space *aSpace, RSValue *aProperties);
			void Initialize() override;
	private:
		// pointer to text box child

	};
}//end yte
#endif