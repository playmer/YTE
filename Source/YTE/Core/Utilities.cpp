#include "YTE/Core/Utilities.hpp"

YTEDefineExternalType(glm::i32vec2)
{
  RegisterType<glm::i32vec2>();
    TypeBuilder<glm::i32vec2> builder;

  builder.Field<&glm::i32vec2::x>( "x", PropertyBinding::GetSet);
  builder.Field<&glm::i32vec2::y>( "y", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::vec2)
{
  RegisterType<glm::vec2>();
    TypeBuilder<glm::vec2> builder;

  builder.Field<&glm::vec2::x>( "x", PropertyBinding::GetSet);
  builder.Field<&glm::vec2::y>( "y", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::vec3)
{
  RegisterType<glm::vec3>();
    TypeBuilder<glm::vec3> builder;
  builder.Field<&glm::vec3::x>( "x", PropertyBinding::GetSet);
  builder.Field<&glm::vec3::y>( "y", PropertyBinding::GetSet);
  builder.Field<&glm::vec3::y>( "z", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::vec4)
{
  RegisterType<glm::vec4>();
    TypeBuilder<glm::vec4> builder;
  builder.Field<&glm::vec4::x>( "x", PropertyBinding::GetSet);
  builder.Field<&glm::vec4::y>( "y", PropertyBinding::GetSet);
  builder.Field<&glm::vec4::y>( "z", PropertyBinding::GetSet);
  builder.Field<&glm::vec4::w>( "w", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::quat)
{
  RegisterType<glm::quat>();
    TypeBuilder<glm::quat> builder;
  builder.Field<&glm::quat::x>( "x", PropertyBinding::GetSet);
  builder.Field<&glm::quat::y>( "y", PropertyBinding::GetSet);
  builder.Field<&glm::quat::y>( "z", PropertyBinding::GetSet);
  builder.Field<&glm::quat::w>( "w", PropertyBinding::GetSet);
}
