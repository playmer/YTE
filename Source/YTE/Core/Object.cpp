/******************************************************************************/
/*!
\file   System.h
\author Joshua T. Fisher
\par    email: j.fisher\@digipen.edu
\date   2014-04-30
\brief
This file contains Space, which is what a level will be loaded into.
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#include "YTE/Core/Object.hpp"

namespace YTE
{
  YTEDefineType(Object)
  {
    YTERegisterType(Object);
  }

  Property* Object::GetProperty(const std::string &aName, BoundType *aType)
  {
    if (aType == nullptr)
    {
      return nullptr;
    }

    auto propertyRange = aType->GetPropertyRange(aName);

    for (auto &property : propertyRange)
    {
      if (property.second->GetAttribute<Serializable>())
      {
        return property.second.get();
      }
    }

    auto fieldRange = aType->GetFieldRange(aName);

    for (auto &field : fieldRange)
    {
      if (field.second->GetAttribute<Serializable>())
      {
        return field.second.get();
      }
    }

    return GetProperty(aName, aType->GetBaseType());;
  }
}
