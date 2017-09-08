/******************************************************************************/
/*!
\author Joshua T. Fisher
All content (c) 2016 DigiPen  (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Core_ContentSystem_h
#define YTE_Core_ContentSystem_h

#include <shared_mutex>

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Core/Utilities.hpp"

namespace YTE
{
  template<typename Type>
  class Resource : EventHandler
  {
  public:
    SharedPointer<Type> operator *()  { return mData; }
    SharedPointer<Type> operator ->() { return mData; }
  private:
    SharedPointer<Type> mData;
  };


  template<typename Key, typename ResourceType>
  class ContentLoader
  {
  public:
    virtual ~ContentLoader() {};

    virtual Resource<ResourceType> LoadResource(Key &aKey) = 0;

    template <typename KeyPossibleType>
    Resource<ResourceType> GetResource(const KeyPossibleType &aKey)
    {
      std::shared_lock<std::shared_timed_mutex> lock{ mResourcesMutex };
      auto resource = mResources.Find(aKey);

      return *resource;
    }

    template <typename KeyPossibleType>
    void RefreshResource(const KeyPossibleType &aKey)
    {
      std::unique_lock<std::shared_timed_mutex> lock{ mResourcesMutex };
      mResources[aKey] = LoadResource(aKey);
    }

    template <typename KeyPossibleType>
    void EraseResource(const KeyPossibleType &aKey)
    {
      std::unique_lock<std::shared_timed_mutex> lock{ mResourcesMutex };

      mResources.Erase(aKey);
    }

  protected:
    OrderedMap<Key, Resource<ResourceType>> mResources;
    std::shared_timed_mutex mResourcesMutex;
  };
}

#endif
