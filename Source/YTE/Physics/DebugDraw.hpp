/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/
#pragma once

#ifndef YTE_Physics_DebugDraw_h
#define YTE_Physics_DebugDraw_h

#include "Bullet/LinearMath/btIDebugDraw.h"

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

    virtual ~DebugDrawer() {};
    
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor);
    
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
    
    //virtual void drawSphere(const btVector3& p, btScalar radius, const btVector3& color);
    
    virtual void drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha);
    
    virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {};
    
    virtual void reportErrorWarning(const char* warningString);
    
    virtual void draw3dText(const btVector3& location, const char* textString);
    
    virtual void setDebugMode(int debugMode);
    
    virtual int getDebugMode() const { return mDebugMode; }
    
  private:
    int mDebugMode;
  };
}

DeclareExternalType(btIDebugDraw::DebugDrawModes);

#endif