#pragma once

#include "YTE/Graphics/Generics/Mesh.hpp"

#include "YTETools/YTEToolsMeta.hpp"

namespace YTE::Tools
{
  YTETools_Shared void WriteMeshToFile(std::string const& aName, YTE::Mesh const& aMesh);
  YTETools_Shared void ImportMesh(YTE::Mesh& aMesh, const std::string &aFile);
  //YTETools_Shared void ImportSubMesh(YTE::Mesh* aYTEMesh, 
  //                                   YTE::Submesh& aSubmesh, 
  //                                   const aiScene* aScene, 
  //                                   const aiMesh* aMesh, 
  //                                   YTE::Skeleton* aSkeleton, 
  //                                   uint32_t aBoneStartingVertexOffset);
}