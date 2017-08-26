/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   6/7/2015
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Utilities_JsonHelpers_h
#define YTE_Utilities_JsonHelpers_h

namespace YTE
{
  // TODO (Josh): Fix the serialization.

  ///////////////////////////////
  // Real2
  ///////////////////////////////
  inline glm::vec2 ValueAsReal2(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector2");
    auto x = static_cast<float>(real4->value.FindMember("x")->value.GetDouble());
    auto y = static_cast<float>(real4->value.FindMember("y")->value.GetDouble());
    return glm::vec2{ x, y };
  }

  inline void Real2AsValue(RSValue &aValue, glm::vec2 aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", aVector.x, aAllocator);
    subObject.AddMember("y", aVector.y, aAllocator);

    aValue.AddMember("Vector2", subObject, aAllocator);
  }

  ///////////////////////////////
  // Real3
  ///////////////////////////////
  inline glm::vec3 ValueAsReal3(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector3");
    auto x = static_cast<float>(real4->value.FindMember("x")->value.GetDouble());
    auto y = static_cast<float>(real4->value.FindMember("y")->value.GetDouble());
    auto z = static_cast<float>(real4->value.FindMember("z")->value.GetDouble());
    return glm::vec3{ x, y, z };
  }

  inline void Real3AsValue(RSValue &aValue, glm::vec3 aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", aVector.x, aAllocator);
    subObject.AddMember("y", aVector.y, aAllocator);
    subObject.AddMember("z", aVector.z, aAllocator);

    aValue.AddMember("Vector3", subObject, aAllocator);
  }

  ///////////////////////////////
  // Real4
  ///////////////////////////////
  inline glm::vec4 ValueAsReal4(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector3");
    auto x = static_cast<float>(real4->value.FindMember("x")->value.GetDouble());
    auto y = static_cast<float>(real4->value.FindMember("y")->value.GetDouble());
    auto z = static_cast<float>(real4->value.FindMember("z")->value.GetDouble());
    auto w = static_cast<float>(real4->value.FindMember("w")->value.GetDouble());
    return glm::vec4{ x, y, z, w };
  }

  inline void Real4AsValue(RSValue &aValue, glm::vec4 aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", aVector.x, aAllocator);
    subObject.AddMember("y", aVector.y, aAllocator);
    subObject.AddMember("z", aVector.z, aAllocator);
    subObject.AddMember("w", aVector.w, aAllocator);

    aValue.AddMember("Vector4", subObject, aAllocator);
  }

  ///////////////////////////////
  // Quaternion
  ///////////////////////////////
  inline glm::quat ValueAsQuaternion(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector3");
    auto x = static_cast<float>(real4->value.FindMember("x")->value.GetDouble());
    auto y = static_cast<float>(real4->value.FindMember("y")->value.GetDouble());
    auto z = static_cast<float>(real4->value.FindMember("z")->value.GetDouble());
    auto w = static_cast<float>(real4->value.FindMember("w")->value.GetDouble());
    return glm::quat{ x, y, z, w };
  }

  inline void QuaternionAsValue(RSValue &aValue, glm::quat aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", aVector.x, aAllocator);
    subObject.AddMember("y", aVector.y, aAllocator);
    subObject.AddMember("z", aVector.z, aAllocator);
    subObject.AddMember("w", aVector.w, aAllocator);

    aValue.AddMember("Quaternion", subObject, aAllocator);
  }
}

#endif