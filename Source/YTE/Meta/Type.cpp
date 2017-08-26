#include "YTE/Meta/Type.hpp"

namespace YTE
{
  std::unordered_map<std::string, Type*> Type::sGlobalTypes;

  DefineType(DocumentedObject)
  {
    YTERegisterType(DocumentedObject);
    YTEBindProperty(&DocumentedObject::GetDocumentation, &DocumentedObject::SetDocumentation, "Documentation");
  }

  DefineType(Type)
  {
    YTERegisterType(Type);

  }

  DefineType(Function)
  {
    YTERegisterType(Function);

  }


  DefineType(Property)
  {
    YTERegisterType(Property);

  }


  DefineType(Field)
  {
    YTERegisterType(Field);
  }

  DefineExternalType(void)
  {
    YTERegisterType(void);
  }

  DefineExternalType(bool)
  {
    YTERegisterType(bool);
  }

  DefineExternalType(s8)
  {
    YTERegisterType(s8);
  }

  DefineExternalType(i8)
  {
    YTERegisterType(i8);
  }

  DefineExternalType(i16)
  {
    YTERegisterType(i16);
  }

  DefineExternalType(i32)
  {
    YTERegisterType(i32);
  }

  DefineExternalType(i64)
  {
    YTERegisterType(i64);
  }

  DefineExternalType(u8)
  {
    YTERegisterType(u8);
  }

  DefineExternalType(u16)
  {
    YTERegisterType(u16);
  }

  DefineExternalType(u32)
  {
    YTERegisterType(u32);
  }

  DefineExternalType(u64)
  {
    YTERegisterType(u64);
  }

  DefineExternalType(float)
  {
    YTERegisterType(float);
  }

  DefineExternalType(double)
  {
    YTERegisterType(double);
  }

  DefineExternalType(std::string)
  {
    YTERegisterType(std::string);
  }

  DefineExternalType(YTE::String)
  {
    YTERegisterType(YTE::String);
  }

}