//#include "imgui.h"
#include "YTE/Core/Engine.hpp"

#include "YTE/Graphics/ImguiContext.hpp"

#include "YTE/Platform/DeviceEnums.hpp"

namespace YTE
{
  YTEDefineType(ImguiContext)
  {
    YTERegisterType(ImguiContext);

    GetStaticType()->AddAttribute<RunInEditor>();

    std::vector<std::vector<Type*>> deps = { { TypeId<GraphicsView>() } };

    GetStaticType()->AddAttribute<ComponentDependencies>(deps);
  }

  ImguiContext::~ImguiContext()
  {
    ImGui::DestroyContext(mContext);
  }

  void ImguiContext::Initialize()
  {
    mContext = ImGui::CreateContext();
  }

  const char* ImguiContext::GetClipboardTextImplementation(void *aSelf)
  {
    auto self = static_cast<ImguiContext*>(aSelf);
    self->mClipboard = ::YTE::GetClipboardText();

    return self->mClipboard.c_str();
  }

  void ImguiContext::SetClipboardTextImplementation(void *aSelf, const char* text)
  {
    auto self = static_cast<ImguiContext*>(aSelf);
    self->mClipboard = text;
    ::YTE::SetClipboardText(self->mClipboard);
  }

  void ImguiContext::Start()
  {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& io = ImGui::GetIO();

    auto window = mView->GetWindow();

    window->mMouse.YTERegister(Events::MouseScroll, this, &ImguiContext::MouseScrollCallback);
    window->mKeyboard.YTERegister(Events::KeyPress, this, &ImguiContext::KeyPressCallback);
    window->mKeyboard.YTERegister(Events::KeyRelease, this, &ImguiContext::KeyReleaseCallback);
    window->mKeyboard.YTERegister(Events::CharacterTyped, this, &ImguiContext::CharacterTypedCallback);

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


  void ImguiContext::ImguiUpdate(LogicUpdate *aUpdate)
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

    // Setup inputs
    // (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
    if (window->IsFocused())
    {
      auto position = mouse.GetCursorPosition();
      io.MousePos = ImVec2((float)position.x, (float)position.y);
    }
    else
    {
      io.MousePos = ImVec2(-std::numeric_limits<float>::max(), 
                           -std::numeric_limits<float>::max());
    }

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

    // Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
    ImGui::NewFrame();
  }

  void ImguiContext::MouseScrollCallback(MouseWheelEvent *aEvent)
  {
    ImGui::SetCurrentContext(mContext);
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)aEvent->ScrollMovement.x;
    io.MouseWheel += (float)aEvent->ScrollMovement.y;
  }

  void ImguiContext::KeyPressCallback(KeyboardEvent *aEvent)
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

  void ImguiContext::KeyReleaseCallback(KeyboardEvent *aEvent)
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

  void ImguiContext::CharacterTypedCallback(KeyboardEvent *aEvent)
  {
    ImGui::SetCurrentContext(mContext);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter((ImWchar)aEvent->CharacterTyped);
  }

  void ImguiContext::ShowMetricsWindow(bool* p_open)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ShowMetricsWindow(p_open);
  }

  const char* ImguiContext::GetVersion()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetVersion();
  }

  void ImguiContext::StyleColorsDark(ImGuiStyle* dst)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::StyleColorsDark(dst);
  }

  void ImguiContext::StyleColorsClassic(ImGuiStyle* dst)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::StyleColorsClassic(dst);
  }

  void ImguiContext::StyleColorsLight(ImGuiStyle* dst)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::StyleColorsLight(dst);
  }



  bool ImguiContext::Begin(const char* name, bool* p_open, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Begin(name, p_open, flags);
  }

  void ImguiContext::End()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::End();
  }

  bool ImguiContext::BeginChild(const char* str_id, const ImVec2& size, bool border, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginChild(str_id, size, border, flags);
  }

  bool ImguiContext::BeginChild(ImGuiID id, const ImVec2& size, bool border, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginChild(id, size, border, flags);
  }

  void ImguiContext::EndChild()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndChild();
  }

  ImVec2 ImguiContext::GetContentRegionMax()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetContentRegionMax();
  }

  ImVec2 ImguiContext::GetContentRegionAvail()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetContentRegionAvail();
  }

  float ImguiContext::GetContentRegionAvailWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetContentRegionAvailWidth();
  }

  ImVec2 ImguiContext::GetWindowContentRegionMin()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowContentRegionMin();
  }

  ImVec2 ImguiContext::GetWindowContentRegionMax()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowContentRegionMax();
  }

  float ImguiContext::GetWindowContentRegionWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowContentRegionWidth();
  }

  ImDrawList* ImguiContext::GetWindowDrawList()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowDrawList();
  }

  ImVec2 ImguiContext::GetWindowPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowPos();
  }

  ImVec2 ImguiContext::GetWindowSize()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowSize();
  }

  float ImguiContext::GetWindowWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowWidth();
  }

  float ImguiContext::GetWindowHeight()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetWindowHeight();
  }

  bool ImguiContext::IsWindowCollapsed()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowCollapsed();
  }

  bool ImguiContext::IsWindowAppearing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowAppearing();
  }

  void ImguiContext::SetWindowFontScale(float scale)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowFontScale(scale);
  }


  void ImguiContext::SetNextWindowPos(const ImVec2& pos, ImGuiCond cond, const ImVec2& pivot)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowPos(pos, cond, pivot);
  }

  void ImguiContext::SetNextWindowSize(const ImVec2& size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowSize(size, cond);
  }

  void ImguiContext::SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback, void* custom_callback_data)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowSizeConstraints(size_min, size_max, custom_callback, custom_callback_data);
  }

  void ImguiContext::SetNextWindowContentSize(const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowContentSize(size);
  }

  void ImguiContext::SetNextWindowCollapsed(bool collapsed, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowCollapsed(collapsed, cond);
  }

  void ImguiContext::SetNextWindowFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowFocus();
  }

  void ImguiContext::SetNextWindowBgAlpha(float alpha)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextWindowBgAlpha(alpha);
  }

  void ImguiContext::SetWindowPos(const ImVec2& pos, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowPos(pos, cond);
  }

  void ImguiContext::SetWindowSize(const ImVec2& size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowSize(size, cond);
  }

  void ImguiContext::SetWindowCollapsed(bool collapsed, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowCollapsed(collapsed, cond);
  }

  void ImguiContext::SetWindowFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowFocus();
  }

  void ImguiContext::SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowPos(name, pos, cond);
  }

  void ImguiContext::SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowSize(name, size, cond);
  }

  void ImguiContext::SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowCollapsed(name, collapsed, cond);
  }

  void ImguiContext::SetWindowFocus(const char* name)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetWindowFocus(name);
  }


  float ImguiContext::GetScrollX()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollX();
  }

  float ImguiContext::GetScrollY()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollY();
  }

  float ImguiContext::GetScrollMaxX()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollMaxX();
  }

  float ImguiContext::GetScrollMaxY()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetScrollMaxY();
  }

  void ImguiContext::SetScrollX(float scroll_x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollX(scroll_x);
  }

  void ImguiContext::SetScrollY(float scroll_y)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollY(scroll_y);
  }

  void ImguiContext::SetScrollHere(float center_y_ratio)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollHere(center_y_ratio);
  }

  void ImguiContext::SetScrollFromPosY(float pos_y, float center_y_ratio)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetScrollFromPosY(pos_y, center_y_ratio);
  }

  void ImguiContext::SetStateStorage(ImGuiStorage* tree)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetStateStorage(tree);
  }

  ImGuiStorage* ImguiContext::GetStateStorage()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStateStorage();
  }



  void ImguiContext::PushFont(ImFont* font)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushFont(font);
  }

  void ImguiContext::PopFont()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopFont();
  }

  void ImguiContext::PushStyleColor(ImGuiCol idx, ImU32 col)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleColor(idx, col);
  }

  void ImguiContext::PushStyleColor(ImGuiCol idx, const ImVec4& col)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleColor(idx, col);
  }

  void ImguiContext::PopStyleColor(int count)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopStyleColor(count);
  }

  void ImguiContext::PushStyleVar(ImGuiStyleVar idx, float val)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleVar(idx, val);
  }

  void ImguiContext::PushStyleVar(ImGuiStyleVar idx, const ImVec2& val)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushStyleVar(idx, val);
  }

  void ImguiContext::PopStyleVar(int count)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopStyleVar(count);
  }

  const ImVec4& ImguiContext::GetStyleColorVec4(ImGuiCol idx)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStyleColorVec4(idx);
  }

  ImFont* ImguiContext::GetFont()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFont();
  }

  float ImguiContext::GetFontSize()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFontSize();
  }

  ImVec2 ImguiContext::GetFontTexUvWhitePixel()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFontTexUvWhitePixel();
  }

  ImU32 ImguiContext::GetColorU32(ImGuiCol idx, float alpha_mul)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColorU32(idx, alpha_mul);
  }

  ImU32 ImguiContext::GetColorU32(const ImVec4& col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColorU32(col);
  }

  ImU32 ImguiContext::GetColorU32(ImU32 col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColorU32(col);
  }



  void ImguiContext::PushItemWidth(float item_width)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushItemWidth(item_width);
  }

  void ImguiContext::PopItemWidth()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopItemWidth();
  }

  float ImguiContext::CalcItemWidth()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CalcItemWidth();
  }

  void ImguiContext::PushTextWrapPos(float wrap_pos_x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushTextWrapPos(wrap_pos_x);
  }

  void ImguiContext::PopTextWrapPos()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopTextWrapPos();
  }

  void ImguiContext::PushAllowKeyboardFocus(bool allow_keyboard_focus)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushAllowKeyboardFocus(allow_keyboard_focus);
  }

  void ImguiContext::PopAllowKeyboardFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopAllowKeyboardFocus();
  }

  void ImguiContext::PushButtonRepeat(bool repeat)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushButtonRepeat(repeat);
  }

  void ImguiContext::PopButtonRepeat()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopButtonRepeat();
  }



  void ImguiContext::Separator()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Separator();
  }

  void ImguiContext::SameLine(float pos_x, float spacing_w)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SameLine(pos_x, spacing_w);
  }

  void ImguiContext::NewLine()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::NewLine();
  }

  void ImguiContext::Spacing()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Spacing();
  }

  void ImguiContext::Dummy(const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Dummy(size);
  }

  void ImguiContext::Indent(float indent_w)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Indent(indent_w);
  }

  void ImguiContext::Unindent(float indent_w)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Unindent(indent_w);
  }

  void ImguiContext::BeginGroup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::BeginGroup();
  }

  void ImguiContext::EndGroup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndGroup();
  }

  ImVec2 ImguiContext::GetCursorPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorPos();
  }

  float ImguiContext::GetCursorPosX()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorPosX();
  }

  float ImguiContext::GetCursorPosY()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorPosY();
  }

  void ImguiContext::SetCursorPos(const ImVec2& local_pos)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorPos(local_pos);
  }

  void ImguiContext::SetCursorPosX(float x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorPosX(x);
  }

  void ImguiContext::SetCursorPosY(float y)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorPosY(y);
  }

  ImVec2 ImguiContext::GetCursorStartPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorStartPos();
  }

  ImVec2 ImguiContext::GetCursorScreenPos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetCursorScreenPos();
  }

  void ImguiContext::SetCursorScreenPos(const ImVec2& pos)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetCursorScreenPos(pos);
  }

  void ImguiContext::AlignTextToFramePadding()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::AlignTextToFramePadding();
  }

  float ImguiContext::GetTextLineHeight()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTextLineHeight();
  }

  float ImguiContext::GetTextLineHeightWithSpacing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTextLineHeightWithSpacing();
  }

  float ImguiContext::GetFrameHeight()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFrameHeight();
  }

  float ImguiContext::GetFrameHeightWithSpacing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFrameHeightWithSpacing();
  }




  void ImguiContext::Columns(int count, const char* id, bool border)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Columns(count, id, border);
  }

  void ImguiContext::NextColumn()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::NextColumn();
  }

  int ImguiContext::GetColumnIndex()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnIndex();
  }

  float ImguiContext::GetColumnWidth(int column_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnWidth(column_index);
  }

  void ImguiContext::SetColumnWidth(int column_index, float width)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetColumnWidth(column_index, width);
  }

  float ImguiContext::GetColumnOffset(int column_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnOffset(column_index);
  }

  void ImguiContext::SetColumnOffset(int column_index, float offset_x)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetColumnOffset(column_index, offset_x);
  }

  int ImguiContext::GetColumnsCount()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetColumnsCount();
  }





  void ImguiContext::PushID(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(str_id);
  }

  void ImguiContext::PushID(const char* str_id_begin, const char* str_id_end)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(str_id_begin, str_id_end);
  }

  void ImguiContext::PushID(const void* ptr_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(ptr_id);
  }

  void ImguiContext::PushID(int int_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushID(int_id);
  }

  void ImguiContext::PopID()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopID();
  }

  ImGuiID ImguiContext::GetID(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetID(str_id);
  }

  ImGuiID ImguiContext::GetID(const char* str_id_begin, const char* str_id_end)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetID(str_id_begin, str_id_end);
  }

  ImGuiID ImguiContext::GetID(const void* ptr_id)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetID(ptr_id);
  }



  void ImguiContext::TextUnformatted(const char* text, const char* text_end)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextUnformatted(text, text_end);
  }

  void ImguiContext::Text(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::Text(fmt, variadicArguments);
  }

  void ImguiContext::TextV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextV(fmt, args);
  }

  void ImguiContext::TextColored(const ImVec4& col, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::TextColored(col, fmt, variadicArguments);
  }

  void ImguiContext::TextColoredV(const ImVec4& col, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextColoredV(col, fmt, args);
  }

  void ImguiContext::TextDisabled(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::TextDisabled(fmt, variadicArguments);
  }

  void ImguiContext::TextDisabledV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextDisabledV(fmt, args);
  }

  void ImguiContext::TextWrapped(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::TextWrapped(fmt, variadicArguments);
  }

  void ImguiContext::TextWrappedV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TextWrappedV(fmt, args);
  }

  void ImguiContext::LabelText(const char* label, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::LabelText(label, fmt, variadicArguments);
  }

  void ImguiContext::LabelTextV(const char* label, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LabelTextV(label, fmt, args);
  }

  void ImguiContext::BulletText(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::BulletText(fmt, variadicArguments);
  }

  void ImguiContext::BulletTextV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::BulletTextV(fmt, args);
  }

  void ImguiContext::Bullet()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Bullet();
  }



  bool ImguiContext::Button(const char* label, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Button(label, size);
  }

  bool ImguiContext::SmallButton(const char* label)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SmallButton(label);
  }

  bool ImguiContext::ArrowButton(const char* str_id, ImGuiDir dir)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ArrowButton(str_id, dir);
  }

  bool ImguiContext::InvisibleButton(const char* str_id, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InvisibleButton(str_id, size);
  }

  void ImguiContext::Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Image(user_texture_id, size, uv0, uv1, tint_col, border_col);
  }

  bool ImguiContext::ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ImageButton(user_texture_id, size, uv0, uv1, frame_padding, bg_col, tint_col);
  }

  bool ImguiContext::Checkbox(const char* label, bool* v)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Checkbox(label, v);
  }

  bool ImguiContext::CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CheckboxFlags(label, flags, flags_value);
  }

  bool ImguiContext::RadioButton(const char* label, bool active)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::RadioButton(label, active);
  }

  bool ImguiContext::RadioButton(const char* label, int* v, int v_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::RadioButton(label, v, v_button);
  }

  void ImguiContext::PlotLines(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotLines(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
  }

  void ImguiContext::PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotLines(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
  }

  void ImguiContext::PlotHistogram(const char* label, const float* values, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size, int stride)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotHistogram(label, values, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size, stride);
  }

  void ImguiContext::PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, const char* overlay_text, float scale_min, float scale_max, ImVec2 graph_size)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PlotHistogram(label, values_getter, data, values_count, values_offset, overlay_text, scale_min, scale_max, graph_size);
  }

  void ImguiContext::ProgressBar(float fraction, const ImVec2& size_arg, const char* overlay)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ProgressBar(fraction, size_arg, overlay);
  }





  bool ImguiContext::BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginCombo(label, preview_value, flags);
  }

  void ImguiContext::EndCombo()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndCombo();
  }

  bool ImguiContext::Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Combo(label, current_item, items, items_count, popup_max_height_in_items);
  }

  bool ImguiContext::Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Combo(label, current_item, items_separated_by_zeros, popup_max_height_in_items);
  }

  bool ImguiContext::Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Combo(label, current_item, items_getter, data, items_count, popup_max_height_in_items);
  }





  bool ImguiContext::DragFloat(const char* label, float* v, float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiContext::DragFloat2(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat2(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiContext::DragFloat3(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat3(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiContext::DragFloat4(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloat4(label, v, v_speed, v_min, v_max, display_format, power);
  }

  bool ImguiContext::DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed, float v_min, float v_max, const char* display_format, const char* display_format_max, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragFloatRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max, power);
  }

  bool ImguiContext::DragInt(const char* label, int* v, float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiContext::DragInt2(const char* label, int v[2], float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt2(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiContext::DragInt3(const char* label, int v[3], float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt3(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiContext::DragInt4(const char* label, int v[4], float v_speed, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragInt4(label, v, v_speed, v_min, v_max, display_format);
  }

  bool ImguiContext::DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed, int v_min, int v_max, const char* display_format, const char* display_format_max)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::DragIntRange2(label, v_current_min, v_current_max, v_speed, v_min, v_max, display_format, display_format_max);
  }



  bool ImguiContext::InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputText(label, buf, buf_size, flags, callback, user_data);
  }

  bool ImguiContext::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiTextEditCallback callback, void* user_data)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputTextMultiline(label, buf, buf_size, size, flags, callback, user_data);
  }

  bool ImguiContext::InputFloat(const char* label, float* v, float step, float step_fast, int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat(label, v, step, step_fast, decimal_precision, extra_flags);
  }

  bool ImguiContext::InputFloat2(const char* label, float v[2], int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat2(label, v, decimal_precision, extra_flags);
  }

  bool ImguiContext::InputFloat3(const char* label, float v[3], int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat3(label, v, decimal_precision, extra_flags);
  }

  bool ImguiContext::InputFloat4(const char* label, float v[4], int decimal_precision, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputFloat4(label, v, decimal_precision, extra_flags);
  }

  bool ImguiContext::InputInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt(label, v, step, step_fast, extra_flags);
  }

  bool ImguiContext::InputInt2(const char* label, int v[2], ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt2(label, v, extra_flags);
  }

  bool ImguiContext::InputInt3(const char* label, int v[3], ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt3(label, v, extra_flags);
  }

  bool ImguiContext::InputInt4(const char* label, int v[4], ImGuiInputTextFlags extra_flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::InputInt4(label, v, extra_flags);
  }



  bool ImguiContext::SliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiContext::SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat2(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiContext::SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat3(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiContext::SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderFloat4(label, v, v_min, v_max, display_format, power);
  }

  bool ImguiContext::SliderAngle(const char* label, float* v_rad, float v_degrees_min, float v_degrees_max)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderAngle(label, v_rad, v_degrees_min, v_degrees_max);
  }

  bool ImguiContext::SliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt(label, v, v_min, v_max, display_format);
  }

  bool ImguiContext::SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt2(label, v, v_min, v_max, display_format);
  }

  bool ImguiContext::SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt3(label, v, v_min, v_max, display_format);
  }

  bool ImguiContext::SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SliderInt4(label, v, v_min, v_max, display_format);
  }

  bool ImguiContext::VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* display_format, float power)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::VSliderFloat(label, size, v, v_min, v_max, display_format, power);
  }

  bool ImguiContext::VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* display_format)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::VSliderInt(label, size, v, v_min, v_max, display_format);
  }




  bool ImguiContext::ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorEdit3(label, col, flags);
  }

  bool ImguiContext::ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorEdit4(label, col, flags);
  }

  bool ImguiContext::ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorPicker3(label, col, flags);
  }

  bool ImguiContext::ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags, const float* ref_col)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorPicker4(label, col, flags, ref_col);
  }

  bool ImguiContext::ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags, ImVec2 size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorButton(desc_id, col, flags, size);
  }

  void ImguiContext::SetColorEditOptions(ImGuiColorEditFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetColorEditOptions(flags);
  }



  bool ImguiContext::TreeNode(const char* label)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNode(label);
  }

  bool ImguiContext::TreeNode(const char* str_id, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNode(str_id, fmt, variadicArguments);
  }

  bool ImguiContext::TreeNode(const void* ptr_id, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNode(ptr_id, fmt, variadicArguments);
  }

  bool ImguiContext::TreeNodeV(const char* str_id, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeV(str_id, fmt, args);
  }

  bool ImguiContext::TreeNodeV(const void* ptr_id, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeV(ptr_id, fmt, args);
  }

  bool ImguiContext::TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeEx(label, flags);
  }

  bool ImguiContext::TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeEx(str_id, flags, fmt, variadicArguments);
  }

  bool ImguiContext::TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeEx(ptr_id, flags, fmt, variadicArguments);
  }

  bool ImguiContext::TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeExV(str_id, flags, fmt, args);
  }

  bool ImguiContext::TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::TreeNodeExV(ptr_id, flags, fmt, args);
  }

  void ImguiContext::TreePush(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreePush(str_id);
  }

  void ImguiContext::TreePush(const void* ptr_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreePush(ptr_id);
  }

  void ImguiContext::TreePop()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreePop();
  }

  void ImguiContext::TreeAdvanceToLabelPos()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::TreeAdvanceToLabelPos();
  }

  float ImguiContext::GetTreeNodeToLabelSpacing()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTreeNodeToLabelSpacing();
  }

  void ImguiContext::SetNextTreeNodeOpen(bool is_open, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetNextTreeNodeOpen(is_open, cond);
  }

  bool ImguiContext::CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CollapsingHeader(label, flags);
  }

  bool ImguiContext::CollapsingHeader(const char* label, bool* p_open, ImGuiTreeNodeFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CollapsingHeader(label, p_open, flags);
  }



  bool ImguiContext::Selectable(const char* label, bool selected, ImGuiSelectableFlags flags, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Selectable(label, selected, flags, size);
  }

  bool ImguiContext::Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::Selectable(label, p_selected, flags, size);
  }

  bool ImguiContext::ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBox(label, current_item, items, items_count, height_in_items);
  }

  bool ImguiContext::ListBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBox(label, current_item, items_getter, data, items_count, height_in_items);
  }

  bool ImguiContext::ListBoxHeader(const char* label, const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBoxHeader(label, size);
  }

  bool ImguiContext::ListBoxHeader(const char* label, int items_count, int height_in_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ListBoxHeader(label, items_count, height_in_items);
  }

  void ImguiContext::ListBoxFooter()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ListBoxFooter();
  }



  void ImguiContext::Value(const char* prefix, bool b)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, b);
  }

  void ImguiContext::Value(const char* prefix, int v)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, v);
  }

  void ImguiContext::Value(const char* prefix, unsigned int v)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, v);
  }

  void ImguiContext::Value(const char* prefix, float v, const char* float_format)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::Value(prefix, v, float_format);
  }



  void ImguiContext::SetTooltip(const char* fmt, ...)
  {
    va_list variadicArguments;
    va_start(variadicArguments, fmt);

    ImGui::SetCurrentContext(mContext);
    ImGui::SetTooltip(fmt, variadicArguments);
  }

  void ImguiContext::SetTooltipV(const char* fmt, va_list args)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetTooltipV(fmt, args);
  }

  void ImguiContext::BeginTooltip()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::BeginTooltip();
  }

  void ImguiContext::EndTooltip()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndTooltip();
  }



  bool ImguiContext::BeginMainMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginMainMenuBar();
  }

  void ImguiContext::EndMainMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndMainMenuBar();
  }

  bool ImguiContext::BeginMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginMenuBar();
  }

  void ImguiContext::EndMenuBar()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndMenuBar();
  }

  bool ImguiContext::BeginMenu(const char* label, bool enabled)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginMenu(label, enabled);
  }

  void ImguiContext::EndMenu()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndMenu();
  }

  bool ImguiContext::MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::MenuItem(label, shortcut, selected, enabled);
  }

  bool ImguiContext::MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::MenuItem(label, shortcut, p_selected, enabled);
  }



  void ImguiContext::OpenPopup(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::OpenPopup(str_id);
  }

  bool ImguiContext::BeginPopup(const char* str_id, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopup(str_id, flags);
  }

  bool ImguiContext::BeginPopupContextItem(const char* str_id, int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupContextItem(str_id, mouse_button);
  }

  bool ImguiContext::BeginPopupContextWindow(const char* str_id, int mouse_button, bool also_over_items)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupContextWindow(str_id, mouse_button, also_over_items);
  }

  bool ImguiContext::BeginPopupContextVoid(const char* str_id, int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupContextVoid(str_id, mouse_button);
  }

  bool ImguiContext::BeginPopupModal(const char* name, bool* p_open, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginPopupModal(name, p_open, flags);
  }

  void ImguiContext::EndPopup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndPopup();
  }

  bool ImguiContext::OpenPopupOnItemClick(const char* str_id, int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::OpenPopupOnItemClick(str_id, mouse_button);
  }

  bool ImguiContext::IsPopupOpen(const char* str_id)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsPopupOpen(str_id);
  }

  void ImguiContext::CloseCurrentPopup()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CloseCurrentPopup();
  }



  void ImguiContext::LogToTTY(int max_depth)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogToTTY(max_depth);
  }

  void ImguiContext::LogToFile(int max_depth, const char* filename)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogToFile(max_depth, filename);
  }

  void ImguiContext::LogToClipboard(int max_depth)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogToClipboard(max_depth);
  }

  void ImguiContext::LogFinish()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogFinish();
  }

  void ImguiContext::LogButtons()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::LogButtons();
  }

  //void ImguiContext::LogText(const char* fmt, ...) IM_FMTARGS(1)
  //{
  //  ImGui::SetCurrentContext(mContext);
  //  ImGui::LogText(const char* fmt, ...) IM_FMTARGS(1);
  //}




  bool ImguiContext::BeginDragDropSource(ImGuiDragDropFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginDragDropSource(flags);
  }

  bool ImguiContext::SetDragDropPayload(const char* type, const void* data, size_t size, ImGuiCond cond)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::SetDragDropPayload(type, data, size, cond);
  }

  void ImguiContext::EndDragDropSource()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndDragDropSource();
  }

  bool ImguiContext::BeginDragDropTarget()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginDragDropTarget();
  }

  const ImGuiPayload* ImguiContext::AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::AcceptDragDropPayload(type, flags);
  }

  void ImguiContext::EndDragDropTarget()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndDragDropTarget();
  }


  void ImguiContext::PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PushClipRect(clip_rect_min, clip_rect_max, intersect_with_current_clip_rect);
  }

  void ImguiContext::PopClipRect()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::PopClipRect();
  }


  void ImguiContext::SetItemDefaultFocus()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetItemDefaultFocus();
  }

  void ImguiContext::SetKeyboardFocusHere(int offset)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetKeyboardFocusHere(offset);
  }


  bool ImguiContext::IsItemHovered(ImGuiHoveredFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemHovered(flags);
  }

  bool ImguiContext::IsItemActive()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemActive();
  }

  bool ImguiContext::IsItemFocused()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemFocused();
  }

  bool ImguiContext::IsItemClicked(int mouse_button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemClicked(mouse_button);
  }

  bool ImguiContext::IsItemVisible()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsItemVisible();
  }

  bool ImguiContext::IsAnyItemHovered()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyItemHovered();
  }

  bool ImguiContext::IsAnyItemActive()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyItemActive();
  }

  bool ImguiContext::IsAnyItemFocused()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyItemFocused();
  }

  ImVec2 ImguiContext::GetItemRectMin()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetItemRectMin();
  }

  ImVec2 ImguiContext::GetItemRectMax()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetItemRectMax();
  }

  ImVec2 ImguiContext::GetItemRectSize()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetItemRectSize();
  }

  void ImguiContext::SetItemAllowOverlap()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetItemAllowOverlap();
  }

  bool ImguiContext::IsWindowFocused(ImGuiFocusedFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowFocused(flags);
  }

  bool ImguiContext::IsWindowHovered(ImGuiHoveredFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsWindowHovered(flags);
  }

  bool ImguiContext::IsRectVisible(const ImVec2& size)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsRectVisible(size);
  }

  bool ImguiContext::IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsRectVisible(rect_min, rect_max);
  }

  float ImguiContext::GetTime()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetTime();
  }

  int ImguiContext::GetFrameCount()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetFrameCount();
  }

  ImDrawList* ImguiContext::GetOverlayDrawList()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetOverlayDrawList();
  }

  ImDrawListSharedData* ImguiContext::GetDrawListSharedData()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetDrawListSharedData();
  }

  const char* ImguiContext::GetStyleColorName(ImGuiCol idx)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetStyleColorName(idx);
  }

  ImVec2 ImguiContext::CalcTextSize(const char* text, const char* text_end, bool hide_text_after_double_hash, float wrap_width)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::CalcTextSize(text, text_end, hide_text_after_double_hash, wrap_width);
  }

  void ImguiContext::CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CalcListClipping(items_count, items_height, out_items_display_start, out_items_display_end);
  }


  bool ImguiContext::BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::BeginChildFrame(id, size, flags);
  }

  void ImguiContext::EndChildFrame()
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::EndChildFrame();
  }


  ImVec4 ImguiContext::ColorConvertU32ToFloat4(ImU32 in)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorConvertU32ToFloat4(in);
  }

  ImU32 ImguiContext::ColorConvertFloat4ToU32(const ImVec4& in)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ColorConvertFloat4ToU32(in);
  }

  void ImguiContext::ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ColorConvertRGBtoHSV(r, g, b, out_h, out_s, out_v);
  }

  void ImguiContext::ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::ColorConvertHSVtoRGB(h, s, v, out_r, out_g, out_b);
  }


  int ImguiContext::GetKeyIndex(ImGuiKey imgui_key)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetKeyIndex(imgui_key);
  }

  bool ImguiContext::IsKeyDown(int user_key_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsKeyDown(user_key_index);
  }

  bool ImguiContext::IsKeyPressed(int user_key_index, bool repeat)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsKeyPressed(user_key_index, repeat);
  }

  bool ImguiContext::IsKeyReleased(int user_key_index)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsKeyReleased(user_key_index);
  }

  int ImguiContext::GetKeyPressedAmount(int key_index, float repeat_delay, float rate)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetKeyPressedAmount(key_index, repeat_delay, rate);
  }

  bool ImguiContext::IsMouseDown(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseDown(button);
  }

  bool ImguiContext::IsAnyMouseDown()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsAnyMouseDown();
  }

  bool ImguiContext::IsMouseClicked(int button, bool repeat)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseClicked(button, repeat);
  }

  bool ImguiContext::IsMouseDoubleClicked(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseDoubleClicked(button);
  }

  bool ImguiContext::IsMouseReleased(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseReleased(button);
  }

  bool ImguiContext::IsMouseDragging(int button, float lock_threshold)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseDragging(button, lock_threshold);
  }

  bool ImguiContext::IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMouseHoveringRect(r_min, r_max, clip);
  }

  bool ImguiContext::IsMousePosValid(const ImVec2* mouse_pos)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::IsMousePosValid(mouse_pos);
  }

  ImVec2 ImguiContext::GetMousePos()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMousePos();
  }

  ImVec2 ImguiContext::GetMousePosOnOpeningCurrentPopup()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMousePosOnOpeningCurrentPopup();
  }

  ImVec2 ImguiContext::GetMouseDragDelta(int button, float lock_threshold)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMouseDragDelta(button, lock_threshold);
  }

  void ImguiContext::ResetMouseDragDelta(int button)
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::ResetMouseDragDelta(button);
  }

  ImGuiMouseCursor ImguiContext::GetMouseCursor()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetMouseCursor();
  }

  void ImguiContext::SetMouseCursor(ImGuiMouseCursor type)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetMouseCursor(type);
  }

  void ImguiContext::CaptureKeyboardFromApp(bool capture)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CaptureKeyboardFromApp(capture);
  }

  void ImguiContext::CaptureMouseFromApp(bool capture)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::CaptureMouseFromApp(capture);
  }

  const char* ImguiContext::GetClipboardText()
  {
    ImGui::SetCurrentContext(mContext);
    return ImGui::GetClipboardText();
  }

  void ImguiContext::SetClipboardText(const char* text)
  {
    ImGui::SetCurrentContext(mContext);
    ImGui::SetClipboardText(text);
  }
}