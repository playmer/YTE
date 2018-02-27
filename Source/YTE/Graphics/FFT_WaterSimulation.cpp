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

#include "YTE/Graphics/GraphicsView.hpp" 
#include "YTE/Graphics/GraphicsSystem.hpp" 
#include "YTE/Graphics/FFT_WaterSimulation.hpp"
#include "YTE/Graphics/Generics/InstantiatedHeightmap.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Vulkan/VkRenderer.hpp"

#include "YTE/Physics/Orientation.hpp"
#include "YTE/Physics/Transform.hpp"

#include "YTE/Math/Random.hpp"
#include "YTE/Math/Values.hpp"

#include <iomanip>

// just makes things faster than always adding one
// note that this is undefined at the end of the file
#define mGridSizePlus1 (mGridSize)
#define DefaultGridSize 64

// --------------------------
// Definitions

namespace YTE
{
  //static std::vector<std::string> PopulateDropDownList(Component *aComponent)
  //{
  //  YTEUnusedArgument(aComponent);
  //  return { "Solid", "Wireframe" };
  //}

  YTEDefineType(FFT_WaterSimulation)
  {
    YTERegisterType(FFT_WaterSimulation);
    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { Transform::GetStaticType() } };

    FFT_WaterSimulation::GetStaticType()->AddAttribute<ComponentDependencies>(deps);

    //YTEBindProperty(&FFT_WaterSimulation::GetRenderMode, &FFT_WaterSimulation::SetRenderMode, "Rendering Mode")
    //  .AddAttribute<EditorProperty>()
    //  .AddAttribute<DropDownStrings>(PopulateDropDownList)
    //  .SetDocumentation("The type of rendering you want to use. Options include:\n"
    //                    " - Solid: Rendering using solid triangles\n"
    //                    " - Wireframe: Rendering using lines between the triangles");

    YTEBindProperty(&FFT_WaterSimulation::GetTimeDilationEffect, &FFT_WaterSimulation::SetTimeDilationEffect, "Time Dilation")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Adjusts the wave's speed. Can be used to slow or quicken the pace of the algorithm");

    YTEBindProperty(&FFT_WaterSimulation::GetVertexDistance, &FFT_WaterSimulation::SetVertexDistance, "Vertex Distance")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Adjusts the distance between each individual vertex. This will expand or shrink the overall size of the mesh");

    //YTEBindProperty(&FFT_WaterSimulation::GetInstancingAmount, &FFT_WaterSimulation::SetInstancingAmount, "Grid Size")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("Adjusts the size of the grid that the water is repeated on.");

    YTEBindProperty(&FFT_WaterSimulation::GetReset, &FFT_WaterSimulation::SetReset, "Reset Simulation")
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Completely resets the simulation to run from scratch again");

    YTEBindProperty(&FFT_WaterSimulation::GetGravitationalPull, &FFT_WaterSimulation::SetGravitationalPull, "Gravitational Pull")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the gravitational pull on the waves in the y (up) direction. This requires a reset");

    YTEBindProperty(&FFT_WaterSimulation::GetGridSize, &FFT_WaterSimulation::SetGirdSize, "Complexity")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the size of the size of the N*N gird used to calculate the water. This requires a reset");

    YTEBindProperty(&FFT_WaterSimulation::GetWaveHeight, &FFT_WaterSimulation::SetWaveHeight, "Wave maximum height")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the max wave height the algorithm can achieve. This requires a reset");

    YTEBindProperty(&FFT_WaterSimulation::GetWindFactor, &FFT_WaterSimulation::SetWindFactor, "Wind Factor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Sets the wind direction and magnitude. This requires a reset");

    //YTEBindProperty(&FFT_WaterSimulation::GetUseFFT, &FFT_WaterSimulation::SetUseFFT, "Use FFT")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("If checked, the system will use a FFT to calculate the water");

    YTEBindProperty(&FFT_WaterSimulation::GetShaderSetName, &FFT_WaterSimulation::SetShaderSetName, "Shader Set Name")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("What shader to use for the object");

    // material
    //YTEBindProperty(&FFT_WaterSimulation::GetDiffuse, &FFT_WaterSimulation::SetDiffuse, "Diffuse")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("The diffuse color");
    //
    //YTEBindProperty(&FFT_WaterSimulation::GetAmbient, &FFT_WaterSimulation::SetAmbient, "Ambient")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("The ambient color");
    //
    //YTEBindProperty(&FFT_WaterSimulation::GetSpecular, &FFT_WaterSimulation::SetSpecular, "Specular")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("The specular color");
    //
    //YTEBindProperty(&FFT_WaterSimulation::GetEmissive, &FFT_WaterSimulation::SetEmissive, "Emissive")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("The emissive color");
    //
    //YTEBindProperty(&FFT_WaterSimulation::GetShinniness, &FFT_WaterSimulation::SetShinniness, "Shininess")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("The shininess component");
    //
    //YTEBindProperty(&FFT_WaterSimulation::GetOpacity, &FFT_WaterSimulation::SetOpacity, "Opacity")
    //  .AddAttribute<EditorProperty>()
    //  .SetDocumentation("The opacity component");

    YTEBindProperty(&FFT_WaterSimulation::GetRunWithEngineUpdate, &FFT_WaterSimulation::SetRunWithEngineUpdate, "Run in Editor")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Run while in editor");
    YTEBindProperty(&FFT_WaterSimulation::GetRunInSteps, &FFT_WaterSimulation::SetRunInSteps, "Dont Run Every Frame")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Prevents the FFT from running every frame");
    YTEBindProperty(&FFT_WaterSimulation::GetStepCount, &FFT_WaterSimulation::SetStepCount, "Run in __ Frames")
      .AddAttribute<Serializable>()
      .AddAttribute<EditorProperty>()
      .SetDocumentation("Runs the FFT every ___ frames");

    YTEBindProperty(&FFT_WaterSimulation::GetInstancingAmount, &FFT_WaterSimulation::SetInstancingAmount, "Instance Count")
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
    , mRenderMode(RenderMode::Triangle)   // triangle by default
    , mGravitationalPull(1000.0f)           // normal gravity
    , mGridSize(DefaultGridSize)          // grid size of 32 just for now
    , mWaveHeight(0.0001f)                // normal wave height
    , mWindFactor(1280.0f, 640.0f)           // normal wind
    , mVertexDistanceX(DefaultGridSize)   // same as grid size
    , mVertexDistanceZ(DefaultGridSize)   // ""
    , mTimeDilationEffect(0.1f)           // no time dilation at all
    , mTime(0.0f)                         // we start with no time
    , mReset(false)                       // dont reset
    , mKFFTConfig(NULL)
    , mTransform(nullptr)
    , mUseFFT(true)
    , mShaderSetName("FFT_WaterSimulation")
    , mResetNeeded(true)
    , mStartingMaterial(glm::vec4(0.5f, 0.75f, 1.0f, 1.0f),
                        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f),
                        glm::vec4(0.75f, 0.75f, 0.75f, 1.0f),
                        glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
                        glm::vec4(1.0f),
                        glm::vec4(1.0f),
                        1.0f,
                        1000.0f,
                        1000.0f,
                        1.0f,
                        1.0f,
                        1.0f)
    , mRunWithEngineUpdate(true)
    , mRunInSteps(true)
    , mSteps(0)
    , mStepsCount(5)
    , mInstanceCount(1)
    , mConstructing(true)
  {
    auto engine = aSpace->GetEngine();
    mGraphicsView = mSpace->GetComponent<GraphicsView>();
    mRenderer = dynamic_cast<VkRenderer*>(engine->GetComponent<GraphicsSystem>()->GetRenderer());
    mWindow = mSpace->GetComponent<GraphicsView>()->GetWindow();
    DeserializeByType(aProperties, this, FFT_WaterSimulation::GetStaticType());
  }


  // ------------------------------------
  void FFT_WaterSimulation::Initialize()
  {
    mOwner->YTERegister(Events::PositionChanged, this, &FFT_WaterSimulation::TransformEvent);
    mOwner->YTERegister(Events::ScaleChanged, this, &FFT_WaterSimulation::TransformEvent);
    mOwner->YTERegister(Events::RotationChanged, this, &FFT_WaterSimulation::TransformEvent);
    mSpace->GetEngine()->YTERegister(Events::LogicUpdate, this, &FFT_WaterSimulation::EditorUpdate);
    mSpace->YTERegister(Events::LogicUpdate, this, &FFT_WaterSimulation::Update);
    mTransform = mOwner->GetComponent<Transform>();

    mConstructing = false;

    srand(100);
    Destruct();
    Construct();
    StartKFFT();
    InstanceReset();

    //AdjustPositions();
  }


  // ------------------------------------
  void FFT_WaterSimulation::Reset()
  {
    mTime = 0.0f;
    Destruct();
    Construct();
    InstanceReset();
    //AdjustPositions();

    //std::cout << "Water Simulation Object Reset:"
    //  << "\n\t\tGridSize:             " << mGridSize
    //  << "\n\t\tGravity Factor:       " << mGravitationalPull
    //  << "\n\t\tWave Height:          " << mWaveHeight
    //  << "\n\t\tWind Factor:          " << mWindFactor.x << ", " << mWindFactor.y
    //  << "\n\t\tVertex Distance:      " << mVertexDistanceX << ", " << mVertexDistanceZ
    //  << "\n\t\tTime Dilation:        " << mTimeDilationEffect
    //  << "\n\t\tInstanceing Distance: " << mInstancingDistance << std::endl;
  }


  // ------------------------------------
  void FFT_WaterSimulation::Construct()
  {
    mH_Tilde.GetVector().resize(squared(mGridSize));
    mH_TildeSlopeX.GetVector().resize(squared(mGridSize));
    mH_TildeSlopeZ.GetVector().resize(squared(mGridSize));
    mH_TildeDX.GetVector().resize(squared(mGridSize));
    mH_TildeDZ.GetVector().resize(squared(mGridSize));

    mDrawableVertices.resize(squared(mGridSizePlus1));
    mDrawableIndices_Wireframe.reserve((squared(mGridSize) * 6) + (4 * mGridSize));
    mDrawableIndices_Triangle.reserve((squared(mGridSize) * 6));
    mVertices.resize(squared(mGridSizePlus1));

    mInstancingMatrices.resize(squared(mInstanceCount));
    //AdjustPositions();


    // ------------------------------------
    // fill out the vertices initial data


    for (int z = 0; z < mGridSize; ++z)
    {
      for (int x = 0; x < mGridSize; ++x)
      {
        int vertex = z * mGridSize + x; // 2D pointer arithmetic for a 1D array

                                        // calls phillips which sets up the wind factor, and the wave heights
        complex ht0 = Calc_hTildeSub0(x, z);
        complex ht0conj = Calc_hTildeSub0(-x, -z).conjugate();

        // set initial data for actual water simulation (wind factor and wave heights are used)
        mDrawableVertices[vertex].mHTilde0 = glm::vec3(ht0.mReal, ht0.mImaginary, 0.0f);
        mDrawableVertices[vertex].mHTilde0mkConjugate = glm::vec3(ht0conj.mReal,
                                            ht0conj.mImaginary, 0.0f);
        mVertices[vertex].mTextureCoordinates.x = static_cast<float>(x) / static_cast<float>((mGridSize / 2) - 1);
        mVertices[vertex].mTextureCoordinates.y = static_cast<float>(z) / static_cast<float>((mGridSize / 2) - 1);

        // sets positions, and uses the length parameter to space out the grid in 3D space
        // x - (gridSize / 2.0f) = the physical position of the vertex without length expansion
        // (vertexDistance / gridSize) = the length expansion or reduction
        // multiply together to find the grid position
        // note that y has no adjustments
        glm::vec3 pos(((x - (mGridSize / 2.0f)) * (mVertexDistanceX / mGridSize)), 0.0f,
          ((z - (mGridSize / 2.0f)) * (mVertexDistanceZ / mGridSize)));
        mDrawableVertices[vertex].mPosition = mDrawableVertices[vertex].mOriginalPosition = pos;

        // initial normal is just the basic normal
        mDrawableVertices[vertex].mNormal = glm::vec3(0.0f, 1.0f, 0.0f);

        //std::cout << "For " << vertex << ": " << mDrawableVertices[vertex].mOriginalPosition.x
        //  << ", " << mDrawableVertices[vertex].mOriginalPosition.y
        //  << ", " << mDrawableVertices[vertex].mOriginalPosition.z
        //  << " vs " << mDrawableVertices[vertex].mPosition.x
        //  << ", " << mDrawableVertices[vertex].mPosition.y
        //  << ", " << mDrawableVertices[vertex].mPosition.z << std::endl;
      }
    }


    int indexCounter_Line = 0;
    //int indexCounter_Tri = 0;
    for (int z = 0; z < mGridSize - 1; ++z)
    {
      for (int x = 0; x < mGridSize - 1; ++x)
      {
        int vertex = z * mGridSize + x; // 2D pointer arithmetic for a 1D array
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
        //if (x >= mGridSize || z >= mGridSize)
        //{
        //  continue;
        //}

        // triangles
        //std::cout << vertex << " | " << mDrawableVertices[vertex].mPosition.x << ", " << mDrawableVertices[vertex].mPosition.y << mDrawableVertices[vertex].mPosition.z << std::endl;
        //std::cout << vertex + mGridSize << " | " << mDrawableVertices[vertex + mGridSize].mPosition.x << ", " << mDrawableVertices[vertex + mGridSize].mPosition.y << mDrawableVertices[vertex + mGridSize].mPosition.z << std::endl;
        //std::cout << vertex + mGridSize + 1 << " | " << mDrawableVertices[vertex + mGridSize + 1].mPosition.x << ", " << mDrawableVertices[vertex + mGridSize + 1].mPosition.y << mDrawableVertices[vertex + mGridSize + 1].mPosition.z << std::endl;
        //std::cout << "==========================" << std::endl;
        //std::cout << vertex << " | " << mDrawableVertices[vertex].mPosition.x << ", " << mDrawableVertices[vertex].mPosition.y << mDrawableVertices[vertex].mPosition.z << std::endl;
        //std::cout << vertex + mGridSize + 1 << " | " << mDrawableVertices[vertex + mGridSize + 1].mPosition.x << ", " << mDrawableVertices[vertex + mGridSize + 1].mPosition.y << mDrawableVertices[vertex + mGridSize + 1].mPosition.z << std::endl;
        //std::cout << vertex + 1 << " | " << mDrawableVertices[vertex + 1].mPosition.x << ", " << mDrawableVertices[vertex + 1].mPosition.y << mDrawableVertices[vertex + 1].mPosition.z << std::endl;
        //std::cout << "==========================" << std::endl;

        mDrawableIndices_Triangle.push_back(vertex);
        mDrawableIndices_Triangle.push_back(vertex + mGridSize);
        mDrawableIndices_Triangle.push_back(vertex + mGridSize + 1);
        mDrawableIndices_Triangle.push_back(vertex);
        mDrawableIndices_Triangle.push_back(vertex + mGridSize + 1);
        mDrawableIndices_Triangle.push_back(vertex + 1);




        // wireframe
        mDrawableIndices_Wireframe.push_back(vertex);
        mDrawableIndices_Wireframe.push_back(vertex + 1);
        mDrawableIndices_Wireframe.push_back(vertex);
        mDrawableIndices_Wireframe.push_back(vertex + mGridSizePlus1);
        mDrawableIndices_Wireframe.push_back(vertex);
        mDrawableIndices_Wireframe.push_back(vertex + mGridSizePlus1 + 1);

        mDrawableIndices_Wireframe[indexCounter_Line++] = vertex;
        mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + 1;
        mDrawableIndices_Wireframe[indexCounter_Line++] = vertex;
        mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + mGridSizePlus1;
        mDrawableIndices_Wireframe[indexCounter_Line++] = vertex;
        mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + mGridSizePlus1 + 1;

        // wrapping code
        if (x == mGridSize - 1)
        {
          mDrawableIndices_Wireframe.push_back(vertex + 1);
          mDrawableIndices_Wireframe.push_back(vertex + mGridSizePlus1 + 1);

          mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + 1;
          mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + mGridSizePlus1 + 1;
        }
        if (z == mGridSize - 1)
        {
          mDrawableIndices_Wireframe.push_back(vertex + 1);
          mDrawableIndices_Wireframe.push_back(vertex + mGridSizePlus1 + 1);

          mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + 1;
          mDrawableIndices_Wireframe[indexCounter_Line++] = vertex + mGridSizePlus1 + 1;
        }
        

        //mDrawableIndices_Triangle.push_back(vertex);
        //mDrawableIndices_Triangle.push_back(vertex + mGridSizePlus1);
        //mDrawableIndices_Triangle.push_back(vertex + mGridSizePlus1 + 1);
        //mDrawableIndices_Triangle.push_back(vertex);
        //mDrawableIndices_Triangle.push_back(vertex + mGridSizePlus1 + 1);
        //mDrawableIndices_Triangle.push_back(vertex + 1);

        //mDrawableIndices_Triangle.push_back(vertex + mGridSizePlus1);
        //mDrawableIndices_Triangle.push_back(vertex + mGridSizePlus1 + 1);
        //mDrawableIndices_Triangle.push_back(vertex);
        //mDrawableIndices_Triangle.push_back(vertex + mGridSizePlus1 + 1);
        //mDrawableIndices_Triangle.push_back(vertex + 1);
        //mDrawableIndices_Triangle.push_back(vertex);



        //mDrawableIndices_Triangle[indexCounter_Tri++] = vertex;
        //mDrawableIndices_Triangle[indexCounter_Tri++] = vertex + mGridSizePlus1;
        //mDrawableIndices_Triangle[indexCounter_Tri++] = vertex + mGridSizePlus1 + 1;
        //mDrawableIndices_Triangle[indexCounter_Tri++] = vertex;
        //mDrawableIndices_Triangle[indexCounter_Tri++] = vertex + mGridSizePlus1 + 1;
        //mDrawableIndices_Triangle[indexCounter_Tri++] = vertex + 1;
      }
    }


    //CreateHeightmap();
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
    mDrawableVertices.clear();
    mDrawableIndices_Wireframe.clear();
    mDrawableIndices_Triangle.clear();

    DestroyHeightmap();
  }


  // ------------------------------------
  void FFT_WaterSimulation::StartKFFT()
  {
    int size[] = { mGridSize, mGridSize };
    const int sizeDem = 2;
    mKFFTConfig = kiss_fftnd_alloc(size, sizeDem, 0, NULL, NULL);
    if (mKFFTConfig == NULL)
    {
      std::cout << "FFT_WaterSimulation: Failed to create KFFT Config! No FFT calculations will be made"
        << std::endl;
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::StopKFFT()
  {
    kiss_fft_cleanup();
    free(mKFFTConfig);
    mKFFTConfig = NULL;
  }


  // ------------------------------------
  void FFT_WaterSimulation::RunKFFT()
  {
    // perform the FFT on the rows of the water
    kiss_fftnd(mKFFTConfig, mH_Tilde.GetKFFTArray(), mH_Tilde.GetKFFTArray()); 
    kiss_fftnd(mKFFTConfig, mH_TildeSlopeX.GetKFFTArray(), mH_TildeSlopeX.GetKFFTArray());
    kiss_fftnd(mKFFTConfig, mH_TildeSlopeZ.GetKFFTArray(), mH_TildeSlopeZ.GetKFFTArray()); 
    kiss_fftnd(mKFFTConfig, mH_TildeDX.GetKFFTArray(), mH_TildeDX.GetKFFTArray()); 
    kiss_fftnd(mKFFTConfig, mH_TildeDZ.GetKFFTArray(), mH_TildeDZ.GetKFFTArray()); 
  }


  // ------------------------------------
  void FFT_WaterSimulation::Render(bool use_fft)
  {
    // evaluate the current position for the waves
    if (use_fft == false)
    {
      WaveGeneration_NoFFT();
    }
    else
    {
      WaveGeneration();
    }

    UpdateHeightmap();
  }


  // ------------------------------------
  float FFT_WaterSimulation::Dispersion(int x, int z)
  {
    // dispersion applies the choppiness of the waves
    // this function is called every frame
    float w_0 = 2.0f * pi / 200.0f; //! what is this doing? This does tiling
    float kx = pi * (2 * x - mGridSize) / mVertexDistanceX;
    float kz = pi * (2 * z - mGridSize) / mVertexDistanceZ;
    return floor(sqrt(mGravitationalPull * sqrt(squared(kx) + squared(kz))) / w_0) * w_0;
  }


  // ------------------------------------
  float FFT_WaterSimulation::PhillipsSpectrum(int x, int z)
  {
    glm::vec2 k(pi * (2 * x - mGridSize) / mVertexDistanceX,
                pi * (2 * z - mGridSize) / mVertexDistanceZ);

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

    // correct math:
    // A * (exp(-1.0f / (k_Len_Squared * L2)) / k_length4) * squared(k_dot_w);

    // original math:
    // A * (exp(-1.0f / (k_Len_Squared * L2)) / k_length4) * squared(squared(squared(k_dot_w)))
    //                                 * exp(-k_length2 * 12);
    //
    //return mWaveHeight * (exp(-1.0f / (kLen_Squared * L_Squared)) / squared(kLen_Squared)) *
    //   kdotWind * kdotWind * kdotWind * kdotWind * kdotWind * kdotWind * exp(-kLen_Squared * l2);
    //
    //wrong = return mWaveHeight * exp(-1.0f / (kLen_Squared * L_Squared)) / squared(kLen_Squared)
    // * kdotWind * kdotWind * kdotWind * kdotWind * kdotWind * kdotWind * exp(-kLen_Squared * l2);

    return mWaveHeight * (exp(-1.0f / (kLen_Squared * L_Squared)) / squared(kLen_Squared)) *
      (squared(squared(kdotWind)) * squared(kdotWind)) * exp(-kLen_Squared * l2);
  }


  // ------------------------------------
  complex FFT_WaterSimulation::Calc_hTilde(int x, int z)
  {
    int vertex = z * mGridSizePlus1 + x;

    complex hT0(mDrawableVertices[vertex].mHTilde0.x, mDrawableVertices[vertex].mHTilde0.y);
    complex hT0conj(mDrawableVertices[vertex].mHTilde0mkConjugate.x,
                    mDrawableVertices[vertex].mHTilde0mkConjugate.y);

    // disperse the waves
    float disp = static_cast<float>(Dispersion(x, z) * mTime);

    // Euler's Formula to get around SimpMath::complex exp
    float cosine = cos(disp);
    float sine = sin(disp);

    // note that we do not do the cos and sin again since this is run every frame
    complex unit0(cosine, sine);
    complex unit1(cosine, -sine);

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
  WaterComplexNormalVertex FFT_WaterSimulation::h_D_and_n(glm::vec2 pos)
  {
    // start out with empty data
    WaterComplexNormalVertex hDn{ complex(0.0f, 0.0f),
                                  glm::vec2(0.0f, 0.0f),
                                  glm::vec3(0.0f, 0.0f, 0.0f) };

    for (int z = 0; z < mGridSize; ++z)
    {
      glm::vec2 k;
      k.y = 2.0f * pi * (z - mGridSize / 2.0f) / mVertexDistanceX;

      for (int x = 0; x < mGridSize; ++x)
      {
        k.x = 2.0f * pi * (x - mGridSize / 2.0f) / mVertexDistanceX;

        float kdotPos = glm::dot(k, pos);

        // NOTE: K is supposed to have a mult by i (imaginary) here
        complex unit = complex(cos(kdotPos), sin(kdotPos)); 
        complex hTilde_unit = Calc_hTilde(x, z) * unit;

        // wave height is adjusted here based on surrounding waves
        hDn.mWaveHeight += hTilde_unit;


        // normals are adjusted here based on surrounding waves        
        hDn.mNormal += glm::vec3(-(k.x) * hTilde_unit.mImaginary, 0.0f,
           -(k.y) * hTilde_unit.mImaginary); // y here is z

        // cache
        float kLen = glm::length(k);

        if (floatNotZero(kLen))
        {
          // would have done div by zero, and would have added ~zero to the Displacement,
          // so well just skip
          continue; 
        }

        // Displacement is adjusted here                                        y is z here
        // NOTE: K is supposed to be mult by -i (imaginary) here, only the negative is shown
        hDn.mDisplacement += glm::vec2(k.x / kLen * hTilde_unit.mImaginary,
          k.y / kLen * hTilde_unit.mImaginary);
      }
    }

    // normalize the normal, and subtract the original placement for true placement
    hDn.mNormal = glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f) - hDn.mNormal);

    // replace the final missing i (imaginary) multiplies from the k here (note that squared(i) = -1.0f)
    hDn.mDisplacement.x *= -1.0f;
    hDn.mDisplacement.y *= -1.0f; // y here is z

    return hDn;
  }


  // ------------------------------------
  void FFT_WaterSimulation::WaveGeneration()
  {
    // loop all of the vertices and do something similar to the h_D_and_n func
    for (int z = 0; z < mGridSize; ++z)
    {
      glm::vec2 k;
      k.y = pi * (2.0f * z - mGridSize) / mVertexDistanceZ;
      for (int x = 0; x < mGridSize; ++x)
      {
        k.x = pi * (2.0f * x - mGridSize) / mVertexDistanceX;
        float kLen = glm::length(k);
        int vertex = z * mGridSize + x;

        mH_Tilde[vertex] = Calc_hTilde(x, z);
        mH_TildeSlopeX[vertex] = mH_Tilde[vertex] * complex(0, k.x);    
        mH_TildeSlopeZ[vertex] = mH_Tilde[vertex] * complex(0, k.y); 

        if (floatNotZero(kLen))
        {
          mH_TildeDX[vertex] = complex(0.0f, 0.0f);
          mH_TildeDZ[vertex] = complex(0.0f, 0.0f);
        }
        else
        {
          mH_TildeDX[vertex] = mH_Tilde[vertex] * complex(0, -(k.x) / kLen);
          mH_TildeDZ[vertex] = mH_Tilde[vertex] * complex(0, -(k.y) / kLen);
        }
      }
    }


    RunKFFT();


    // Apply these new values to the system
    float sign; // because we bit twiddled
    float signs[] = { 1.0f, -1.0f };
    for (int z = 0; z < mGridSize; ++z)
    {
      for (int x = 0; x < mGridSize; ++x)
      {
        int tilde = z * mGridSize + x; // accessor to the h_tilde
        int vertex = z * mGridSizePlus1 + x; // accessor to the vertex

        sign = signs[(x + z) & 1];

        mH_Tilde[tilde] *= sign;   // why?

                                   // height adjustment
        mDrawableVertices[vertex].mPosition.y = -(mH_Tilde[tilde].mReal);

        // displacement update
        mH_TildeDX[tilde] *= sign;
        mH_TildeDZ[tilde] *= sign;  // stupid lambda is being used in this statement as the -1.0f!!
        mDrawableVertices[vertex].mPosition.x = mDrawableVertices[vertex].mOriginalPosition.x + 
                                                mH_TildeDX[tilde].mReal * -1.0f;
        mDrawableVertices[vertex].mPosition.z = mDrawableVertices[vertex].mOriginalPosition.z + 
                                                mH_TildeDZ[tilde].mReal * -1.0f;

        // normal update
        mH_TildeSlopeX[tilde] *= sign;
        mH_TildeSlopeZ[tilde] *= sign;
        mDrawableVertices[vertex].mNormal = glm::normalize(glm::vec3(-(mH_TildeSlopeX[tilde].mReal),
                                                           1.0f,
                                                           -(mH_TildeSlopeZ[tilde].mReal)));

        // tilling
        if (x == 0 && z == 0) {
          int index = squared(mGridSize) - 1;
          mDrawableVertices[index].mPosition.y = -(mH_Tilde[tilde].mReal);

          mDrawableVertices[index].mPosition.x = mDrawableVertices[index].mOriginalPosition.x +
                                                 mH_TildeDX[tilde].mReal * -1.0f;
          mDrawableVertices[index].mPosition.z = mDrawableVertices[index].mOriginalPosition.z +
                                                 mH_TildeDZ[tilde].mReal * -1.0f;

          mDrawableVertices[index].mNormal.x = mDrawableVertices[vertex].mNormal.x;
          mDrawableVertices[index].mNormal.y = mDrawableVertices[vertex].mNormal.y;
          mDrawableVertices[index].mNormal.z = mDrawableVertices[vertex].mNormal.z;
        }
        if (x == 0) {
          int index = vertex + (mGridSize - 1);
          mDrawableVertices[index].mPosition.y = -(mH_Tilde[tilde].mReal);

          mDrawableVertices[index].mPosition.x = mDrawableVertices[index].mOriginalPosition.x + 
                                                 mH_TildeDX[tilde].mReal * -1.0f;
          mDrawableVertices[index].mPosition.z = mDrawableVertices[index].mOriginalPosition.z + 
                                                 mH_TildeDZ[tilde].mReal * -1.0f;

          mDrawableVertices[index].mNormal.x = mDrawableVertices[vertex].mNormal.x;
          mDrawableVertices[index].mNormal.y = mDrawableVertices[vertex].mNormal.y;
          mDrawableVertices[index].mNormal.z = mDrawableVertices[vertex].mNormal.z;
        }
        if (z == 0) {
          int index = vertex + (mGridSize * (mGridSize - 1));
          mDrawableVertices[index].mPosition.y = -(mH_Tilde[tilde].mReal);

          mDrawableVertices[index].mPosition.x = mDrawableVertices[index].mOriginalPosition.x + 
                                                 mH_TildeDX[tilde].mReal * -1.0f;
          mDrawableVertices[index].mPosition.z = mDrawableVertices[index].mOriginalPosition.z + 
                                                 mH_TildeDZ[tilde].mReal * -1.0f;

          mDrawableVertices[index].mNormal.x = mDrawableVertices[vertex].mNormal.x;
          mDrawableVertices[index].mNormal.y = mDrawableVertices[vertex].mNormal.y;
          mDrawableVertices[index].mNormal.z = mDrawableVertices[vertex].mNormal.z;
        }

        // update draw list
        mVertices[vertex].mPosition = mDrawableVertices[vertex].mPosition;
        mVertices[vertex].mNormal = mDrawableVertices[vertex].mNormal;
        //mVertices[vertex].mTextureCoordinates = mDrawableVertices[vertex].mOriginalPosition;

        //std::cout << "For " << vertex << ": " << mDrawableVertices[vertex].mOriginalPosition.x
        //  << ", " << mDrawableVertices[vertex].mOriginalPosition.y
        //  << ", " << mDrawableVertices[vertex].mOriginalPosition.z
        //  << " vs " << mDrawableVertices[vertex].mPosition.x
        //  << ", " << mDrawableVertices[vertex].mPosition.y
        //  << ", " << mDrawableVertices[vertex].mPosition.z
        //  << " vs " << mVertices[vertex].mPosition.x
        //  << ", " << mVertices[vertex].mPosition.y
        //  << ", " << mVertices[vertex].mPosition.z << std::endl;
      }
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::WaveGeneration_NoFFT()
  {
    // loop and evaluate
    for (int z = 0; z < mGridSize; ++z)
    {
      for (int x = 0; x < mGridSize; ++x)
      {
        int vertex = z * mGridSizePlus1 + x;

        // do computation for this position
        // Note that all real computation is here
        WaterComplexNormalVertex hDn = h_D_and_n(glm::vec2(mDrawableVertices[vertex].mPosition.x,
                                                           mDrawableVertices[vertex].mPosition.z));

        // update vertex data
        // The waves height is adjusted here, therefore after this statement,
        // you can get the vertex's actual height position in model coords
        mDrawableVertices[vertex].mPosition.y = hDn.mWaveHeight.mReal;

        // update position on plane: original position + the wave generation offset
        // note the the original position is never changed after the initial data set is done
        mDrawableVertices[vertex].mPosition.x = mDrawableVertices[vertex].mOriginalPosition.x + 
                                                hDn.mDisplacement.x;
        mDrawableVertices[vertex].mPosition.z = mDrawableVertices[vertex].mOriginalPosition.z + 
                                                hDn.mDisplacement.y;

        // update normals
        mDrawableVertices[vertex].mNormal = hDn.mNormal;


        // ------------------------------------
        // update the edges (for tiling)
        if (x == 0 && z == 0)
        {
          int tempVertex = vertex + mGridSize + mGridSizePlus1 * mGridSize;
          // no computation is done here, just value setting
          mDrawableVertices[tempVertex].mPosition.y = hDn.mWaveHeight.mReal;
          mDrawableVertices[tempVertex].mPosition.x = 
                                               mDrawableVertices[tempVertex].mOriginalPosition.x + 
                                               hDn.mDisplacement.x;
          mDrawableVertices[tempVertex].mPosition.z = 
                                               mDrawableVertices[tempVertex].mOriginalPosition.z + 
                                               hDn.mDisplacement.y;
          mDrawableVertices[tempVertex].mNormal = hDn.mNormal;
        }
        if (x == 0)
        {
          int tempVertex = vertex + mGridSize;
          mDrawableVertices[tempVertex].mPosition.y = hDn.mWaveHeight.mReal;
          mDrawableVertices[tempVertex].mPosition.x = 
                                               mDrawableVertices[tempVertex].mOriginalPosition.x + 
                                               hDn.mDisplacement.x;
          mDrawableVertices[tempVertex].mPosition.z = 
                                               mDrawableVertices[tempVertex].mOriginalPosition.z + 
                                               hDn.mDisplacement.y;
          mDrawableVertices[tempVertex].mNormal = hDn.mNormal;
        }
        if (z == 0)
        {
          int tempVertex = vertex + mGridSizePlus1 * mGridSize;
          mDrawableVertices[tempVertex].mPosition.y = hDn.mWaveHeight.mReal;
          mDrawableVertices[tempVertex].mPosition.x = 
                                               mDrawableVertices[tempVertex].mOriginalPosition.x + 
                                               hDn.mDisplacement.x;
          mDrawableVertices[tempVertex].mPosition.z = 
                                               mDrawableVertices[tempVertex].mOriginalPosition.z + 
                                               hDn.mDisplacement.y;
          mDrawableVertices[tempVertex].mNormal = hDn.mNormal;
        }

        // update draw list
        mVertices[vertex].mPosition = mDrawableVertices[vertex].mPosition;
        mVertices[vertex].mNormal = mDrawableVertices[vertex].mNormal;
        //mVertices[vertex].mTextureCoordinates = mDrawableVertices[vertex].mOriginalPosition;
      }
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetRenderMode(std::string aRenderMode)
  {
    if ((false == (aRenderMode != "Solid")) && (false == (aRenderMode != "Wireframe")) ||
        aRenderMode == mRenderModeStr)
    {
      return;
    }

    mRenderModeStr = aRenderMode;

    if (aRenderMode == "Solid")
    {
      mRenderMode = RenderMode::Triangle;
    }
    else if (aRenderMode == "Wireframe")
    {
      mRenderMode = RenderMode::Wireframe;
    }

    mResetNeeded = true;
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetTimeDilationEffect(float aTimeDilationEffect)
  {
    if (aTimeDilationEffect < 0.0001f)
    {
      std::cout << "FFT_WaterSimulation Warning: Time Dilation cannot be set to zero or a negative number"
        << std::endl;
      return;
    }
    else
    {
      mTimeDilationEffect = aTimeDilationEffect;
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

    // loop through an change the original positions
    // note that only Construct() sets original positions, and each frame adds the displacement
    // offset to the original position to derive the new position, which means we can freely adjust
    // this variable like we do below without changing anything else, or doing any complicated math
    //for (int z = 0; z < mGridSizePlus1; ++z)
    //{
    //  for (int x = 0; x < mGridSizePlus1; ++x)
    //  {
    //    int vertex = z * mGridSizePlus1 + x;
    //
    //    // new default position
    //    mDrawableVertices[vertex].mOriginalPosition =
    //      glm::vec3(((x - (mGridSize / 2.0f)) * (mVertexDistanceX / mGridSize)),
    //        0.0f,
    //        ((z - (mGridSize / 2.0f)) * (mVertexDistanceZ / mGridSize)));
    //  }
    //}

    mResetNeeded = true;
  }


  // ------------------------------------
  void FFT_WaterSimulation::UpdateTime(double dt)
  {
    mTime += dt * mTimeDilationEffect;
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetGirdSize(int aGridSize)
  {
    int oldGridSize = mGridSize;
    YTEUnusedArgument(oldGridSize);
    mGridSize = aGridSize;

    //if (mGridSize != 2 && mGridSize != 4 && mGridSize != 8 && mGridSize != 16 && mGridSize != 32 &&
    //  mGridSize != 64 && mGridSize != 128 && mGridSize != 256 && mGridSize != 512)
    //{
    //  std::cout << "WaterSimulation Warning: WaterSimulation was passed a "
    //    << "Grid Size of " << mGridSize << " which is not a power of 2! The default grid size of "
    //    << "8 will be used." << std::endl;
    //
    //  const int defaultGridSize = 32;  // 8 was chosen since its small so easily computed
    //
    //                                  // update the vertex distances if they were the same as the grid size
    //  if (oldGridSize == mVertexDistanceX)
    //  {
    //    mVertexDistanceX = defaultGridSize;
    //    mVertexDistanceZ = defaultGridSize; // also set here since they were set the same above
    //  }
    //
    //  mGridSize = defaultGridSize;
    //}

    mResetNeeded = true;
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
  std::string FFT_WaterSimulation::GetRenderMode()
  {
    return mRenderModeStr;
  }


  // ------------------------------------
  float FFT_WaterSimulation::GetTimeDilationEffect()
  {
    return mTimeDilationEffect;
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
    //AdjustPositions();
  }


  // ------------------------------------
  void FFT_WaterSimulation::SetUseFFT(bool aValue)
  {
    mUseFFT = aValue;
  }


  // ------------------------------------
  int FFT_WaterSimulation::GetInstancingAmount()
  {
    return mInstanceCount;
  }


  // ------------------------------------
  bool FFT_WaterSimulation::GetUseFFT()
  {
    return mUseFFT;
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
        trans.x += (x * (mTransform->GetWorldScale().x + (mVertexDistanceX - 3)));
        trans.z += (z * (mTransform->GetWorldScale().z + (mVertexDistanceZ - 3)));

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

    //if (mInstancingDistance == 1)
    //{
    //  glm::mat4 model = glm::scale(glm::mat4(1.0f), mTransform->GetScale());
    //  model = glm::translate(model, mTransform->GetTranslation());
    //
    //  mInstancingMatrices[0] = (model);
    //  return;
    //}

    //// do the rest
    //for (int i = 0; i < mInstancingDistance; ++i)
    //{
    //  for (int j = 0; j < mInstancingDistance; ++j)
    //  {
    //    glm::mat4 model = glm::scale(glm::mat4(1.0f), mTransform->GetScale());
    //    model = glm::translate(model, mTransform->GetTranslation()
    //      + glm::vec3(mVertexDistanceX * j, 0, mVertexDistanceZ * -i));
    //
    //    mInstancingMatrices[i + j] = (model);
    //  }
    //}

  }


  // ------------------------------------
  void FFT_WaterSimulation::Update(LogicUpdate* aEvent)
  {
    if (mResetNeeded)
    {
      Reset();
      mResetNeeded = false;
    }

    UpdateTime(aEvent->Dt);
    Render(mUseFFT);
  }


  // ------------------------------------
  void FFT_WaterSimulation::EditorUpdate(LogicUpdate* aEvent)
  {
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
      Render(mUseFFT);
    }
    else if (mRunWithEngineUpdate && mRunInSteps)
    {
      if (mSteps >= mStepsCount)
      {
        mSteps = 0;
        UpdateTime(aEvent->Dt * (mStepsCount + 1));
        Render(mUseFFT);
      }
      else
      {
        ++mSteps;
      }
    }
  }


  // ------------------------------------
  //Vertex ComputationalToNormal(WaterComputationalVertex& rhs)
  //{
  //  glm::vec3 color(0.25f, 0.25f, 1.0f);
  //  glm::vec3 boneWeight0(0, 0, 0);
  //  glm::vec2 boneWeight1(0, 0);
  //  glm::ivec3 boneID0(0, 0, 0);
  //  glm::ivec2 boneID1(0, 0);
  //
  //  Vertex v(rhs.mPosition,
  //           rhs.mNormal,
  //           color,
  //           rhs.mHTilde0,
  //           rhs.mHTilde0,
  //           rhs.mHTilde0mkConjugate,
  //           boneWeight0,
  //           boneWeight1,
  //           boneID0,
  //           boneID1);
  //
  //  return v;
  //}


  // ------------------------------------
  void FFT_WaterSimulation::CreateHeightmap()
  {
    // creates vertices to draw with
    //std::vector<Vertex> vertices;
    //
    //for (auto a : mDrawableVertices)
    //{
    //  vertices.push_back(ComputationalToNormal(a));
    //}

    // create object
    std::stringstream sstream;
    sstream << "__Heightmap_" << mGridSize;
    std::string name = sstream.str();
    int size = squared(mInstanceCount);

    for (int i = 0; i < size; ++i)
    {
      mInstantiatedHeightmap.push_back(std::make_unique<InstantiatedHeightmap>());
      if (mRenderMode == RenderMode::Triangle)
      {
        mInstantiatedHeightmap[i]->Initialize(name,
          mVertices,
          mDrawableIndices_Triangle,
          mShaderSetName,
          mStartingMaterial,
          mGraphicsView,
          mRenderer,
          "copywriteWaterTextureDiffuse.png",
          "copywriteFoamTextureSpecular.png",
          "copywritePerlinNoiseTextureNormal.png");
      }
      else if (mRenderMode == RenderMode::Wireframe)
      {
        mInstantiatedHeightmap[i]->Initialize(name,
          mVertices,
          mDrawableIndices_Wireframe,
          mShaderSetName,
          mStartingMaterial,
          mGraphicsView,
          mRenderer,
          "copywriteWaterTextureDiffuse.png",
          "copywriteFoamTextureSpecular.png",
          "copywritePerlinNoiseTextureNormal.png");
      }
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
    // creates vertices to draw with
    //std::vector<Vertex> vertices;
    //
    //for (auto a : mDrawableVertices)
    //{
    //  vertices.push_back(ComputationalToNormal(a));
    //}

    // update
    if (mRenderMode == RenderMode::Triangle)
    {
      mInstantiatedHeightmap[0]->UpdateMesh(mVertices, mDrawableIndices_Triangle);
    }
    else if (mRenderMode == RenderMode::Wireframe)
    {
      mInstantiatedHeightmap[0]->UpdateMesh(mVertices, mDrawableIndices_Wireframe);
    }
  }


  // ------------------------------------
  void FFT_WaterSimulation::InstanceReset()
  {
    mInstancingMatrices.resize(squared(mInstanceCount));
    DestroyHeightmap();
    CreateHeightmap();
    AdjustPositions();
  }

  InstantiatedModel* FFT_WaterSimulation::GetInstantiatedModel()
  {
    if (mInstantiatedHeightmap.size())
    {
      return mInstantiatedHeightmap[0]->GetInstantiatedModel();
    }

    return nullptr;
  }
}

#undef mGridSizePlus1
#undef DefaultGridSize