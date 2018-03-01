#pragma once

#include <tuple>

#include "YTE/StandardLibrary/Utilities.hpp"

namespace YTE
{
  template<typename T>
  inline size_t SizeOf()
  {
    return sizeof(T);
  }

  template<>
  inline size_t SizeOf<void>()
  {
    return 0;
  }

  template <typename T>
  inline void GenericDestructByte(byte *aMemory)
  {
    (reinterpret_cast<T*>(aMemory))->~T();
  }

  template <typename T>
  inline void GenericDestruct(void* aMemory)
  {
    (static_cast<T*>(aMemory))->~T();
  }

  template <typename T, typename Enable = void>
  struct GenericDefaultConstructStruct
  {

  };

  template <typename T>
  struct GenericDefaultConstructStruct<T, typename std::enable_if<std::is_default_constructible<T>::value == false>::type>
  {
    static inline void DefaultConstruct(void *)
    {
      runtime_assert(false, "Trying to default construct something without a default constructor.");
    }
  };

  template <typename T>
  struct GenericDefaultConstructStruct<T, typename std::enable_if<std::is_default_constructible<T>::value>::type>
  {
    static inline void DefaultConstruct(void *aMemory)
    {
      new (aMemory) T();
    }
  };


  template <typename T, typename Enable = void>
  struct GenericCopyConstructStruct
  {

  };

  template <typename T>
  struct GenericCopyConstructStruct<T, typename std::enable_if<std::is_copy_constructible<T>::value == false>::type>
  {
    static inline void CopyConstruct(const void *, void *)
    {
      runtime_assert(false, "Trying to copy construct something without a copy constructor.");
    }
  };

  template <typename T>
  struct GenericCopyConstructStruct<T, typename std::enable_if<std::is_copy_constructible<T>::value>::type>
  {
    static inline void CopyConstruct(const void *aObject, void *aMemory)
    {
      new (aMemory) T(*static_cast<const T*>(aObject));
    }
  };



  template <typename T, typename Enable = void>
  struct GenericMoveConstructStruct
  {

  };

  template <typename T>
  struct GenericMoveConstructStruct<T, typename std::enable_if<std::is_move_constructible<T>::value == false>::type>
  {
    static inline void MoveConstruct(void *, void *)
    {
      runtime_assert(false, "Trying to move construct something without a move constructor.");
    }
  };

  template <typename T>
  struct GenericMoveConstructStruct<T, typename std::enable_if<std::is_move_constructible<T>::value>::type>
  {
    static inline void MoveConstruct(void *aObject, void *aMemory)
    {
      new (aMemory) T(std::move(*static_cast<T*>(aObject)));
    }
  };

  template <typename T>
  inline void GenericDefaultConstruct(void *aMemory)
  {
    GenericDefaultConstructStruct<T>::DefaultConstruct(aMemory);
  }

  template <typename T>
  inline void GenericCopyConstruct(const void* aObject, void* aMemory)
  {
    GenericCopyConstructStruct<T>::CopyConstruct(aObject, aMemory);
  }

  template <typename T>
  inline void GenericMoveConstruct(void* aObject, void* aMemory)
  {
    GenericMoveConstructStruct<T>::MoveConstruct(aObject, aMemory);
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
    using ObjectType = nullptr_t;
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

    static_assert(std::is_same<ObjectType1, nullptr_t>::value == false || std::is_same<ObjectType2, nullptr_t>::value == false,
      "One or both of the getter and setter pair must be a valid function, they cannot both be nullptr.");

    using ObjectType = static_switch <TrueOrFalse(std::is_same<ObjectType1, nullptr_t>::value), ObjectType1, ObjectType2>;
  };

  template <typename Return, typename Arg = Return>
  struct CountFunctionArguments
  {

  };

  template <typename Return>
  struct CountFunctionArguments<Return(*)()>
  {
    constexpr static size_t Size()
    {
      return 0;
    }
  };

  template <typename Return, typename Object>
  struct CountFunctionArguments<Return(Object::*)()>
  {
    constexpr static size_t Size()
    {
      return 0;
    }
  };

  template <typename Return, typename Object>
  struct CountFunctionArguments<Return(Object::*)() const>
  {
    constexpr static size_t Size()
    {
      return 0;
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
  inline void GenericDoNothing(byte *)
  {
  }

  template <typename T>
  struct StripQualifiers
  {
    typedef T type;
  };

  template <>
  struct StripQualifiers<void *>
  {
    typedef void * type;
  };

  template <>
  struct StripQualifiers<const void *>
  {
    typedef void * type;
  };

  template <typename T>
  struct StripQualifiers<const T>
  {
    typedef typename StripQualifiers<T>::type type;
  };

  template <typename T>
  struct StripQualifiers<T *>
  {
    typedef typename StripQualifiers<T>::type type;
  };

  template <typename T>
  struct StripQualifiers<T &>
  {
    typedef typename StripQualifiers<T>::type type;
  };

  template <typename T>
  struct StripQualifiers<T &&>
  {
    typedef typename StripQualifiers<T>::type type;
  };

  template <typename T>
  struct StripQualifiers<const T *>
  {
    typedef typename StripQualifiers<T>::type type;
  };
  
  template <typename T>
  struct StripQualifiers<const T &>
  {
    typedef typename StripQualifiers<T>::type type;
  };
  
  template <typename T>
  struct StripQualifiers<const T &&>
  {
    typedef typename StripQualifiers<T>::type type;
  };

  template <typename T>
  struct StripSingleQualifier
  {
    typedef T type;
  };

  template <typename T>
  struct StripSingleQualifier<const T>
  {
    typedef T type;
  };

  template <>
  struct StripSingleQualifier<void *>
  {
    typedef void * type;
  };

  template <>
  struct StripSingleQualifier<const void *>
  {
    typedef void * type;
  };

  template <typename T>
  struct StripSingleQualifier<T *>
  {
    typedef T type;
  };

  template <typename T>
  struct StripSingleQualifier<T &>
  {
    typedef T type;
  };

  template <typename T>
  struct StripSingleQualifier<T &&>
  {
    typedef T type;
  };

  template <typename T>
  struct StripSingleQualifier<const T *>
  {
    typedef T type;
  };
  
  template <typename T>
  struct StripSingleQualifier<const T &>
  {
    typedef T type;
  };

  template <typename T>
  struct StripSingleQualifier<const T &&>
  {
    typedef T type;
  };
}
