#include "YTE/Core/Space.hpp"

#include "YTE/Graphics/GraphicsView.hpp"
#include "YTE/Graphics/LineDrawer.hpp"
#include "YTE/Graphics/Generics/Renderer.hpp"

namespace YTE
{

  YTEDefineType(LineDrawer)
  {
    YTERegisterType(LineDrawer);
    GetStaticType()->AddAttribute<RunInEditor>();
  }


  LineDrawer::LineDrawer(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : BaseModel{ aOwner, aSpace, aProperties }
  {
  }

  void LineDrawer::Initialize()
  {
    mMeshName = "__LineDrawer";
    mMeshName += mOwner->GetGUID().ToString();
    mSubmeshes.emplace_back(Submesh{});
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
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mPosition = glm::vec3{ aPoint1.x, aPoint1.y, aPoint1.z };
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mPosition = glm::vec3{ aPoint2.x, aPoint2.y, aPoint2.z };
    submesh.mVertexBuffer.emplace_back();
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }

  void LineDrawer::AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2)
  {
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mPosition = aPoint1;
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mPosition = aPoint2;
    submesh.mVertexBuffer.emplace_back(vert);
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }

  void LineDrawer::AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2)
  {
    auto &submesh = mSubmeshes[0];

    Vertex vert;
    vert.mPosition = glm::vec3{ aPoint1, 0.f };
    submesh.mVertexBuffer.emplace_back(vert);
    vert.mPosition = glm::vec3{ aPoint2, 0.f };
    submesh.mVertexBuffer.emplace_back(vert);
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
    submesh.mIndexBuffer.emplace_back(static_cast<u32>(submesh.mIndexBuffer.size()));
  }

  // This will set the lines given since the last Start() call to be drawn.
  void LineDrawer::End()
  {
    mSubmeshes[0].mVertexBufferSize = mSubmeshes[0].mVertexBuffer.size() * sizeof(Vertex);
    mSubmeshes[0].mIndexBufferSize = mSubmeshes[0].mIndexBuffer.size() * sizeof(u32);

    auto view = mOwner->GetSpace()->GetComponent<GraphicsView>();

    auto mesh = mRenderer->CreateSimpleMesh(mMeshName, mSubmeshes, true);

    mInstantiatedLines = mRenderer->CreateModel(view, mesh);
    mInstantiatedLines->mType = ShaderType::ShaderLines;
  }
}