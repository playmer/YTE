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

#include "YTE/Utilities/JsonHelpers.h"

namespace YTE
{
  class Object : public Base
  {
  public:
    DeclareType(Object);

    template <typename MetaBoundType>
    inline void DeserializeByType(RSValue *aProperties, MetaBoundType aSelf, BoundType *aType);

    // Search type and it's basetype for a property by the given name.
    static inline Property* GetProperty(const std::string &aName, BoundType *aType)
    {
      if (aType == nullptr)
      {
        return nullptr;
      }

      Property *toReturn = aType->GetFirstProperty(aName);;

      if (toReturn == nullptr)
      {
        toReturn = aType->GetFirstField(aName);
      }

      if (toReturn == nullptr)
      {
        GetProperty(aName, aType->GetBaseType());
      }

      return toReturn;
    }

    // If you've asserted here, this isn't implemented, but Serialize has been called.
    virtual RSValue Serialize(RSAllocator &)
    {
      DebugAssert(true, "Serialize has not been implemented for this type.");

      return RSValue();
    };

    virtual void Deserialize(RSValue*) {};

  protected:
    template <typename MetaBoundType>
    inline RSValue SerializeByType(RSAllocator &aAllocator, MetaBoundType aSelf, BoundType *aType);

    template <typename MetaBoundType, typename IterationType>
    inline void SerializeByFieldOrProperties(OrderedMultiMap<std::string, std::unique_ptr<IterationType>> &aMap,
                                             RSValue &aValue,
                                             RSAllocator &aAllocator, 
                                             MetaBoundType aSelf, 
                                             BoundType *aType);
  };
    
  template <typename MetaBoundType>
  void Object::DeserializeByType(RSValue *aProperties, MetaBoundType aSelf, BoundType *aType)
  {
    // Nothing to serialize
    if (aProperties == nullptr)
    {
      return;
    }

    for (auto propertiesIt = aProperties->MemberBegin(); propertiesIt  < aProperties->MemberEnd(); ++propertiesIt)
    {
      std::string propertyName = propertiesIt->name.GetString();
      RSValue *value = &propertiesIt->value;
    
      Property *namedProperty = Object::GetProperty(propertyName, aType);
    
      if (namedProperty == nullptr)
      {
        std::cout << "You have likely removed " << propertyName.c_str()
                  << " from " << aType->GetName().c_str()
                  << " but are still attempting to deserialize it." << std::endl;
        continue;
      }
    
      if (!namedProperty->GetAttribute<EditorProperty>())
      {
        std::cout << namedProperty->GetName() << " of Type " << aType->GetName()  
                  << " does not appear to be a serializable property." << std::endl;
        continue;
      }
    
        // If the bound field/property does not have the Property Attribute, do nothing.
      auto setter = namedProperty->GetSetter();
      auto setterType = setter->GetParameters()->at(1).mType->GetMostBasicType();

      // Type is a float
      if (setterType == TypeId<float>())
      {
        setter->Invoke(aSelf, static_cast<float>(value->GetDouble()));
      }
      else if (setterType->GetEnumOf())
      {
        BoundType *enumType = setterType;
      
        // TODO (Josh): Would prefer to give an error like this, but we cannot find the name of just a Type.
        //DebugAssert(enumType == nullptr, 
        //            "Type %s contains a property named %s of type %s, "
        //            "a bound type could not be found for this type.", 
        //            aType->Name.c_str(),
        //            namedProperty->Name.c_str(),
        //            namedProperty->PropertyType->NameLocation.
        //            );
        DebugAssert(enumType == nullptr, 
                    "Type %s contains a property named %s, "
                    "a bound type could not be found for this property.", 
                    aType->GetName().c_str(),
                    namedProperty->GetName().c_str());
      
        auto enumValue = enumType->GetFirstProperty(value->GetString());
          
        DebugAssert(enumValue == nullptr,
                    "Did not find value for enum property %s, on type %s", 
                    namedProperty->GetName().c_str(),
                    enumType->GetName().c_str());
      

        // TODO (Josh): Finish Enums
        debugbreak();
        //Call getCall(enumValue->Get);
        //getCall.Invoke(reportForEnum);
        //u32 enumAsInt = getCall.Get<u32>(Zilch::Call::Return);
        //
        //call.Set(0, enumAsInt);
      }
      // Type is an int.
      else if (setterType == TypeId<i32>())
      {
        setter->Invoke(aSelf, value->GetInt());
      }
      // Type is a string.
      else if (setterType == TypeId<String>())
      {
        String string = value->GetString();
        setter->Invoke(aSelf, string);
      }
      // Type is a string.
      else if (setterType == TypeId<std::string>())
      {
        std::string string = value->GetString();
        setter->Invoke(aSelf, string);
      }
      // Type is a Boolean.
      else if (setterType == TypeId<bool>())
      {
        setter->Invoke(aSelf, value->GetBool());
      }
      // Type is a Real2.
      else if (setterType == TypeId<glm::vec2>())
      {
        setter->Invoke(aSelf, ValueAsReal2(value));
      }
      // Type is a Real3.
      else if (setterType == TypeId<glm::vec3>())
      {
        setter->Invoke(aSelf, ValueAsReal3(value));
      }
      // Type is a Real4.
      else if (setterType == TypeId<glm::vec4>())
      {
        setter->Invoke(aSelf, ValueAsReal4(value));
      }
      // Type is a Quaternion.
      else if (setterType == TypeId<glm::quat>())
      {
        setter->Invoke(aSelf, ValueAsQuaternion(value));
      }
      // Type is invalid.
      else
      {
        std::cout << "Attempting to read property " << propertyName.c_str()
                  << " from " << aType->GetName().c_str()
                  << " which is not currently supported" << std::endl;
      }
    }
  };

  
  template <typename MetaBoundType, typename IterationType>
  inline void Object::SerializeByFieldOrProperties(OrderedMultiMap<std::string, std::unique_ptr<IterationType>> &aMap,
                                                   RSValue &aValue,
                                                   RSAllocator &aAllocator, 
                                                   MetaBoundType aSelf, 
                                                   BoundType *aType)
  {
    for (auto &property : aMap)
    {
      auto &propertyName = property.second->GetName();

      // If the bound field/property does not have the Property Attribute, do nothing.
      if (!property.second->GetAttribute<EditorProperty>())
      {
        continue;
      }

      // Set up the get so we can serialize it's value.
      auto getter = property.second->GetGetter();
      auto any = getter->Invoke(aSelf);
      auto propertyType = getter->GetReturnType()->GetMostBasicType();

      RSValue propertyValue;

      // Type is a float
      if (propertyType == TypeId<float>())
      {
        propertyValue.SetDouble(any.As<float>());
      }
      else if (propertyType->GetEnumOf())
      {
        // TODO (Josh): Fix Enums
        debugbreak();
        //Call call(property->Get, aState);
        //call.SetHandle(This, aSelf);
        //call.Invoke(reportForEnum);
        //u32 enumAsInt = call.Get<u32>(Return);
        //
        //String enumAsStr = property->PropertyType->GenericToString((byte*)&enumAsInt);
        //aBuilder.Value(enumAsStr);
      }
      // Type is an int.
      else if (propertyType == TypeId<u32>())
      {
        propertyValue.SetUint(any.As<u32>());
      }
      else if (propertyType == TypeId<u64>())
      {
        propertyValue.SetUint64(any.As<u64>());
      }
      else if (propertyType == TypeId<i64>())
      {
        propertyValue.SetInt64(any.As<i64>());
      }
      else if (propertyType == TypeId<i32>())
      {
        propertyValue.SetInt(any.As<i32>());
      }
      // Type is a string.
      else if (propertyType == TypeId<String>())
      {
        auto &value = any.As<String>();
        propertyValue.SetString(value.c_str(), static_cast<RSSizeType>(value.Size()), aAllocator);
      }
      else if (propertyType == TypeId<std::string>())
      {
        auto &value = any.As<std::string>();
        propertyValue.SetString(value.c_str(), static_cast<RSSizeType>(value.size()), aAllocator);
      }
      // Type is a Boolean.
      else if (propertyType == TypeId<bool>())
      {
        propertyValue.SetBool(any.As<bool>());
      }
      // Type is a Real2.
      else if (propertyType == TypeId<glm::vec2>())
      {
        auto value = any.As<glm::vec2>();
        Real2AsValue(propertyValue, value, aAllocator);
      }
      // Type is a Real3.
      else if (propertyType == TypeId<glm::vec3>())
      {
        auto value = any.As<glm::vec3>();
        Real3AsValue(propertyValue, value, aAllocator);
      }
      // Type is a Real4.
      else if (propertyType == TypeId<glm::vec4>())
      {
        auto value = any.As<glm::vec4>();
        Real4AsValue(propertyValue, value, aAllocator);
      }
      // Type is a Quaternion.
      else if (propertyType == TypeId<glm::quat>())
      {
        auto value = any.As<glm::quat>();
        QuaternionAsValue(propertyValue, value, aAllocator);
      }

      RSValue propertyNameValue;
      propertyNameValue.SetString(propertyName.c_str(),
                                  static_cast<RSSizeType>(propertyName.size()),
                                  aAllocator);
    
      aValue.AddMember(propertyNameValue, propertyValue, aAllocator);
    }
  }



  template <typename MetaBoundType>
  RSValue Object::SerializeByType(RSAllocator &aAllocator, MetaBoundType aSelf, BoundType *aType)
  {
    RSValue toReturn;
    toReturn.SetObject();

    auto &fields = aType->GetFields();
    auto &properties = aType->GetProperties();
    
    SerializeByFieldOrProperties<MetaBoundType, Property>(fields, toReturn, aAllocator, aSelf, aType);
    SerializeByFieldOrProperties<MetaBoundType, Property>(properties, toReturn, aAllocator, aSelf, aType);

    return toReturn;;
  }
}

#endif