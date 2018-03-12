#include "ModifyBoatMesh.hpp"
#include "YTE/GameComponents/Helpers/BoatPhysicsMath.hpp"
#include "YTE/GameComponents/Helpers/ModifyBoatMesh.hpp"
#include "YTE/GameComponents/Helpers/SlammingForceData.hpp"
#include "YTE/GameComponents/Helpers/TriangleData.hpp"

#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Model.hpp"

#include "YTE/Physics/RigidBody.hpp"

#include <cmath>

namespace YTE
{

  ModifyBoatMesh::ModifyBoatMesh(Composition *aComposition)
  {
    boatTrans = aComposition->GetComponent<Transform>();
    boatModel = aComposition->GetComponent<Model>();
    boatRB = aComposition->GetComponent<RigidBody>();
    auto mesh = boatModel->GetMesh();
    auto submeshes = mesh->GetSubmeshes();
    
    for (auto& submesh : submeshes)
    {
      size_t triCount = submesh.GetTriangleCount();
      for (size_t i = 0; i < triCount; ++i) 
      {
        auto tri = submesh.GetTriangle(i);
        tri += glm::uvec3((glm::uint)boatVertices.size());
        boatTriangles.push_back(submesh.GetTriangle(i));
      }

      for (auto& vert : submesh.mVertexBuffer)
      {
        boatVertices.push_back(vert.mPosition);
      }
    }

    boatVerticesGlobal.resize(boatVertices.size());
    allDistancesToWater.resize(boatVertices.size());

    slammingForceData.resize(boatTriangles.size());

    CalculateOriginalTrianglesArea();
  }

  void ModifyBoatMesh::GenerateUnderwaterMesh(FFT_WaterSimulation* aSim, float dt)
{
    abovewaterTriangleData.clear();
    underwaterTriangleData.clear();
    auto ubo = boatModel->GetInstantiatedModel()[0]->GetUBOModelData();
    auto m2w = ubo.mModelMatrix;

    for (auto& data : slammingForceData)
    {
      data.previousSubmergedArea = data.submergedArea;
    }
    indexOfOriginalTriangle.clear();
    timeSinceStart += dt;

    // if you try to use boat physics on something without verts, you can fix this. BAKA
    glm::vec3 max = boatVertices[0];
    glm::vec3 min = boatVertices[0];
    for (int i = 0; i < boatVertices.size(); ++i)
    {
      glm::vec4 gPos = m2w * glm::vec4(boatVertices[i], 1);
      boatVerticesGlobal[i] = glm::vec3(gPos.x, gPos.y, gPos.z);
      for (unsigned j = 0; j < 3; ++j)
      {
        max[j] = std::max(gPos[j], max[j]);
        min[j] = std::max(gPos[j], min[j]);
      }

      auto height = aSim->GetHeight(gPos.x, gPos.z);
      allDistancesToWater[i] = gPos.y - height;
    }
    underwaterLength = glm::distance(max, min);

    AddTriangles(aSim);
  }

  std::vector<TriangleData>& ModifyBoatMesh::UnderwaterTriangleData()
  {
    return underwaterTriangleData;
  }

  std::vector<TriangleData>& ModifyBoatMesh::AbovewaterTriangleData()
  {
    return abovewaterTriangleData;
  }

  std::vector<YTE::SlammingForceData>& ModifyBoatMesh::SlammingData()
  {
    return slammingForceData;
  }

  std::vector<int>& ModifyBoatMesh::IndexOfOrigTris()
  {
    return indexOfOriginalTriangle;
  }

  float ModifyBoatMesh::BoatArea()
  {
    return boatArea;
  }

  float ModifyBoatMesh::CalculateUnderwaterLength()
  {
    return underwaterLength;
  }

  void ModifyBoatMesh::CalculateOriginalTrianglesArea()
  {
    int triangleCounter = 0;
    for (auto& tri : boatTriangles)
    {
      glm::vec3 p1 = boatVertices[tri[0]];
      glm::vec3 p2 = boatVertices[tri[1]];
      glm::vec3 p3 = boatVertices[tri[2]];

      //Calculate the area of the triangle
      float triangleArea = BoatPhysicsMath::GetTriangleArea(p1, p2, p3);

      //Store the area in a list
      slammingForceData[triangleCounter].originalArea = triangleArea;
      slammingForceData[triangleCounter].triangleCenter = (p1 + p2 + p3) / 3.0f;

      //The total area
      boatArea += triangleArea;

      triangleCounter += 1;
    }
  }

  void ModifyBoatMesh::AddTriangles(FFT_WaterSimulation* aSim)
{
    std::vector<VertexData> vertexData;

    vertexData.push_back(VertexData());
    vertexData.push_back(VertexData());
    vertexData.push_back(VertexData());

    int triangleCounter = 0;
    for (auto& tri : boatTriangles)
    {
      // get vertex data for the 3 verts of the triangle
      for (int i = 0; i < 3; ++i)
      {
        vertexData[i].distance = allDistancesToWater[tri[i]];
        vertexData[i].index = i;
        vertexData[i].globalVertexPos = boatVerticesGlobal[tri[i]];
      }

      // all verts above the water
      int above = 0;
      for (unsigned i = 0; i < 3; ++i)
      {
        if (vertexData[i].distance > 0.0f)
        {
          ++above;
        }
      }

      if (above == 3)
      {
        glm::vec3 p1 = vertexData[0].globalVertexPos;
        glm::vec3 p2 = vertexData[1].globalVertexPos;
        glm::vec3 p3 = vertexData[2].globalVertexPos;
        abovewaterTriangleData.push_back(TriangleData(p1, p2, p3, boatRB, boatTrans, aSim, timeSinceStart));

        slammingForceData[triangleCounter].submergedArea = 0.0f;
        ++triangleCounter; //TODO(Evan): Make sure this is important. Nick didn't have it but it probs is
        continue;
      }

      if (above == 0)
      {
        glm::vec3 p1 = vertexData[0].globalVertexPos;
        glm::vec3 p2 = vertexData[1].globalVertexPos;
        glm::vec3 p3 = vertexData[2].globalVertexPos;
        underwaterTriangleData.push_back(TriangleData(p1, p2, p3, boatRB, boatTrans, aSim, timeSinceStart));
        slammingForceData[triangleCounter].submergedArea = slammingForceData[triangleCounter].originalArea;
        indexOfOriginalTriangle.push_back(triangleCounter);
      }
      else
      {
        std::sort(vertexData.begin(), vertexData.end(),
          [](const VertexData& x, const VertexData& y)->bool 
          {
            return x.distance > y.distance;
          });

        if (above == 1)
        {
          AddTrianglesOneAboveWater(vertexData, triangleCounter, aSim);
        }
        else
        {
          AddTrianglesTwoAboveWater(vertexData, triangleCounter, aSim);

        }
      }
      ++triangleCounter;
    }
  }

  void ModifyBoatMesh::AddTrianglesOneAboveWater(std::vector<VertexData>& vertexData, int triangleCounter, FFT_WaterSimulation* aSim)
  {
    glm::vec3 H = vertexData[0].globalVertexPos;

    int M_index = vertexData[0].index - 1;
    if (M_index < 0)
    {
      M_index = 2;
    }

    float h_H = vertexData[0].distance;
    float h_M = 0.0f;
    float h_L = 0.0f;

    glm::vec3 M;
    glm::vec3 L;

    if (vertexData[1].index == M_index)
    {
      M = vertexData[1].globalVertexPos;
      L = vertexData[2].globalVertexPos;

      h_M = vertexData[1].distance;
      h_L = vertexData[2].distance;
    }
    else
    {
      M = vertexData[2].globalVertexPos;
      L = vertexData[1].globalVertexPos;

      h_M = vertexData[2].distance;
      h_L = vertexData[1].distance;
    }

    glm::vec3 MH = H - M;
    float t_M = -h_M / (h_H - h_M);
    glm::vec3 MI_M = t_M * MH;
    glm::vec3 I_M = MI_M + M;

    glm::vec3 LH = H - L;
    float t_L = -h_L / (h_H - h_L);
    glm::vec3 LI_L = t_L * LH;
    glm::vec3 I_L = LI_L + L;

    underwaterTriangleData.push_back(TriangleData(M, I_M, I_L, boatRB, boatTrans, aSim, timeSinceStart));
    underwaterTriangleData.push_back(TriangleData(M, I_L, L, boatRB, boatTrans, aSim, timeSinceStart));
    abovewaterTriangleData.push_back(TriangleData(I_M, H, I_L, boatRB, boatTrans, aSim, timeSinceStart));
    float totalArea = BoatPhysicsMath::GetTriangleArea(M, I_M, I_L) + BoatPhysicsMath::GetTriangleArea(M, I_L, L);
    slammingForceData[triangleCounter].submergedArea = totalArea;
    indexOfOriginalTriangle.push_back(triangleCounter);
    indexOfOriginalTriangle.push_back(triangleCounter);
  }

  void ModifyBoatMesh::AddTrianglesTwoAboveWater(std::vector<VertexData>& vertexData, int triangleCounter, FFT_WaterSimulation* aSim)
  {
    glm::vec3 L = vertexData[2].globalVertexPos;

    int H_index = vertexData[2].index + 1;
    if (H_index > 2)
    {
      H_index = 0;
    }

    float h_L = vertexData[2].distance;
    float h_H = 0.0f;
    float h_M = 0.0f;

    glm::vec3 H;
    glm::vec3 M;

    if (vertexData[1].index == H_index)
    {
      H = vertexData[1].globalVertexPos;
      M = vertexData[0].globalVertexPos;

      h_H = vertexData[1].distance;
      h_M = vertexData[0].distance;
    }
    else
    {
      H = vertexData[0].globalVertexPos;
      M = vertexData[1].globalVertexPos;

      h_H = vertexData[0].distance;
      h_M = vertexData[1].distance;
    }

    glm::vec3 LM = M - L;
    float t_M = -h_L / (h_M - h_L);
    glm::vec3 LJ_M = t_M * LM;
    glm::vec3 J_M = LJ_M + L;

    glm::vec3 LH = H - L;
    float t_H = -h_L / (h_H - h_L);
    glm::vec3 LJ_H = t_H * LH;
    glm::vec3 J_H = LJ_H + L;


    underwaterTriangleData.push_back(TriangleData(L, J_H, J_M, boatRB, boatTrans, aSim, timeSinceStart));
    abovewaterTriangleData.push_back(TriangleData(J_H, H, J_M, boatRB, boatTrans, aSim, timeSinceStart));
    abovewaterTriangleData.push_back(TriangleData(J_M, H, M, boatRB, boatTrans, aSim, timeSinceStart));

    //Calculate the submerged area
    slammingForceData[triangleCounter].submergedArea = BoatPhysicsMath::GetTriangleArea(L, J_H, J_M);

    indexOfOriginalTriangle.push_back(triangleCounter);
  }

}



