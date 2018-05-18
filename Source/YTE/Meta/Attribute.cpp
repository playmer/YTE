#include "YTE/Meta/Attribute.hpp"
#include "YTE/Core/Object.hpp"

namespace YTE
{
  YTEDefineType(Attribute)
  {
    RegisterType<Attribute>();
    TypeBuilder<Attribute> builder;
  }

  YTEDefineType(Serializable)
  {
    RegisterType<Serializable>();
    TypeBuilder<Serializable> builder;
  }

  YTEDefineType(RunInEditor)
  {
    RegisterType<RunInEditor>();
    TypeBuilder<RunInEditor> builder;
  }

  YTEDefineType(EditableColor)
  {
    RegisterType<EditableColor>();
    TypeBuilder<EditableColor> builder;
  }

  void PropertyChecked(const char *aType, DocumentedObject *aObject)
  {
    auto property = dynamic_cast<Property*>(aObject);

    auto getter = property->GetGetter();
    auto setter = property->GetSetter();

    DebugObjection(nullptr == getter,
                   "%s %s missing getter",
                   aType,
                   property->GetName().c_str());

    DebugObjection(nullptr == setter,
                   "%s %s missing setter",
                   aType,
                   property->GetName().c_str());

    auto parameters = setter->GetParameters();

    DebugObjection(parameters.size() != 2,
                   "%s %s must have a setter that takes only one parameter,"
                   " an instance of an object of the same type as it's getter returns.",
                   aType,
                   property->GetName().c_str());

    DebugObjection(parameters[1].mType->GetMostBasicType() != getter->GetReturnType()->GetMostBasicType(),
                   "%s %s must have a setter that takes as it's "
                   "first parameter, the same type as it's getter returns. \n"
                   "  Setter First Parameter Type: %s"
                   "  Getter Return Type : %s",
                   aType,
                   property->GetName().c_str(),
                   parameters[1].mType->GetName().c_str(),
                   getter->GetName().c_str());

    UnusedArguments(aType, getter, parameters);
  }

  Serializable::Serializable(DocumentedObject *aObject)
  {
    PropertyChecked("Serializable", aObject);
  }

  YTEDefineType(EditorProperty)
  {
    RegisterType<EditorProperty>();
    TypeBuilder<EditorProperty> builder;
  }

  YTEDefineType(DropDownStrings)
  {
    RegisterType<DropDownStrings>();
    TypeBuilder<DropDownStrings> builder;
  }

  YTEDefineType(EditorHeader)
  {
    RegisterType<EditorHeader>();
    TypeBuilder<EditorHeader> builder;
  }

  YTEDefineType(EditorHeaderList)
  {
    RegisterType<EditorHeaderList>();
    TypeBuilder<EditorHeaderList> builder;
  }

  YTEDefineType(RedirectObject)
  {
    RegisterType<RedirectObject>();
    TypeBuilder<RedirectObject> builder;
  }

  EditorProperty::EditorProperty(DocumentedObject *aObject, bool aVisible)
    : mVisible(aVisible)
  {
    PropertyChecked("EditorProperty", aObject);
  }

  DropDownStrings::DropDownStrings(DocumentedObject *aObject, StrGettor aStrGettor)
    : mStringGettor(aStrGettor)
  {
    auto prop = dynamic_cast<Property*>(aObject);
    UnusedArguments(prop);

    DebugObjection(aStrGettor == nullptr, 
                   "SelectableStrings %s must be passed a valid function pointer"
                   "that returns a vector of strings.  It is currently being passed a nullptr.", 
                   prop->GetName().c_str());
  }

  StrGettor DropDownStrings::GetStringGettor()
  {
    return mStringGettor;
  }


  EditorHeader::EditorHeader(YTE::DocumentedObject *aObject, std::string aName)
    : mName(aName)
  {
    UnusedArguments(aObject);
  }

  std::string EditorHeader::GetName()
  {
    return mName;
  }


  EditorHeaderList::EditorHeaderList(YTE::DocumentedObject *aObject,
                                     Deserializer aDeserializer,
                                     Serializer aSerializer, 
                                     Lister aLister, 
                                     std::string aName)
    : mLister(aLister)
    , mSerializer(aSerializer)
    , mDeserializer(aDeserializer)
    , mName(aName)
  {
    UnusedArguments(aObject);
  }

  std::vector<std::pair<Object*, std::string>> EditorHeaderList::GetList(YTE::Object *aObject)
  {
    return mLister(aObject);
  }

  RSValue EditorHeaderList::Serialize(RSAllocator &aAllocator, Object *aOwner)
  {
    return mSerializer(aAllocator, aOwner);
  }


  void EditorHeaderList::Deserialize(RSValue &aValue, Object *aOwner)
  {
    return mDeserializer(aValue, aOwner);
  }

  std::string EditorHeaderList::GetName()
  {
    return mName;
  }

  

  RedirectObject::RedirectObject(YTE::DocumentedObject *aObject,
                                 Deserializer aDeserializer,
                                 Serializer aSerializer, 
                                 Getter aGetter, 
                                 std::string aName)
    : mGetter(aGetter)
    , mSerializer(aSerializer)
    , mDeserializer(aDeserializer)
    , mName(aName)
  {
    UnusedArguments(aObject);
  }

  std::pair<Object*, std::string> RedirectObject::GetObjectPtr(YTE::Object *aObject)
  {
    return mGetter(aObject);
  }

  RSValue RedirectObject::Serialize(RSAllocator &aAllocator, Object *aOwner)
  {
    return mSerializer(aAllocator, aOwner);
  }


  void RedirectObject::Deserialize(RSValue &aValue, Object *aOwner)
  {
    return mDeserializer(aValue, aOwner);
  }

  std::string RedirectObject::GetName()
  {
    return mName;
  }
}

