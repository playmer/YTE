#include <glm/gtx/euler_angles.hpp>

#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
 
#include "YTE/Graphics/Camera.hpp" 
#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
#include "YTE/Graphics/UBOs.hpp"
 
#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"


/*
 *  Transform
 *                ^
 *                |
 *             Y  |
 *                |
 *                |
 *                |       X
 *                /---------------->
 *               /
 *        Z     /
 *             /
 *            V
 *
 *
 *  Rotation
 *                ^
 *                |
 *          Y    <|-
 *              | | |
 *               -|-           ^
 *                |       X    |
 *                /---------------->
 *            |--/--           |
 *        Z   | /
 *            -/--->
 *            V
 *  
 *    Roll is on Z axis and rotates the cameras view
 *    Pitch is the X axis and rotates the view up and down
 *    Yaw is the Y axis and rotates the view right and left
 *    
 *  Mouse Position Deltas
 *  
 *             ^    ^  -Y
 *              \   |
 *            Z  \  |
 *                \ |
 *      -X         \|
 *      <------------------------->  X
 *                  |\
 *                  | \
 *                  |  \  -Z
 *                  |   \
 *                  V    V
 *                  
 *                  Y
 *                  
 *    Scrolling is the Z axis, -Z is scroll out (toward your body), Z is scroll in (towards screen)
 */

 // NOTE: Spin = x
 //       Tilt = y
 //       Twist = z

namespace YTE 
{
  YTEDefineType(Camera) 
  { 
    RegisterType<Camera>();
    TypeBuilder<Camera> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() }, 
                                             { TypeId<Orientation>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
 
    builder.Property<&Camera::GetFarPlane, &Camera::SetFarPlane>("FarPlane") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The far plane the view will be rendered with."); 
 
    builder.Property<&Camera::GetNearPlane, &Camera::SetNearPlane>("NearPlane") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The near plane the view will be rendered with."); 
 
    builder.Property<&Camera::GetFieldOfViewY, &Camera::SetFieldOfViewY>("FieldOfViewY") 
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("The field of view (y) the view will be rendered with."); 

    builder.Property<&Camera::GetGlobalIlluminaton, &Camera::SetGlobalIlluminaton>("Global Illumination")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the illumination of the world");

    builder.Property<&Camera::GetFogColor, &Camera::SetFogColor>("Fog Color")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the color of the fog");

    builder.Property<&Camera::GetFogPlanes, &Camera::SetFogPlanes>("Fog Near/Far")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts Fog near and far planes (start of fog, full fog)");

    builder.Property<&Camera::GetFogCoeffs, &Camera::SetFogCoeffs>("Fog Coefficients")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Adjusts the fog coefficients of the world");

    builder.Property<&Camera::GetUseOrtho, &Camera::SetUseOrtho>("UseOrthographicProjection")
      .AddAttribute<EditorProperty>()
      .AddAttribute<Serializable>()
      .SetDocumentation("Sets whether this camera uses an orthographic projection or a perspective projection. Perspective by default");
  } 
  
  Camera::Camera(Composition *aOwner,
                 Space *aSpace)
    : Component{ aOwner, aSpace }
    , mEngine{ nullptr }
    , mGraphicsView{ nullptr }
    , mCameraTransform{ nullptr }
    , mCameraOrientation{ nullptr }
    , mFieldOfViewY{ glm::radians(45.0f) }
    , mNearPlane{ 0.1f }
    , mFarPlane{ 20000.f }
    , mUseOrthographicProj{ false }
    , mDt{ 0.0f }
    , mChanged{ true }
  {
  } 
 
  void Camera::Initialize()
  {
    mGraphicsView = mSpace->GetComponent<GraphicsView>();
    mWindow = mGraphicsView->GetWindow();
    mEngine = mSpace->GetEngine();

    mGraphicsView->RegisterEvent<&Camera::RendererResize>(Events::RendererResize, this);
    mGraphicsView->RegisterEvent<&Camera::SurfaceGainedEvent>(Events::SurfaceGained, this);
    mSpace->RegisterEvent<&Camera::Update>(Events::FrameUpdate, this);
    mOwner->RegisterEvent<&Camera::TransformEvent>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&Camera::OrientationEvent>(Events::OrientationChanged, this);

    if (nullptr == mGraphicsView->GetActiveCamera())
    {
      SetCameraAsActive();
    }
 
    mCameraTransform = mOwner->GetComponent<Transform>(); 
    mCameraOrientation = mOwner->GetComponent<Orientation>(); 

    mIllumination.mCameraPosition = glm::vec4(mCameraTransform->GetTranslation(), 1.0f);
    mIllumination.mFogCoefficients = glm::vec4(1.0f, 0.1f, 0.0f, 0.0f);
    mIllumination.mFogColor = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
    mIllumination.mFogPlanes = glm::vec2(mFarPlane / 2.0f, mFarPlane);
    mIllumination.mGlobalIllumination = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    mIllumination.mTime = 0.0f;

    RendererResize(nullptr);
  }

  UBOView Camera::ConstructUBOView()
  {
    auto height = static_cast<float>(mWindow->GetHeight());
    auto width = static_cast<float>(mWindow->GetWidth());

    UBOView view;

    // projection matrix (since its an easy calculation, Ill leave it here for now, but
    // it really doesn't need to happen every view update
    if (!mUseOrthographicProj)
    {
      view.mProjectionMatrix = glm::perspective(mFieldOfViewY,
                                                width / height,
                                                mNearPlane,
                                                mFarPlane);

      // matrix does perspective divide and flips vulkan y axis
      const glm::mat4 clip(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f,-1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 0.5f, 0.0f,
                           0.0f, 0.0f, 0.5f, 1.0f);

      view.mProjectionMatrix = clip * view.mProjectionMatrix;
    }
    else
    {
      view.mProjectionMatrix = glm::ortho(-width / 2.f,
                                          width / 2.f,
                                          -height / 2.f,
                                          height / 2.f,
                                          mNearPlane,
                                          mFarPlane);

        // Flip Vulkan y axis
      const glm::mat4 flip(1.0f, 0.0f, 0.0f, 0.0f,
                           0.0f, -1.0f, 0.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 0.0f,
                           0.0f, 0.0f, 1.0f, 1.0f);

      view.mProjectionMatrix = flip * view.mProjectionMatrix;
    }

    auto translation = mCameraTransform->GetWorldTranslation();
    auto lookAtPoint = (-mCameraOrientation->GetForwardVector()) + translation;
    view.mViewMatrix = glm::lookAt(translation,
                                   lookAtPoint,
                                   mCameraOrientation->GetUpVector());
        
    view.mCameraPosition = glm::vec4(translation, 1.0f);

    return view;
  }


  void Camera::SurfaceGainedEvent(ViewChanged *aEvent)
  {
    mGraphicsView = aEvent->View;
    mWindow = aEvent->Window;
  }

  void Camera::SetCameraAsActive()
  {
    mGraphicsView->SetActiveCamera(this);
  }

  void Camera::TransformEvent(TransformChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mChanged = true;
  }

  void Camera::OrientationEvent(OrientationChanged *aEvent)
  {
    UnusedArguments(aEvent);
    mChanged = true;
  }

  void Camera::Update(LogicUpdate *aEvent)
  {
    YTEProfileFunction();

    if (this == mGraphicsView->GetActiveCamera())
    {
      if (mChanged)
      {
        UBOView view = ConstructUBOView();

        mChanged = false;
        mGraphicsView->UpdateView(this, view);
      }

      mIllumination.mCameraPosition = glm::vec4(mCameraTransform->GetWorldTranslation(), 1.0f);
      mIllumination.mTime += static_cast<float>(aEvent->Dt);
      mGraphicsView->UpdateIllumination(mIllumination);
    }
  }

  void Camera::RendererResize(WindowResize *aEvent)
  {
    UnusedArguments(aEvent);

    mChanged = true;
  }
}
