#include "YTE/Utilities/String/String.hpp"

namespace YTE
{
  ///////////////////////////////////////
  // StringNode
  ///////////////////////////////////////
  String::StringNode String::cEmptyNode = { 1, 0, 0, {0} }; 

  void String::StringNode::AddReference()
  {
    ++mReferenceCount;
  }


  void String::StringNode::ReleaseReference()
  {
    if (--mReferenceCount == 0)
    {
      delete[] this;
    }
  }

  ///////////////////////////////////////
  // String
  ///////////////////////////////////////
  String::String(const char *aString)
  {
    size_t size = std::strlen(aString);

    Assign(AllocateNode(aString, size));
  }

  String::String(const char *aString, size_t aSize)
  {
    Assign(AllocateNode(aString, aSize));
  }

  String::String(const std::string &aString)
  {
    Assign(AllocateNode(aString.c_str(), aString.size()));
  }

  String::String(const String &aString)
  {
    Assign(aString.mNode);
  }

  String::String(String &&aString)
  {
    mNode = aString.mNode;

    aString.mNode = &cEmptyNode;
  }

  String::String(StringNode *aStringNode)
  {
    Assign(aStringNode);
  }

  String::String()
  {
    mNode = &cEmptyNode;
  }


  bool String::operator==(StringRef aString) const
  {
    if (mNode == aString.mNode)
    {
      return true;
    }

    if (mNode->mSize != aString.mNode->mSize)
    {
      return false;
    }

    if (mNode->mHash != aString.mNode->mHash)
    {
      return false;
    }

    return strcmp(mNode->mData, aString.mNode->mData) == 0;
  }

  bool String::operator!=(StringRef aString) const
  {
    return !(*this == aString);
  }

  char String::operator[](size_t aIndex)
  {
    return *(mNode->mData + aIndex);
  }

  String& String::operator=(const String &aString)
  {
    if (mNode != aString.mNode)
    {
      //ReleaseReference();
      Assign(aString.mNode);
    }

    return *this;
  }

  bool String::operator<(const char *aString) const
  {
    auto comparison = std::strcmp(c_str(), aString);
    return comparison < 0;
  }

  bool String::operator<(const std::string &aString) const
  {
    auto comparison = std::strcmp(c_str(), aString.c_str());
    return comparison < 0;
  }

  bool String::operator<(const String &aString) const
  {
    auto comparison = std::strcmp(c_str(), aString.c_str());
    return comparison < 0;
  }

  bool String::operator>(const char *aString) const
  {
    auto comparison = std::strcmp(c_str(), aString);
    return comparison > 0;
  }

  bool String::operator>(const std::string &aString) const
  {
    auto comparison = std::strcmp(c_str(), aString.c_str());
    return comparison > 0;
  }

  bool String::operator>(const String &aString) const
  {
    auto comparison = std::strcmp(c_str(), aString.c_str());
    return comparison > 0;
  }

  bool String::operator==(const char *aString) const
  {
    auto comparison = std::strcmp(c_str(), aString);
    return comparison == 0;
  }

  bool String::operator==(const std::string &aString) const
  {
    auto comparison = std::strcmp(c_str(), aString.c_str());
    return comparison == 0;
  }

  // Output operator for printing lists (<<)
  std::ostream & operator<<(std::ostream & aStream, const String &aString)
  {
    aStream << aString.c_str();
    return aStream;
  }


  // Modified HashString from Zero
  size_t String::HashString(const char* aString, size_t aLength)
  {
    size_t hash = aLength;

    // If the string is too long, don't hash all its characters.
    size_t step = (aLength >> 5) + 1;
    size_t i;

    // Compute hash 
    for (i = aLength; i >= step; i -= step)
    {
      hash = hash ^ ((hash << 5) + (hash >> 2) + (char)aString[i - 1]);
    }

    return hash;
  }

  String::StringNode* String::AllocateNode(const char *aString, size_t aSize)
  {
    size_t sizeOfNode = sizeof(StringNode) + aSize;
    StringNode *node = reinterpret_cast<StringNode*>(new char[sizeOfNode]);
    std::memcpy(node->mData, aString, aSize);
    node->mData[aSize] = NULL;

    node->mSize = aSize;

    node->mHash = HashString(aString, aSize);

    node->mReferenceCount = 0;

    return node;
  }

  void String::Assign(StringNode *aNode)
  {
    mNode = aNode;
    AddReference();
  }


  ///////////////////////////////////////
  // Operator Overloads
  ///////////////////////////////////////
  bool operator <(const char *lfs, const YTE::String &rhs)
  {
    auto comparison = std::strcmp(lfs, rhs.c_str());
    return comparison < 0;
  }

  bool operator <(const std::string &lfs, const YTE::String &rhs)
  {
    auto comparison = std::strcmp(lfs.c_str(), rhs.c_str());
    return comparison < 0;
  }
}