/******************************************************************************/
/*!
* \author Joshua T. Fisher
* \date   2015-6-7
*
* \copyright All content 2016 DigiPen (USA) Corporation, all rights reserved.
*/
/******************************************************************************/

#include "YTE/Physics/DebugDraw.hpp"
#include "YTE/Physics/PhysicsSystem.hpp"

namespace YTE
{
  DebugDrawer::DebugDrawer(std::string aUniqueName, Renderer *aRenderer, GraphicsView *aView)
    : mTriangleDrawer{ aUniqueName + "TriangleDrawer", aRenderer, aView }
    , mLineDrawer{ aUniqueName + "LineDrawer", aRenderer, aView }
    , mDebugMode{DBG_DrawWireframe |
                 DBG_DrawAabb |
                 DBG_DrawContactPoints |
                 DBG_DrawNormals |
                 DBG_DrawFrames }
  {

  }

  void DebugDrawer::Begin()
  {
    mLineDrawer.Start();
    mTriangleDrawer.Start();
  }
  void DebugDrawer::End()
  {
    mLineDrawer.End();
    mTriangleDrawer.End();
  }

  void DebugDrawer::drawLine(const btVector3 &aFrom, 
                             const btVector3 &aTo, 
                             const btVector3 &aFromColor, 
                             const btVector3 &aToColor)
  {
    YTEUnusedArgument(aFrom);
    YTEUnusedArgument(aTo);
    YTEUnusedArgument(aFromColor);
    YTEUnusedArgument(aToColor);
    mLineDrawer.AddLine(BtToOurVec3(aFrom), 
                        BtToOurVec3(aTo), 
                        BtToOurVec3(aFromColor), 
                        BtToOurVec3(aToColor));
  }

  void DebugDrawer::drawLine(const btVector3 &aFrom, 
                             const btVector3 &aTo, 
                             const btVector3 &aColor)
  {
    mLineDrawer.AddLine(BtToOurVec3(aFrom), 
                        BtToOurVec3(aTo), 
                        BtToOurVec3(aColor));
  }

  void DebugDrawer::drawTriangle(const btVector3 &aA, 
                                 const btVector3 &aB, 
                                 const btVector3 &aC, 
                                 const btVector3 &aColor, 
                                 btScalar aAlpha)
  {
    YTEUnusedArgument(aAlpha);
    mTriangleDrawer.AddTriangle(BtToOurVec3(aA), 
                                BtToOurVec3(aB), 
                                BtToOurVec3(aC), 
                                BtToOurVec3(aColor));
  }
    
  void DebugDrawer::reportErrorWarning(const char * warningString)
  {
    std::cout << "Bullet Reported Warning: " << warningString << std::endl;
  }

  void DebugDrawer::draw3dText(const btVector3 &aLocation, const char *aTextString)
  {
    YTEUnusedArgument(aLocation);
    YTEUnusedArgument(aTextString);
  }

  void DebugDrawer::setDebugMode(int aDebugMode)
  {
    YTEUnusedArgument(aDebugMode);
  }


  void DebugDrawer::drawContactPoint(const btVector3& aPointOnB, 
                                     const btVector3& aNormalOnB, 
                                     btScalar aDistance, 
                                     int aLifeTime, 
                                     const btVector3& aColor)
  {
    YTEUnusedArgument(aPointOnB);
    YTEUnusedArgument(aNormalOnB);
    YTEUnusedArgument(aDistance);
    YTEUnusedArgument(aLifeTime);
    YTEUnusedArgument(aColor);
  };
}

// TODO (Josh): Fix enum binding
YTEDefineExternalType(btIDebugDraw::DebugDrawModes)
{
  YTERegisterType(btIDebugDraw::DebugDrawModes);
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DisableBulletLCP, "DisableBulletLCP");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawAabb, "DrawAabb");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawConstraintLimits, "DrawConstraintLimits");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawConstraints, "DrawConstraints");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawContactPoints, "DrawContactPoints");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawFeaturesText, "DrawFeaturesText");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawFrames, "DrawFrames");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawNormals, "DrawNormals");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawText, "DrawText");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_DrawWireframe, "DrawWireframe");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_EnableCCD, "EnableCCD");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_EnableSatComparison, "EnableSatComparison");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_FastWireframe, "FastWireframe");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_NoDeactivation, "NoDeactivation");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_NoHelpText, "NoHelpText");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_ProfileTimings, "ProfileTimings");
//  YTEBindEnumValue(builder, type, btIDebugDraw::DebugDrawModes::DBG_MAX_DEBUG_DRAW_MODE, "MAX_DEBUG_DRAW_MODE");
}


