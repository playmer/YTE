#include "YTE/Meta/Attribute.hpp"

namespace YTE
{
  DefineType(Attribute)
  {
    YTERegisterType(Attribute);
  }

  DefineType(EditorProperty)
  {
    YTERegisterType(EditorProperty);
  }

  DefineType(DropDownStrings)
  {
    YTERegisterType(DropDownStrings);
  }

  EditorProperty::EditorProperty(DocumentedObject *aObject, bool aVisible)
    : mVisible(aVisible)
  {
    auto property = dynamic_cast<Property*>(aObject);

    auto getter = property->GetGetter();
    auto setter = property->GetSetter();

    DebugAssert(nullptr == getter,
                "EditorProperty %s missing getter",
                property->GetName().c_str());

    DebugAssert(nullptr == setter,
                "EditorProperty %s missing setter",
                property->GetName().c_str());

    auto parameters = setter->GetParameters();

    DebugAssert(parameters->size() != 2,
                "EditorProperty %s must have a setter that takes only one parameter,"
                " an instance of an object of the same type as it's getter returns.",
                property->GetName().c_str());

    DebugAssert((*parameters)[1].mType->GetMostBasicType() != getter->GetReturnType()->GetMostBasicType(),
                "EditorProperty %s must have a setter that takes as it's "
                "first parameter, the same type as it's getter returns. \n"
                "  Setter First Parameter Type: %s"
                "  Getter Return Type : %s",
                property->GetName().c_str(),
                (*parameters)[1].mType->GetName().c_str(),
                getter->GetName().c_str());
  }

  DropDownStrings::DropDownStrings(DocumentedObject *aObject, StrGettor aStrGettor)
    : mStringGettor(aStrGettor)
  {
    auto prop = dynamic_cast<Property*>(aObject);

    DebugAssert(aStrGettor == nullptr, "SelectableStrings %s must be passed a valid function pointer"
    "that returns a vector of strings.  It is currently being passed a nullptr.", prop->GetName());
  }

  StrGettor DropDownStrings::GetStringGettor()
  {
    return mStringGettor;
  }
}