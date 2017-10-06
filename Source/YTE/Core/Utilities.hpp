/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-12-14
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_Types_h
#define YTE_Core_Types_h

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Meta/Type.hpp"

#include "YTE/Platform/DialogBox.hpp"

#include "YTE/StandardLibrary/OrderedMultiMap.hpp"
#include "YTE/StandardLibrary/OrderedMap.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Utilities/String/String.h"

#include "YTE/Core/StaticIntents.hpp"

namespace YTE
{
  using RSValue = rapidjson::Value;
  using RSDocument = rapidjson::Document;
  using RSStringBuffer = rapidjson::StringBuffer;
  using RSPrettyWriter = rapidjson::PrettyWriter<RSStringBuffer>;
  using RSAllocator = rapidjson::Document::AllocatorType;
  using RSSizeType = rapidjson::SizeType;

  template <typename Type> using SharedPointer = std::shared_ptr<Type>;

  template <typename Type, typename Deleter = std::default_delete<Type>> using UniquePointer = std::unique_ptr<Type, Deleter>;

  using BoundType = Type;

  using FactoryMap = OrderedMap<BoundType*, UniquePointer<StringComponentFactory>>;
  using FactorySetupCallback = void(*)(FactoryMap &);

  using CompositionMap = OrderedMultiMap<String, std::unique_ptr<Composition>>;
  using ComponentMap   = OrderedMap<Type*, std::unique_ptr<Component>>;
}

YTEDeclareExternalType(glm::i32vec2);
YTEDeclareExternalType(glm::vec2);
YTEDeclareExternalType(glm::vec3);
YTEDeclareExternalType(glm::vec4);
YTEDeclareExternalType(glm::quat);

#endif
