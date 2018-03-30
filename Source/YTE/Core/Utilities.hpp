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

#if YTE_CAN_PROFILE
#include <easy/profiler.h>
#endif

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "fmt/format.h"

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Meta/Type.hpp"

#include "YTE/Platform/DialogBox.hpp"

#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/StandardLibrary/OrderedMultiMap.hpp"
#include "YTE/StandardLibrary/OrderedMap.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTE/Core/StaticIntents.hpp"

#if YTE_CAN_PROFILE 
  #define YTEProfileName(aName) EASY_BLOCK(aName)
  #define YTEProfileFunction() EASY_FUNCTION(profiler::colors::Red)
  #define YTEProfileBlock(aColor, aName) EASY_BLOCK(aName, aColor)
#else
  namespace profiler
  {
    inline void startListen(uint16_t aBlah = 1) { (void)aBlah; }
    inline void stopListen() { }
  }

  #define YTEProfileName(aName)
  #define YTEProfileFunction(aColor)
  #define YTEProfileBlock(aColor, aName)
#endif

namespace YTE
{
  template <typename Type> 
  using SharedPointer = std::shared_ptr<Type>;

  template <typename Type, typename Deleter = std::default_delete<Type>> 
  using UniquePointer = std::unique_ptr<Type, Deleter>;

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
