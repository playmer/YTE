#pragma once
#include "YTE/Meta/Meta.hpp"

#include "YTE/Platform/DialogBox.hpp"

namespace YTE
{
  class Component;
  class Object;

  class Attribute : public Base
  {
  public:
    YTEDeclareType(Attribute)
  };

  class Serializable : public Attribute
  {
  public:
    YTEDeclareType(Serializable);
    YTE_Shared Serializable(DocumentedObject *aObject);
  };

  class RunInEditor : public Attribute
  {
  public:
    YTEDeclareType(RunInEditor);
    RunInEditor(DocumentedObject *) {}
  };

  class EditableColor : public Attribute
  {
  public:
    YTEDeclareType(EditableColor);
    EditableColor(DocumentedObject *) {}
  };

  class EditorProperty : public Attribute
  {
  public:
    YTEDeclareType(EditorProperty);
    YTE_Shared EditorProperty(DocumentedObject *aObject, bool aVisible = true);

    bool mVisible;
  };

  typedef std::vector <std::string> (*StrGettor)(Component*);

  class DropDownStrings : public Attribute
  {
  public:
    YTEDeclareType(DropDownStrings);
    YTE_Shared DropDownStrings(DocumentedObject *aObject, StrGettor aStrGettor);

    YTE_Shared StrGettor GetStringGettor();

  private:
    StrGettor mStringGettor;
  };


  using Lister = std::vector<std::pair<Object*, std::string>>(*)(Object*);
  using Getter = std::pair<Object*, std::string>(*)(Object*);
  using Serializer = RSValue(*)(RSAllocator &aAllocator, Object *aOwner);
  using Deserializer = void(*)(RSValue &aValue, Object *aOwner);

  // Applied to Properties and Fields
  class EditorHeader : public Attribute
  {
  public:
    YTEDeclareType(EditorHeader);
    YTE_Shared EditorHeader(DocumentedObject *aObject, std::string aName);

    YTE_Shared std::string GetName();

  private:
    std::string mName;
  };


  // Applied to Types. (eg. YTE::Components, YTE::Objects)
  class EditorHeaderList : public Attribute
  {
  public:
    YTEDeclareType(EditorHeaderList);
    EditorHeaderList(DocumentedObject *aObject, 
                     Deserializer aDeserializer, 
                     Serializer aSerializer, 
                     Lister aLister, 
                     std::string aName);

    YTE_Shared std::vector<std::pair<Object*, std::string>> GetList(Object *aObject);
    YTE_Shared RSValue Serialize(RSAllocator &aAllocator, Object *aOwner);
    YTE_Shared void Deserialize(RSValue &aValue, Object *aOwner);

    YTE_Shared std::string GetName();

  private:
    Lister mLister;
    Serializer mSerializer;
    Deserializer mDeserializer;

    std::string mName;
  };


  // Applied to Properties and Fields
  class RedirectObject : public Attribute
  {
  public:
    YTEDeclareType(RedirectObject);
    YTE_Shared RedirectObject(DocumentedObject *aObject,
                              Deserializer aDeserializer, 
                              Serializer aSerializer, 
                              Getter aLister, 
                              std::string aName);

    YTE_Shared std::pair<Object*, std::string> GetObjectPtr(Object *aObject);
    YTE_Shared RSValue Serialize(RSAllocator &aAllocator, Object *aOwner);
    YTE_Shared void Deserialize(RSValue &aValue, Object *aOwner);

    YTE_Shared std::string GetName();

  private:
    Getter mGetter;
    Serializer mSerializer;
    Deserializer mDeserializer;

    std::string mName;
  };
}



