#pragma once
#include "YTE/Meta/Type.hpp"

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
    Serializable(DocumentedObject *aObject);
  };

  class EditorProperty : public Attribute
  {
  public:
    YTEDeclareType(EditorProperty);
    EditorProperty(DocumentedObject *aObject, bool aVisible = true);

    bool mVisible;
  };

  typedef std::vector <std::string> (*StrGettor)(Component*);

  class DropDownStrings : public Attribute
  {
  public:
    YTEDeclareType(DropDownStrings);
    DropDownStrings(DocumentedObject *aObject, StrGettor aStrGettor);

    StrGettor GetStringGettor();

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
    EditorHeader(DocumentedObject *aObject, std::string aName);

    std::string GetName();

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

    std::vector<std::pair<Object*, std::string>> GetList(Object *aObject);
    RSValue Serialize(RSAllocator &aAllocator, Object *aOwner);
    void Deserialize(RSValue &aValue, Object *aOwner);

    std::string GetName();

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
    RedirectObject(DocumentedObject *aObject,
                   Deserializer aDeserializer, 
                   Serializer aSerializer, 
                   Getter aLister, 
                   std::string aName);

    std::pair<Object*, std::string> GetObjectPtr(Object *aObject);
    RSValue Serialize(RSAllocator &aAllocator, Object *aOwner);
    void Deserialize(RSValue &aValue, Object *aOwner);

    std::string GetName();

  private:
    Getter mGetter;
    Serializer mSerializer;
    Deserializer mDeserializer;

    std::string mName;
  };
}



