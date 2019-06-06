#pragma once

#ifndef YTE_CSharpCodeGenerator_hpp
#define YTE_CSharpCodeGenerator_hpp

#include <string_view>
#include <utility>
#include <vector>

#include "YTE/Core/Utilities.hpp" 

namespace YTE
{
  class CSharpCodeGenerator
  {
  public:
    CSharpCodeGenerator();

    void AddEnums(std::string_view aName, 
                  std::vector<std::pair<std::string_view, u64>> aEnums);

    std::string ToString();

    void OpenNamespace(std::string_view aNamespace);
    void CloseNamespace();


    non_member std::string GenerateVTableName(Function *aFunction);
    non_member std::string GenerateVTableName(Property *aFunction, bool aGetter);
    non_member std::string GenerateVTableName(Field *aFunction, bool aGetter);

    non_member std::string ParseTypeName(Type *aType,
                                         bool aPeriodsToUnderscores = false,
                                         bool aIgnoreYTE = true);

    void AddType(Type *aType);
    void AddFunction(const std::string &aTypeName, Type *aType, Function *aFunction);


    void AddPropertyOrField(const std::string &aTypeName, Type *aType, Property *aProperty);
    void AddUnsafeField(const std::string &aTypeName, Type *aType, Field *aProperty);

    void AddClass(std::string_view aLine, Type *aBaseType, bool aIgnoreYTE = true);
    void AddUnsafe();

    void AddScope();
    void RemoveScope();

    // Preliminary gets places after the indent, but before the line provided.
    void AddLine(std::string_view aLine, bool aComment = false);

  private:
    void AddConstructor(std::string_view aName, 
                        std::pair<std::string, std::vector<std::string>> &VtableNameAndItems, 
                        Type *aType);
    std::pair<std::string, std::vector<std::string>> AddVTable(std::string_view aName, Type *aType);

    std::string_view GetIndent();
    void AddIndent();
    void RemoveIndent();

    const size_t mIndentSize;
    size_t mNestedNamespaces;
    std::vector<char> mIndent;
    std::string mText;
  };
}

#endif