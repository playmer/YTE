///////////////////
// Author: Andrew Griffin
// YTE - Graphics
///////////////////
#pragma once
#ifndef YTE_Graphics_FFT_WaterSimulation_hpp
#define YTE_Graphics_FFT_WaterSimulation_hpp

// dependencies
#include <glm/glm.hpp>
#include <kiss_fft.h>
#include <kiss_fftnd.h>

// YTE
#include "YTE/Core/Component.hpp"
#include "YTE/Core/ForwardDeclarations.hpp"

#include "YTE/Graphics/BaseModel.hpp"
#include "YTE/Graphics/ForwardDeclarations.hpp"
#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Platform/ForwardDeclarations.hpp"

#include "YTE/Physics/ForwardDeclarations.hpp"

#include "YTE/Math/Complex.hpp"           // complex
#include "YTE/Math/Complex_KISSFFT.hpp"   // complex_kfft
#include "YTE/Math/VertexTypes_FFT.hpp"   // water complex normal vertex


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
    void TransformEvent(TransformChanged *aEvent);

  public:
    /*******************/
    // Standard ctor, uses grid size of 8
    /*******************/
    FFT_WaterSimulation(Composition *aOwner, Space *aSpace, RSValue *aProperties);
    FFT_WaterSimulation(FFT_WaterSimulation& aRhs) = delete;

    /*******************/
    // Initializer for YTE
    /*******************/
    void Initialize() override;

    /*******************/
    // Standard dtor
    /*******************/
    ~FFT_WaterSimulation() override;

    /*******************/
    // Renders the object
    // aLightPosition is the position of the light in the scene that is considered the sun
    // use_fft allows you to choose whether you want to render using fft or not
    /*******************/
    void Render();

    /*******************/
    // Updates the entire item, schedules vulkan memory copy, and prepares for the next draw cycle
    /*******************/
    void Update(LogicUpdate *aEvent);

    /*******************/
    // Updates the entire item when using in the editor, schedules vulkan memory copy, and prepares for the next draw cycle
    /*******************/
    void EditorUpdate(LogicUpdate *aEvent);

    std::vector<InstantiatedModel*> GetInstantiatedModel() override;

    glm::vec3 GetHeight(float x, float z);

    Any MT_A(JobHandle& aJob);
    Any MT_B(JobHandle& aJob);
    Any MT_C(JobHandle& aJob);
    Any MT_D(JobHandle& aJob);
    Any MT_E(JobHandle& aJob);


  private:
    /*******************/
    // Dispersion controls the x and z offsets from the original position of the vertex
    // This doesnt modify the vertices
    // This returns a distancing value to offset the original position of the vertex to its wave
    // based position
    /*******************/
    float Dispersion(int x, int z);

    /*******************/
    // Phillips Spectrum applies the wave height, wind factor, and initial sinusoids to the waves
    /*******************/
    float PhillipsSpectrum(int x, int z);

    /*******************/
    // Calculates the h tilde sub 0 (math related function of sums for sinusoids)
    // returns the complex value for that point
    /*******************/
    complex Calc_hTildeSub0(int x, int z);

    /*******************/
    // Calculates the h tilde (math related function of sums for sinusoids)
    // returns the complex value for that point
    /*******************/
    complex Calc_hTilde(int x, int z);

    /*******************/
    // Processes waves through a FFT simulation using kiss_fft
    /*******************/
    void WaveGeneration();

    /*******************/
    // Updates the time of the sinusoid translations
    // Note that a dt value doesnt work, the FFT and DFT translate the sinusoids based on how
    // much time has elapsed, not the delta time
    /*******************/
    void UpdateTime(double dt);

    /*******************/
    // Constructs the initial data states of the variables
    // this is called by every constructor
    /*******************/
    void Construct();

    /*******************/
    // Destructs the data feilds used
    /*******************/
    void Destruct();

    /*******************/
    // Resets the entire class to use any new data that is passed in
    /*******************/
    void Reset();

    /*******************/
    // Adjusts the matrices that control positioning
    /*******************/
    void AdjustPositions();

    /*******************/
    // Calc the transform
    /*******************/
    void CreateTransform();

    void ReloadShaders();


    // ------------------------------------
    // KISS FFT Related

    /*******************/
    // Starts up a KFFT system to perform the FFT calculations
    /*******************/
    void StartKFFT();

    /*******************/
    // Stops the currently running KFFT and releases any data related to it
    /*******************/
    void StopKFFT();

    /*******************/
    // Performs a KFFT FFT algorithm on the data set
    /*******************/
    void RunKFFT();


  public:
    // settors
    void SetGravitationalPull(float aGravitationalPull);
    void SetGridSize(int aGridSize);
    void SetWaveHeight(float aWaveHeight);
    void SetWindFactor(glm::vec2 aWindFactor);
    void SetVertexDistance(glm::vec2 aDistance);
    void SetTimeDilationEffect(float aTimeDilationEffect);
    void SetReset(bool aReset);
    void SetInstancingAmount(int aAmount);
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
    float GetGravitationalPull();
    int GetGridSize();
    float GetWaveHeight();
    glm::vec2 GetWindFactor();
    glm::vec2 GetVertexDistance();
    float GetVertexDistanceX();
    float GetVertexDistanceZ();
    float GetTimeDilationEffect();
    bool GetReset();
    int GetInstancingAmount();
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

    const std::vector<Vertex>& GetVertices();

  private:
    void CreateHeightmap();
    void DestroyHeightmap();
    void UpdateHeightmap();
    void InstanceReset();

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
    std::vector<UBOModel> mInstancingMatrices;
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




    complex_kfft mH_Tilde;         
    complex_kfft mH_TildeSlopeX;  
    complex_kfft mH_TildeSlopeZ;  
    complex_kfft mH_TildeDX;      
    complex_kfft mH_TildeDZ;      

    // drawing information
    std::vector<WaterComputationalVertex> mComputationalVertices; // vertices
    std::vector<Vertex> mVertices; // vertices
    std::vector<unsigned int> mIndices;     // indices

    // KISS FFT specific
    kiss_fftnd_cfg mKFFTConfig[5];

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