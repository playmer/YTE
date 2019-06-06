#include <set>

#include "YTE/CSharp/CSharpSystem.hpp"
#include "YTE/CSharp/CSharp_CodeGenerator.hpp"

#include "YTE/Utilities/Utilities.h"

namespace YTE
{
  std::string CSharpCodeGenerator::ParseTypeName(Type *aType, 
                                                 bool aPeriodsToUnderscores,
                                                 bool aIgnoreYTE)
  {
    if (auto redirect = aType->GetAttribute<CSharpRedirect>(); nullptr != redirect)
    {
      return redirect->MonoPrimitiveName();
    }

    auto items = split(aType->GetName(), ':', true);

    std::string toReturn;

    for (auto &argument : enumerate(items))
    {
      if (*argument.first == "YTE" && aIgnoreYTE)
      {
        continue;
      }

      toReturn += *argument.first;

      if (argument.second + 1 != items.size())
      {
        toReturn += ".";
      }
    }

    if (aPeriodsToUnderscores)
    {
      for (auto &character : toReturn)
      {
        if ('.' == character)
        {
          character = '_';
        }
      }
    }

    return toReturn;
  }

  CSharpCodeGenerator::CSharpCodeGenerator()
    : mIndentSize(2)
    , mNestedNamespaces(0)
    , mIndent(0)
  {

  }

  std::string CSharpCodeGenerator::ToString()
  {
    while (mNestedNamespaces)
    {
      CloseNamespace();
    }

    return mText;
  }

  void CSharpCodeGenerator::OpenNamespace(std::string_view aNamespace)
  {
    ++mNestedNamespaces;

    std::string toAdd{ "namespace " };
    toAdd += aNamespace;
    AddLine(toAdd);

    AddScope();
  }

  void CSharpCodeGenerator::CloseNamespace()
  {
    --mNestedNamespaces;

    RemoveScope();
    AddLine("");
  }

  void CSharpCodeGenerator::AddIndent()
  {
    for (size_t i = 0; i < mIndentSize ; ++i)
    {
      mIndent.emplace_back(' ');
    }
  }

  void CSharpCodeGenerator::RemoveIndent()
  {
    for (size_t i = 0; i < mIndentSize; ++i)
    {
      mIndent.pop_back();
    }
  }

  std::string_view CSharpCodeGenerator::GetIndent()
  {
    return std::string_view{ mIndent.data(), mIndent.size() };
  }

  void CSharpCodeGenerator::AddEnums(std::string_view aName,
                                     std::vector<std::pair<std::string_view, u64>> aEnums)
  {
    std::string toAdd{ "public enum " };
    toAdd += aName;
    toAdd += " : long";

    AddLine(toAdd);
    AddScope();

    for (auto &enumPair : aEnums)
    {
      std::string enumToAdd{ enumPair.first };
      enumToAdd += " = ";
      enumToAdd += std::to_string(enumPair.second);
      enumToAdd += ",";

      AddLine(enumToAdd);
    }

    RemoveScope();
  }

  void CSharpCodeGenerator::AddClass(std::string_view aLine, Type *aBaseType, bool aIgnoreYTE)
  {
    std::string toAdd{ "public " };

    toAdd += "class ";
    toAdd += aLine;

    if (nullptr != aBaseType)
    {
      toAdd += " : ";
      toAdd += ParseTypeName(aBaseType, false, aIgnoreYTE);
    }

    AddLine(toAdd);
    AddScope();
  }

  std::string CSharpCodeGenerator::GenerateVTableName(Function *aFunction)
  {
    std::string variableName{ "function__" };

    variableName += ParseTypeName(aFunction->GetReturnType()->GetMostBasicType(), true);
    variableName += "__";
    variableName += aFunction->GetName();

    for (auto &argument : enumerate(aFunction->GetParameters()))
    {
      variableName += "__";
      variableName += ParseTypeName(argument.first->mType->GetMostBasicType(), true);
    }

    return variableName;
  }

  std::string CSharpCodeGenerator::GenerateVTableName(Property *aProperty, bool aGetter)
  {
    std::string variableName{ "property__" };

    if (aGetter)
    {
      variableName += "getter_";
    }
    else
    {
      variableName += "setter_";
    }

    variableName += aProperty->GetName();

    return variableName;

  }

  std::string CSharpCodeGenerator::GenerateVTableName(Field *aField, bool aGetter)
  {
    std::string variableName{ "field__" };

    if (aGetter)
    {
      variableName += "getter_";
    }
    else
    {
      variableName += "setter_";
    }

    variableName += aField->GetName();

    return variableName;
  }

  std::string GenerateVTablePointerAndSet(std::string &aName)
  {
    std::string vtablePointer{ "public static System.UInt64 " };

    vtablePointer += aName;
    vtablePointer += ";";

    return vtablePointer;
  }

  std::pair<std::string, std::vector<std::string>> CSharpCodeGenerator::AddVTable(std::string_view aName, Type *aType)
  {
    std::vector<std::string> names;

    std::string vTableName{ aName };
    vTableName += "_vtable";
    AddClass(vTableName, nullptr);

    // Generate static variables for Function pointers.
    for (auto &functionIt : aType->GetFunctions())
    {
      auto &function = functionIt.second;

      auto variableName = GenerateVTableName(function.get());

      auto toAdd = GenerateVTablePointerAndSet(variableName);

      AddLine(toAdd);

      names.emplace_back(variableName);
    }

    // Generate static variables for property pointers.
    for (auto &propertyIt : aType->GetProperties())
    {
      auto &property = propertyIt.second;

      if (nullptr != property->GetGetter())
      {
        auto getterName = GenerateVTableName(property.get(), true);
        auto toAddGetter = GenerateVTablePointerAndSet(getterName);
        AddLine(toAddGetter);
        names.emplace_back(getterName);
      }

      if (nullptr != property->GetSetter())
      {
        auto setterName = GenerateVTableName(property.get(), false);
        auto toAddSetter = GenerateVTablePointerAndSet(setterName);
        AddLine(toAddSetter);
        names.emplace_back(setterName);
      }
    }

    // Generate static variables for Field pointers.
    for (auto &fieldIt : aType->GetFields())
    {
      auto field = static_cast<Field*>(fieldIt.second.get());

      if (nullptr != field->GetGetter())
      {
        auto getterName = GenerateVTableName(field, true);
        auto toAddGetter = GenerateVTablePointerAndSet(getterName);
        AddLine(toAddGetter);
        names.emplace_back(getterName);
      }

      if (nullptr != field->GetSetter())
      {
        auto setterName = GenerateVTableName(field, false);
        auto toAddSetter = GenerateVTablePointerAndSet(setterName);
        AddLine(toAddSetter);
        names.emplace_back(setterName);
      }
    }

    // Generate setters for all the variables we just created.
    for (auto &name : names)
    {
      std::string line{"private static void SetVTablePointer_"};
      line += name;
      line += "(System.UInt64 aValue) { ";
      line += name;
      line += " = aValue; }";

      AddLine(line);
    }

    // Generate getters for all the variables we just created.
    for (auto &name : names)
    {
      std::string line{ "private static System.UInt64 GetVTablePointer_" };
      line += name;
      line += "() { return ";
      line += name;
      line += "; }";

      AddLine(line);
    }

    RemoveScope();

    return std::make_pair(vTableName, std::move(names));
  }

  void CSharpCodeGenerator::AddConstructor(std::string_view aName,
                                           std::pair<std::string, std::vector<std::string>> &VtableNameAndItems,
                                           Type *aType)
  {

  }

  void CSharpCodeGenerator::AddType(Type *aType)
  {
    if (nullptr != aType->GetAttribute<CSharpRedirect>())
    {
      return;
    }

    auto items = split(aType->GetMostBasicType()->GetName(), ':', true);

    auto namespaces = items.size() - 1;

    if (0 != items.size() && items[0] == "YTE")
    {
      --namespaces;
    }

    for (size_t i = 0; i < namespaces; ++i)
    {
      OpenNamespace(items[i]);
    }

    auto vtable = AddVTable(items.back(), aType);

    AddClass(items.back(), aType->GetBaseType());

    AddLine("public System.UInt64 mNativeHandle;");
    AddLine("private bool mShouldDelete;");

    AddConstructor(items.back(), vtable, aType);

    for (auto &function : aType->GetFunctions())
    {
      AddFunction(items.back(), aType, function.second.get());
    }

    for (auto &property : aType->GetProperties())
    {
      AddPropertyOrField(items.back(), aType, property.second.get());
    }

    for (auto &field : aType->GetFields())
    {
      AddPropertyOrField(items.back(), aType, field.second.get());
    }

    AddLine("");
    RemoveScope();
    AddLine("");

    for (size_t i = 0; i < namespaces; ++i)
    {
      CloseNamespace();
    }
  }


  void CSharpCodeGenerator::AddFunction(const std::string &aTypeName, 
                                        Type *aType,
                                        Function *aFunction)
  {
    bool isOverride = false;

    if (aType && aType->GetBaseType())
    {
      auto baseType = aType->GetBaseType();

      while (baseType && isOverride == false)
      {
        auto range = baseType->GetFunctionRange(aFunction->GetName());

        for (auto &functionIt : range)
        {
          if (aFunction->IsSame(*functionIt.second))
          {
            isOverride = true;
            break;
          }
        }

        baseType = baseType->GetBaseType();
      }
    }


    if (0 != aFunction->GetDocumentation().size())
    {
      AddLine("");
      AddLine(aFunction->GetDocumentation(), true);
    }

    std::string functionDeclaration{ "public " };

    if (isOverride)
    {
      functionDeclaration += "new ";
    }

    if (aFunction->IsStaticOrFree())
    {
      functionDeclaration +=  "static ";
    }

    functionDeclaration += ParseTypeName(aFunction->GetReturnType()->GetMostBasicType());
    functionDeclaration += " ";
    functionDeclaration += aFunction->GetName();
    functionDeclaration += "(";
    
    for (auto &argument : enumerate(aFunction->GetParameters()))
    {
      if (false == aFunction->IsStaticOrFree() && 0 == argument.second)
      {
        continue;
      }

      functionDeclaration += ParseTypeName(argument.first->mType->GetMostBasicType());
      functionDeclaration += " ";
      functionDeclaration += argument.first->mName;

      if (argument.second + 1 != aFunction->GetParameters().size())
      {
        functionDeclaration += ", ";
      }
    }

    functionDeclaration += ")";

    AddLine(functionDeclaration);

    AddScope();

    std::string argumentsArray{ "object[] arguments = { " };

    if (false == aFunction->IsStaticOrFree())
    {
      argumentsArray += "mNativeHandle";
    }

    for (auto &argument : enumerate(aFunction->GetParameters()))
    {
      if (false == aFunction->IsStaticOrFree() && 
          0 == argument.second)
      {
        continue;
      }
      else if (0 < argument.second)
      {
        argumentsArray += ", ";
      }

      auto redirect = argument.first->mType->GetMostBasicType()->GetAttribute<CSharpRedirect>();
      if (nullptr != redirect)
      {
        argumentsArray += argument.first->mName;
      }
      else
      {
        argumentsArray += argument.first->mName;
        argumentsArray += ".mNativeHandle";
      }
    }

    argumentsArray += " };";

    AddLine(argumentsArray);

    std::string vTableClassName{ aTypeName };
    vTableClassName += "_vtable";
    vTableClassName += '.';
    vTableClassName += GenerateVTableName(aFunction);

    if (TypeId<void>() != aFunction->GetReturnType())
    {
      std::string functionCall{"return ("};
      functionCall += ParseTypeName(aFunction->GetReturnType()->GetMostBasicType());
      functionCall += ")YTEInternalCall.functionCall_internal(";
      functionCall += vTableClassName;
      functionCall += ", arguments);";

      AddLine(functionCall);
    }
    else
    {
      std::string functionCall{};
      functionCall += "YTEInternalCall.functionCall_void_internal(";
      functionCall += vTableClassName;
      functionCall += ", arguments);";

      AddLine(functionCall);
    }

    RemoveScope();
  }

  void CSharpCodeGenerator::AddUnsafeField(const std::string &aTypeName, 
                                           Type *aType, 
                                           Field *aField)
  {

    auto offset = std::to_string(aField->GetOffset());

    auto redirect = aField->GetPropertyType()->GetAttribute<CSharpRedirect>();

    if (nullptr != aField->GetGetter())
    {
      AddLine("get");
      AddScope();
      AddUnsafe();

      //int* getValuePtr = (int*)((byte*)mNativeHandle + 4);
      std::string castAndSet{ redirect->MonoPrimitiveName() };
      castAndSet += "* getValuePtr = (";
      castAndSet += redirect->MonoPrimitiveName();
      castAndSet += "*)((byte*)mNativeHandle + ";
      castAndSet += offset;
      castAndSet += ");";

      AddLine(castAndSet);
      AddLine("return *getValuePtr;");

      RemoveScope();
      RemoveScope();
    }

    if (nullptr != aField->GetSetter())
    {
      AddLine("set");
      AddScope();
      AddUnsafe();

      //int* getValuePtr = (int*)((byte*)mNativeHandle + 4);
      std::string castAndSet{ redirect->MonoPrimitiveName() };
      castAndSet += "* setValuePtr = (";
      castAndSet += redirect->MonoPrimitiveName();
      castAndSet += "*)((byte*)mNativeHandle + ";
      castAndSet += offset;
      castAndSet += ");";

      AddLine(castAndSet);
      AddLine("*setValuePtr = value;");

      RemoveScope();
      RemoveScope();
    }
  }

  void CSharpCodeGenerator::AddPropertyOrField(const std::string &aTypeName,
                                               Type *aType,
                                               Property *aProperty)
  {
      // TODO (Josh): Add checks for shadowed Fields or Properties.
    if (0 != aProperty->GetDocumentation().size())
    {
      AddLine("");
      AddLine(aProperty->GetDocumentation(), true);
    }

    bool isField = Field::GetStaticType() == aProperty->GetType();

    std::string propertyDecorator{ "public " };

    propertyDecorator += ParseTypeName(aProperty->GetPropertyType()->GetMostBasicType());
    propertyDecorator += " ";
    propertyDecorator += aProperty->GetName();
    AddLine(propertyDecorator);

    AddScope();

    auto redirect = aProperty->GetPropertyType()->GetAttribute<CSharpRedirect>();
    if (isField && nullptr != redirect && mono_get_string_class() != redirect->GetMonoClass())
    {
      AddUnsafeField(aTypeName, aType, static_cast<Field*>(aProperty));
      RemoveScope();
      return;
    }

    std::string vTableClassName{ aTypeName };
    vTableClassName += "_vtable";
    vTableClassName += '.';

    std::string vTableGetterName{ vTableClassName };
    std::string vTableSetterName{ vTableClassName };

    if (isField)
    {
      auto asField = static_cast<Field*>(aProperty);
      vTableGetterName += GenerateVTableName(asField, true);
      vTableSetterName += GenerateVTableName(asField, false);
    }
    else
    {
      vTableGetterName += GenerateVTableName(aProperty, true);
      vTableSetterName += GenerateVTableName(aProperty, false);
    }

    if (nullptr != aProperty->GetGetter())
    {
      AddLine("get");
      AddScope();

      std::string vTableClassName{ aTypeName };
      vTableClassName += "_vtable";
      vTableClassName += '.';
      
      std::string functionCall{ "return (" };
      functionCall += ParseTypeName(aProperty->GetPropertyType()->GetMostBasicType());
      functionCall += ")YTEInternalCall.getCall_internal(";
      functionCall += vTableGetterName;
      functionCall += ", mNativeHandle);";
      
      AddLine(functionCall);

      RemoveScope();
    }

    if (nullptr != aProperty->GetSetter())
    {
      AddLine("set");
      AddScope();

      std::string vTableClassName{ aTypeName };
      vTableClassName += "_vtable";
      vTableClassName += '.';

      std::string functionCall{ "YTEInternalCall.setCall_internal(" };
      functionCall += vTableGetterName;
      functionCall += ", mNativeHandle, ";

      if (nullptr != redirect)
      {
        functionCall += "value);";
      }
      else
      {
        functionCall += "value.mNativeHandle);";
      }

      AddLine(functionCall);

      RemoveScope();
    }

    RemoveScope();
  }

  void CSharpCodeGenerator::AddUnsafe()
  {
    AddLine("unsafe");
    AddScope();
  }

  void CSharpCodeGenerator::AddScope()
  {
    AddLine("{");
    AddIndent();
  }

  void CSharpCodeGenerator::RemoveScope()
  {
    RemoveIndent();
    AddLine("}");
  }

  void CSharpCodeGenerator::AddLine(std::string_view aLine, bool aComment)
  {
    // First line:
    std::string_view line = aLine.substr(0, aLine.find_first_of("\n"));

    // Everything after the first line:
    if ((aLine.data() + aLine.size()) != (line.data() + line.size()))
    {
      aLine = aLine.substr(line.size() + 1);
    }
    else
    {
      aLine = aLine.substr(line.size());
    }

    do 
    {
      mText += GetIndent();

      if (aComment)
      {
        mText += "// ";
      }

      mText += line;
      mText += "\n";
      
      // Next line:
      line = aLine.substr(0, aLine.find_first_of("\n"));

      // Everything after the next line:
      if ((aLine.data() + aLine.size()) != (line.data() + line.size()))
      {
        aLine = aLine.substr(line.size() + 1);
      }
      else
      {
        aLine = aLine.substr(line.size());
      }
    } while (0 != line.size() || 0 != aLine.size());
  }
}