#pragma once

#include <cassert>
#include <fstream>
#include <string>
#include <vector>

#include "YTE/Platform/TargetDefinitions.hpp"

namespace YTE
{

  struct FileWriter
  {
    template<typename tType>
    static constexpr size_t GetSize()
    {
      return sizeof(std::aligned_storage_t<sizeof(tType), alignof(tType)>);
    }

    YTE_Shared FileWriter(std::string const& aFile);
    YTE_Shared ~FileWriter();

    template <size_t aSize>
    void MemoryCopy(char const* aSource, size_t aNumber = 1)
    {
      size_t bytesToCopy = aSize * aNumber;
      mData.reserve(mData.size() + bytesToCopy);

      for (size_t i = 0; i < bytesToCopy; ++i)
      {
        mData.emplace_back(*(aSource++));
      }
    }

    template <typename tType>
    char const* SourceCast(tType const* aSource)
    {
      return reinterpret_cast<char const*>(aSource);
    }

    template<typename tType>
    void Write(tType const& aValue)
    {
      MemoryCopy<GetSize<tType>()>(SourceCast(&aValue));
    }

    template<typename tType>
    void Write(tType const* aValue)
    {
      MemoryCopy<GetSize<tType>()>(SourceCast(aValue));
    }

    template<typename tType>
    void Write(tType const* aValue, size_t aSize)
    {
      MemoryCopy<GetSize<tType>()>(SourceCast(aValue), aSize);
    }

    std::ofstream mFile;
    std::vector<char> mData;
    bool mOpened = false;
  };
  
  struct FileReader
  {
    YTE_Shared FileReader(std::string const& aFile);

    template<typename tType>
    static constexpr size_t GetSize()
    {
      return sizeof(std::aligned_storage_t<sizeof(tType), alignof(tType)>);
    }

    template<typename tType>
    tType& Read()
    {
      auto bytesToRead = GetSize<tType>();

      assert((mBytesRead + bytesToRead) <= mData.size());

      auto &value = *reinterpret_cast<tType*>(mData.data() + mBytesRead);

      mBytesRead += bytesToRead;

      return value;
    }
    
    template<typename tType>
    void Read(tType* aBuffer, size_t aSize)
    {
      if (aSize == 0)
      {
        return;
      }

      auto bytesToRead = GetSize<tType>() * aSize;
      assert((mBytesRead + bytesToRead) <= mData.size());

      memcpy(aBuffer, mData.data() + mBytesRead, bytesToRead);
    
      mBytesRead += bytesToRead;
    }

    std::vector<char> mData;
    size_t mBytesRead = 0;
    bool mOpened = false;
  };
}
