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

#include <sstream>

namespace YTE
{
  // TODO (Josh): Fix the serialization.

  ///////////////////////////////
  // Float
  ///////////////////////////////
  inline float ValueAsFloat(RSValue *aValue)
  {
    if (aValue->IsDouble())
    {
      return static_cast<float>(aValue->GetDouble());
    }
    else if (aValue->IsString())
    {
      std::string hex = aValue->GetString();

      unsigned int x = std::stoul(hex, nullptr, 16);
      return *reinterpret_cast<float*>(&x);
    }

    return 0.0f;
  }

  inline double ValueAsDouble(RSValue *aValue)
  {
    if (aValue->IsDouble())
    {
      return aValue->GetDouble();
    }
    else if (aValue->IsString())
    {
      std::string hex = aValue->GetString();

      unsigned long long x = std::stoull(hex, nullptr, 16);
      return *reinterpret_cast<double*>(&x);
    }

    return 0.0f;
  }

  inline std::string FloatToString(float aFloat)
  {
    std::ostringstream stm;
    stm << std::hex << std::uppercase << *reinterpret_cast<unsigned int*>(&aFloat);

    return stm.str();
  }

  inline std::string DoubleToString(double aDouble)
  {
    std::ostringstream stm;
    stm << std::hex << std::uppercase << *reinterpret_cast<unsigned long long*>(&aDouble);

    return stm.str();
  }

  inline void FloatAsValue(RSValue &aValue, float aFloat, RSAllocator &aAllocator)
  {
    auto str = FloatToString(aFloat);
    aValue.SetString(str.c_str(), static_cast<RSSizeType>(str.size()), aAllocator);
  }

  inline RSValue FloatAsValue(float aFloat, RSAllocator &aAllocator)
  {
    RSValue value;
    auto str = FloatToString(aFloat);
    value.SetString(str.c_str(), static_cast<RSSizeType>(str.size()), aAllocator);
    return value;
  }

  inline void DoubleAsValue(RSValue &aValue, double aDouble, RSAllocator &aAllocator)
  {
    auto str = DoubleToString(aDouble);
    aValue.SetString(str.c_str(), static_cast<RSSizeType>(str.size()), aAllocator);
  }

  inline RSValue DoubleAsValue(double aDouble, RSAllocator &aAllocator)
  {
    RSValue value;
    auto str = DoubleToString(aDouble);
    value.SetString(str.c_str(), static_cast<RSSizeType>(str.size()), aAllocator);
    return value;
  }

  ///////////////////////////////
  // Real2
  ///////////////////////////////
  inline glm::vec2 ValueAsReal2(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector2");
    auto x = ValueAsFloat(&real4->value.FindMember("x")->value);
    auto y = ValueAsFloat(&real4->value.FindMember("y")->value);
    return glm::vec2{ x, y };
  }

  inline void Real2AsValue(RSValue &aValue, glm::vec2 aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", FloatAsValue(aVector.x, aAllocator), aAllocator);
    subObject.AddMember("y", FloatAsValue(aVector.y, aAllocator), aAllocator);

    aValue.AddMember("Vector2", subObject, aAllocator);
  }

  ///////////////////////////////
  // Real3
  ///////////////////////////////
  inline glm::vec3 ValueAsReal3(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector3");
    auto x = ValueAsFloat(&real4->value.FindMember("x")->value);
    auto y = ValueAsFloat(&real4->value.FindMember("y")->value);
    auto z = ValueAsFloat(&real4->value.FindMember("z")->value);
    return glm::vec3{ x, y, z };
  }

  inline void Real3AsValue(RSValue &aValue, glm::vec3 aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", FloatAsValue(aVector.x, aAllocator), aAllocator);
    subObject.AddMember("y", FloatAsValue(aVector.y, aAllocator), aAllocator);
    subObject.AddMember("z", FloatAsValue(aVector.z, aAllocator), aAllocator);

    aValue.AddMember("Vector3", subObject, aAllocator);
  }

  ///////////////////////////////
  // Real4
  ///////////////////////////////
  inline glm::vec4 ValueAsReal4(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Vector4");
    auto x = ValueAsFloat(&real4->value.FindMember("x")->value);
    auto y = ValueAsFloat(&real4->value.FindMember("y")->value);
    auto z = ValueAsFloat(&real4->value.FindMember("z")->value);
    auto w = ValueAsFloat(&real4->value.FindMember("w")->value);
    return glm::vec4{ x, y, z, w };
  }

  inline void Real4AsValue(RSValue &aValue, glm::vec4 aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", FloatAsValue(aVector.x, aAllocator), aAllocator);
    subObject.AddMember("y", FloatAsValue(aVector.y, aAllocator), aAllocator);
    subObject.AddMember("z", FloatAsValue(aVector.z, aAllocator), aAllocator);
    subObject.AddMember("w", FloatAsValue(aVector.w, aAllocator), aAllocator);

    aValue.AddMember("Vector4", subObject, aAllocator);
  }

  ///////////////////////////////
  // Quaternion
  ///////////////////////////////
  inline glm::quat ValueAsQuaternion(RSValue *aValue)
  {
    auto real4 = aValue->FindMember("Quaternion");
    auto x = ValueAsFloat(&real4->value.FindMember("x")->value);
    auto y = ValueAsFloat(&real4->value.FindMember("y")->value);
    auto z = ValueAsFloat(&real4->value.FindMember("z")->value);
    auto w = ValueAsFloat(&real4->value.FindMember("w")->value);
    return glm::quat{ w, x, y, z};
  }

  inline void QuaternionAsValue(RSValue &aValue, glm::quat aVector, RSAllocator &aAllocator)
  {
    aValue.SetObject();

    RSValue subObject;
    subObject.SetObject();
    subObject.AddMember("x", FloatAsValue(aVector.x, aAllocator), aAllocator);
    subObject.AddMember("y", FloatAsValue(aVector.y, aAllocator), aAllocator);
    subObject.AddMember("z", FloatAsValue(aVector.z, aAllocator), aAllocator);
    subObject.AddMember("w", FloatAsValue(aVector.w, aAllocator), aAllocator);

    aValue.AddMember("Quaternion", subObject, aAllocator);
  }
}

#endif