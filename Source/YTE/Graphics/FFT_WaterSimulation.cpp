///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////

// dependencies
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <fstream>

// YTE
#include "YTE/Core/Engine.hpp" 
#include "YTE/Core/Space.hpp" 
#include "YTE/Core/Utilities.hpp" 
#include "YTE/Core/Threading/JobSystem.hpp"

#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/Generics/InstantiatedHeightmap.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"
#include "YTE/Graphics/Vulkan/VkRenderedSurface.hpp"
#include "YTE/Graphics/Vulkan/VkWaterInfluenceMapManager.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Math/Random.hpp"
#include "YTE/Math/Values.hpp"

#include <iomanip>

#define DefaultGridSize 32

// switch entirely over to the mGridSizePlus1 system. I need this to work for tiling

//Intensity maps
//y - height *= intensity;
//normal y *= 1.0f / intensity;
//
//128x128
//normal gravity
//58x58 wind
//25x25x1 size
//0.00005 height


// --------------------------
// Definitions

namespace YTE
{
  YTEDefineType(FFT_WaterSimulation)
  {
    RegisterType<FFT_WaterSimulation>();
    TypeBuilder<FFT_WaterSimulation> builder;
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<Transform>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    builder.Property<&FFT_WaterSimulation::GetTimeDilationEffect, &FFT_WaterSimulation::SetTimeDilationEffect>( "TimeDilation")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Adjusts the wave's speed. Can be used to slow or quicken the pace of the algorithm");

    builder.Property<&FFT_WaterSimulation::GetVertexDistance, &FFT_WaterSimulation::SetVertexDistance>( "VertexDistance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Adjusts the distance between each individual vertex. This will expand or shrink the overall size of the mesh");

    builder.Property<&FFT_WaterSimulation::GetReset, &FFT_WaterSimulation::SetReset>( "ResetSimulation")
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Completely resets the simulation to run from scratch again");

    builder.Property<&FFT_WaterSimulation::GetGravitationalPull, &FFT_WaterSimulation::SetGravitationalPull>( "GravitationalPull")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the gravitational pull on the waves in the y (up) direction. This requires a reset");

    builder.Property<&FFT_WaterSimulation::GetGridSize, &FFT_WaterSimulation::SetGridSize>( "Complexity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the size of the size of the N*N gird used to calculate the water. This requires a reset");

    builder.Property<&FFT_WaterSimulation::GetWaveHeight, &FFT_WaterSimulation::SetWaveHeight>( "WaveMaximumHeight")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the max wave height the algorithm can achieve. This requires a reset");

    builder.Property<&FFT_WaterSimulation::GetWindFactor, &FFT_WaterSimulation::SetWindFactor>( "WindFactor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the wind direction and magnitude. This requires a reset");

    builder.Property<&FFT_WaterSimulation::GetShaderSetName, &FFT_WaterSimulation::SetShaderSetName>( "ShaderSetName")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("What shader to use for the object");

    builder.Property<&FFT_WaterSimulation::GetRunWithEngineUpdate, &FFT_WaterSimulation::SetRunWithEngineUpdate>( "RunInEditor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Run while in editor");

    builder.Property<&FFT_WaterSimulation::GetRunInSteps, &FFT_WaterSimulation::SetRunInSteps>( "DontRunEveryFrame")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Prevents the FFT from running every frame");

    builder.Property<&FFT_WaterSimulation::GetStepCount, &FFT_WaterSimulation::SetStepCount>( "RunIn_X_Frames")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Runs the FFT every ___ frames");

    builder.Property<&FFT_WaterSimulation::GetInstancingAmount, &FFT_WaterSimulation::SetInstancingAmount>( "InstanceCount")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("How far to instance from the origin of the world");

    builder.Property<&FFT_WaterSimulation::GetUseHTilde, &FFT_WaterSimulation::SetUseHTilde>( "UseHTilde")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("How far to instance from the origin of the world");

    builder.Property<&FFT_WaterSimulation::GetUsePhillips, &FFT_WaterSimulation::SetUsePhillips>( "UsePhillips")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("How far to instance from the origin of the world");

    builder.Property<&FFT_WaterSimulation::GetUseNoDisplacement, &FFT_WaterSimulation::SetUseNoDisplacement>( "UseNoDisplacement")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("How far to instance from the origin of the world");

    builder.Property<&FFT_WaterSimulation::GetUseNewKs, &FFT_WaterSimulation::SetUseNewKs>( "UseNewKs")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("How far to instance from the origin of the world");

    builder.Property<&FFT_WaterSimulation::GetUseHTildeSubZero, &FFT_WaterSimulation::SetUseHTildeSubZero>( "UseHTildeSubZero")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("How far to instance from the origin of the world");
  }



  // ============    ============    ============    ============    ============    ============    ============
  void FFT_WaterSimulation::TransformEvent(TransformChanged *aEvent)
  {
    YTEUnusedArgument(aEvent);

    // update all the positions for the objects being rendered
    AdjustPositions();
  }


  // ------------------------------------
  FFT_WaterSimulation::FFT_WaterSimulation(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : BaseModel{ aOwner, aSpace, aProperties }
    , mGravitationalPull(9.81f)           // normal gravity
    , mGridSize(DefaultGridSize)          // grid size of 32 just for now
    , mGridSizePlus1(mGridSize + 1)       // grid size of 32 just for now
    , mWaveHeight(0.00005f)               // normal wave height
    , mWindFactor(58.0f, 58.0f)           // normal wind
    , mVertexDistanceX(DefaultGridSize)   // same as grid size
    , mVertexDistanceZ(DefaultGridSize)   // ""
    , mTimeDilationEffect(1.0f)           // no time dilation at all
    , mTime(0.0f)                         // we start with no time
    , mReset(false)                       // dont reset
    , mTransform(nullptr)
    , mShaderSetName("FFT_WaterSimulation")
    , mResetNeeded(true)
    , mRunWithEngineUpdate(true)
    , mRunInSteps(false)
    , mSteps(0)
    , mStepsCount(5)
    , mInstanceCount(1)
    , mConstructing(true)
  {
    for (int i = 0; i < 5; ++i)
    {
      mKFFTConfig[i] = NULL;
    }

    auto engine = aSpace->GetEngine();
    mGraphicsView = mSpace->GetComponent<GraphicsView>();
    mRenderer = dynamic_cast<VkRenderer*>(engine->GetComponent<GraphicsSystem>()->GetRenderer());
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    DeserializeByType(aProperties, this, GetStaticType());
  }


  // ------------------------------------
  void FFT_WaterSimulation::Initialize()
  {
    mOwner->RegisterEvent<&FFT_WaterSimulation::TransformEvent>(Events::PositionChanged, this);
    mOwner->RegisterEvent<&FFT_WaterSimulation::TransformEvent>(Events::ScaleChanged, this);
    mOwner->RegisterEvent<&FFT_WaterSimulation::TransformEvent>(Events::RotationChanged, this);
    mSpace->GetEngine()->RegisterEvent<&FFT_WaterSimulation::EditorUpdate>(Events::LogicUpdate, this);
    mSpace->RegisterEvent<&FFT_WaterSimulation::Update>(Events::LogicUpdate, this);
    mTransform = mOwner->GetComponent<Transform>();

    auto engine = mOwner->GetEngine();
    mJobSystem = engine->GetComponent<JobSystem>();

    mConstructing = false;

    Destruct();
    Construct();
    StartKFFT();
    InstanceReset();
  }


  // ------------------------------------
  void FFT_WaterSimulation::Reset()
  {
    mTime = 0.0f;
    Destruct();
    Construct();
    InstanceReset();
  }


  // ------------------------------------
  void FFT_WaterSimulation::Construct()
  {
    mH_Tilde.GetVector().resize(squared(mGridSize));
    mH_TildeSlopeX.GetVector().resize(squared(mGridSize));
    mH_TildeSlopeZ.GetVector().resize(squared(mGridSize));
    mH_TildeDX.GetVector().resize(squared(mGridSize));
    mH_TildeDZ.GetVector().resize(squared(mGridSize));

    mComputationalVertices.resize(squared(mGridSizePlus1));
    mIndices.reserve((squared(mGridSize) * 6));
    mVertices.resize(squared(mGridSizePlus1));

    mInstancingMatrices.resize(squared(mInstanceCount));


    // ------------------------------------
    // fill out the vertices initial data


    for (int z = 0; z < mGridSizePlus1; ++z)
    {
      for (int x = 0; x < mGridSizePlus1; ++x)
      {
        int vertex = z * mGridSizePlus1 + x; // 2D pointer arithmetic for a 1D array

        // calls phillips which sets up the wind factor, and the wave heights
        complex ht0 = Calc_hTildeSub0(x, z);
        complex ht0conj;
        if (UseHTildeSubZero)
        {
          // FROM CONNOR
          ht0conj = Calc_hTildeSub0(x, z).conjugate();
          // END FROM CONNOR
        }
        else
        {
          ht0conj = Calc_hTildeSub0(-x, -z).conjugate();
        }

        // set initial data for actual water simulation (wind factor and wave heights are used)
        mComputationalVertices[vertex].mHTilde0 = glm::vec3(ht0.mReal, ht0.mImaginary, 0.0f);
        mComputationalVertices[vertex].mHTilde0mkConjugate = glm::vec3(ht0conj.mReal,
                                                                       ht0conj.mImaginary, 0.0f);
        mVertices[vertex].mTextureCoordinates.x = static_cast<float>(x) / static_cast<float>((mGridSize / 2));
        mVertices[vertex].mTextureCoordinates.y = static_cast<float>(z) / static_cast<float>((mGridSize / 2));

        // sets positions, and uses the length parameter to space out the grid in 3D space
        // x - (gridSize / 2.0f) = the physical position of the vertex without length expansion
        // (vertexDistance / gridSize) = the length expansion or reduction
        // multiply together to find the grid position
        // note that y has no adjustments
        //glm::vec3 pos(((x - (mGridSize / 2.0f)) * (mVertexDistanceX / mGridSize)), 0.0f,
        //              ((z - (mGridSize / 2.0f)) * (mVertexDistanceZ / mGridSize)));

        glm::vec3 pos(x * (mVertexDistanceX / mGridSize), 0.0f, z * (mVertexDistanceZ / mGridSize));
        
        mComputationalVertices[vertex].mPosition = mComputationalVertices[vertex].mOriginalPosition = pos;

        // initial normal is just the basic normal
        mComputationalVertices[vertex].mNormal = glm::vec3(0.0f, 1.0f, 0.0f);
        
        //
        //// calls phillips which sets up the wind factor, and the wave heights
        //complex ht0 = Calc_hTildeSub0(x, z);
        //complex ht0conj = Calc_hTildeSub0(-x, -z).conjugate();
        //
        //// set initial data for actual water simulation (wind factor and wave heights are used)
        //mDrawableVertices[vertex].mHTilde0 = glm::vec3(ht0.mReal, ht0.mImaginary, 0.0f);
        //mDrawableVertices[vertex].mHTilde0mkConjugate = glm::vec3(ht0conj.mReal,
        //  ht0conj.mImaginary, 0.0f);
        //
        //// sets positions, and uses the length parameter to space out the grid in 3D space
        //// x - (gridSize / 2.0f) = the physical position of the vertex without length expansion
        //// (vertexDistance / gridSize) = the length expansion or reduction
        //// multiply together to find the grid position
        //// note that y has no adjustments
        //glm::vec3 pos(((x - (mGridSize / 2.0f)) * (mVertexDistanceX / mGridSize)), 0.0f,
        //              ((z - (mGridSize / 2.0f)) * (mVertexDistanceZ / mGridSize)));
        //mDrawableVertices[vertex].mPosition = mDrawableVertices[vertex].mOriginalPosition = pos;
        //
        //// initial normal is just the basic normal
        //mDrawableVertices[vertex].mNormal = glm::vec3(0.0f, 1.0f, 0.0f);


        // ------------------------------------
        // index mapping
        // this goes from 0->GridSize so this if statement will stop before the last go
        if (x >= mGridSize || z >= mGridSize)
        {
          continue;
        }

        mIndices.push_back(vertex);
        mIndices.push_back(vertex + mGridSizePlus1);
        mIndices.push_back(vertex + mGridSizePlus1 + 1);
        mIndices.push_back(vertex);
        mIndices.push_back(vertex + mGridSizePlus1 + 1);
        mIndices.push_back(vertex + 1);
      }
    }


    InstanceReset();
    mResetNeeded = false;
  }


  // ------------------------------------
  FFT_WaterSimulation::~FFT_WaterSimulation()
  {
    Destruct();
    StopKFFT();
  }


  // ------------------------------------
  void FFT_WaterSimulation::Destruct()
  {
    mH_Tilde.GetVector().clear();
    mH_TildeDX.GetVector().clear();
    mH_TildeDZ.GetVector().clear();
    mH_TildeSlopeX.GetVector().clear();
    mH_TildeSlopeZ.GetVector().clear();
    mComputationalVertices.clear();
    mIndices.clear();

    DestroyHeightmap();
  }


  // ------------------------------------
  void FFT_WaterSimulation::StartKFFT()
  {
    int size[] = { mGridSize, mGridSize };
    const int sizeDem = 2;

    for (int i = 0; i < 5; ++i)
    {
      mKFFTConfig[i] = kiss_fftnd_alloc(size, sizeDem, 1, NULL, NULL);
     
      if (mKFFTConfig[i] == NULL)
      {
        std::cout << "FFT_WaterSimulation: Failed to create KFFT Config! No FFT calculations will be made"
          << std::endl;
      }
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::StopKFFT()
  {
    kiss_fft_cleanup();
    for (int i = 0; i < 5; ++i)
    {
      free(mKFFTConfig[i]);
      mKFFTConfig[i] = NULL;
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::RunKFFT()
  {
    // perform the FFT on the rows of the water
    auto handle1 = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>, this, &FFT_WaterSimulation::MT_A));
    auto handle2 = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>, this, &FFT_WaterSimulation::MT_B));
    auto handle3 = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>, this, &FFT_WaterSimulation::MT_C));
    auto handle4 = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>, this, &FFT_WaterSimulation::MT_D));
    auto handle5 = mJobSystem->QueueJobThisThread(YTEMakeDelegate(Delegate<Any(*)(JobHandle&)>, this, &FFT_WaterSimulation::MT_E));

    mJobSystem->WaitThisThread(handle1);
    mJobSystem->WaitThisThread(handle2);
    mJobSystem->WaitThisThread(handle3);
    mJobSystem->WaitThisThread(handle4);
    mJobSystem->WaitThisThread(handle5);

    // original
    //kiss_fftnd(mKFFTConfig, mH_Tilde.GetKFFTArray(), mH_Tilde.GetKFFTArray());
    //kiss_fftnd(mKFFTConfig, mH_TildeSlopeX.GetKFFTArray(), mH_TildeSlopeX.GetKFFTArray());
    //kiss_fftnd(mKFFTConfig, mH_TildeSlopeZ.GetKFFTArray(), mH_TildeSlopeZ.GetKFFTArray());
    //kiss_fftnd(mKFFTConfig, mH_TildeDX.GetKFFTArray(), mH_TildeDX.GetKFFTArray());
    //kiss_fftnd(mKFFTConfig, mH_TildeDZ.GetKFFTArray(), mH_TildeDZ.GetKFFTArray());
  }


  // ------------------------------------
  void FFT_WaterSimulation::Render()
  {
    YTEProfileFunction();

    // evaluate the current position for the waves
    WaveGeneration();
    UpdateHeightmap();
  }


  // ------------------------------------
  float FFT_WaterSimulation::Dispersion(int x, int z)
  {
    // dispersion applies the choppiness of the waves
    // this function is called every frame
    float w_0 = 2.0f * pi / 200.0f; //! what is this doing? This does tiling   TODO(Andrew): Should this be a mGridSize instead of 200?
    float kx = pi * (2 * x - mGridSize) / mVertexDistanceX;
    float kz = pi * (2 * z - mGridSize) / mVertexDistanceZ;
    return floor(sqrt(mGravitationalPull * sqrt(squared(kx) + squared(kz))) / w_0) * w_0;
  }


  // ------------------------------------
  float FFT_WaterSimulation::PhillipsSpectrum(int x, int z)
  {
    if (UsePhillips)
    {
      // FROM CONNOR NOT MINE
      glm::vec2 k;
      float m = z - (mGridSize / 2.0f);
      k.y = (2.0f * pi * m) / mVertexDistanceZ;
      float n = x - (mGridSize / 2.0f);
      k.x = (2.0f * pi * n) / mVertexDistanceX;

      float kMag = glm::length(k);
      float wMag = glm::length(mWindFactor);

      glm::vec2 wNorm = glm::normalize(mWindFactor);

      if (kMag < 0.000001f)
      {
        return 0.0f;
      }

      float kMag2 = kMag * kMag;
      float kMag4 = kMag2 * kMag2;

      float L = wMag * wMag / mGravitationalPull;

      float L2 = L * L;


      glm::vec2 kNorm = glm::normalize(k);
      float kdotw = glm::dot(kNorm, wNorm);
      float kdotw2 = kdotw * kdotw;

      float exponent = -1.0f / (kMag2 * L2);
      float damping = 0.001f;

      float l2 = L2 * damping * damping;

      float factor = exp(exponent) / kMag4;
      float factor2 = exp(-kMag2 * l2);

      return mWaveHeight * factor * kdotw2 * factor2;
      //// END FROM CONNOR NOT MINE
    }
    else
    {
      glm::vec2 k(pi * (2.0f * x - mGridSize) / mVertexDistanceX,
                  pi * (2.0f * z - mGridSize) / mVertexDistanceZ);

      float kLen = glm::length(k); // cached since it used a lot
      if (floatNotZero(kLen))
      {
        return 0.0f;  // value is so small that the phillips value will not be noticeable
      }

      // needed for later
      float kLen_Squared = squared(kLen);

      // apply the wind
      float kdotWind = glm::dot(glm::normalize(k), glm::normalize(mWindFactor));

      // find the L (wind speed) value, and apply the velocity to have
      // gravity too (limits winds effect)
      float L = squared(glm::length(mWindFactor)) / mGravitationalPull;
      float L_Squared = squared(L);

      //! what does this do?
      float damping = 0.001f;
      float l2 = L_Squared * squared(damping);

      return mWaveHeight * (exp(-1.0f / (kLen_Squared * L_Squared)) / squared(kLen_Squared)) * squared(kdotWind) * exp(-kLen_Squared * l2);
    }
  }


  // ------------------------------------
  complex FFT_WaterSimulation::Calc_hTilde(int x, int z)
  {
    int vertex = z * mGridSizePlus1 + x;

    complex hT0(mComputationalVertices[vertex].mHTilde0.x, mComputationalVertices[vertex].mHTilde0.y);
    complex hT0conj(mComputationalVertices[vertex].mHTilde0mkConjugate.x,
                    mComputationalVertices[vertex].mHTilde0mkConjugate.y);

    // disperse the waves
    float disp = static_cast<float>(Dispersion(x, z) * mTime);

    // Euler's Formula to get around SimpMath::complex exp
    float cosine = cos(disp);
    float sine = sin(disp);

    // note that we do not do the cos and sin again since this is run every frame
    complex unit0(cosine, sine);
    complex unit1;
    if (UseHTilde)
    {
      // FROM CONNOR
      unit1 = complex(-cosine, -sine);
      // END FROM CONNOR
    }
    else
    {
      unit1 = complex(cosine, -sine);
    }

    // return the completed function
    return hT0 * unit0 + hT0conj * unit1;
  }


  // ------------------------------------
  complex FFT_WaterSimulation::Calc_hTildeSub0(int x, int z)
  {
    // the 2.0f here is an additional 1 / sqrt(2.0f) that is multiplied to the phillips,
    return RandomGuassian() * sqrt(PhillipsSpectrum(x, z) / 2.0f);
  }


  // ------------------------------------
  void FFT_WaterSimulation::WaveGeneration()
  {
    YTEProfileFunction();

    // loop all of the vertices and do something similar to the h_D_and_n func
    for (int z = 0; z < mGridSize; ++z)
    {
      glm::vec2 k;

      if (UseNewKs)
      {
        // FROM CONNOR
        float m = z - (mGridSize / 2.0f);
        k.y = (pi * m) / mVertexDistanceZ;
        // END FROM CONNOR
      }
      else
      {
        k.y = pi * (2.0f * z - mGridSize) / mVertexDistanceZ;
      }
      
      for (int x = 0; x < mGridSize; ++x)
      {
        if (UseNewKs)
        {
          // FROM CONNOR
          float n = x - (mGridSize / 2.0f);
          k.x = (pi * n) / mVertexDistanceX;
          // END FROM CONNOR
        }
        else
        {
          k.x = pi * (2.0f * x - mGridSize) / mVertexDistanceX;
        }

        float kLen = glm::length(k);
        int vertex = z * mGridSize + x;

        auto &h_Tilde = mH_Tilde[vertex];

        h_Tilde = Calc_hTilde(x, z);
        mH_TildeSlopeX[vertex] = h_Tilde * complex(0, k.x);    
        mH_TildeSlopeZ[vertex] = h_Tilde * complex(0, k.y); 

        if (floatNotZero(kLen))
        {
          mH_TildeDX[vertex] = complex(0.0f, 0.0f);
          mH_TildeDZ[vertex] = complex(0.0f, 0.0f);
        }
        else
        {
          mH_TildeDX[vertex] = h_Tilde * complex(0, -(k.x) / kLen);
          mH_TildeDZ[vertex] = h_Tilde * complex(0, -(k.y) / kLen);
        }
      }
    }


    RunKFFT();


    // Apply these new values to the system
    float sign; // because we bit twiddled
    float signs[] = { 1.0f, -1.0f };
    for (int z = 0; z < mGridSizePlus1; ++z)
    {
      for (int x = 0; x < mGridSizePlus1; ++x)
      {
        int tilde = z * mGridSize + x; // accessor to the h_tilde
        int vertex = z * mGridSizePlus1 + x; // accessor to the vertex

        auto &computationalVertex = mComputationalVertices[vertex];
        auto &vertexAtMainIndex = mVertices[vertex];

        if (z >= mGridSize || x >= mGridSize)
        {
          // update draw list
          vertexAtMainIndex.mPosition = computationalVertex.mPosition;
          vertexAtMainIndex.mNormal = computationalVertex.mNormal;
          continue;
        }

        sign = signs[(x + z) & 1];

        mH_Tilde[tilde] *= sign;   // why?

                                   // height adjustment
        computationalVertex.mPosition.y = (mH_Tilde[tilde].mReal);

        if (UseNoDisplacement)
        {
          // FROM CONNOR
          // they do not use displacement at all, so no DX, no DZ, no position x and z updating, only position y updating
          computationalVertex.mPosition.x = computationalVertex.mOriginalPosition.x;
          computationalVertex.mPosition.z = computationalVertex.mOriginalPosition.z;
          // END FROM CONNOR
        }
        else
        {
          auto &hTildeDX = mH_TildeDX[tilde];
          auto &hTildeDZ = mH_TildeDZ[tilde];

          // displacement update
          hTildeDX *= sign;
          hTildeDZ *= sign;  // stupid lambda is being used in this statement as the -1.0f!!
          computationalVertex.mPosition.x = computationalVertex.mOriginalPosition.x +
            hTildeDX.mReal * -1.0f;
          computationalVertex.mPosition.z = computationalVertex.mOriginalPosition.z +
            hTildeDZ.mReal * -1.0f;
        }


        // normal update
        auto &hTildeSlopeX = mH_TildeSlopeX[tilde];
        auto &hTildeSlopeZ = mH_TildeSlopeZ[tilde];
        hTildeSlopeX *= sign;
        hTildeSlopeZ *= sign;
        computationalVertex.mNormal = glm::normalize(glm::vec3(-(hTildeSlopeX.mReal),
                                                               1.0f,
                                                               -(hTildeSlopeZ.mReal)));
        // tiling
        bool useNoDis = UseNoDisplacement;
        auto tiling = [&vertex, &tilde, &useNoDis](std::vector<WaterComputationalVertex>& aVertices, 
                                                   complex_kfft& aH_TildeDX, 
                                                   complex_kfft& aH_TildeDZ, 
                                                   int aIndex)
        {
          auto &vertexAtIndex = aVertices[aIndex];
          auto &vertexAtVertex = aVertices[vertex];

          vertexAtIndex.mPosition.y = vertexAtVertex.mPosition.y;

          if (useNoDis)
          {
            // FROM CONNOR
            // dont use displacement
            vertexAtIndex.mPosition.x = vertexAtIndex.mOriginalPosition.x;
            vertexAtIndex.mPosition.z = vertexAtIndex.mOriginalPosition.z;
            // END FROM CONNOR
          }
          else
          {
            vertexAtIndex.mPosition.x = vertexAtIndex.mOriginalPosition.x +
                                            aH_TildeDX[tilde].mReal * -1.0f;
            vertexAtIndex.mPosition.z = vertexAtIndex.mOriginalPosition.z +
                                            aH_TildeDZ[tilde].mReal * -1.0f;
          }

          vertexAtIndex.mNormal = vertexAtVertex.mNormal;
        };

        // tilling
        if (x == 0 && z == 0)
        {
          //int index = squared(mGridSize) - 1;
          int index = mGridSize + (mGridSizePlus1 * mGridSize);
          tiling(mComputationalVertices, mH_TildeDX, mH_TildeDZ, index);
        }
        if (x == 0)
        {
          //int index = vertex + (mGridSize - 1);
          int index = vertex + mGridSize;
          tiling(mComputationalVertices, mH_TildeDX, mH_TildeDZ, index);
        }
        if (z == 0)
        {
          //int index = vertex + (mGridSize * (mGridSize - 1));
          int index = vertex + (mGridSizePlus1 * mGridSize);
          tiling(mComputationalVertices, mH_TildeDX, mH_TildeDZ, index);
        }



        // update draw list
        vertexAtMainIndex.mPosition = computationalVertex.mPosition;
        vertexAtMainIndex.mNormal = computationalVertex.mNormal;
      }
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetTimeDilationEffect(float aTimeDilationEffect)
  {
    if (aTimeDilationEffect < 0.0f)
    {
      std::cout << "FFT_WaterSimulation Warning: Time Dilation cannot be set to a negative number"
        << std::endl;
      return;
    }
    else
    {
      mTimeDilationEffect = static_cast<double>(aTimeDilationEffect);
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetVertexDistance(glm::vec2 aDistance)
  {
    if (aDistance.x < 0.0f || aDistance.y < 0.0f)
    {
      std::cout << "WaterSimulation Warning: Vertex Distance must be positive" << std::endl;
      return;
    }

    mVertexDistanceX = aDistance.x;
    mVertexDistanceZ = aDistance.y;

    mResetNeeded = true;
  }


  // ------------------------------------
  void FFT_WaterSimulation::UpdateTime(double dt)
  {
    mTime += dt * mTimeDilationEffect;
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetGridSize(int aGridSize)
  {
    if (aGridSize != 2 && aGridSize != 4 && aGridSize != 8 && aGridSize != 16 && aGridSize != 32 &&
        aGridSize != 64 && aGridSize != 128 && aGridSize != 256 && aGridSize != 512 && aGridSize != 1024)
    {
      mOwner->GetEngine()->Log(LogType::Error, fmt::format("WaterSimulation: Passed a GridSize of {} which is not a power of 2! Default GridSize of 32 will be used.\n", aGridSize));

      const int defaultGridSize = 32;  // 32 was chosen since its small so easily computed

                                       // update the vertex distances if they were the same as the grid size
      if (mGridSize == mVertexDistanceX)
      {
        mVertexDistanceX = defaultGridSize;
      }
      if (mGridSize == mVertexDistanceZ)
      {
        mVertexDistanceZ = defaultGridSize; // also set here since they were set the same above
      }

      mGridSize = defaultGridSize;
      mGridSizePlus1 = mGridSize + 1;
    }
    else
    {
      mGridSize = aGridSize;
      mGridSizePlus1 = mGridSize + 1;
    }

    mResetNeeded = true;
    mGridSize = DefaultGridSize;
    mGridSizePlus1 = DefaultGridSize + 1;
  }
  

  // ------------------------------------
  void FFT_WaterSimulation::SetGravitationalPull(float aGravitationalPull)
  {
    mGravitationalPull = aGravitationalPull;
    mResetNeeded = true;
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetWindFactor(glm::vec2 aWindFactor)
  {
    mWindFactor = aWindFactor;
    mResetNeeded = true;
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetWaveHeight(float aWaveHeight)
  {
    mWaveHeight = aWaveHeight / 1000.0f;
    mResetNeeded = true;
  }
  

  // ------------------------------------
  void FFT_WaterSimulation::SetReset(bool aReset)
  {
    mReset = aReset;

    if (mReset == true)
    {
      mResetNeeded = true;
      mReset = false;
    }
  }


  // ------------------------------------
  float FFT_WaterSimulation::GetGravitationalPull()
  {
    return mGravitationalPull;
  }


  // ------------------------------------
  int FFT_WaterSimulation::GetGridSize()
  {
    return mGridSize;
  }


  // ------------------------------------
  float FFT_WaterSimulation::GetTimeDilationEffect()
  {
    return static_cast<float>(mTimeDilationEffect);
  }


  // ------------------------------------
  glm::vec2 FFT_WaterSimulation::GetVertexDistance()
  {
    return glm::vec2(mVertexDistanceX, mVertexDistanceZ);
  }


  // ------------------------------------
  float FFT_WaterSimulation::GetVertexDistanceX()
  {
    return mVertexDistanceX;
  }


  // ------------------------------------
  float FFT_WaterSimulation::GetVertexDistanceZ()
  {
    return mVertexDistanceZ;
  }


  // ------------------------------------
  float FFT_WaterSimulation::GetWaveHeight()
  {
    return mWaveHeight * 1000.0f;
  }
  

  // ------------------------------------
  glm::vec2 FFT_WaterSimulation::GetWindFactor()
  {
    return mWindFactor;
  }


  // ------------------------------------
  bool FFT_WaterSimulation::GetReset()
  {
    return mReset;
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetInstancingAmount(int aAmount)
  {
    if (aAmount < 1)
    {
      mInstanceCount = 1;
      std::cout << "WaterSimulation Warning: Instancing amount cannot be below 1" << std::endl;
    }
    else
    {
      mInstanceCount = aAmount;
    }

    if (!mConstructing)
    {
      InstanceReset();
    }
  }

  
  // ------------------------------------
  int FFT_WaterSimulation::GetInstancingAmount()
  {
    return mInstanceCount;
  }


  // ------------------------------------
  void FFT_WaterSimulation::CreateTransform()
  {
    if (mTransform == nullptr)
    {
      return;
    }

    for (int z = 0; z < mInstanceCount; ++z)
    {
      for (int x = 0; x < mInstanceCount; ++x)
      {
        int i = z * mInstanceCount + x;

        mInstancingMatrices[i].mModelMatrix = glm::mat4(1.0f) * glm::toMat4(mTransform->GetWorldRotation());
        mInstancingMatrices[i].mModelMatrix = glm::scale(mInstancingMatrices[i].mModelMatrix, mTransform->GetWorldScale());

        glm::vec3 trans = mTransform->GetWorldTranslation();
        //trans.x += (x * (mTransform->GetWorldScale().x + (mVertexDistanceX - 3)));
        //trans.z += (z * (mTransform->GetWorldScale().z + (mVertexDistanceZ - 3)));
        trans.x += mVertexDistanceX * x;
        trans.z += mVertexDistanceZ * -z;


        mInstancingMatrices[i].mModelMatrix = glm::translate(mInstancingMatrices[i].mModelMatrix, trans);
      }
    }
  }

  
  // ------------------------------------
  void FFT_WaterSimulation::AdjustPositions()
  {
    CreateTransform();
    
    for (int i = 0; i < mInstantiatedHeightmap.size(); ++i)
    {
      mInstantiatedHeightmap[i]->GetInstantiatedModel()->UpdateUBOModel(mInstancingMatrices[i]);
    }

    VkRenderer* vkrender = static_cast<VkRenderer*>(mRenderer);
    vkrender->GetSurface(mGraphicsView->GetWindow())->GetViewData(mGraphicsView)->mWaterInfluenceMapManager.SetBaseHeight(mTransform->GetTranslation().y);
  }


  // ------------------------------------
  void FFT_WaterSimulation::Update(LogicUpdate* aEvent)
  {
    YTEProfileFunction();

    if (mResetNeeded)
    {
      Reset();
      mResetNeeded = false;
    }

    UpdateTime(aEvent->Dt);
    Render();
  }


  // ------------------------------------
  void FFT_WaterSimulation::EditorUpdate(LogicUpdate* aEvent)
  {
    YTEProfileFunction();

    if (!mSpace->GetEngine()->IsEditor())
    {
      return;
    }

    if (mSpace->GetName() == "YTEditor Play Space")
    {
      return;
    }

    if (mResetNeeded)
    {
      Reset();
      mResetNeeded = false;
    }

    if (mRunWithEngineUpdate && !mRunInSteps)
    {
      UpdateTime(aEvent->Dt);
      Render();
    }
    else if (mRunWithEngineUpdate && mRunInSteps)
    {
      if (mSteps >= mStepsCount)
      {
        mSteps = 0;
        UpdateTime(aEvent->Dt * (mStepsCount + 1));
        Render();
      }
      else
      {
        ++mSteps;
      }
    }
  }

  glm::vec3 FFT_WaterSimulation::GetHeight(float x, float z)
  {
    auto firstMatrix = mInstancingMatrices[0];
    auto world2Instance = glm::inverse(firstMatrix.mModelMatrix);
    glm::vec4 point(x, 0, z, 1);
    point = world2Instance * point;

    //auto& map = mInstantiatedHeightmap[0];
    int x_coord = static_cast<int>(std::round(point.x)) % mGridSize;
    int z_coord = static_cast<int>(std::round(point.z)) % mGridSize;

    while (x_coord < 0)
    {
      x_coord += mGridSize;
    }
    while (z_coord < 0)
    {
      z_coord += mGridSize;
    }

    unsigned int index = z_coord * mGridSizePlus1 + x_coord;

    while (index > mVertices.size())
    {
      index -= static_cast<unsigned int>(mVertices.size());
    }

    auto vert = mVertices[index];

    point = firstMatrix.mModelMatrix * glm::vec4(vert.mPosition, 1);

    // influence maps [ MUST COPY THE SHADER LOGIC ]
    auto manager = static_cast<VkRenderer*>(mRenderer)->GetAllWaterInfluenceMaps(mGraphicsView);
    auto& data = manager->mWaterInformationData;

    float HeightInfluence = 1.0f;

    for (unsigned i = 0; i < data.mNumberOfInfluences; ++i)
    {
      UBOWaterInfluenceMap& map = data.mInformation[i];
      glm::vec2 vertToCenter = glm::vec2(point.x, point.z) - glm::vec2(map.mCenter.x, map.mCenter.z);
      float vtcLen = glm::length(vertToCenter);

      if (map.mActive > 0)
      {
        if (vtcLen <= map.mRadius)
        {
          // influence 
          // converts the length to a value of 0 being the center, and 1 being the radius distance from the center
          float influenceAmount = vtcLen / map.mRadius;

          if (map.mWaveIntensity >= 0.0f)
          {
            
            // 0 will make the height be the base height
            // 1 will not change the height
            if (0 == map.mWaveInfluenceFunction)
            {
              HeightInfluence *= (influenceAmount * (1.0f - map.mWaveIntensity));
            }
            else if (1 == map.mWaveInfluenceFunction)
            {
              HeightInfluence *= ((influenceAmount * influenceAmount) * (1.0f - map.mWaveIntensity));
            }
            else if (2 == map.mWaveInfluenceFunction)
            {
              HeightInfluence *= ((influenceAmount * influenceAmount * influenceAmount) * (1.0f - map.mWaveIntensity));
            }
            else if (3 == map.mWaveInfluenceFunction)
            {
              HeightInfluence *= (log2(influenceAmount) * (1.0f - map.mWaveIntensity));
            }
            //HeightInfluence *= (influenceAmount * (1.0f - map.mWaveIntensity));
          }  
        }
      }
    }

    float yPos = point.y;
    yPos -= data.mBaseHeight;
    yPos *= HeightInfluence;
    yPos += data.mBaseHeight;
    point.y = yPos;

    return glm::vec3(point.x, point.y, point.z);
  }


  // ------------------------------------
  const std::vector<Vertex>& FFT_WaterSimulation::GetVertices()
  {
    return mVertices;
  }

  // ------------------------------------
  void FFT_WaterSimulation::CreateHeightmap()
  {
    // create objects
    std::string guid = mOwner->GetGUID().ToIdentifierString();
    std::string name = fmt::format("{}_heightmap_{}", guid, mGridSize);
    int size = squared(mInstanceCount);

    for (int i = 0; i < size; ++i)
    {
      mInstantiatedHeightmap.push_back(std::make_unique<InstantiatedHeightmap>());
        mInstantiatedHeightmap[i]->Initialize(name,
          mVertices,
          mIndices,
          mShaderSetName,
          mGraphicsView,
          mRenderer,
          "water.png",
          "foam.png",
          "copywritePerlinNoiseTextureNormal.png");
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::DestroyHeightmap()
  {
    for (int i = 0; i < mInstantiatedHeightmap.size(); ++i)
    {
      mInstantiatedHeightmap[i].reset(nullptr);
    }

    mInstantiatedHeightmap.clear();
  }


  // ------------------------------------
  void FFT_WaterSimulation::UpdateHeightmap()
  {
    YTEProfileFunction();

    // update
    mInstantiatedHeightmap[0]->UpdateMesh(mVertices, mIndices);
  }


  // ------------------------------------
  void FFT_WaterSimulation::InstanceReset()
  {
    mInstancingMatrices.resize(squared(mInstanceCount));
    DestroyHeightmap();
    CreateHeightmap();
    AdjustPositions();
  }



  // ------------------------------------
  std::vector<InstantiatedModel*> FFT_WaterSimulation::GetInstantiatedModel()
  {
    if (mInstantiatedHeightmap.size())
    {
      std::vector<InstantiatedModel*> models;
      for (size_t i = 0; i < mInstantiatedHeightmap.size(); ++i)
      {
        models.push_back(mInstantiatedHeightmap[i]->GetInstantiatedModel());
      }
      return models;
    }

    return { };
  }

  
  
  // ------------------------------------
  void FFT_WaterSimulation::ReloadShaders()
  {
    InstanceReset();
  }


  // ------------------------------------
  Any FFT_WaterSimulation::MT_A(JobHandle & aJob)
  {
    YTEUnusedArgument(aJob);
    kiss_fftnd(mKFFTConfig[0], mH_Tilde.GetKFFTArray(), mH_Tilde.GetKFFTArray());
    return Any();
  }
  Any FFT_WaterSimulation::MT_B(JobHandle & aJob)
  {
    YTEUnusedArgument(aJob);
    kiss_fftnd(mKFFTConfig[1], mH_TildeSlopeX.GetKFFTArray(), mH_TildeSlopeX.GetKFFTArray());
    return Any();
  }
  Any FFT_WaterSimulation::MT_C(JobHandle & aJob)
  {
    YTEUnusedArgument(aJob);
    kiss_fftnd(mKFFTConfig[2], mH_TildeSlopeZ.GetKFFTArray(), mH_TildeSlopeZ.GetKFFTArray());
    return Any();
  }
  Any FFT_WaterSimulation::MT_D(JobHandle & aJob)
  {
    YTEUnusedArgument(aJob);
    kiss_fftnd(mKFFTConfig[3], mH_TildeDX.GetKFFTArray(), mH_TildeDX.GetKFFTArray());
    return Any();
  }
  Any FFT_WaterSimulation::MT_E(JobHandle & aJob)
  {
    YTEUnusedArgument(aJob);
    kiss_fftnd(mKFFTConfig[4], mH_TildeDZ.GetKFFTArray(), mH_TildeDZ.GetKFFTArray());
    return Any();
  }
}

#undef DefaultGridSize