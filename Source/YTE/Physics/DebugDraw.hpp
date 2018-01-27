/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_DebugDraw_hpp
#define YTE_Physics_DebugDraw_hpp

#include "LinearMath/btIDebugDraw.h"

#include "YTE/Meta/Type.hpp"

namespace YTE
{
  // TODO (Josh): Re-implement this
  class DebugDrawer : public btIDebugDraw
  {
  public:
    DebugDrawer();

    void Begin();
    void End();

    ~DebugDrawer() override {};
    
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor) override;
    void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    //void drawSphere(const btVector3& p, btScalar radius, const btVector3& color);
    void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha) override;
    void drawContactPoint(const btVector3& aPointOnB, const btVector3& aNormalOnB, btScalar aDistance, int aLifeTime, const btVector3& aColor) override;
    void reportErrorWarning(const char* warningString) override;
    void draw3dText(const btVector3& location, const char* textString) override;
    void setDebugMode(int debugMode) override;
    int getDebugMode() const  override { return mDebugMode; }
    
  private:
    int mDebugMode;
  };
}

YTEDeclareExternalType(btIDebugDraw::DebugDrawModes);

#endif