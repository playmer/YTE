#pragma once

#include <tuple>

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  // Enable if helpers
  namespace EnableIf
  {
    template<typename tType>
    using IsNotVoid = std::enable_if_t<!std::is_void_v<tType>>;

    template<typename tType>
    using IsVoid = std::enable_if_t<std::is_void_v<tType>>;

    template<typename tType>
    using IsMoveConstructible = std::enable_if_t<std::is_move_constructible_v<tType>>;

    template<typename tType>
    using IsNotMoveConstructible = std::enable_if_t<!std::is_move_constructible_v<tType>>;

    template<typename tType>
    using IsCopyConstructible = std::enable_if_t<std::is_copy_constructible_v<tType>>;

    template<typename tType>
    using IsNotCopyConstructible = std::enable_if_t<!std::is_copy_constructible_v<tType>>;

    template<typename tType>
    using IsDefaultConstructible = std::enable_if_t<std::is_default_constructible_v<tType>>;

    template<typename tType>
    using IsNotDefaultConstructible = std::enable_if_t<!std::is_default_constructible_v<tType>>;
  }

  template<typename tType>
  inline size_t SizeOf()
  {
    return sizeof(tType);
  }

  template<>
  inline size_t SizeOf<void>()
  {
    return 0;
  }

  template <typename tType>
  inline void GenericDestructByte(byte *aMemory)
  {
    (reinterpret_cast<tType*>(aMemory))->~tType();
  }

  template <typename tType>
  inline void GenericDestruct(void* aMemory)
  {
    (static_cast<tType*>(aMemory))->~tType();
  }

  template <typename tType, typename = void>
  struct GenericDefaultConstructStruct
  {

  };

  template <typename tType>
  struct GenericDefaultConstructStruct<tType, EnableIf::IsNotDefaultConstructible<tType>>
  {
    static inline void DefaultConstruct(void *)
    {
      runtime_assert(false, "Trying to default construct something without a default constructor.");
    }
  };

  template <typename tType>
  struct GenericDefaultConstructStruct<tType, EnableIf::IsDefaultConstructible<tType>>
  {
    static inline void DefaultConstruct(void *aMemory)
    {
      new (aMemory) tType();
    }
  };


  template <typename T, typename Enable = void>
  struct GenericCopyConstructStruct
  {

  };

  template <typename tType>
  struct GenericCopyConstructStruct<tType, EnableIf::IsNotCopyConstructible<tType>>
  {
    static inline void CopyConstruct(const void *, void *)
    {
      runtime_assert(false, "Trying to copy construct something without a copy constructor.");
    }
  };

  template <typename tType>
  struct GenericCopyConstructStruct<tType, EnableIf::IsCopyConstructible<tType>>
  {
    static inline void CopyConstruct(void const* aObject, void *aMemory)
    {
      new (aMemory) tType(*static_cast<tType const*>(aObject));
    }
  };

  template <typename tType, typename Enable = void>
  struct GenericMoveConstructStruct
  {

  };

  template <typename tType>
  struct GenericMoveConstructStruct<tType, EnableIf::IsNotMoveConstructible<tType>>
  {
    static inline void MoveConstruct(void *, void *)
    {
      runtime_assert(false, "Trying to move construct something without a move constructor.");
    }
  };

  template <typename tType>
  struct GenericMoveConstructStruct<tType, EnableIf::IsMoveConstructible<tType>>
  {
    static inline void MoveConstruct(void *aObject, void *aMemory)
    {
      new (aMemory) tType(std::move(*static_cast<tType*>(aObject)));
    }
  };

  template <typename tType>
  inline void GenericDefaultConstruct(void *aMemory)
  {
    GenericDefaultConstructStruct<tType>::DefaultConstruct(aMemory);
  }

  template <typename tType>
  inline void GenericCopyConstruct(const void* aObject, void* aMemory)
  {
    GenericCopyConstructStruct<tType>::CopyConstruct(aObject, aMemory);
  }

  template <typename tType>
  inline void GenericMoveConstruct(void* aObject, void* aMemory)
  {
    GenericMoveConstructStruct<tType>::MoveConstruct(aObject, aMemory);
  }


  template <>
  inline void GenericDestruct<void>(void*)
  {
  }

  template <>
  inline void GenericDefaultConstruct<void>(void*)
  {
  }

  template <>
  inline void GenericCopyConstruct<void>(const void*, void*)
  {
  }


  template <typename T>
  struct RemovePossibleReference
  {
    using type = T;
  };

  template <typename T>
  struct RemovePossibleReference<T&>
  {
    using type = T;
  };

  template <typename T>
  struct Identity
  {
    T operator()(T x) const { return x; }
  };


  template<typename T>
  struct remove_all_pointers : std::conditional_t<std::is_pointer_v<T>,
    remove_all_pointers<std::remove_pointer_t<T>>,
    Identity<T>>
  {
  };

  template<typename T>
  using remove_all_pointers_t = typename remove_all_pointers<T>::type;



  template <typename Return, typename Arg = Return>
  struct DecomposeFieldPointer {};

  template <typename Object, typename Field>
  struct DecomposeFieldPointer<Field Object::*>
  {
    using ObjectType = Object;
    using FieldType = Field;
  };


  template <typename Return, typename Arg = Return>
  struct DecomposeFunctionType {};

  template <typename Return, typename Event>
  struct DecomposeFunctionType<Return(*)(Event*)>
  {
    using ReturnType = Return;
    using EventType = Event;
  };

  template <typename Return, typename Object, typename Event>
  struct DecomposeFunctionType<Return(Object::*)(Event*)>
  {
    using ReturnType = Return;
    using ObjectType = Object;
    using EventType = Event;
  };


  //template <typename tType>
  //struct DecomposeFunctionObjectType
  //{
  //};

  template <typename Return>
  struct DecomposeFunctionObjectType
  {
    using ObjectType = std::nullptr_t;
  };

  template <typename Return, typename ...Arguments>
  struct DecomposeFunctionObjectType<Return(*)(Arguments...)>
  {
    using ReturnType = Return;
  };

  template <typename Return, typename Object>
  struct DecomposeFunctionObjectType<Return(Object::*)()>
  {
    using ReturnType = Return;
    using ObjectType = Object;
  };

  template <typename Return, typename Object, typename ...Arguments>
  struct DecomposeFunctionObjectType<Return(Object::*)(Arguments...)>
  {
    using ReturnType = Return;
    using ObjectType = Object;
  };

  template <typename Return, typename Object, typename ...Arguments>
  struct DecomposeFunctionObjectType<Return(Object::*)(Arguments...) const>
  {
    using ReturnType = Return;
    using ObjectType = Object;
  };

  template<std::size_t N, typename... T>
  using static_switch = typename std::tuple_element<N, std::tuple<T...> >::type;

  constexpr size_t TrueOrFalse(bool aBool)
  {
    return aBool ? true : false;
  }

  template <typename tFunctionType1, typename tFunctionType2>
  struct DecomposePropertyType
  {
    using ObjectType1 = typename DecomposeFunctionObjectType<tFunctionType1>::ObjectType;
    using ObjectType2 = typename DecomposeFunctionObjectType<tFunctionType2>::ObjectType;

    static_assert(std::is_same<ObjectType1, std::nullptr_t>::value == false || std::is_same<ObjectType2, std::nullptr_t>::value == false,
      "One or both of the getter and setter pair must be a valid function, they cannot both be nullptr.");

    using ObjectType = static_switch <TrueOrFalse(std::is_same<ObjectType1, std::nullptr_t>::value), ObjectType1, ObjectType2>;
  };

  template <typename Return>
  struct CountFunctionArguments
  {

  };

  template <typename Return, typename ...Arguments>
  struct CountFunctionArguments<Return(Arguments...)>
  {
    constexpr static size_t Size()
    {
      return sizeof...(Arguments);
    }
  };

  template <typename Return, typename ...Arguments>
  struct CountFunctionArguments<Return(*)(Arguments...)>
  {
    constexpr static size_t Size()
    {
      return sizeof...(Arguments);
    }
  };

  template <typename Return, typename Object, typename ...Arguments>
  struct CountFunctionArguments<Return(Object::*)(Arguments...)>
  {
    constexpr static size_t Size()
    {
      return sizeof...(Arguments);
    }
  };

  template <typename Return, typename Object, typename ...Arguments>
  struct CountFunctionArguments<Return(Object::*)(Arguments...) const>
  {
    constexpr static size_t Size()
    {
      return sizeof...(Arguments);
    }
  };


  // Helper to call the constructor of a type.
  inline void GenericDoNothing(byte*)
  {
  }

  template <typename tType>
  struct StripQualifiers
  {
    using type = tType;
  };

  template<typename tType>
  using StripQualifiersT = typename StripQualifiers<tType>::type;

  template <>
  struct StripQualifiers<void*>
  {
    using type = void*;
  };

  template <>
  struct StripQualifiers<void const*>
  {
    using type =  void*;
  };

  template <typename tType>
  struct StripQualifiers<tType const>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripQualifiers<tType*>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripQualifiers<tType&>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripQualifiers<tType&&>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripQualifiers<tType const*>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripQualifiers<tType const &>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripQualifiers<tType const&&>
  {
    using type = StripQualifiersT<tType>;
  };

  template <typename tType>
  struct StripSingleQualifier
  {
    using type = tType;
  };

  template <typename tType>
  struct StripSingleQualifier<tType const>
  {
    using type = tType;
  };

  template <>
  struct StripSingleQualifier<void*>
  {
    using type = void*;
  };

  template <>
  struct StripSingleQualifier<void const*>
  {
    using type = void*;
  };

  template <typename tType>
  struct StripSingleQualifier<tType*>
  {
    using type = tType;
  };

  template <typename tType>
  struct StripSingleQualifier<tType&>
  {
    using type = tType;
  };

  template <typename tType>
  struct StripSingleQualifier<tType&&>
  {
    using type = tType;
  };

  template <typename tType>
  struct StripSingleQualifier<tType const*>
  {
    using type = tType;
  };

  template <typename tType>
  struct StripSingleQualifier<tType const&>
  {
    using type = tType;
  };

  template <typename tType>
  struct StripSingleQualifier<tType const&&>
  {
    using type = tType;
  };

  template<typename tType>
  using StripSingleQualifierT = typename StripSingleQualifier<tType>::type;

  template <auto tValue>
  constexpr decltype(tValue) ReturnValue()
  {
    return tValue;
  }
}
