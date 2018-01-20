/******************************************************************************/
/*!
\file   BoatController.hpp
\author Jonathan Ackerman
\par    email: jonathan.ackerman\@digipen.edu
\date   2018-01-19

All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/GameComponents/BoatController.hpp"

namespace YTE
{
    YTEDefineType(BoatController)
    {
        YTERegisterType(BoatController);
    }
    /*
    BoatController::BoatController(Composition *aOwner, Space *aSpace)
        : Component(aOwner, aSpace)
    {
    }
    */
    void BoatController::Initialize()
    {
        mOwner->YTERegister(Events::ToggleSailEvent, this, &BoatController::ChangeSailState);
    }

    void BoatController::ChangeSailState()
    {
        //
    }
}//end yte namespace