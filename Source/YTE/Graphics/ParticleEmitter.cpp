#include "YTE/Graphics/ParticleEmitter.hpp"


namespace YTE
{

  YTEDefineType(ParticleEmitter)
  {
    YTERegisterType(ParticleEmitter);
  }


  ParticleEmitter::ParticleEmitter(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component(aOwner, aSpace)
  {
  }

  ParticleEmitter::~ParticleEmitter()
  {
  }

  void ParticleEmitter::Initialize()
  {
  }

}

