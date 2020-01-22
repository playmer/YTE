#pragma once

#ifndef YTE_Core_Object_hpp
#define YTE_Core_Object_hpp

#include "YTE/Core/ScriptBind.hpp"
#include "YTE/Core/Utilities.hpp"

#include "YTE/Utilities/JsonHelpers.hpp"

namespace YTE
{
  class Object : public Base
  {
  public:
    YTEDeclareType(Object);

    YTE_Shared static void DeserializeByType(RSValue *aProperties, Object *aSelf, Type *aType);

    // Search type and it's basetype for a property by the given name.
    YTE_Shared static Property* GetProperty(const std::string &aName, Type *aType);

    // If you've asserted here, this isn't implemented, but Serialize has been called.
    virtual RSValue Serialize(RSAllocator &)
    {
      DebugObjection(true, "Serialize has not been implemented for this type.");

      return RSValue();
    };

    virtual void Deserialize(RSValue*) {};

  protected:
    YTE_Shared static RSValue SerializeByType(RSAllocator &aAllocator, Object *aSelf, Type *aType);

    YTE_Shared static void SerializeByFieldOrProperties(OrderedMultiMap<std::string, std::unique_ptr<Property>> &aMap,
                                                        RSValue &aValue,
                                                        RSAllocator &aAllocator, 
                                                        Object *aSelf, 
                                                        Type *aType);
  };
}
#endif