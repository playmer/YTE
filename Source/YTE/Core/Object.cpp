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
    RegisterType<Object>();
    TypeBuilder<Object> builder;
  }

  Property* Object::GetProperty(const std::string &aName, Type *aType)
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

    return GetProperty(aName, aType->GetBaseType());
  }


  void Object::DeserializeByType(RSValue *aProperties, Object *aSelf, Type *aType)
  {
    // Nothing to serialize
    if (aProperties == nullptr)
    {
      return;
    }

    bool deserializedEditorHeader{ false };

    auto count = aProperties->MemberCount();

    if (rapidjson::kNullType == aProperties->GetType())
    {
      return;
    }

    static const char* kTypeNames[] =
    { "Null", "False", "True", "Object", "Array", "String", "Number" };

    std::cout << count << ": " << kTypeNames[aProperties->GetType()] << "\n";

    for (auto propertiesIt = aProperties->MemberBegin(); propertiesIt < aProperties->MemberEnd(); ++propertiesIt)
    {
      std::string propertyName = propertiesIt->name.GetString();
      RSValue *value = &propertiesIt->value;

      Property *namedProperty = Object::GetProperty(propertyName, aType);

      if (namedProperty == nullptr)
      {
        if (auto listerAttribute = aType->GetAttribute<EditorHeaderList>();
            nullptr != listerAttribute && 
            false == deserializedEditorHeader &&
            listerAttribute->GetName() == propertiesIt->name.GetString())
        {
          listerAttribute->Deserialize(*value, aSelf);
          deserializedEditorHeader = true;
          continue;
        }

        //fmt::format("You have likely removed {}, from {}, but are still attempting to deserialize it.",)

        std::cout << "You have likely removed " << propertyName.c_str()
                  << " from " << aType->GetName().c_str()
                  << " but are still attempting to deserialize it." << std::endl;
        continue;
      }

      if (auto redirectAttribute = namedProperty->GetAttribute<RedirectObject>();
          nullptr != redirectAttribute)
      {
        redirectAttribute->Deserialize(*value, aSelf);
        continue;
      }

      // If the bound field/property does not have the Property Attribute, do nothing.
      auto setter = namedProperty->GetSetter();
      auto setterType = setter->GetParameters().at(1).mType->GetMostBasicType();

      // Type is a float
      if (setterType == TypeId<float>())
      {
        setter->Invoke(aSelf, ValueAsFloat(value));
      }
      else if (setterType == TypeId<double>())
      {
        setter->Invoke(aSelf, ValueAsDouble(value));
      }
      else if (setterType->GetEnumOf())
      {
        Type *enumType = setterType;

        // TODO (Josh): Would prefer to give an error like this, but we cannot find the name of just a Type.
        //DebugObjection(enumType == nullptr, 
        //            "Type %s contains a property named %s of type %s, "
        //            "a bound type could not be found for this type.", 
        //            aType->Name.c_str(),
        //            namedProperty->Name.c_str(),
        //            namedProperty->PropertyType->NameLocation.
        //            );
        DebugObjection(enumType == nullptr,
          "Type %s contains a property named %s, "
          "a bound type could not be found for this property.",
          aType->GetName().c_str(),
          namedProperty->GetName().c_str());

        auto enumValue = enumType->GetFirstProperty(value->GetString());

        UnusedArguments(enumValue);

        DebugObjection(enumValue == nullptr,
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


  void Object::SerializeByFieldOrProperties(OrderedMultiMap<std::string, std::unique_ptr<Property>> &aMap,
                                            RSValue &aValue,
                                            RSAllocator &aAllocator,
                                            Object *aSelf,
                                            Type *aType)
  {
    // TODO (Josh): Why is this being passed?.
    UnusedArguments(aType);

    for (auto const& [name, property] : aMap)
    {
      auto &propertyName = property->GetName();

      // If the bound field/property does not have the Property Attribute, do nothing.
      if (!property->GetAttribute<Serializable>())
      {
        continue;
      }

      if (auto redirectAttribute = property->GetAttribute<RedirectObject>();
          nullptr != redirectAttribute)
      {
        auto object = redirectAttribute->Serialize(aAllocator, aSelf);

        RSValue propName;
        propName.SetString(redirectAttribute->GetName().c_str(),
                           static_cast<RSSizeType>(redirectAttribute->GetName().size()),
                           aAllocator);

        aValue.AddMember(propName, object, aAllocator);
        continue;
      }

      // Set up the get so we can serialize its value.
      auto getter = property->GetGetter();
      auto any = getter->Invoke(aSelf);
      auto propertyType = getter->GetReturnType()->GetMostBasicType();

      RSValue propertyValue;

      // Type is a float
      if (propertyType == TypeId<float>())
      {
        auto value = any.As<float>(); 
        FloatAsValue(propertyValue, value, aAllocator);
      }
      else if (propertyType == TypeId<double>())
      {
        auto value = any.As<double>();
        DoubleAsValue(propertyValue, value, aAllocator);
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

  RSValue Object::SerializeByType(RSAllocator &aAllocator, Object *aSelf, Type *aType)
  {
    RSValue toReturn;
    toReturn.SetObject();

    auto type = aType;

    while (type)
    {
      auto &fields = type->GetFields();
      auto &properties = type->GetProperties();

      SerializeByFieldOrProperties(fields, toReturn, aAllocator, aSelf, type);
      SerializeByFieldOrProperties(properties, toReturn, aAllocator, aSelf, type);


      if (auto listerAttribute = type->GetAttribute<EditorHeaderList>();
      nullptr != listerAttribute)
      {
        auto array = listerAttribute->Serialize(aAllocator, aSelf);

        RSValue arrayName;
        arrayName.SetString(listerAttribute->GetName().c_str(),
                            static_cast<RSSizeType>(listerAttribute->GetName().size()),
                            aAllocator);

        toReturn.AddMember(arrayName, array, aAllocator);
      }

      type = type->GetBaseType();
    }

    return toReturn;
  }
}
