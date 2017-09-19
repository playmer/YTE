#pragma once
#include "YTE/Meta/Type.hpp"

#include "YTE/Platform/DialogBox.hpp"

namespace YTE
{
  class Component;

  class Attribute : public Base
  {
  public:
    YTEDeclareType(Attribute)
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
}