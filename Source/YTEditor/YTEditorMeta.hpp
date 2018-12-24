#pragma once

#include "YTE/Platform/TargetDefinitions.hpp"

#if YTE_Windows
  #if defined(YTEditor_Internal)
    #define YTEditor_Shared __declspec( dllexport )
  #else
    #define YTEditor_Shared __declspec( dllimport )
  #endif
#elif YTE_Linux
  #if defined(YTEditor_Internal)
    #define YTEditor_Shared __attribute__((visibility("default")))
  #else
    #define YTEditor_Shared
  #endif
#endif

#define YTEditorDeclareType(Name)                                     \
void Dummy() {}                                                       \
typedef decltype(::YTE::GetDummy(&Name::Dummy)) TempSelfType;         \
typedef decltype(::YTE::GetSelfType<TempSelfType>(nullptr)) BaseType; \
typedef TempSelfType SelfType;                                        \
YTEditor_Shared static ::YTE::Type sType;                                  \
YTEditor_Shared static ::YTE::Type* GetStaticType() { return &sType; };    \
YTEditor_Shared ::YTE::Type* GetType() { return &sType; };                 \
YTEditor_Shared static void InitializeType();


#define YTEditorDefineType(Name)                         \
::YTE::Type Name::sType{#Name,                           \
                 static_cast<Name*>(nullptr),            \
                 static_cast<Name::BaseType*>(nullptr)}; \
void Name::InitializeType()
