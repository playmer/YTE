#include "YTE/Core/Engine.hpp"
#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsSystem.hpp"
#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Drawers.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

namespace YTE
{

  YTEDefineType(LineDrawer)
  {
    YTERegisterType(LineDrawer);
  }

  LineDrawer::LineDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mRenderer(aRenderer)
    , mView(aView)
    , mMeshName(aUniqueName)
  {
    mMeshName = aUniqueName;
    mSubmeshes.emplace_back(Submesh{});
    mSubmeshes[0].mShaderSetName = "Line";

    ShaderUsage useVert(true, false);
    ShaderUsage useFrag(false, true);
    mSubmeshes[0].mUBOs.clear();
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::View, useVert, sizeof(UBOView));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Animation, useVert, sizeof(UBOAnimation));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::ModelMaterial, useFrag, sizeof(UBOMaterial));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::SubmeshMaterial, useFrag, sizeof(UBOMaterial));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Lights, useFrag, sizeof(UBOLightMan));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Illumination, useFrag, sizeof(UBOIllumination));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Model, useVert, sizeof(UBOModel));
  }

  LineDrawer::~LineDrawer()
  {

  }

  // This will clear the previous version of the Lines.
  void LineDrawer::Start()
  {
    mInstantiatedLines.reset();
    auto &submesh = mSubmeshes[0];

    submesh.mVertexBufferSize = 0;
    submesh.mIndexBufferSize = 0;
    submesh.mVertexBuffer.clear();
    submesh.mIndexBuffer.clear();
  }

  void LineDrawer::AddLine(glm::vec4 aPoint1, glm::vec4 aPoint2)
  {
    AddLine(glm::vec3{ aPoint1.x, aPoint1.y, aPoint1.z },
            glm::vec3{ aPoint2.x, aPoint2.y, aPoint2.z },
            mColor);
  }

  void LineDrawer::AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2)
  {
    AddLine(aPoint1,
            aPoint2,
            mColor);
  }

  void LineDrawer::AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2)
  {
    AddLine(glm::vec3{ aPoint1, 0.0f },
            glm::vec3{ aPoint2, 0.0f },
            mColor);
  }

  void LineDrawer::AddLine(glm::vec4 aPoint1, glm::vec4 aPoint2, glm::vec3 aColor)
  {
    AddLine(glm::vec3{ aPoint1.x, aPoint1.y, aPoint1.z },
            glm::vec3{ aPoint2.x, aPoint2.y, aPoint2.z },
            aColor);
  }

  void LineDrawer::AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2, glm::vec3 aColor)
  {
    AddLine(glm::vec3{ aPoint1, 0.0f },
            glm::vec3{ aPoint2, 0.0f },
            aColor);
  }

  void LineDrawer::AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aColor)
  {
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mColor = aColor;
    vert.mPosition = aPoint1;
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mPosition = aPoint2;
    submesh.mVertexBuffer.emplace_back(vert);
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }


  void LineDrawer::AddLine(glm::vec3 aPoint1, 
                           glm::vec3 aPoint2, 
                           glm::vec3 aFromColor, 
                           glm::vec3 aToColor)
  {
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mColor = aFromColor;
    vert.mPosition = aPoint1;
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mColor = aToColor;
    vert.mPosition = aPoint2;
    submesh.mVertexBuffer.emplace_back(vert);
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }

  // This will set the lines given since the last Start() call to be drawn.
  void LineDrawer::End()
  {
    mSubmeshes[0].mVertexBufferSize = mSubmeshes[0].mVertexBuffer.size() * sizeof(Vertex);
    mSubmeshes[0].mIndexBufferSize = mSubmeshes[0].mIndexBuffer.size() * sizeof(u32);

    if (0 == mSubmeshes[0].mVertexBufferSize)
    {
      return;
    }

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmeshes, true);

    mInstantiatedLines = mRenderer->CreateModel(mView, mesh);
    mInstantiatedLines->mType = ShaderType::Lines;
    auto data = mInstantiatedLines->GetUBOModelData();
    mInstantiatedLines->UpdateUBOModel(data);
    mInstantiatedLines->mLineWidth = 5.0f;
  }



  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Triangle Drawer
  /////////////////////////////////////////////////////////////////////////////////////////////////
  YTEDefineType(TriangleDrawer)
  {
    YTERegisterType(TriangleDrawer);
  }

  TriangleDrawer::TriangleDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mRenderer(aRenderer)
    , mView(aView)
    , mMeshName(aUniqueName)
  {
    mMeshName = aUniqueName;
    mSubmeshes.emplace_back(Submesh{});
    mSubmeshes[0].mShaderSetName = "Line";

    ShaderUsage useVert(true, false);
    ShaderUsage useFrag(false, true);
    mSubmeshes[0].mUBOs.clear();
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::View, useVert, sizeof(UBOView));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Animation, useVert, sizeof(UBOAnimation));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::ModelMaterial, useFrag, sizeof(UBOMaterial));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::SubmeshMaterial, useFrag, sizeof(UBOMaterial));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Lights, useFrag, sizeof(UBOLightMan));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Illumination, useFrag, sizeof(UBOIllumination));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Model, useVert, sizeof(UBOModel));
  }

  TriangleDrawer::~TriangleDrawer()
  {

  }

  // This will clear the previous version of the Lines.
  void TriangleDrawer::Start()
  {
    mInstantiatedLines.reset();
    auto &submesh = mSubmeshes[0];

    submesh.mVertexBufferSize = 0;
    submesh.mIndexBufferSize = 0;
    submesh.mVertexBuffer.clear();
    submesh.mIndexBuffer.clear();
  }

  void TriangleDrawer::AddTriangle(glm::vec4 aPoint1, 
                                   glm::vec4 aPoint2, 
                                   glm::vec4 aPoint3)
  {
    AddTriangle(glm::vec3{ aPoint1.x, aPoint1.y, aPoint1.z },
                glm::vec3{ aPoint2.x, aPoint2.y, aPoint2.z },
                glm::vec3{ aPoint3.x, aPoint3.y, aPoint3.z },
                mColor);
  }

  void TriangleDrawer::AddTriangle(glm::vec3 aPoint1, 
                                   glm::vec3 aPoint2, 
                                   glm::vec3 aPoint3)
  {
    AddTriangle(aPoint1,
                aPoint2,
                aPoint3,
                mColor);
  }

  void TriangleDrawer::AddTriangle(glm::vec2 aPoint1, 
                                   glm::vec2 aPoint2, 
                                   glm::vec2 aPoint3)
  {
    AddTriangle(glm::vec3{ aPoint1, 0.0f },
                glm::vec3{ aPoint2, 0.0f },
                glm::vec3{ aPoint3, 0.0f },
                mColor);
  }

  void TriangleDrawer::AddTriangle(glm::vec4 aPoint1, 
                                   glm::vec4 aPoint2, 
                                   glm::vec4 aPoint3, 
                                   glm::vec3 aColor)
  {
    AddTriangle(glm::vec3{ aPoint1.x, aPoint1.y, aPoint1.z },
                glm::vec3{ aPoint2.x, aPoint2.y, aPoint2.z },
                glm::vec3{ aPoint3.x, aPoint3.y, aPoint3.z },
                aColor);
  }

  void TriangleDrawer::AddTriangle(glm::vec2 aPoint1,
                                   glm::vec2 aPoint2, 
                                   glm::vec2 aPoint3, 
                                   glm::vec3 aColor)
  {
    AddTriangle(glm::vec3{ aPoint1, 0.0f },
                glm::vec3{ aPoint2, 0.0f },
                glm::vec3{ aPoint3, 0.0f },
                aColor);
  }

  void TriangleDrawer::AddTriangle(glm::vec3 aPoint1,
                                   glm::vec3 aPoint2,
                                   glm::vec3 aPoint3,
                                   glm::vec3 aColor)
  {
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mColor = aColor;
    vert.mPosition = aPoint1;
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mPosition = aPoint2;
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mPosition = aPoint3;
    submesh.mVertexBuffer.emplace_back(vert);
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }


  // This will set the lines given since the last Start() call to be drawn.
  void TriangleDrawer::End()
  {
    mSubmeshes[0].mVertexBufferSize = mSubmeshes[0].mVertexBuffer.size() * sizeof(Vertex);
    mSubmeshes[0].mIndexBufferSize = mSubmeshes[0].mIndexBuffer.size() * sizeof(u32);

    if (0 == mSubmeshes[0].mVertexBufferSize)
    {
      return;
    }

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmeshes, true);

    mInstantiatedLines = mRenderer->CreateModel(mView, mesh);
    mInstantiatedLines->mType = ShaderType::Wireframe;
    auto data = mInstantiatedLines->GetUBOModelData();
    mInstantiatedLines->UpdateUBOModel(data);
    mInstantiatedLines->mLineWidth = 5.0f;
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Curve Drawer
  /////////////////////////////////////////////////////////////////////////////////////////////////

  YTEDefineType(CurveDrawer)
  {
    YTERegisterType(CurveDrawer);
  }

  CurveDrawer::CurveDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mRenderer(aRenderer)
    , mView(aView)
    , mMeshName(aUniqueName)
  {
    mMeshName = aUniqueName;
    mSubmeshes.emplace_back(Submesh{});
    mSubmeshes[0].mShaderSetName = "Line";

    ShaderUsage useVert(true, false);
    ShaderUsage useFrag(false, true);
    mSubmeshes[0].mUBOs.clear();
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::View, useVert, sizeof(UBOView));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Animation, useVert, sizeof(UBOAnimation));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::ModelMaterial, useFrag, sizeof(UBOMaterial));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::SubmeshMaterial, useFrag, sizeof(UBOMaterial));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Lights, useFrag, sizeof(UBOLightMan));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Illumination, useFrag, sizeof(UBOIllumination));
    mSubmeshes[0].mUBOs.emplace_back(UBOTypeIDs::Model, useVert, sizeof(UBOModel));
  }

  CurveDrawer::~CurveDrawer()
  {

  }

  // This will clear the previous version of the Lines.
  void CurveDrawer::Start()
  {
    mInstantiatedLines.reset();
    auto &submesh = mSubmeshes[0];

    submesh.mVertexBufferSize = 0;
    submesh.mIndexBufferSize = 0;
    submesh.mVertexBuffer.clear();
    submesh.mIndexBuffer.clear();
  }

  void CurveDrawer::AddPoint(glm::vec4 aPoint)
  {
    AddPoint(glm::vec3{ aPoint.x, aPoint.y, aPoint.z },
            mColor);
  }

  void CurveDrawer::AddPoint(glm::vec3 aPoint)
  {
    AddPoint(aPoint,
             mColor);
  }

  void CurveDrawer::AddPoint(glm::vec2 aPoint)
  {
    AddPoint(glm::vec3{ aPoint, 0.0f },
             mColor);
  }

  void CurveDrawer::AddPoint(glm::vec4 aPoint, glm::vec3 aColor)
  {
    AddPoint(glm::vec3{ aPoint.x, aPoint.y, aPoint.z },
             aColor);
  }

  void CurveDrawer::AddPoint(glm::vec2 aPoint,
                               glm::vec3 aColor)
  {
    AddPoint(glm::vec3{ aPoint, 0.0f },
             aColor);
  }

  void CurveDrawer::AddPoint(glm::vec3 aPoint,
                               glm::vec3 aColor)
  {
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mColor = aColor;
    vert.mPosition = aPoint;
    submesh.mVertexBuffer.emplace_back(vert);
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }


  // This will set the lines given since the last Start() call to be drawn.
  void CurveDrawer::End()
  {
    mSubmeshes[0].mVertexBufferSize = mSubmeshes[0].mVertexBuffer.size() * sizeof(Vertex);
    mSubmeshes[0].mIndexBufferSize = mSubmeshes[0].mIndexBuffer.size() * sizeof(u32);

    if (0 == mSubmeshes[0].mVertexBufferSize)
    {
      return;
    }

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmeshes, true);

    mInstantiatedLines = mRenderer->CreateModel(mView, mesh);
    mInstantiatedLines->mType = ShaderType::Curves;
    auto data = mInstantiatedLines->GetUBOModelData();
    mInstantiatedLines->UpdateUBOModel(data);
    mInstantiatedLines->mLineWidth = 5.0f;
  }
}