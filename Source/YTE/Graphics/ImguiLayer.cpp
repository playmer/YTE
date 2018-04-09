//#include "imgui.h"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/ImguiLayer.hpp"

#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
  YTEDefineType(ImguiLayer)
  {
    YTERegisterType(ImguiLayer);

    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<GraphicsView>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }


  ImguiLayer::ImguiLayer(Composition *aOwner, Space *aSpace, RSValue *aProperties)
    : Component{aOwner, aSpace}
  {
    YTEUnusedArgument(aProperties);
  }

  ImguiLayer::~ImguiLayer()
  {
    ImGui::DestroyContext(mContext);
  }

  void ImguiLayer::Initialize()
  {
    mContext = ImGui::CreateContext();
    mView = mOwner->GetComponent<GraphicsView>();
    mView->SetDrawerCombinationType("AlphaBlend");
    mView->SetDrawerType("ImguiDrawer");
  }

  const char* ImguiLayer::GetClipboardTextImplementation(void *aSelf)
  {
    auto self = static_cast<ImguiLayer*>(aSelf);
    self->mClipboard = ::YTE::GetClipboardText();

    return self->mClipboard.c_str();
  }

  void ImguiLayer::SetClipboardTextImplementation(void *aSelf, const char* text)
  {
    auto self = static_cast<ImguiLayer*>(aSelf);
    self->mClipboard = text;
    ::YTE::SetClipboardText(self->mClipboard);
  }

  void ImguiLayer::Start()
  {
    ImGui::SetCurrentContext(mContext);
    StyleColorsDark();

    ImGuiIO& io = ImGui::GetIO();

    auto window = mView->GetWindow();

    mOwner->GetEngine()->YTERegister(Events::PreLogicUpdate, this, &ImguiLayer::ImguiUpdate);
    
    window->mMouse.YTERegister(Events::MouseScroll, this, &ImguiLayer::MouseScrollCallback);
    window->mKeyboard.YTERegister(Events::KeyPress, this, &ImguiLayer::KeyPressCallback);
    window->mKeyboard.YTERegister(Events::KeyRelease, this, &ImguiLayer::KeyReleaseCallback);
    window->mKeyboard.YTERegister(Events::CharacterTyped, this, &ImguiLayer::CharacterTypedCallback);

    io.KeyMap[ImGuiKey_Tab] = enum_cast(Keys::Tab);
    io.KeyMap[ImGuiKey_LeftArrow] = enum_cast(Keys::Left);
    io.KeyMap[ImGuiKey_RightArrow] = enum_cast(Keys::Right);
    io.KeyMap[ImGuiKey_UpArrow] = enum_cast(Keys::Up);
    io.KeyMap[ImGuiKey_DownArrow] = enum_cast(Keys::Down);
    io.KeyMap[ImGuiKey_PageUp] = enum_cast(Keys::PageUp);
    io.KeyMap[ImGuiKey_PageDown] = enum_cast(Keys::PageDown);
    io.KeyMap[ImGuiKey_Home] = enum_cast(Keys::Home);
    io.KeyMap[ImGuiKey_End] = enum_cast(Keys::End);
    io.KeyMap[ImGuiKey_Insert] = enum_cast(Keys::Insert);
    io.KeyMap[ImGuiKey_Delete] = enum_cast(Keys::Delete);
    io.KeyMap[ImGuiKey_Backspace] = enum_cast(Keys::Backspace);
    io.KeyMap[ImGuiKey_Space] = enum_cast(Keys::Space);
    io.KeyMap[ImGuiKey_Enter] = enum_cast(Keys::Return);
    io.KeyMap[ImGuiKey_Escape] = enum_cast(Keys::Escape);
    io.KeyMap[ImGuiKey_A] = enum_cast(Keys::A);
    io.KeyMap[ImGuiKey_C] = enum_cast(Keys::C);
    io.KeyMap[ImGuiKey_V] = enum_cast(Keys::V);
    io.KeyMap[ImGuiKey_X] = enum_cast(Keys::X);
    io.KeyMap[ImGuiKey_Y] = enum_cast(Keys::Y);
    io.KeyMap[ImGuiKey_Z] = enum_cast(Keys::Z);

    io.SetClipboardTextFn = SetClipboardTextImplementation;
    io.GetClipboardTextFn = GetClipboardTextImplementation;
    io.ClipboardUserData = this;

    // TODO
    // Load cursors
    // FIXME: GLFW doesn't expose suitable cursors for ResizeAll, ResizeNESW, ResizeNWSE. We revert to arrow cursor for those.
    //g_MouseCursors[ImGuiMouseCursor_Arrow]      = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    //g_MouseCursors[ImGuiMouseCursor_TextInput]  = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    //g_MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    //g_MouseCursors[ImGuiMouseCursor_ResizeNS]   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    //g_MouseCursors[ImGuiMouseCursor_ResizeEW]   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    //g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    //g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);

    mView->SetDrawerType("ImguiDrawer");
  }


  void ImguiLayer::ImguiUpdate(LogicUpdate *aUpdate)
  {
    ImGui::SetCurrentContext(mContext);

    ImGuiIO& io = ImGui::GetIO();

    auto window = mView->GetWindow();
    auto &mouse = window->mMouse;

    // Setup display size (every frame to accommodate for window resizing)
    io.DisplaySize = ImVec2((float)window->GetWidth(),
                            (float)window->GetHeight());

    // Setup time step
    io.DeltaTime = aUpdate->Dt > 0.0 ? (float)(aUpdate->Dt) : (float)(1.0f / 60.0f);

    // TODO: Technically we should only do this when the window is focused, but
    //       we don't currently know that for windows in the editor.
    auto position = mouse.GetCursorPosition();
    io.MousePos = ImVec2((float)position.x, (float)position.y);

    //Mouse buttons : left, right, middle
    io.MouseDown[0] = mouse.IsButtonDown(MouseButtons::Left);
    io.MouseDown[1] = mouse.IsButtonDown(MouseButtons::Right);
    io.MouseDown[2] = mouse.IsButtonDown(MouseButtons::Middle);

    io.ImeWindowHandle = window->GetWindowId();

    // TODO:
    // Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
    //ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
    //if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
    //{
    //  glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    //}
    //else
    //{
    //  glfwSetCursor(g_Window, g_MouseCursors[cursor] ? g_MouseCursors[cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
    //  glfwSetInputMode(g_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //}

    // Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard 
    // flag that you can use to dispatch inputs (or not) to your application.
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    ShowMetricsWindow();
  }

  void ImguiLayer::MouseScrollCallback(MouseWheelEvent *aEvent)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)aEvent->ScrollMovement.x;
    io.MouseWheel += (float)aEvent->ScrollMovement.y;
  }

  void ImguiLayer::KeyPressCallback(KeyboardEvent *aEvent)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[enum_cast(aEvent->Key)] = true;
    
    io.KeyCtrl  = io.KeysDown[enum_cast(Keys::Control)];
    io.KeyShift = io.KeysDown[enum_cast(Keys::Shift)];
    io.KeyAlt   = io.KeysDown[enum_cast(Keys::Alt)];

    //TODO:
    //io.KeySuper = io.KeysDown[enum_cast(Keys::)];
  }

  void ImguiLayer::KeyReleaseCallback(KeyboardEvent *aEvent)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& io = ImGui::GetIO();
    io.KeysDown[enum_cast(aEvent->Key)] = false;

    io.KeyCtrl = io.KeysDown[enum_cast(Keys::Control)];
    io.KeyShift = io.KeysDown[enum_cast(Keys::Shift)];
    io.KeyAlt = io.KeysDown[enum_cast(Keys::Alt)];

    //TODO:
    //io.KeySuper = io.KeysDown[enum_cast(Keys::)];
  }

  void ImguiLayer::CharacterTypedCallback(KeyboardEvent *aEvent)
  {
    ImGui::SetCurrentContext(mContext);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter((ImWchar)aEvent->CharacterTyped);
  }



  /////////////////////////////////////////////////////////////////////////
  // ImGuizmo
  /////////////////////////////////////////////////////////////////////////
  void ImguiLayer::SetDrawlist()
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::SetDrawlist();
  }

  void ImguiLayer::BeginFrame()
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::BeginFrame();
  }

  bool ImguiLayer::IsOver()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGuizmo::IsOver();
  }

  bool ImguiLayer::IsUsing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGuizmo::IsUsing();
  }

  void ImguiLayer::Enable(bool enable)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::Enable(enable);
  }

  void ImguiLayer::DecomposeMatrixToComponents(const float *matrix, 
                                               float *translation, 
                                               float *rotation, 
                                               float *scale)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::DecomposeMatrixToComponents(matrix,
                                          translation,
                                          rotation,
                                          scale);
  }

  void ImguiLayer::RecomposeMatrixFromComponents(const float *translation, 
                                                 const float *rotation, 
                                                 const float *scale, 
                                                 float *matrix)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::RecomposeMatrixFromComponents(translation,
                                            rotation,
                                            scale,
                                            matrix);
  }

  void ImguiLayer::SetRect(float x, float y, float width, float height)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::SetRect(x, y, width, height);
  }

  void ImguiLayer::DrawCube(const float *view, const float *projection, float *matrix)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::DrawCube(view, projection, matrix);
  }

  void ImguiLayer::Manipulate(const float *view, 
                              const float *projection, 
                              ImGuizmo::OPERATION operation, 
                              ImGuizmo::MODE mode, 
                              float *matrix, 
                              float *deltaMatrix, 
                              float *snap, 
                              float *localBounds, 
                              float *boundsSnap)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuizmo::Manipulate(view, 
                         projection, 
                         operation, 
                         mode, 
                         matrix, 
                         deltaMatrix, 
                         snap, 
                         localBounds, 
                         boundsSnap);
  }

  /////////////////////////////////////////////////////////////////////////
  // ImGui
  /////////////////////////////////////////////////////////////////////////
  ImGuiIO& ImguiLayer::GetIO()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetIO();
  }

  ImGuiStyle& ImguiLayer::GetStyle()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStyle();
  }

  void ImguiLayer::ShowMetricsWindow(bool* p_open)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ShowMetricsWindow(p_open);
  }

  const char* ImguiLayer::GetVersion()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetVersion();
  }

  void ImguiLayer::StyleColorsDark(ImGuiStyle* dst)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::StyleColorsDark(dst);
  }

  void ImguiLayer::StyleColorsClassic(ImGuiStyle* dst)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::StyleColorsClassic(dst);
  }

  void ImguiLayer::StyleColorsLight(ImGuiStyle* dst)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::StyleColorsLight(dst);
  }



  bool ImguiLayer::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Begin(name, p_open, flags);
  }

  void ImguiLayer::End()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::End();
  }

  bool ImguiLayer::BeginChild(const char* str_id, const ImVec2& size, bool border, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginChild(str_id, size, border, flags);
  }

  bool ImguiLayer::BeginChild(ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginChild(id, size, border, flags);
  }

  void ImguiLayer::EndChild()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndChild();
  }

  ImVec2 ImguiLayer::GetContentRegionMax()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetContentRegionMax();
  }

  ImVec2 ImguiLayer::GetContentRegionAvail()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetContentRegionAvail();
  }

  float ImguiLayer::GetContentRegionAvailWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetContentRegionAvailWidth();
  }

  ImVec2 ImguiLayer::GetWindowContentRegionMin()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowContentRegionMin();
  }

  ImVec2 ImguiLayer::GetWindowContentRegionMax()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowContentRegionMax();
  }

  float ImguiLayer::GetWindowContentRegionWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowContentRegionWidth();
  }

  ImDrawList* ImguiLayer::GetWindowDrawList()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowDrawList();
  }

  ImVec2 ImguiLayer::GetWindowPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowPos();
  }

  ImVec2 ImguiLayer::GetWindowSize()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowSize();
  }

  float ImguiLayer::GetWindowWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowWidth();
  }

  float ImguiLayer::GetWindowHeight()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowHeight();
  }

  bool ImguiLayer::IsWindowCollapsed()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowCollapsed();
  }

  bool ImguiLayer::IsWindowAppearing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowAppearing();
  }

  void ImguiLayer::SetWindowFontScale(float scale)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowFontScale(scale);
  }


  void ImguiLayer::SetNextWindowPos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowPos(pos, cond, pivot);
  }

  void ImguiLayer::SetNextWindowSize(const ImVec2& size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowSize(size, cond);
  }

  void ImguiLayer::SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback, void* custom_callback_data)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowSizeConstraints(size_min, size_max, custom_callback, custom_callback_data);
  }

  void ImguiLayer::SetNextWindowContentSize(const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowContentSize(size);
  }

  void ImguiLayer::SetNextWindowCollapsed(bool collapsed, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowCollapsed(collapsed, cond);
  }

  void ImguiLayer::SetNextWindowFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowFocus();
  }

  void ImguiLayer::SetNextWindowBgAlpha(float alpha)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowBgAlpha(alpha);
  }

  void ImguiLayer::SetWindowPos(const ImVec2& pos, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowPos(pos, cond);
  }

  void ImguiLayer::SetWindowSize(const ImVec2& size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowSize(size, cond);
  }

  void ImguiLayer::SetWindowCollapsed(bool collapsed, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowCollapsed(collapsed, cond);
  }

  void ImguiLayer::SetWindowFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowFocus();
  }

  void ImguiLayer::SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowPos(name, pos, cond);
  }

  void ImguiLayer::SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowSize(name, size, cond);
  }

  void ImguiLayer::SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowCollapsed(name, collapsed, cond);
  }

  void ImguiLayer::SetWindowFocus(const char* name)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowFocus(name);
  }


  float ImguiLayer::GetScrollX()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollX();
  }

  float ImguiLayer::GetScrollY()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollY();
  }

  float ImguiLayer::GetScrollMaxX()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollMaxX();
  }

  float ImguiLayer::GetScrollMaxY()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollMaxY();
  }

  void ImguiLayer::SetScrollX(float scroll_x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollX(scroll_x);
  }

  void ImguiLayer::SetScrollY(float scroll_y)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollY(scroll_y);
  }

  void ImguiLayer::SetScrollHere(float center_y_ratio)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollHere(center_y_ratio);
  }

  void ImguiLayer::SetScrollFromPosY(float pos_y, float center_y_ratio)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollFromPosY(pos_y, center_y_ratio);
  }

  void ImguiLayer::SetStateStorage(ImGuiStorage* tree)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetStateStorage(tree);
  }

  ImGuiStorage* ImguiLayer::GetStateStorage()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStateStorage();
  }



  void ImguiLayer::PushFont(ImFont* font)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushFont(font);
  }

  void ImguiLayer::PopFont()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopFont();
  }

  void ImguiLayer::PushStyleColor(ImGuiCol idx, ImU32 col)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleColor(idx, col);
  }

  void ImguiLayer::PushStyleColor(ImGuiCol idx, const ImVec4& col)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleColor(idx, col);
  }

  void ImguiLayer::PopStyleColor(int count)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopStyleColor(count);
  }

  void ImguiLayer::PushStyleVar(ImGuiStyleVar idx, float val)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleVar(idx, val);
  }

  void ImguiLayer::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleVar(idx, val);
  }

  void ImguiLayer::PopStyleVar(int count)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopStyleVar(count);
  }

  const ImVec4& ImguiLayer::GetStyleColorVec4(ImGuiCol idx)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStyleColorVec4(idx);
  }

  ImFont* ImguiLayer::GetFont()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFont();
  }

  float ImguiLayer::GetFontSize()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFontSize();
  }

  ImVec2 ImguiLayer::GetFontTexUvWhitePixel()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFontTexUvWhitePixel();
  }

  ImU32 ImguiLayer::GetColorU32(ImGuiCol idx, float alpha_mul)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColorU32(idx, alpha_mul);
  }

  ImU32 ImguiLayer::GetColorU32(const ImVec4& col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColorU32(col);
  }

  ImU32 ImguiLayer::GetColorU32(ImU32 col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColorU32(col);
  }



  void ImguiLayer::PushItemWidth(float item_width)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushItemWidth(item_width);
  }

  void ImguiLayer::PopItemWidth()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopItemWidth();
  }

  float ImguiLayer::CalcItemWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CalcItemWidth();
  }

  void ImguiLayer::PushTextWrapPos(float wrap_pos_x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushTextWrapPos(wrap_pos_x);
  }

  void ImguiLayer::PopTextWrapPos()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopTextWrapPos();
  }

  void ImguiLayer::PushAllowKeyboardFocus(bool allow_keyboard_focus)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushAllowKeyboardFocus(allow_keyboard_focus);
  }

  void ImguiLayer::PopAllowKeyboardFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopAllowKeyboardFocus();
  }

  void ImguiLayer::PushButtonRepeat(bool repeat)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushButtonRepeat(repeat);
  }

  void ImguiLayer::PopButtonRepeat()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopButtonRepeat();
  }



  void ImguiLayer::Separator()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Separator();
  }

  void ImguiLayer::SameLine(float pos_x, float spacing_w)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SameLine(pos_x, spacing_w);
  }

  void ImguiLayer::NewLine()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::NewLine();
  }

  void ImguiLayer::Spacing()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Spacing();
  }

  void ImguiLayer::Dummy(const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Dummy(size);
  }

  void ImguiLayer::Indent(float indent_w)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Indent(indent_w);
  }

  void ImguiLayer::Unindent(float indent_w)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Unindent(indent_w);
  }

  void ImguiLayer::BeginGroup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::BeginGroup();
  }

  void ImguiLayer::EndGroup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndGroup();
  }

  ImVec2 ImguiLayer::GetCursorPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorPos();
  }

  float ImguiLayer::GetCursorPosX()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorPosX();
  }

  float ImguiLayer::GetCursorPosY()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorPosY();
  }

  void ImguiLayer::SetCursorPos(const ImVec2& local_pos)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorPos(local_pos);
  }

  void ImguiLayer::SetCursorPosX(float x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorPosX(x);
  }

  void ImguiLayer::SetCursorPosY(float y)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorPosY(y);
  }

  ImVec2 ImguiLayer::GetCursorStartPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorStartPos();
  }

  ImVec2 ImguiLayer::GetCursorScreenPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorScreenPos();
  }

  void ImguiLayer::SetCursorScreenPos(const ImVec2& pos)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorScreenPos(pos);
  }

  void ImguiLayer::AlignTextToFramePadding()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::AlignTextToFramePadding();
  }

  float ImguiLayer::GetTextLineHeight()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTextLineHeight();
  }

  float ImguiLayer::GetTextLineHeightWithSpacing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTextLineHeightWithSpacing();
  }

  float ImguiLayer::GetFrameHeight()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFrameHeight();
  }

  float ImguiLayer::GetFrameHeightWithSpacing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFrameHeightWithSpacing();
  }




  void ImguiLayer::Columns(int count, const char* id, bool border)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Columns(count, id, border);
  }

  void ImguiLayer::NextColumn()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::NextColumn();
  }

  int ImguiLayer::GetColumnIndex()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnIndex();
  }

  float ImguiLayer::GetColumnWidth(int column_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnWidth(column_index);
  }

  void ImguiLayer::SetColumnWidth(int column_index, float width)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetColumnWidth(column_index, width);
  }

  float ImguiLayer::GetColumnOffset(int column_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnOffset(column_index);
  }

  void ImguiLayer::SetColumnOffset(int column_index, float offset_x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetColumnOffset(column_index, offset_x);
  }

  int ImguiLayer::GetColumnsCount()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnsCount();
  }





  void ImguiLayer::PushID(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(str_id);
  }

  void ImguiLayer::PushID(const char* str_id_begin, const char* str_id_end)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(str_id_begin, str_id_end);
  }

  void ImguiLayer::PushID(const void* ptr_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(ptr_id);
  }

  void ImguiLayer::PushID(int int_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(int_id);
  }

  void ImguiLayer::PopID()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopID();
  }

  ImGuiID ImguiLayer::GetID(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetID(str_id);
  }

  ImGuiID ImguiLayer::GetID(const char* str_id_begin, const char* str_id_end)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetID(str_id_begin, str_id_end);
  }

  ImGuiID ImguiLayer::GetID(const void* ptr_id)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetID(ptr_id);
  }



  void ImguiLayer::TextUnformatted(const char* text, const char* text_end)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextUnformatted(text, text_end);
  }

  void ImguiLayer::Text(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::Text(fmt, variadicArguments);
  }

  void ImguiLayer::TextV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextV(fmt, args);
  }

  void ImguiLayer::TextColored(const ImVec4& col, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::TextColored(col, fmt, variadicArguments);
  }

  void ImguiLayer::TextColoredV(const ImVec4& col, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextColoredV(col, fmt, args);
  }

  void ImguiLayer::TextDisabled(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::TextDisabled(fmt, variadicArguments);
  }

  void ImguiLayer::TextDisabledV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextDisabledV(fmt, args);
  }

  void ImguiLayer::TextWrapped(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::TextWrapped(fmt, variadicArguments);
  }

  void ImguiLayer::TextWrappedV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextWrappedV(fmt, args);
  }

  void ImguiLayer::LabelText(const char* label, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::LabelText(label, fmt, variadicArguments);
  }

  void ImguiLayer::LabelTextV(const char* label, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LabelTextV(label, fmt, args);
  }

  void ImguiLayer::BulletText(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::BulletText(fmt, variadicArguments);
  }

  void ImguiLayer::BulletTextV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::BulletTextV(fmt, args);
  }

  void ImguiLayer::Bullet()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Bullet();
  }



  bool ImguiLayer::Button(const char* label, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Button(label, size);
  }

  bool ImguiLayer::SmallButton(const char* label)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SmallButton(label);
  }

  bool ImguiLayer::ArrowButton(const char* str_id, ImGuiDir dir)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ArrowButton(str_id, dir);
  }

  bool ImguiLayer::InvisibleButton(const char* str_id, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InvisibleButton(str_id, size);
  }

  void ImguiLayer::Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
  }

  bool ImguiLayer::ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ImageButton(user_texture_id, size, uv0, uv1, frame_padding, bg_col, tint_col);
  }

  bool ImguiLayer::Checkbox(const char* label, bool* v)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Checkbox(label, v);
  }

  bool ImguiLayer::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CheckboxFlags(label, flags, flags_value);
  }

  bool ImguiLayer::RadioButton(const char* label, bool active)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::RadioButton(label, active);
  }

  bool ImguiLayer::RadioButton(const char* label, int* v, int v_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::RadioButton(label, v, v_button);
  }

  void ImguiLayer::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
  }

  void ImguiLayer::PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
  }

  void ImguiLayer::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
  }

  void ImguiLayer::PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
  }

  void ImguiLayer::ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ProgressBar(fraction, size_arg, overlay);
  }





  bool ImguiLayer::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginCombo(label, preview_value, flags);
  }

  void ImguiLayer::EndCombo()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndCombo();
  }

  bool ImguiLayer::Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
  }

  bool ImguiLayer::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Combo(label, current_item, items_separated_by_zeros, popup_max_height_in_items);
  }

  bool ImguiLayer::Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Combo(label, current_item, items_getter, data, items_count, popup_max_height_in_items);
  }





  bool ImguiLayer::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max, power);
  }

  bool ImguiLayer::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiLayer::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt2(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiLayer::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt3(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiLayer::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt4(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiLayer::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
  }



  bool ImguiLayer::InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputText(label, buf, buf_size, flags, callback, user_data);
  }

  bool ImguiLayer::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputTextMultiline(label, buf, buf_size, size, flags, callback, user_data);
  }

  bool ImguiLayer::InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat(label, v, step, step_fast, decimal_precision, extra_flags);
  }

  bool ImguiLayer::InputFloat2(const char* label, float v[2], int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat2(label, v, decimal_precision, extra_flags);
  }

  bool ImguiLayer::InputFloat3(const char* label, float v[3], int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat3(label, v, decimal_precision, extra_flags);
  }

  bool ImguiLayer::InputFloat4(const char* label, float v[4], int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat4(label, v, decimal_precision, extra_flags);
  }

  bool ImguiLayer::InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt(label, v, step, step_fast, extra_flags);
  }

  bool ImguiLayer::InputInt2(const char* label, int v[2], ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt2(label, v, extra_flags);
  }

  bool ImguiLayer::InputInt3(const char* label, int v[3], ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt3(label, v, extra_flags);
  }

  bool ImguiLayer::InputInt4(const char* label, int v[4], ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt4(label, v, extra_flags);
  }



  bool ImguiLayer::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat2(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat3(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat4(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max);
  }

  bool ImguiLayer::SliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt(label, v, v_min, v_max, display_format);
  }

  bool ImguiLayer::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt2(label, v, v_min, v_max, display_format);
  }

  bool ImguiLayer::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt3(label, v, v_min, v_max, display_format);
  }

  bool ImguiLayer::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt4(label, v, v_min, v_max, display_format);
  }

  bool ImguiLayer::VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::VSliderFloat(label, size, v, v_min, v_max, display_format, power);
  }

  bool ImguiLayer::VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::VSliderInt(label, size, v, v_min, v_max, display_format);
  }




  bool ImguiLayer::ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorEdit3(label, col, flags);
  }

  bool ImguiLayer::ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorEdit4(label, col, flags);
  }

  bool ImguiLayer::ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorPicker3(label, col, flags);
  }

  bool ImguiLayer::ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorPicker4(label, col, flags, ref_col);
  }

  bool ImguiLayer::ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, ImVec2 size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorButton(desc_id, col, flags, size);
  }

  void ImguiLayer::SetColorEditOptions(ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetColorEditOptions(flags);
  }



  bool ImguiLayer::TreeNode(const char* label)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNode(label);
  }

  bool ImguiLayer::TreeNode(const char* str_id, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNode(str_id, fmt, variadicArguments);
  }

  bool ImguiLayer::TreeNode(const void* ptr_id, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNode(ptr_id, fmt, variadicArguments);
  }

  bool ImguiLayer::TreeNodeV(const char* str_id, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeV(str_id, fmt, args);
  }

  bool ImguiLayer::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeV(ptr_id, fmt, args);
  }

  bool ImguiLayer::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeEx(label, flags);
  }

  bool ImguiLayer::TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeEx(str_id, flags, fmt, variadicArguments);
  }

  bool ImguiLayer::TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeEx(ptr_id, flags, fmt, variadicArguments);
  }

  bool ImguiLayer::TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeExV(str_id, flags, fmt, args);
  }

  bool ImguiLayer::TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeExV(ptr_id, flags, fmt, args);
  }

  void ImguiLayer::TreePush(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreePush(str_id);
  }

  void ImguiLayer::TreePush(const void* ptr_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreePush(ptr_id);
  }

  void ImguiLayer::TreePop()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreePop();
  }

  void ImguiLayer::TreeAdvanceToLabelPos()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreeAdvanceToLabelPos();
  }

  float ImguiLayer::GetTreeNodeToLabelSpacing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTreeNodeToLabelSpacing();
  }

  void ImguiLayer::SetNextTreeNodeOpen(bool is_open, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextTreeNodeOpen(is_open, cond);
  }

  bool ImguiLayer::CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CollapsingHeader(label, flags);
  }

  bool ImguiLayer::CollapsingHeader(const char* label, bool* p_open, ImGuiTreeNodeFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CollapsingHeader(label, p_open, flags);
  }



  bool ImguiLayer::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Selectable(label, selected, flags, size);
  }

  bool ImguiLayer::Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Selectable(label, p_selected, flags, size);
  }

  bool ImguiLayer::ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBox(label, current_item, items, items_count, height_in_items);
  }

  bool ImguiLayer::ListBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBox(label, current_item, items_getter, data, items_count, height_in_items);
  }

  bool ImguiLayer::ListBoxHeader(const char* label, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBoxHeader(label, size);
  }

  bool ImguiLayer::ListBoxHeader(const char* label, int items_count, int height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBoxHeader(label, items_count, height_in_items);
  }

  void ImguiLayer::ListBoxFooter()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ListBoxFooter();
  }



  void ImguiLayer::Value(const char* prefix, bool b)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, b);
  }

  void ImguiLayer::Value(const char* prefix, int v)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, v);
  }

  void ImguiLayer::Value(const char* prefix, unsigned int v)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, v);
  }

  void ImguiLayer::Value(const char* prefix, float v, const char* float_format)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, v, float_format);
  }



  void ImguiLayer::SetTooltip(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::SetTooltip(fmt, variadicArguments);
  }

  void ImguiLayer::SetTooltipV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetTooltipV(fmt, args);
  }

  void ImguiLayer::BeginTooltip()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::BeginTooltip();
  }

  void ImguiLayer::EndTooltip()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndTooltip();
  }



  bool ImguiLayer::BeginMainMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginMainMenuBar();
  }

  void ImguiLayer::EndMainMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndMainMenuBar();
  }

  bool ImguiLayer::BeginMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginMenuBar();
  }

  void ImguiLayer::EndMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndMenuBar();
  }

  bool ImguiLayer::BeginMenu(const char* label, bool enabled)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginMenu(label, enabled);
  }

  void ImguiLayer::EndMenu()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndMenu();
  }

  bool ImguiLayer::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::MenuItem(label, shortcut, selected, enabled);
  }

  bool ImguiLayer::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::MenuItem(label, shortcut, p_selected, enabled);
  }



  void ImguiLayer::OpenPopup(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::OpenPopup(str_id);
  }

  bool ImguiLayer::BeginPopup(const char* str_id, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopup(str_id, flags);
  }

  bool ImguiLayer::BeginPopupContextItem(const char* str_id, int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupContextItem(str_id, mouse_button);
  }

  bool ImguiLayer::BeginPopupContextWindow(const char* str_id, int mouse_button, bool also_over_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupContextWindow(str_id, mouse_button, also_over_items);
  }

  bool ImguiLayer::BeginPopupContextVoid(const char* str_id, int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupContextVoid(str_id, mouse_button);
  }

  bool ImguiLayer::BeginPopupModal(const char* name, bool* p_open, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupModal(name, p_open, flags);
  }

  void ImguiLayer::EndPopup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndPopup();
  }

  bool ImguiLayer::OpenPopupOnItemClick(const char* str_id, int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::OpenPopupOnItemClick(str_id, mouse_button);
  }

  bool ImguiLayer::IsPopupOpen(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsPopupOpen(str_id);
  }

  void ImguiLayer::CloseCurrentPopup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CloseCurrentPopup();
  }



  void ImguiLayer::LogToTTY(int max_depth)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogToTTY(max_depth);
  }

  void ImguiLayer::LogToFile(int max_depth, const char* filename)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogToFile(max_depth, filename);
  }

  void ImguiLayer::LogToClipboard(int max_depth)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogToClipboard(max_depth);
  }

  void ImguiLayer::LogFinish()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogFinish();
  }

  void ImguiLayer::LogButtons()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogButtons();
  }

  //void ImguiLayer::LogText(const char* fmt, ...) IM_FMTARGS(1)
  //{
  //  ImGui::SetCurrentContext(mContext);
  //  ImGui::LogText(const char* fmt, ...) IM_FMTARGS(1);
  //}




  bool ImguiLayer::BeginDragDropSource(ImGuiDragDropFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginDragDropSource(flags);
  }

  bool ImguiLayer::SetDragDropPayload(const char* type, const void* data, size_t size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SetDragDropPayload(type, data, size, cond);
  }

  void ImguiLayer::EndDragDropSource()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndDragDropSource();
  }

  bool ImguiLayer::BeginDragDropTarget()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginDragDropTarget();
  }

  const ImGuiPayload* ImguiLayer::AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::AcceptDragDropPayload(type, flags);
  }

  void ImguiLayer::EndDragDropTarget()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndDragDropTarget();
  }


  void ImguiLayer::PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
  }

  void ImguiLayer::PopClipRect()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopClipRect();
  }


  void ImguiLayer::SetItemDefaultFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetItemDefaultFocus();
  }

  void ImguiLayer::SetKeyboardFocusHere(int offset)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetKeyboardFocusHere(offset);
  }


  bool ImguiLayer::IsItemHovered(ImGuiHoveredFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemHovered(flags);
  }

  bool ImguiLayer::IsItemActive()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemActive();
  }

  bool ImguiLayer::IsItemFocused()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemFocused();
  }

  bool ImguiLayer::IsItemClicked(int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemClicked(mouse_button);
  }

  bool ImguiLayer::IsItemVisible()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemVisible();
  }

  bool ImguiLayer::IsAnyItemHovered()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyItemHovered();
  }

  bool ImguiLayer::IsAnyItemActive()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyItemActive();
  }

  bool ImguiLayer::IsAnyItemFocused()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyItemFocused();
  }

  ImVec2 ImguiLayer::GetItemRectMin()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetItemRectMin();
  }

  ImVec2 ImguiLayer::GetItemRectMax()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetItemRectMax();
  }

  ImVec2 ImguiLayer::GetItemRectSize()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetItemRectSize();
  }

  void ImguiLayer::SetItemAllowOverlap()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetItemAllowOverlap();
  }

  bool ImguiLayer::IsWindowFocused(ImGuiFocusedFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowFocused(flags);
  }

  bool ImguiLayer::IsWindowHovered(ImGuiHoveredFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowHovered(flags);
  }

  bool ImguiLayer::IsRectVisible(const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsRectVisible(size);
  }

  bool ImguiLayer::IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsRectVisible(rect_min, rect_max);
  }

  float ImguiLayer::GetTime()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTime();
  }

  int ImguiLayer::GetFrameCount()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFrameCount();
  }

  ImDrawList* ImguiLayer::GetOverlayDrawList()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetOverlayDrawList();
  }

  ImDrawListSharedData* ImguiLayer::GetDrawListSharedData()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetDrawListSharedData();
  }

  const char* ImguiLayer::GetStyleColorName(ImGuiCol idx)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStyleColorName(idx);
  }

  ImVec2 ImguiLayer::CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
  }

  void ImguiLayer::CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
  }


  bool ImguiLayer::BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginChildFrame(id, size, flags);
  }

  void ImguiLayer::EndChildFrame()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndChildFrame();
  }


  ImVec4 ImguiLayer::ColorConvertU32ToFloat4(ImU32 in)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorConvertU32ToFloat4(in);
  }

  ImU32 ImguiLayer::ColorConvertFloat4ToU32(const ImVec4& in)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorConvertFloat4ToU32(in);
  }

  void ImguiLayer::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
  }

  void ImguiLayer::ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
  }


  int ImguiLayer::GetKeyIndex(ImGuiKey imgui_key)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetKeyIndex(imgui_key);
  }

  bool ImguiLayer::IsKeyDown(int user_key_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsKeyDown(user_key_index);
  }

  bool ImguiLayer::IsKeyPressed(int user_key_index, bool repeat)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsKeyPressed(user_key_index, repeat);
  }

  bool ImguiLayer::IsKeyReleased(int user_key_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsKeyReleased(user_key_index);
  }

  int ImguiLayer::GetKeyPressedAmount(int key_index, float repeat_delay, float rate)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetKeyPressedAmount(key_index, repeat_delay, rate);
  }

  bool ImguiLayer::IsMouseDown(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseDown(button);
  }

  bool ImguiLayer::IsAnyMouseDown()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyMouseDown();
  }

  bool ImguiLayer::IsMouseClicked(int button, bool repeat)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseClicked(button, repeat);
  }

  bool ImguiLayer::IsMouseDoubleClicked(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseDoubleClicked(button);
  }

  bool ImguiLayer::IsMouseReleased(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseReleased(button);
  }

  bool ImguiLayer::IsMouseDragging(int button, float lock_threshold)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseDragging(button, lock_threshold);
  }

  bool ImguiLayer::IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseHoveringRect(r_min, r_max, clip);
  }

  bool ImguiLayer::IsMousePosValid(const ImVec2* mouse_pos)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMousePosValid(mouse_pos);
  }

  ImVec2 ImguiLayer::GetMousePos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMousePos();
  }

  ImVec2 ImguiLayer::GetMousePosOnOpeningCurrentPopup()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMousePosOnOpeningCurrentPopup();
  }

  ImVec2 ImguiLayer::GetMouseDragDelta(int button, float lock_threshold)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMouseDragDelta(button, lock_threshold);
  }

  void ImguiLayer::ResetMouseDragDelta(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ResetMouseDragDelta(button);
  }

  ImGuiMouseCursor ImguiLayer::GetMouseCursor()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMouseCursor();
  }

  void ImguiLayer::SetMouseCursor(ImGuiMouseCursor type)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetMouseCursor(type);
  }

  void ImguiLayer::CaptureKeyboardFromApp(bool capture)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CaptureKeyboardFromApp(capture);
  }

  void ImguiLayer::CaptureMouseFromApp(bool capture)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CaptureMouseFromApp(capture);
  }

  const char* ImguiLayer::GetClipboardText()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetClipboardText();
  }

  void ImguiLayer::SetClipboardText(const char* text)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetClipboardText(text);
  }
}