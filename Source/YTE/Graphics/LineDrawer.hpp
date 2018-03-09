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

#include "YTE/Graphics/BaseModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

namespace YTE
{
  class LineDrawer : public BaseModel
  {
  public:
    YTEDeclareType(LineDrawer);
    LineDrawer(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    ~LineDrawer();

    void Initialize() override;

    // This will clear the previous version of the Lines.
    void Start();
    void AddLine(glm::vec4 aPoint1, glm::vec4 aPoint2);
    void AddLine(glm::vec3 aPoint1, glm::vec3 aPoint2);
    void AddLine(glm::vec2 aPoint1, glm::vec2 aPoint2);

    // This will set the lines given since the last Start() call to be drawn.
    void End();

    std::vector<InstantiatedModel*> GetInstantiatedModel() override
    {
      std::vector<InstantiatedModel*> toReturn;

      if (mInstantiatedLines)
      {
          toReturn.emplace_back(mInstantiatedLines.get());
      }

      return toReturn;
    }

  private:
    Renderer *mRenderer;
    Window *mWindow;
    Transform *mTransform;
    UBOModel mUBOModel;

    std::string mMeshName;

    std::vector<Submesh> mSubmeshes;

    std::unique_ptr<InstantiatedModel> mInstantiatedLines;
  };
}
