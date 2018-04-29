#include "YTE/Core/Utilities.hpp"

YTEDefineExternalType(glm::i32vec2)
{
  RegisterType<glm::i32vec2>();

  YTEBindField(&glm::i32vec2::x, "x", PropertyBinding::GetSet);
  YTEBindField(&glm::i32vec2::y, "y", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::vec2)
{
  RegisterType<glm::vec2>();

  YTEBindField(&glm::vec2::x, "x", PropertyBinding::GetSet);
  YTEBindField(&glm::vec2::y, "y", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::vec3)
{
  RegisterType<glm::vec3>();
  YTEBindField(&glm::vec3::x, "x", PropertyBinding::GetSet);
  YTEBindField(&glm::vec3::y, "y", PropertyBinding::GetSet);
  YTEBindField(&glm::vec3::y, "z", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::vec4)
{
  RegisterType<glm::vec4>();
  YTEBindField(&glm::vec4::x, "x", PropertyBinding::GetSet);
  YTEBindField(&glm::vec4::y, "y", PropertyBinding::GetSet);
  YTEBindField(&glm::vec4::y, "z", PropertyBinding::GetSet);
  YTEBindField(&glm::vec4::w, "w", PropertyBinding::GetSet);
}

YTEDefineExternalType(glm::quat)
{
  RegisterType<glm::quat>();
  YTEBindField(&glm::quat::x, "x", PropertyBinding::GetSet);
  YTEBindField(&glm::quat::y, "y", PropertyBinding::GetSet);
  YTEBindField(&glm::quat::y, "z", PropertyBinding::GetSet);
  YTEBindField(&glm::quat::w, "w", PropertyBinding::GetSet);
}
