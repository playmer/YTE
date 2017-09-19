#include "YTE/Meta/Type.hpp"

namespace YTE
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  YTEDefineType(DocumentedObject)
  {
    YTERegisterType(DocumentedObject);
    YTEBindProperty(&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation, "Documentation");
  }

  YTEDefineType(Type)
  {
    YTERegisterType(Type);

  }

  YTEDefineType(Function)
  {
    YTERegisterType(Function);

  }


  YTEDefineType(Property)
  {
    YTERegisterType(Property);

  }


  YTEDefineType(Field)
  {
    YTERegisterType(Field);
  }

  YTEDefineExternalType(void)
  {
    YTERegisterType(void);
  }

  YTEDefineExternalType(bool)
  {
    YTERegisterType(bool);
  }

  YTEDefineExternalType(s8)
  {
    YTERegisterType(s8);
  }

  YTEDefineExternalType(i8)
  {
    YTERegisterType(i8);
  }

  YTEDefineExternalType(i16)
  {
    YTERegisterType(i16);
  }

  YTEDefineExternalType(i32)
  {
    YTERegisterType(i32);
  }

  YTEDefineExternalType(i64)
  {
    YTERegisterType(i64);
  }

  YTEDefineExternalType(u8)
  {
    YTERegisterType(u8);
  }

  YTEDefineExternalType(u16)
  {
    YTERegisterType(u16);
  }

  YTEDefineExternalType(u32)
  {
    YTERegisterType(u32);
  }

  YTEDefineExternalType(u64)
  {
    YTERegisterType(u64);
  }

  YTEDefineExternalType(float)
  {
    YTERegisterType(float);
  }

  YTEDefineExternalType(double)
  {
    YTERegisterType(double);
  }

  YTEDefineExternalType(std::string)
  {
    YTERegisterType(std::string);
  }

  YTEDefineExternalType(YTE::String)
  {
    YTERegisterType(YTE::String);
  }

}