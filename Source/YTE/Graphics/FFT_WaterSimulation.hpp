///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////
#pragma once
#ifndef YTE_Graphics_FFT_WaterSimulation_hpp
#define YTE_Graphics_FFT_WaterSimulation_hpp

// dependencies
#include <glm/glm.hpp>

// YTE
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/BaseModel.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Math/Complex.hpp"           // complex
#include "YTE/Math/VertexTypes_FFT.hpp"   // water complex normal vertex

#include "YTE/StandardLibrary/PrivateImplementation.hpp"


// --------------------------
// Declarations

namespace YTE
{
  /*******************/
  // Simulates water using fft or dft algorithms
  // Use with caution, this class will be changing
  /*******************/
  class FFT_WaterSimulation : public BaseModel
  {
  public:
    // declare myself
    YTEDeclareType(FFT_WaterSimulation);

    Window* GetWindow()
    {
      return mWindow;
    }


    /*******************/
    // Position Events for changes in transform component
    /*******************/
    YTE_Shared void TransformEvent(TransformChanged *aEvent);

  public:
    /*******************/
    // Standard ctor, uses grid size of 8
    /*******************/
    YTE_Shared FFT_WaterSimulation(Composition *aOwner, Space *aSpace);
    YTE_Shared FFT_WaterSimulation(FFT_WaterSimulation& aRhs) = delete;

    /*******************/
    // Initializer for YTE
    /*******************/
    YTE_Shared void AssetInitialize() override;
    YTE_Shared void Initialize() override;

    /*******************/
    // Standard dtor
    /*******************/
    YTE_Shared ~FFT_WaterSimulation() override;

    /*******************/
    // Renders the object
    // aLightPosition is the position of the light in the scene that is considered the sun
    // use_fft allows you to choose whether you want to render using fft or not
    /*******************/
    YTE_Shared void Render();

    /*******************/
    // Updates the entire item, schedules vulkan memory copy, and prepares for the next draw cycle
    /*******************/
    YTE_Shared void Update(LogicUpdate *aEvent);

    /*******************/
    // Updates the entire item when using in the editor, schedules vulkan memory copy, and prepares for the next draw cycle
    /*******************/
    YTE_Shared void EditorUpdate(LogicUpdate *aEvent);

    YTE_Shared std::vector<InstantiatedModel*> GetInstantiatedModel() override;

    YTE_Shared glm::vec3 GetHeight(float x, float z);

    YTE_Shared Any MT_A(JobHandle& aJob);
    YTE_Shared Any MT_B(JobHandle& aJob);
    YTE_Shared Any MT_C(JobHandle& aJob);
    YTE_Shared Any MT_D(JobHandle& aJob);
    YTE_Shared Any MT_E(JobHandle& aJob);


  private:
    /*******************/
    // Dispersion controls the x and z offsets from the original position of the vertex
    // This doesnt modify the vertices
    // This returns a distancing value to offset the original position of the vertex to its wave
    // based position
    /*******************/
  YTE_Shared float Dispersion(int x, int z);

    /*******************/
    // Phillips Spectrum applies the wave height, wind factor, and initial sinusoids to the waves
    /*******************/
    YTE_Shared float PhillipsSpectrum(int x, int z);

    /*******************/
    // Calculates the h tilde sub 0 (math related function of sums for sinusoids)
    // returns the complex value for that point
    /*******************/
    YTE_Shared complex Calc_hTildeSub0(int x, int z);

    /*******************/
    // Calculates the h tilde (math related function of sums for sinusoids)
    // returns the complex value for that point
    /*******************/
    YTE_Shared complex Calc_hTilde(int x, int z);

    /*******************/
    // Processes waves through a FFT simulation using kiss_fft
    /*******************/
    YTE_Shared void WaveGeneration();

    /*******************/
    // Updates the time of the sinusoid translations
    // Note that a dt value doesnt work, the FFT and DFT translate the sinusoids based on how
    // much time has elapsed, not the delta time
    /*******************/
    YTE_Shared void UpdateTime(double dt);

    /*******************/
    // Constructs the initial data states of the variables
    // this is called by every constructor
    /*******************/
    YTE_Shared void Construct();

    /*******************/
    // Destructs the data feilds used
    /*******************/
    YTE_Shared void Destruct();

    /*******************/
    // Resets the entire class to use any new data that is passed in
    /*******************/
    YTE_Shared void Reset();

    /*******************/
    // Adjusts the matrices that control positioning
    /*******************/
    YTE_Shared void AdjustPositions();

    /*******************/
    // Calc the transform
    /*******************/
    YTE_Shared void CreateTransform();

    YTE_Shared void ReloadShaders();


    // ------------------------------------
    // KISS FFT Related

    /*******************/
    // Starts up a KFFT system to perform the FFT calculations
    /*******************/
    YTE_Shared void StartKFFT();

    /*******************/
    // Stops the currently running KFFT and releases any data related to it
    /*******************/
    YTE_Shared void StopKFFT();

    /*******************/
    // Performs a KFFT FFT algorithm on the data set
    /*******************/
    YTE_Shared void RunKFFT();


  public:
    // settors
    YTE_Shared void SetGravitationalPull(float aGravitationalPull);
    YTE_Shared void SetGridSize(int aGridSize);
    YTE_Shared void SetWaveHeight(float aWaveHeight);
    YTE_Shared void SetWindFactor(glm::vec2 aWindFactor);
    YTE_Shared void SetVertexDistance(glm::vec2 aDistance);
    YTE_Shared void SetTimeDilationEffect(float aTimeDilationEffect);
    YTE_Shared void SetReset(bool aReset);
    YTE_Shared void SetInstancingAmount(int aAmount);
    void SetShaderSetName(std::string aName)
    {
      mShaderSetName = aName;
      
      if (!mConstructing)
      {
        ReloadShaders();
      }
    }
    void SetRunWithEngineUpdate(bool aValue)
    {
      mRunWithEngineUpdate = aValue;
    }
    void SetRunInSteps(bool aValue)
    {
      mRunInSteps = aValue;
    }
    void SetStepCount(int aCount)
    {
      mStepsCount = aCount;
    }
    

  public:
    // gettors
    YTE_Shared float GetGravitationalPull();
    YTE_Shared int GetGridSize();
    YTE_Shared float GetWaveHeight();
    YTE_Shared glm::vec2 GetWindFactor();
    YTE_Shared glm::vec2 GetVertexDistance();
    YTE_Shared float GetVertexDistanceX();
    YTE_Shared float GetVertexDistanceZ();
    YTE_Shared float GetTimeDilationEffect();
    YTE_Shared bool GetReset();
    YTE_Shared int GetInstancingAmount();
    std::string GetShaderSetName()
    {
      return mShaderSetName;
    }
    bool GetRunWithEngineUpdate()
    {
      return mRunWithEngineUpdate;
    }
    bool GetRunInSteps()
    {
      return mRunInSteps;
    }
    int GetStepCount()
    {
      return mStepsCount;
    }
    InstantiatedHeightmap* GetHeightmap(int i)
    {
      if (i < mInstantiatedHeightmap.size())
      {
        return mInstantiatedHeightmap[i].get();
      }
      return nullptr;
    }

    YTE_Shared const std::vector<Vertex>& GetVertices();

  private:
    YTE_Shared void CreateHeightmap();
    YTE_Shared void DestroyHeightmap();
    YTE_Shared void UpdateHeightmap();
    YTE_Shared void InstanceReset();

  private:
    // variables
    float mGravitationalPull;
    int mGridSize;
    int mGridSizePlus1;
    float mWaveHeight;       
    glm::vec2 mWindFactor;   
    float mVertexDistanceX;  
    float mVertexDistanceZ;  
    double mTimeDilationEffect;
    double mTime;
    bool mReset;
    std::vector<UBOs::Model> mInstancingMatrices;
    std::string mRenderModeStr;
    bool mResetNeeded;
    int mInstanceCount;
    JobSystem* mJobSystem;

    bool mRunWithEngineUpdate;
    bool mRunInSteps;
    int mSteps;
    int mStepsCount;
    bool mConstructing;



    // FROM CONNORs
    bool UsePhillips = false;
    bool UseHTilde = false;
    bool UseNewKs = true;
    bool UseNoDisplacement = false;
    bool UseHTildeSubZero = false;

    bool GetUseHTildeSubZero() { return UseHTildeSubZero; }
    bool GetUsePhillips() { return UsePhillips; }
    bool GetUseHTilde() { return UseHTilde; }
    bool GetUseNewKs() { return UseNewKs; }
    bool GetUseNoDisplacement() { return UseNoDisplacement; }
    void SetUseHTildeSubZero(bool a) { UseHTildeSubZero = a; }
    void SetUsePhillips(bool a) { UsePhillips = a; }
    void SetUseHTilde(bool a) { UseHTilde = a; }
    void SetUseNewKs(bool a) { UseNewKs = a; }
    void SetUseNoDisplacement(bool a) { UseNoDisplacement = a; }
    // END FROM CONNOR


    // drawing information
    std::vector<WaterComputationalVertex> mComputationalVertices; // vertices
    std::vector<Vertex> mVertices; // vertices
    std::vector<unsigned int> mIndices;     // indices

    PrivateImplementationLocal<256> mData;

    // vulkan specific
    Renderer *mRenderer;
    std::vector<std::unique_ptr<InstantiatedHeightmap>> mInstantiatedHeightmap;
    std::string mShaderSetName;
    GraphicsView *mGraphicsView;

    // YTE specific
    Window *mWindow;
    Transform *mTransform;
  };
}

#endif