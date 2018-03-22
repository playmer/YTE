/******************************************************************************/
/*!
\file   ComponentSearchBar.hpp
\author Nicholas Ammann
\par    email: nicholas.ammann\@digipen.edu
\par    Course: GAM 300
\date   8/15/2017
\brief
The search bar for finding and adding components to the current object.

All content (c) 2017 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include <qlineedit.h>

#include "YTE/Core/ForwardDeclarations.hpp"
#include "YTE/Meta/Type.hpp"
#include "YTE/Utilities/Utilities.hpp"

class QCompleter;

namespace YTEditor
{
  class ComponentTools;

  class ComponentSearchBar : public QLineEdit
  {
  public:
    ComponentSearchBar(ComponentTools * compTools, QWidget * parent = nullptr);
    ~ComponentSearchBar();

    // takes a vector of component types from the YTE::Engine
    // this can be found by getting the ComponentSystem component from the engine
    // then calling GetComponentTypes() on that component
    void SetComponentList(const std::vector<YTE::Type*>& aTypeList);

    void AddComponent(QString aCompName);

    void ItemActivated(QString aCompName);

    void OnReturnPressed();

    void OnTabPressed();

  private:

    QCompleter *mCompleter;

    QStringList mComponentList;

    std::vector<YTE::Type*> mComponentTypes;

    YTE::Type* FindBoundType(std::string &aName);

    ComponentTools *mComponentTools;

  };

}