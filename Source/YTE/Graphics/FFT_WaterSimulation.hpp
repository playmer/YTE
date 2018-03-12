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
    /*******************/
    // Chooses what kind of rendermode you want to render the water in
    // Note that this is changable at runtime
    /*******************/
    enum class RenderMode
    {
      Triangle = 0,
      Wireframe,

      RenderModeCount
    };

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
    void Render(bool use_fft);

    /*******************/
    // Updates the entire item, schedules vulkan memory copy, and prepares for the next draw cycle
    /*******************/
    void Update(LogicUpdate *aEvent);

    /*******************/
    // Updates the entire item when using in the editor, schedules vulkan memory copy, and prepares for the next draw cycle
    /*******************/
    void EditorUpdate(LogicUpdate *aEvent);

    std::vector<InstantiatedModel*> GetInstantiatedModel() override;

    float GetHeight(float x, float z);

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
    // Calculates the wave height, displacement (through dispersion), and normal vector
    // of a position pos
    // note that the return is a struct of all three values
    /*******************/
    WaterComplexNormalVertex h_D_and_n(glm::vec2 pos);

    /*******************/
    // Processes waves through a DFT simulation
    // Note that this is a very slow, non-improveable, non-multi-threadable solution
    /*******************/
    void WaveGeneration_NoFFT();

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
    void SetRenderMode(std::string aRenderMode);
    void SetGravitationalPull(float aGravitationalPull);
    void SetGirdSize(int aGridSize);
    void SetWaveHeight(float aWaveHeight);
    void SetWindFactor(glm::vec2 aWindFactor);
    void SetVertexDistance(glm::vec2 aDistance);
    void SetTimeDilationEffect(float aTimeDilationEffect);
    void SetReset(bool aReset);
    void SetInstancingAmount(int aAmount);
    void SetUseFFT(bool aValue);
    void SetShaderSetName(std::string aName)
    {
      mShaderSetName = aName;
      ReloadShaders();
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
    std::string GetRenderMode();
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
    bool GetUseFFT();
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

  private:
    void CreateHeightmap();
    void DestroyHeightmap();
    void UpdateHeightmap();
    void InstanceReset();

  private:
    // variables
    RenderMode mRenderMode;  
    float mGravitationalPull;
    int mGridSize;           
    float mWaveHeight;       
    glm::vec2 mWindFactor;   
    float mVertexDistanceX;  
    float mVertexDistanceZ;  
    double mTimeDilationEffect;
    double mTime;
    bool mReset;
    std::vector<UBOModel> mInstancingMatrices;
    std::string mRenderModeStr;
    bool mUseFFT;
    bool mResetNeeded;
    int mInstanceCount;

    bool ran;
    bool mRunWithEngineUpdate;
    bool mRunInSteps;
    int mSteps;
    int mStepsCount;
    bool mConstructing;

    // can i combine these somehow?
    // do i want to? i can multi-thread them in this format
    complex_kfft mH_Tilde;         
    complex_kfft mH_TildeSlopeX;  
    complex_kfft mH_TildeSlopeZ;  
    complex_kfft mH_TildeDX;      
    complex_kfft mH_TildeDZ;      

    // drawing information
    std::vector<WaterComputationalVertex> mDrawableVertices; // vertices
    std::vector<Vertex> mVertices; // vertices
    std::vector<unsigned int> mDrawableIndices_Wireframe;    // indices
    std::vector<unsigned int> mDrawableIndices_Triangle;     // indices

    // KISS FFT specific
    kiss_fftnd_cfg mKFFTConfig;

    // vulkan specific
    Renderer *mRenderer;
    std::vector<std::unique_ptr<InstantiatedHeightmap>> mInstantiatedHeightmap;
    std::string mShaderSetName;
    UBOMaterial mStartingMaterial;
    GraphicsView *mGraphicsView;

    // YTE specific
    Window *mWindow;
    Transform *mTransform;


    // material gettor and settor
    public:
      // gettors
      UBOMaterial GetStartingMaterial()
      {
        return mStartingMaterial;
      }

      //std::string GetShadingStyle()
      //{
      //  return mShadingStyle;
      //}

      glm::vec3 GetDiffuse()
      {
        return glm::vec3(mStartingMaterial.mDiffuse);
      }

      glm::vec3 GetAmbient()
      {
        return glm::vec3(mStartingMaterial.mAmbient);
      }

      glm::vec3 GetSpecular()
      {
        return glm::vec3(mStartingMaterial.mSpecular);
      }

      float GetOpacity()
      {
        return mStartingMaterial.mOpacity;
      }

      glm::vec3 GetEmissive()
      {
        return glm::vec3(mStartingMaterial.mEmissive);
      }

      float GetShinniness()
      {
        return mStartingMaterial.mShininess;
      }



      // settors
      void SetStartingMaterial(UBOMaterial aMaterial)
      {
        mStartingMaterial = aMaterial;
      }

      //void SetShadingStyle(std::string aStyle)
      //{
      //  return mShadingStyle;
      //}

      void SetDiffuse(glm::vec3 aDiffuse)
      {
        mStartingMaterial.mDiffuse = glm::vec4(aDiffuse, 1.0f);
      }

      void SetAmbient(glm::vec3 aAmbient)
      {
        mStartingMaterial.mAmbient = glm::vec4(aAmbient, 1.0f);
      }

      void SetSpecular(glm::vec3 aSpecular)
      {
        mStartingMaterial.mSpecular = glm::vec4(aSpecular, 1.0f);
      }

      void SetOpacity(float aOpacity)
      {
        mStartingMaterial.mOpacity = aOpacity;
      }

      void SetEmissive(glm::vec3 aEmissive)
      {
        mStartingMaterial.mEmissive = glm::vec4(aEmissive, 1.0f);
      }

      void SetShinniness(float aShininess)
      {
        mStartingMaterial.mShininess = aShininess;
      }
  };
}

#endif