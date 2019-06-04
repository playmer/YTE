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
    RegisterType<LineDrawer>();
    TypeBuilder<LineDrawer> builder;
  }

  LineDrawer::LineDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mRenderer(aRenderer)
    , mView(aView)
    , mMeshName(aUniqueName)
  {
    mMeshName = aUniqueName;
    mSubmesh.mShaderSetName = "Line";
  }

  LineDrawer::~LineDrawer()
  {

  }

  // This will clear the previous version of the Lines.
  void LineDrawer::Start()
  {
    mInstantiatedLines.reset();
    mSubmesh.mVertexData.Clear();
    mSubmesh.mIndexData.clear();
    mSubmesh.mShaderSetName = "Line";
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
    Vertex vert;
    vert.mColor = glm::vec4{ aColor, 1.0f };
    vert.mPosition = aPoint1;
    
    mSubmesh.mVertexData.AddVertex(vert);
    vert.mPosition = aPoint2;
    mSubmesh.mVertexData.AddVertex(vert);
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
  }


  void LineDrawer::AddLine(glm::vec3 aPoint1, 
                           glm::vec3 aPoint2, 
                           glm::vec3 aFromColor, 
                           glm::vec3 aToColor)
  {
    Vertex vert;
    vert.mColor = glm::vec4{ aFromColor, 1.0f };
    vert.mPosition = aPoint1;
    mSubmesh.mVertexData.AddVertex(vert);
    vert.mColor = glm::vec4{ aToColor, 1.0f };
    vert.mPosition = aPoint2;
    mSubmesh.mVertexData.AddVertex(vert);
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
  }

  // This will set the lines given since the last Start() call to be drawn.
  void LineDrawer::End()
  {
    if (mSubmesh.mVertexData.mPositionData.empty())
    {
      return;
    }

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmesh, true);

    mInstantiatedLines = mRenderer->CreateModel(mView, mesh);
    mInstantiatedLines->mType = ShaderType::Lines;
    auto data = mInstantiatedLines->GetUBOModelData();
    mInstantiatedLines->UpdateUBOModel(data);
    mInstantiatedLines->mLineWidth = mLineWidth;
  }



  void LineDrawer::SetLineWidth(float aLineWidth)
  {
    mLineWidth = aLineWidth;

    if (mInstantiatedLines)
    {
      mInstantiatedLines->mLineWidth = mLineWidth;
    }
  }



  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Triangle Drawer
  /////////////////////////////////////////////////////////////////////////////////////////////////
  YTEDefineType(TriangleDrawer)
  {
    RegisterType<TriangleDrawer>();
    TypeBuilder<TriangleDrawer> builder;
  }

  TriangleDrawer::TriangleDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mRenderer(aRenderer)
    , mView(aView)
    , mMeshName(aUniqueName)
  {
    mMeshName = aUniqueName;
    mSubmesh.mShaderSetName = "Line";
  }

  TriangleDrawer::~TriangleDrawer()
  {

  }

  // This will clear the previous version of the Lines.
  void TriangleDrawer::Start()
  {
    mInstantiatedLines.reset();

    mSubmesh.mVertexData.Clear();
    mSubmesh.mIndexData.clear();
    mSubmesh.mShaderSetName = "Line";
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
    AddTriangle(aPoint1,
                aPoint2,
                aPoint3,
                glm::vec4{ aColor, 1.0f });
  }

  void TriangleDrawer::AddTriangle(glm::vec3 aPoint1,
                                   glm::vec3 aPoint2,
                                   glm::vec3 aPoint3,
                                   glm::vec4 aColor)
  {
    Vertex vert;
    vert.mColor = aColor;
    vert.mPosition = aPoint1;
    mSubmesh.mVertexData.AddVertex(vert);
    vert.mPosition = aPoint2;
    mSubmesh.mVertexData.AddVertex(vert);
    vert.mPosition = aPoint3;
    mSubmesh.mVertexData.AddVertex(vert);
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
  }


  // This will set the lines given since the last Start() call to be drawn.
  void TriangleDrawer::End()
  {
    if (mSubmesh.mVertexData.mPositionData.empty())
    {
      return;
    }

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmesh, true);

    mInstantiatedLines = mRenderer->CreateModel(mView, mesh);
    mInstantiatedLines->mType = ShaderType::Wireframe;
    auto data = mInstantiatedLines->GetUBOModelData();
    mInstantiatedLines->UpdateUBOModel(data);
    mInstantiatedLines->mLineWidth = mLineWidth;
  }



  void TriangleDrawer::SetLineWidth(float aLineWidth)
  {
    mLineWidth = aLineWidth;

    if (mInstantiatedLines)
    {
      mInstantiatedLines->mLineWidth = mLineWidth;
    }
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////
  // Curve Drawer
  /////////////////////////////////////////////////////////////////////////////////////////////////

  YTEDefineType(CurveDrawer)
  {
    RegisterType<CurveDrawer>();
    TypeBuilder<CurveDrawer> builder;
  }

  CurveDrawer::CurveDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mRenderer(aRenderer)
    , mView(aView)
    , mMeshName(aUniqueName)
  {
    mMeshName = aUniqueName;
    mSubmesh.mShaderSetName = "Line";
  }

  CurveDrawer::~CurveDrawer()
  {

  }

  // This will clear the previous version of the Lines.
  void CurveDrawer::Start()
  {
    mInstantiatedLines.reset();
    mSubmesh.mVertexData.Clear();
    mSubmesh.mIndexData.clear();
    mSubmesh.mShaderSetName = "Line";
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
    Vertex vert;
    vert.mColor = glm::vec4{ aColor, 1.0f };
    vert.mPosition = aPoint;
    mSubmesh.mVertexData.AddVertex(vert);
    mSubmesh.mIndexData.emplace_back(static_cast<u32>(mSubmesh.mIndexData.size()));
  }


  // This will set the lines given since the last Start() call to be drawn.
  void CurveDrawer::End()
  {
    if (mSubmesh.mVertexData.mPositionData.empty())
    {
      return;
    }

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmesh, true);

    mInstantiatedLines = mRenderer->CreateModel(mView, mesh);
    mInstantiatedLines->mType = ShaderType::Curves;
    auto data = mInstantiatedLines->GetUBOModelData();
    mInstantiatedLines->UpdateUBOModel(data);
    mInstantiatedLines->mLineWidth = mLineWidth;
  }

  void CurveDrawer::SetLineWidth(float aLineWidth)
  {
    mLineWidth = aLineWidth;

    if (mInstantiatedLines)
    {
      mInstantiatedLines->mLineWidth = mLineWidth;
    }
  }
}