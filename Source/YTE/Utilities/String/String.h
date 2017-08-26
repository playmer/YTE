#pragma once
#ifndef YTE_Utilities_String_String_H
#define YTE_Utilities_String_String_H

#include <string>
#include <iostream> // ostream
#include <atomic>

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
			inline void AddReference()
			{
        ++mReferenceCount;
			}

			inline void ReleaseReference()
			{
        if (--mReferenceCount == 0)
        {
          delete[] this;
        }
			}

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
    inline String(const char *aString)
    {
      size_t size = std::strlen(aString);

      Assign(AllocateNode(aString, size));
    }

    inline String(const char *aString, size_t aSize)
    {
      Assign(AllocateNode(aString, aSize));
    }

    inline String(const std::string &aString)
    {
      Assign(AllocateNode(aString.c_str(), aString.size()));
    }

    inline String(const String &aString)
    {
      Assign(aString.mNode);
    }

    inline String(String &&aString)
    {
      mNode = aString.mNode;

      aString.mNode = &cEmptyNode;
    }

    inline String(StringNode *aStringNode)
    {
      Assign(aStringNode);
    }

    inline String()
    {
      mNode = &cEmptyNode;
    }



    //////////////////////////////
    // Overloads
    //////////////////////////////
    inline bool operator==(StringRef aString) const
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

    inline bool operator!=(StringRef aString) const
    {
      return !(*this == aString);
    }

    inline char operator[](size_t aIndex)
    {
      return *(mNode->mData + aIndex);
    }

    inline String& operator=(const String &aString)
    {
      if (mNode != aString.mNode)
      {
        //ReleaseReference();
        Assign(aString.mNode);
      }

      return *this;
    }

    inline bool operator<(const char *aString) const
    {
      auto comparison = std::strcmp(c_str(), aString);
      return comparison < 0;
    }

    inline bool operator<(const std::string &aString) const
    {
      auto comparison = std::strcmp(c_str(), aString.c_str());
      return comparison < 0;
    }

    inline bool operator<(const String &aString) const
    {
      auto comparison = std::strcmp(c_str(), aString.c_str());
      return comparison < 0;
    }

    inline bool operator>(const char *aString) const
    {
      auto comparison = std::strcmp(c_str(), aString);
      return comparison > 0;
    }

    inline bool operator>(const std::string &aString) const
    {
      auto comparison = std::strcmp(c_str(), aString.c_str());
      return comparison > 0;
    }

    inline bool operator>(const String &aString) const
    {
      auto comparison = std::strcmp(c_str(), aString.c_str());
      return comparison > 0;
    }

    inline bool operator==(const char *aString) const
    {
      auto comparison = std::strcmp(c_str(), aString);
      return comparison == 0;
    }

    inline bool operator==(const std::string &aString) const
    {
      auto comparison = std::strcmp(c_str(), aString.c_str());
      return comparison == 0;
    }

    //inline bool operator==(const String &aString) const
    //{
    //  auto comparison = std::strcmp(c_str(), aString.c_str());
    //  return comparison == 0;
    //}

    // Output operator for printing lists (<<)
    inline friend std::ostream & operator<<(std::ostream & aStream, const String &aString)
    {
      aStream << aString.c_str();
      return aStream;
    }

    //////////////////////////////
    // Helpers
    //////////////////////////////
      // It costs nothing to provide both.
    inline const char* c_str() const { return mNode->mData; }
    inline const char* Data() const { return mNode->mData; }
    inline size_t Hash() const {
      return mNode->mHash; 
    }
    inline size_t Size() const { return mNode->mSize; }
    inline bool Empty() const { return mNode->mSize == 0; }
    inline StringNode* GetNode() const { return mNode; }


      // Modified HashString from Zero
    inline static size_t HashString(const char* aString, size_t aLength)
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

    inline static StringNode* AllocateNode(const char *aString, size_t aSize)
    {
      size_t sizeOfNode = sizeof(StringNode)+aSize;
      StringNode *node = reinterpret_cast<StringNode*>(new char[sizeOfNode]);
      std::memcpy(node->mData, aString, aSize);
      node->mData[aSize] = NULL;

      node->mSize = aSize;

      node->mHash = HashString(aString, aSize);

      node->mReferenceCount = 0;

      return node;
    }

    inline void Assign(StringNode *aNode)
    {
      mNode = aNode;
      AddReference();
    }

    friend bool operator <(const char *lfs, const YTE::String &rhs);
    friend bool operator <(const std::string &lfs, const YTE::String &rhs);

  private:
    ///////////////////////////////////////
    // Functions
    ///////////////////////////////////////

    //////////////////////////////
    // Helpers
    //////////////////////////////
    inline void AddReference()
    {
      mNode->AddReference();
    }

    inline void ReleaseReference()
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
  struct hash<YTE::String> : public unary_function<YTE::String, size_t>
  {
    size_t operator()(const YTE::String& value) const
    {
      return value.Hash();
    }
  };

  template <>
  struct equal_to<YTE::String> : public unary_function<YTE::String, bool>
  {
    bool operator()(const YTE::String& x, const YTE::String& y) const
    {
      return x == y;
    }
  };

} // namespace std

#endif