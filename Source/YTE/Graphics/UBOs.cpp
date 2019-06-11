#include "YTE/Graphics/UBOs.hpp"

#include "YTE/Meta/Type.hpp"

YTEDefineExternalType(YTE::UBOs::View)
{
  RegisterType<UBOs::View>();
  TypeBuilder<UBOs::View> builder;
}

YTEDefineExternalType(YTE::UBOs::Model)
{
  RegisterType<UBOs::Model>();
  TypeBuilder<UBOs::Model> builder;
}

YTEDefineExternalType(YTE::UBOs::Material)
{
  RegisterType<UBOs::Material>();
  TypeBuilder<UBOs::Material> builder;
}

YTEDefineExternalType(YTE::UBOs::Animation)
{
  RegisterType<UBOs::Animation>();
  TypeBuilder<UBOs::Animation> builder;
}

YTEDefineExternalType(YTE::UBOs::WaterInfluenceMap)
{
  RegisterType<UBOs::WaterInfluenceMap>();
  TypeBuilder<UBOs::WaterInfluenceMap> builder;
}

YTEDefineExternalType(YTE::UBOs::WaterInformationManager)
{
  RegisterType<UBOs::WaterInformationManager>();
  TypeBuilder<UBOs::WaterInformationManager> builder;
}

YTEDefineExternalType(YTE::UBOs::Light)
{
  RegisterType<UBOs::Light>();
  TypeBuilder<UBOs::Light> builder;
}

YTEDefineExternalType(YTE::UBOs::Illumination)
{
  RegisterType<UBOs::Illumination>();
  TypeBuilder<UBOs::Illumination> builder;
}

YTEDefineExternalType(YTE::UBOs::LightManager)
{
  RegisterType<UBOs::LightManager>();
  TypeBuilder<UBOs::LightManager> builder;
}