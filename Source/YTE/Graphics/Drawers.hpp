/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2018/01/19
*
* \copyright All content 2018 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#pragma once

#include "fmt/format.h"

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class LineDrawer
  {
  public:
    YTEDeclareType(LineDrawer)

    LineDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView);
    ~LineDrawer();

    // This will clear the previous version of the Lines.
    void Start();
    void AddLine(glm::vec4 aPoint1, glm::vec4 aPoint2);
    void AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2);
    void AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2);
    void AddLine(glm::vec4 aPoint1, glm::vec4 aPoint2, glm::vec3 aColor);
    void AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aFromColor, glm::vec3 aToColor);
    void AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aColor);
    void AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2, glm::vec3 aColor);

    // This will set the lines given since the last Start() call to be drawn.
    void End();

    glm::vec3 GetColor()
    {
      return mColor;
    }

    void SetColor(glm::vec3 aColor)
    {
      mColor = aColor;
    }

    void SetLineWidth(float aLineWidth);

  private:
    Renderer *mRenderer;
    GraphicsView *mView;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::string mMeshName;
    std::vector<Submesh> mSubmeshes;
    std::unique_ptr<InstantiatedModel> mInstantiatedLines;
    glm::vec3 mColor{ 1.0f, 1.0f, 1.0f };
    float mLineWidth = 5.0f;
  };

  class TriangleDrawer
  {
  public:
    YTEDeclareType(TriangleDrawer)

    TriangleDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView);
    ~TriangleDrawer();

    // This will clear the previous version of the Lines.
    void Start();
    void AddTriangle(glm::vec4 aPoint1, glm::vec4 aPoint2, glm::vec4 aPoint3);
    void AddTriangle(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3);
    void AddTriangle(glm::vec2 aPoint1, glm::vec2 aPoint2, glm::vec2 aPoint3);
    void AddTriangle(glm::vec4 aPoint1, glm::vec4 aPoint2, glm::vec4 aPoint3, glm::vec3 aColor);
    void AddTriangle(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3, glm::vec3 aColor);
    void AddTriangle(glm::vec3 aPoint1, glm::vec3 aPoint2, glm::vec3 aPoint3, glm::vec4 aColor);
    void AddTriangle(glm::vec2 aPoint1, glm::vec2 aPoint2, glm::vec2 aPoint3, glm::vec3 aColor);

    // This will set the lines given since the last Start() call to be drawn.
    void End();

    glm::vec3 GetColor()
    {
      return mColor;
    }

    void SetColor(glm::vec3 aColor)
    {
      mColor = aColor;
    }

    void SetLineWidth(float aLineWidth);

  private:
    Renderer * mRenderer;
    GraphicsView *mView;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::string mMeshName;
    std::vector<Submesh> mSubmeshes;
    std::unique_ptr<InstantiatedModel> mInstantiatedLines;
    glm::vec3 mColor{ 1.0f, 1.0f, 1.0f };

    float mLineWidth = 5.0f;
  };


  class CurveDrawer
  {
  public:
    YTEDeclareType(CurveDrawer)

    CurveDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView);
    ~CurveDrawer();

    // This will clear the previous version of the Lines.
    void Start();
    void AddPoint(glm::vec4 aPoint1);
    void AddPoint(glm::vec3 aPoint1);
    void AddPoint(glm::vec2 aPoint1);
    void AddPoint(glm::vec4 aPoint1, glm::vec3 aColor);
    void AddPoint(glm::vec3 aPoint1, glm::vec3 aColor);
    void AddPoint(glm::vec2 aPoint1, glm::vec3 aColor);

    // This will set the lines given since the last Start() call to be drawn.
    void End();

    glm::vec3 GetColor()
    {
      return mColor;
    }

    void SetColor(glm::vec3 aColor)
    {
      mColor = aColor;
    }

    void SetLineWidth(float aLineWidth);

  private:
    Renderer * mRenderer;
    GraphicsView *mView;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::string mMeshName;
    std::vector<Submesh> mSubmeshes;
    std::unique_ptr<InstantiatedModel> mInstantiatedLines;
    glm::vec3 mColor{ 1.0f, 1.0f, 1.0f };
    float mLineWidth = 5.0f;
  };
}
