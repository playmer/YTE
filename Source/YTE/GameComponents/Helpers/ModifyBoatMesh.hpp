#pragma once

#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Physics/ForwardDeclarations.hpp"
#include "YTE/GameComponents/Helpers/SlammingForceData.hpp"
#include "YTE/GameComponents/Helpers/TriangleData.hpp"
#include "YTE/Core/Utilities.hpp"
#include "YTE/Graphics/Drawers.hpp"

namespace YTE
{
  class ModifyBoatMesh
  {
  public:
    class VertexData
    {
    public:
      float distance;
      int index;
      glm::vec3 globalVertexPos;
    };

    ModifyBoatMesh() = default;
    ModifyBoatMesh(Composition *aComposition);
    void GenerateUnderwaterMesh(FFT_WaterSimulation* aSim, float dt);
    std::vector<TriangleData>& UnderwaterTriangleData();
    std::vector<TriangleData>& AbovewaterTriangleData();
    std::vector<SlammingForceData>& SlammingData();
    std::vector<int>& IndexOfOrigTris();
    float BoatArea();
    float CalculateUnderwaterLength();
    void CalculateOriginalTrianglesArea();

  private:
    void AddTriangles(FFT_WaterSimulation* aSim);
    void AddTrianglesOneAboveWater(std::vector<VertexData>& vertexData, int triangleCounter, FFT_WaterSimulation* aSim);
    void AddTrianglesTwoAboveWater(std::vector<VertexData>& vertexData, int triangleCounter, FFT_WaterSimulation* aSim);

    Transform * boatTrans;
    Model * boatModel;
    RigidBody * boatRB;
    std::vector<glm::vec3> boatVertices;
    std::vector<glm::uvec3> boatTriangles;
    std::vector<glm::vec3> boatVerticesGlobal;
    std::vector<float> allDistancesToWater;
    std::vector<TriangleData> underwaterTriangleData;
    std::vector<TriangleData> abovewaterTriangleData;
    std::vector<SlammingForceData> slammingForceData;
    std::vector<int> indexOfOriginalTriangle;
    float boatArea;
    float timeSinceStart;
    float underwaterLength;

    LineDrawer *lineDrawer;
  };
}
