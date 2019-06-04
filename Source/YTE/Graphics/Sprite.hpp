#pragma once

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/BaseModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class Sprite : public BaseModel
  {
  public:
    YTEDeclareType(Sprite);
    YTE_Shared Sprite(Composition *aOwner, Space *aSpace);
    YTE_Shared ~Sprite();

    YTE_Shared void AssetInitialize() override;
    YTE_Shared void Initialize() override;
    YTE_Shared void CreateSprite();

    YTE_Shared void Update(LogicUpdate *aUpdate);

    std::string GetTexture()
    {
      return mTextureName;
    }

    void SetTexture(std::string &aTexture)
    {
      if (aTexture != mTextureName &&
          0 != aTexture.size())
      {
        mTextureName = aTexture;

        if (mConstructing == false)
        {
          CreateSprite();
        }
      }
    }

    YTE_Shared void CreateTransform();

    YTE_Shared void TransformUpdate(TransformChanged *aEvent);

    std::vector<InstantiatedModel*> GetInstantiatedModel() override
    {
      std::vector<InstantiatedModel*> toReturn;

      if (mInstantiatedSprite)
      {
          toReturn.emplace_back(mInstantiatedSprite.get());
      }
      
      return toReturn;
    }

    void SetColumns(i32 aColumns)
    {
      if (aColumns > 0)
      {
        mColumns = aColumns;
      }
    }

    i32 GetColumns()
    {
      return mColumns;
    }

    void SetRows(i32 aRows)
    {
      if (aRows > 0)
      {
        mRows = aRows;
      }
    }

    i32 GetRows()
    {
      return mRows;
    }

    void SetFrames(i32 aFrames)
    {
      mFrames = aFrames;
    }

    i32 GetFrames()
    {
      return mFrames;
    }

    bool GetAnimating()
    {
      return mAnimating;
    }

    void SetSpeed(float aSpeed)
    {
      mSpeed = aSpeed;
    }

    float GetSpeed()
    {
      return mSpeed;
    }


    YTE_Shared void SetAnimating(bool aAnimating);

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOs::Model mUBOModel;

    std::string mTextureName;
    std::unique_ptr<InstantiatedModel> mInstantiatedSprite;
    VertexData mVertexData;
    bool mConstructing;

    bool mAnimating;
    i32 mColumns;
    i32 mRows;
    i32 mFrames;
    float mSpeed;
    double mTimeAccumulated;
    size_t mCurrentIndex;
  };
}
