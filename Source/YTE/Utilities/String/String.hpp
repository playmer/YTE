#pragma once
#ifndef YTE_Utilities_String_String_H
#define YTE_Utilities_String_String_H

#include <string>
#include <iostream> // ostream
#include <atomic>
#include <cstddef>
#include <cstring>

namespace YTE
{
  class String;

  using StringRef = const String&;

  class String
	{
  public:
		struct StringNode
		{
      ///////////////////////////////////////
      // Functions
      ///////////////////////////////////////
      //////////////////////////////
      // Helpers
      //////////////////////////////
      void AddReference();
      void ReleaseReference();

      ///////////////////////////////////////
      // Fields
      ///////////////////////////////////////
      std::atomic<size_t> mReferenceCount;
      size_t mSize;
      size_t mHash;
      char mData[1];

		};

    ///////////////////////////////////////
    // Functions
    ///////////////////////////////////////
    //////////////////////////////
    // Constructors
    //////////////////////////////
    String(const char *aString);
    String(const char *aString, size_t aSize);
    String(const std::string &aString);
    String(const String &aString);
    String(String &&aString);
    String(StringNode *aStringNode);
    String();

    //////////////////////////////
    // Overloads
    //////////////////////////////
    bool operator==(StringRef aString) const;
    bool operator!=(StringRef aString) const;
    char operator[](size_t aIndex);
    String& operator=(const String &aString);
    bool operator<(const char *aString) const;
    bool operator<(const std::string &aString) const;
    bool operator<(const String &aString) const;
    bool operator>(const char *aString) const;
    bool operator>(const std::string &aString) const;
    bool operator>(const String &aString) const;
    bool operator==(const char *aString) const;
    bool operator==(const std::string &aString) const;

    friend std::ostream & operator<<(std::ostream & aStream, const String &aString);

    //////////////////////////////
    // Helpers
    //////////////////////////////
      // It costs nothing to provide both.
    const char* c_str() const { return mNode->mData; }
    const char* Data() const { return mNode->mData; }
    size_t Hash() const {
      return mNode->mHash; 
    }
    size_t Size() const { return mNode->mSize; }
    bool Empty() const { return mNode->mSize == 0; }
    StringNode* GetNode() const { return mNode; }


      // Modified HashString from Zero
    static size_t HashString(const char* aString, size_t aLength);
    static StringNode* AllocateNode(const char *aString, size_t aSize);
    void Assign(StringNode *aNode);

    friend bool operator <(const char *lfs, const YTE::String &rhs);
    friend bool operator <(const std::string &lfs, const YTE::String &rhs);

  private:
    ///////////////////////////////////////
    // Functions
    ///////////////////////////////////////

    //////////////////////////////
    // Helpers
    //////////////////////////////
    void AddReference()
    {
      mNode->AddReference();
    }

    void ReleaseReference()
    {
      if (mNode)
      {
        mNode->ReleaseReference();
      }
    }


    ///////////////////////////////////////
    // Fields
    ///////////////////////////////////////
    StringNode *mNode;


    ///////////////////////////////////////
    // Constants
    ///////////////////////////////////////
    static StringNode cEmptyNode;
	};
}

namespace std
{
  template <>
  struct hash<YTE::String>
  {
    using argument_type = YTE::String;
    using result_type = size_t;
    
    size_t operator()(const YTE::String& value) const
    {
      return value.Hash();
    }
  };

  template <>
  struct equal_to<YTE::String>
  {
    using argument_type = YTE::String;
    using result_type = bool;

    bool operator()(const YTE::String& x, const YTE::String& y) const
    {
      return x == y;
    }
  };

} // namespace std

#endif