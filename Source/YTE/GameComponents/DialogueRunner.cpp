/******************************************************************************/
/*!
\file   DialogueRunner.cpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018 - 03 - 07

All content (c) 2018 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/DialogueRunner.hpp"

namespace YTE
{
	YTEDefineEvent(FillDialogueBox);
	YTEDefineType(FillDialogueBox) { YTERegisterType(FillDialogueBox); }

	YTEDefineType(DialogueRunner) { YTERegisterType(DialogueRunner); }

	DialogueRunner::DialogueRunner(Composition *aOwner, Space *aSpace, RSValue *aProperties)
		: Component(aOwner, aSpace)
	{
		YTEUnusedArgument(aProperties);

	}

	void DialogueRunner::Initialize()
	{

	}
}//endyte