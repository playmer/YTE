/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Object_h
#define YTE_Core_Object_h

#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Utilities/JsonHelpers.hpp"

namespace YTE
{
  class Object : public Base
  {
  public:
    YTEDeclareType(Object);

    void DeserializeByType(RSValue *aProperties, Object *aSelf, BoundType *aType);

    // Search type and it's basetype for a property by the given name.
    static Property* GetProperty(const std::string &aName, BoundType *aType);

    // If you've asserted here, this isn't implemented, but Serialize has been called.
    virtual RSValue Serialize(RSAllocator &)
    {
      DebugObjection(true, "Serialize has not been implemented for this type.");

      return RSValue();
    };

    virtual void Deserialize(RSValue*) {};

  protected:
    RSValue SerializeByType(RSAllocator &aAllocator, Object *aSelf, BoundType *aType);

    void SerializeByFieldOrProperties(OrderedMultiMap<std::string, std::unique_ptr<Property>> &aMap,
                                      RSValue &aValue,
                                      RSAllocator &aAllocator, 
                                      Object *aSelf, 
                                      BoundType *aType);
  };
}
#endif