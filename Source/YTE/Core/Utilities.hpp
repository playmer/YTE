#pragma once

#ifndef YTE_Core_Types_h
#define YTE_Core_Types_h

#include <cstdint>
#include <memory>
#include <unordered_map>

#include "optick.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"

#include "fmt/format.h"

#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Meta/Meta.hpp"

#include "YTE/Platform/DialogBox.hpp"

#include "YTE/Platform/TargetDefinitions.hpp"

#include "YTE/StandardLibrary/OrderedMultiMap.hpp"
#include "YTE/StandardLibrary/OrderedMap.hpp"
#include "YTE/StandardLibrary/Utilities.hpp"

#include "YTE/Utilities/String/String.hpp"

#include "YTE/Core/StaticIntents.hpp"


namespace YTE
{
  template <typename Type> 
  using SharedPointer = std::shared_ptr<Type>;

  template <typename Type, typename Deleter = std::default_delete<Type>> 
  using UniquePointer = std::unique_ptr<Type, Deleter>;

  using FactoryMap = OrderedMap<Type*, UniquePointer<StringComponentFactory>>;
  using FactorySetupCallback = void(*)(FactoryMap &);

  using CompositionMap = OrderedMultiMap<String, std::unique_ptr<Composition>>;
}

YTEDeclareExternalType(glm::i32vec2);
YTEDeclareExternalType(glm::vec2);
YTEDeclareExternalType(glm::vec3);
YTEDeclareExternalType(glm::vec4);
YTEDeclareExternalType(glm::quat);

#endif
