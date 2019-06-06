#pragma once

namespace YTE
{
  class CSharpComponentSystem;
  class CSharpComponentFactory;
  class CSharpComponent;

  using CSharpComponentInitFn = void(__stdcall *)(MonoObject *, MonoException**);
}