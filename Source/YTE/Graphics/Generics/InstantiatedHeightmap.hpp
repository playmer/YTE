///////////////////
// Author: Andrew Griffin
// YTE - Graphics - Generics
///////////////////

#pragma once

#ifndef YTE_Graphics_Generics_InstantiatedHeightmap_hpp
#define YTE_Graphics_Generics_InstantiatedHeightmap_hpp

#include "YTE/Core/EventHandler.hpp"

#include "YTE/Graphics/Generics/ForwardDeclarations.hpp"
#include "YTE/Graphics/Vertex.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Utilities/Utilities.hpp"

namespace YTE
{
  class InstantiatedHeightmap : public EventHandler
  {
  public:
    YTEDeclareType(InstantiatedHeightmap);

    InstantiatedHeightmap();

    ~InstantiatedHeightmap();

    void Initialize(std::string& aModelName,
                    std::vector<Vertex>& aVertices,
                    std::vector<u32>& aIndices,
                    std::string& aShaderSetName,
                    GraphicsView* aView,
                    Renderer* aRenderer);

    void Initialize(std::string& aModelName,
                    std::vector<Vertex>& aVertices,
                    std::vector<u32>& aIndices,
                    std::string& aShaderSetName,
                    GraphicsView* aView,
                    Renderer* aRenderer,
                    std::string aTextureName,
                    std::string aSpecularTextureName,
                    std::string aNormalTextureName);

    void UpdateMesh(std::vector<Vertex>& aVertices);
    void UpdateMesh(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices);

    InstantiatedModel* GetInstantiatedModel()
    {
      return mModel.get();
    }

  private:
    void CreateMesh(std::vector<Vertex>& aVertices, std::vector<u32>& aIndices, std::string& aModelName);

    std::string mShaderSetName;
    std::unique_ptr<InstantiatedModel> mModel;
    Renderer *mRenderer;
    GraphicsView *mGraphicsView;
    std::string mDiffuseTName;
    std::string mSpecularTName;
    std::string mNormalTName;
  };
}


#endif
