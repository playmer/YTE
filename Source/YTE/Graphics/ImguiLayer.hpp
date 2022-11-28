#pragma once

#include "imgui.h"
#include "ImGuizmo.h"

#include "YTE/Core/Component.hpp"

#include "YTE/Graphics/Generics/Mesh.hpp"
#include "YTE/Graphics/Generics/InstantiatedModel.hpp"
#include "YTE/Graphics/GraphicsView.hpp"

#include "YTE/Platform/Keyboard.hpp"
#include "YTE/Platform/Mouse.hpp"

namespace YTE
{
  class ImguiLayer : public Component
  {
  public:
    YTEDeclareType(ImguiLayer);

    YTE_Shared ImguiLayer(Composition *aOwner, Space *aSpace);
    YTE_Shared ~ImguiLayer() override;

    YTE_Shared void Initialize() override;
    YTE_Shared void Start() override;

    YTE_Shared void MouseScrollCallback(MouseWheelEvent *aEvent);
    YTE_Shared void KeyPressCallback(KeyboardEvent *aEvent);
    YTE_Shared void KeyReleaseCallback(KeyboardEvent *aEvent);
    YTE_Shared void CharacterTypedCallback(KeyboardEvent *aEvent);

    YTE_Shared void ImguiUpdate(LogicUpdate *aUpdate);




    /////////////////////////////////////////////////////////////////////////
    // ImGuizmo
    /////////////////////////////////////////////////////////////////////////
    // call inside your own window and before Manipulate() in order to draw gizmo to that window.
    YTE_Shared void SetDrawlist();

    // call BeginFrame right after ImGui_XXXX_NewFrame();
    YTE_Shared void BeginFrame();

    // return true if mouse cursor is over any gizmo control (axis, plan or screen component)
    YTE_Shared bool IsOver();

    // return true if mouse IsOver or if the gizmo is in moving state
    YTE_Shared bool IsUsing();

    // enable/disable the gizmo. Stay in the state until next call to Enable.
    // gizmo is rendered with gray half transparent color when disabled
    YTE_Shared void Enable(bool enable);

    // helper functions for manualy editing translation/rotation/scale with an input float
    // translation, rotation and scale float points to 3 floats each
    // Angles are in degrees (more suitable for human editing)
    // example:
    // float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    // ImGuizmo::DecomposeMatrixToComponents(gizmoMatrix.m16, matrixTranslation, matrixRotation, matrixScale);
    // ImGui::InputFloat3("Tr", matrixTranslation, 3);
    // ImGui::InputFloat3("Rt", matrixRotation, 3);
    // ImGui::InputFloat3("Sc", matrixScale, 3);
    // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, gizmoMatrix.m16);
    //
    // These functions have some numerical stability issues for now. Use with caution.
    YTE_Shared void DecomposeMatrixToComponents(const float *matrix, float *translation, float *rotation, float *scale);
    YTE_Shared void RecomposeMatrixFromComponents(const float *translation, const float *rotation, const float *scale, float *matrix);

    YTE_Shared void SetRect(float x, float y, float width, float height);

    // Render a cube with face color corresponding to face normal. Usefull for debug/tests
    YTE_Shared void DrawCube(const float *view, const float *projection, float *matrix);

    YTE_Shared void Manipulate(const float *view,
                               const float *projection,
                               ImGuizmo::OPERATION operation,
                               ImGuizmo::MODE mode,
                               float *matrix,
                               float *deltaMatrix = nullptr,
                               float *deltaRotation = nullptr,
                               float *snap = nullptr,
                               float *localBounds = nullptr,
                               float *boundsSnap = nullptr);

    /////////////////////////////////////////////////////////////////////////
    // ImGui
    /////////////////////////////////////////////////////////////////////////
    // Demo, Debug, Informations
    YTE_Shared void          ShowMetricsWindow(bool* p_open = NULL);     // create metrics window. display ImGui internals: draw commands (with individual draw calls and vertices), window list, basic internal state, etc.
    YTE_Shared const char*   GetVersion();

    // Main
    YTE_Shared ImGuiIO&      GetIO();
    YTE_Shared ImGuiStyle&   GetStyle();

    // Styles
    YTE_Shared void          StyleColorsDark(ImGuiStyle* dst = NULL);    // New, recommended style
    YTE_Shared void          StyleColorsClassic(ImGuiStyle* dst = NULL); // Classic imgui style (default)
    YTE_Shared void          StyleColorsLight(ImGuiStyle* dst = NULL);   // Best used with borders and a custom, thicker font

                                                                      // Window
    YTE_Shared bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);   // push window to the stack and start appending to it. see .cpp for details. return false when window is collapsed (so you can early out in your code) but you always need to call End() regardless. 'bool* p_open' creates a widget on the upper-right to close the window (which sets your bool to false).
    YTE_Shared void          End();                                                              // always call even if Begin() return false (which indicates a collapsed window)! finish appending to current window, pop it off the window stack.
    YTE_Shared bool          BeginChild(const char* str_id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);  // begin a scrolling region. size==0.0f: use remaining window size, size<0.0f: use remaining window size minus abs(size). size>0.0f: fixed size. each axis can use a different mode, e.g. ImVec2(0,400).
    YTE_Shared bool          BeginChild(ImGuiID id, const ImVec2& size = ImVec2(0, 0), bool border = false, ImGuiWindowFlags flags = 0);          // "
    YTE_Shared void          EndChild();                                                         // always call even if BeginChild() return false (which indicates a collapsed or clipping child window)
    YTE_Shared ImVec2        GetContentRegionMax();                                              // current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates
    YTE_Shared ImVec2        GetContentRegionAvail();                                            // == GetContentRegionMax() - GetCursorPos()
    YTE_Shared float         GetContentRegionAvailWidth();                                       //
    YTE_Shared ImVec2        GetWindowContentRegionMin();                                        // content boundaries min (roughly (0,0)-Scroll), in window coordinates
    YTE_Shared ImVec2        GetWindowContentRegionMax();                                        // content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates
    YTE_Shared float         GetWindowContentRegionWidth();                                      //
    YTE_Shared ImDrawList*   GetWindowDrawList();                                                // get rendering command-list if you want to append your own draw primitives
    YTE_Shared ImVec2        GetWindowPos();                                                     // get current window position in screen space (useful if you want to do your own drawing via the DrawList api)
    YTE_Shared ImVec2        GetWindowSize();                                                    // get current window size
    YTE_Shared float         GetWindowWidth();
    YTE_Shared float         GetWindowHeight();
    YTE_Shared bool          IsWindowCollapsed();
    YTE_Shared bool          IsWindowAppearing();
    YTE_Shared void          SetWindowFontScale(float scale);                                    // per-window font scale. Adjust IO.FontGlobalScale if you want to scale all windows

    YTE_Shared void          SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0, 0)); // set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc.
    YTE_Shared void          SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);          // set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin()
    YTE_Shared void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max, ImGuiSizeCallback custom_callback = NULL, void* custom_callback_data = NULL); // set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Use callback to apply non-trivial programmatic constraints.
    YTE_Shared void          SetNextWindowContentSize(const ImVec2& size);                       // set next window content size (~ enforce the range of scrollbars). not including window decorations (title bar, menu bar, etc.). set an axis to 0.0f to leave it automatic. call before Begin()
    YTE_Shared void          SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);         // set next window collapsed state. call before Begin()
    YTE_Shared void          SetNextWindowFocus();                                               // set next window to be focused / front-most. call before Begin()
    YTE_Shared void          SetNextWindowBgAlpha(float alpha);                                  // set next window background color alpha. helper to easily modify ImGuiCol_WindowBg/ChildBg/PopupBg.
    YTE_Shared void          SetWindowPos(const ImVec2& pos, ImGuiCond cond = 0);                // (not recommended) set current window position - call within Begin()/End(). prefer using SetNextWindowPos(), as this may incur tearing and side-effects.
    YTE_Shared void          SetWindowSize(const ImVec2& size, ImGuiCond cond = 0);              // (not recommended) set current window size - call within Begin()/End(). set to ImVec2(0,0) to force an auto-fit. prefer using SetNextWindowSize(), as this may incur tearing and minor side-effects.    
    YTE_Shared void          SetWindowCollapsed(bool collapsed, ImGuiCond cond = 0);             // (not recommended) set current window collapsed state. prefer using SetNextWindowCollapsed().
    YTE_Shared void          SetWindowFocus();                                                   // (not recommended) set current window to be focused / front-most. prefer using SetNextWindowFocus().
    YTE_Shared void          SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond = 0);      // set named window position.
    YTE_Shared void          SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond = 0);    // set named window size. set axis to 0.0f to force an auto-fit on this axis.
    YTE_Shared void          SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond = 0);   // set named window collapsed state
    YTE_Shared void          SetWindowFocus(const char* name);                                           // set named window to be focused / front-most. use NULL to remove focus.

    YTE_Shared float         GetScrollX();                                                       // get scrolling amount [0..GetScrollMaxX()]
    YTE_Shared float         GetScrollY();                                                       // get scrolling amount [0..GetScrollMaxY()]
    YTE_Shared float         GetScrollMaxX();                                                    // get maximum scrolling amount ~~ ContentSize.X - WindowSize.X
    YTE_Shared float         GetScrollMaxY();                                                    // get maximum scrolling amount ~~ ContentSize.Y - WindowSize.Y
    YTE_Shared void          SetScrollX(float scroll_x);                                         // set scrolling amount [0..GetScrollMaxX()]
    YTE_Shared void          SetScrollY(float scroll_y);                                         // set scrolling amount [0..GetScrollMaxY()]
    YTE_Shared void          SetScrollHere(float center_y_ratio = 0.5f);                         // adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    YTE_Shared void          SetScrollFromPosY(float pos_y, float center_y_ratio = 0.5f);        // adjust scrolling amount to make given position valid. use GetCursorPos() or GetCursorStartPos()+offset to get valid positions.
    YTE_Shared void          SetStateStorage(ImGuiStorage* tree);                                // replace tree state storage with our own (if you want to manipulate it yourself, typically clear subsection of it)
    YTE_Shared ImGuiStorage* GetStateStorage();

    // Parameters stacks (shared)
    YTE_Shared void          PushFont(ImFont* font);                                             // use NULL as a shortcut to push default font
    YTE_Shared void          PopFont();
    YTE_Shared void          PushStyleColor(ImGuiCol idx, ImU32 col);
    YTE_Shared void          PushStyleColor(ImGuiCol idx, const ImVec4& col);
    YTE_Shared void          PopStyleColor(int count = 1);
    YTE_Shared void          PushStyleVar(ImGuiStyleVar idx, float val);
    YTE_Shared void          PushStyleVar(ImGuiStyleVar idx, const ImVec2& val);
    YTE_Shared void          PopStyleVar(int count = 1);
    YTE_Shared const ImVec4& GetStyleColorVec4(ImGuiCol idx);                                    // retrieve style color as stored in ImGuiStyle structure. use to feed back into PushStyleColor(), otherwhise use GetColorU32() to get style color + style alpha.
    YTE_Shared ImFont*       GetFont();                                                          // get current font
    YTE_Shared float         GetFontSize();                                                      // get current font size (= height in pixels) of current font with current scale applied
    YTE_Shared ImVec2        GetFontTexUvWhitePixel();                                           // get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API
    YTE_Shared ImU32         GetColorU32(ImGuiCol idx, float alpha_mul = 1.0f);                  // retrieve given style color with style alpha applied and optional extra alpha multiplier
    YTE_Shared ImU32         GetColorU32(const ImVec4& col);                                     // retrieve given color with style alpha applied
    YTE_Shared ImU32         GetColorU32(ImU32 col);                                             // retrieve given color with style alpha applied

    // Parameters stacks (current window)
    YTE_Shared void          PushItemWidth(float item_width);                                    // width of items for the common item+label case, pixels. 0.0f = default to ~2/3 of windows width, >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -1.0f always align width to the right side)
    YTE_Shared void          PopItemWidth();
    YTE_Shared float         CalcItemWidth();                                                    // width of item given pushed settings and current cursor position
    YTE_Shared void          PushTextWrapPos(float wrap_pos_x = 0.0f);                           // word-wrapping for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space
    YTE_Shared void          PopTextWrapPos();
    YTE_Shared void          PushAllowKeyboardFocus(bool allow_keyboard_focus);                  // allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets
    YTE_Shared void          PopAllowKeyboardFocus();
    YTE_Shared void          PushButtonRepeat(bool repeat);                                      // in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame.
    YTE_Shared void          PopButtonRepeat();

    // Cursor / Layout
    YTE_Shared void          Separator();                                                        // separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator.
    YTE_Shared void          SameLine(float pos_x = 0.0f, float spacing_w = -1.0f);              // call between widgets or groups to layout them horizontally
    YTE_Shared void          NewLine();                                                          // undo a SameLine()
    YTE_Shared void          Spacing();                                                          // add vertical spacing
    YTE_Shared void          Dummy(const ImVec2& size);                                          // add a dummy item of given size
    YTE_Shared void          Indent(float indent_w = 0.0f);                                      // move content position toward the right, by style.IndentSpacing or indent_w if != 0
    YTE_Shared void          Unindent(float indent_w = 0.0f);                                    // move content position back to the left, by style.IndentSpacing or indent_w if != 0
    YTE_Shared void          BeginGroup();                                                       // lock horizontal starting position + capture group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.)
    YTE_Shared void          EndGroup();
    YTE_Shared ImVec2        GetCursorPos();                                                     // cursor position is relative to window position
    YTE_Shared float         GetCursorPosX();                                                    // "
    YTE_Shared float         GetCursorPosY();                                                    // "
    YTE_Shared void          SetCursorPos(const ImVec2& local_pos);                              // "
    YTE_Shared void          SetCursorPosX(float x);                                             // "
    YTE_Shared void          SetCursorPosY(float y);                                             // "
    YTE_Shared ImVec2        GetCursorStartPos();                                                // initial cursor position
    YTE_Shared ImVec2        GetCursorScreenPos();                                               // cursor position in absolute screen coordinates [0..io.DisplaySize] (useful to work with ImDrawList API)
    YTE_Shared void          SetCursorScreenPos(const ImVec2& pos);                              // cursor position in absolute screen coordinates [0..io.DisplaySize]
    YTE_Shared void          AlignTextToFramePadding();                                          // vertically align/lower upcoming text to FramePadding.y so that it will aligns to upcoming widgets (call if you have text on a line before regular widgets)
    YTE_Shared float         GetTextLineHeight();                                                // ~ FontSize
    YTE_Shared float         GetTextLineHeightWithSpacing();                                     // ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text)
    YTE_Shared float         GetFrameHeight();                                                   // ~ FontSize + style.FramePadding.y * 2
    YTE_Shared float         GetFrameHeightWithSpacing();                                        // ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets)

    // Columns
    // You can also use SameLine(pos_x) for simplified columns. The columns API is still work-in-progress and rather lacking.
    YTE_Shared void          Columns(int count = 1, const char* id = NULL, bool border = true);
    YTE_Shared void          NextColumn();                                                       // next column, defaults to current row or next row if the current row is finished
    YTE_Shared int           GetColumnIndex();                                                   // get current column index
    YTE_Shared float         GetColumnWidth(int column_index = -1);                              // get column width (in pixels). pass -1 to use current column
    YTE_Shared void          SetColumnWidth(int column_index, float width);                      // set column width (in pixels). pass -1 to use current column
    YTE_Shared float         GetColumnOffset(int column_index = -1);                             // get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f
    YTE_Shared void          SetColumnOffset(int column_index, float offset_x);                  // set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column
    YTE_Shared int           GetColumnsCount();

    // ID scopes
    // If you are creating widgets in a loop you most likely want to push a unique identifier (e.g. object pointer, loop index) so ImGui can differentiate them.
    // You can also use the "##foobar" syntax within widget label to distinguish them from each others. Read "A primer on the use of labels/IDs" in the FAQ for more details.
    YTE_Shared void          PushID(const char* str_id);                                         // push identifier into the ID stack. IDs are hash of the entire stack!
    YTE_Shared void          PushID(const char* str_id_begin, const char* str_id_end);
    YTE_Shared void          PushID(const void* ptr_id);
    YTE_Shared void          PushID(int int_id);
    YTE_Shared void          PopID();
    YTE_Shared ImGuiID       GetID(const char* str_id);                                          // calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into ImGuiStorage yourself
    YTE_Shared ImGuiID       GetID(const char* str_id_begin, const char* str_id_end);
    YTE_Shared ImGuiID       GetID(const void* ptr_id);

    // Widgets: Text
    YTE_Shared void          TextUnformatted(const char* text, const char* text_end = NULL);               // raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text.
    YTE_Shared void          Text(const char* fmt, ...); // simple formatted text
    YTE_Shared void          TextV(const char* fmt, va_list args);
    YTE_Shared void          TextColored(const ImVec4& col, const char* fmt, ...); // shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt, ...); PopStyleColor();
    YTE_Shared void          TextColoredV(const ImVec4& col, const char* fmt, va_list args);
    YTE_Shared void          TextDisabled(const char* fmt, ...); // shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt, ...); PopStyleColor();
    YTE_Shared void          TextDisabledV(const char* fmt, va_list args);
    YTE_Shared void          TextWrapped(const char* fmt, ...); // shortcut for PushTextWrapPos(0.0f); Text(fmt, ...); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize().
    YTE_Shared void          TextWrappedV(const char* fmt, va_list args);
    YTE_Shared void          LabelText(const char* label, const char* fmt, ...); // display text+label aligned the same way as value+label widgets
    YTE_Shared void          LabelTextV(const char* label, const char* fmt, va_list args);
    YTE_Shared void          BulletText(const char* fmt, ...); // shortcut for Bullet()+Text()
    YTE_Shared void          BulletTextV(const char* fmt, va_list args);
    YTE_Shared void          Bullet();                                                                     // draw a small circle and keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses

    // Widgets: Main
    YTE_Shared bool          Button(const char* label, const ImVec2& size = ImVec2(0, 0));            // button
    YTE_Shared bool          SmallButton(const char* label);                                         // button with FramePadding=(0,0) to easily embed within text
    YTE_Shared bool          ArrowButton(const char* str_id, ImGuiDir dir);
    YTE_Shared bool          InvisibleButton(const char* str_id, const ImVec2& size);                // button behavior without the visuals, useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.)
    YTE_Shared void          Image(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
    YTE_Shared bool          ImageButton(ImTextureID user_texture_id, const ImVec2& size, const ImVec2& uv0 = ImVec2(0, 0), const ImVec2& uv1 = ImVec2(1, 1), int frame_padding = -1, const ImVec4& bg_col = ImVec4(0, 0, 0, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1));    // <0 frame_padding uses default frame padding settings. 0 for no padding
    YTE_Shared bool          Checkbox(const char* label, bool* v);
    YTE_Shared bool          CheckboxFlags(const char* label, unsigned int* flags, unsigned int flags_value);
    YTE_Shared bool          RadioButton(const char* label, bool active);
    YTE_Shared bool          RadioButton(const char* label, int* v, int v_button);
    YTE_Shared void          PlotLines(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    YTE_Shared void          PlotLines(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    YTE_Shared void          PlotHistogram(const char* label, const float* values, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0), int stride = sizeof(float));
    YTE_Shared void          PlotHistogram(const char* label, float(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset = 0, const char* overlay_text = NULL, float scale_min = FLT_MAX, float scale_max = FLT_MAX, ImVec2 graph_size = ImVec2(0, 0));
    YTE_Shared void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-1, 0), const char* overlay = NULL);

    // Widgets: Combo Box
    // The new BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it. 
    // The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose.
    YTE_Shared bool          BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
    YTE_Shared void          EndCombo(); // only call EndCombo() if BeginCombo() returns true!
    YTE_Shared bool          Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);
    YTE_Shared bool          Combo(const char* label, int* current_item, const char* items_separated_by_zeros, int popup_max_height_in_items = -1);      // Separate items with \0 within a string, end item-list with \0\0. e.g. "One\0Two\0Three\0"
    YTE_Shared bool          Combo(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1);

    // Widgets: Drags (tip: ctrl+click on a drag box to input with keyboard. manually input values aren't clamped, can go off-bounds)
    // For all the Float2/Float3/Float4/Int2/Int3/Int4 versions of every functions, note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can pass address of your first element out of a contiguous set, e.g. &myvector.x
    // Speed are per-pixel of mouse movement (v_speed=0.2f: mouse needs to move by 5 pixels to increase value by 1). For gamepad/keyboard navigation, minimum speed is Max(v_speed, minimum_step_at_given_precision).
    YTE_Shared bool          DragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);     // If v_min >= v_max we have no bound
    YTE_Shared bool          DragFloat2(const char* label, float v[2], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          DragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          DragFloat4(const char* label, float v[4], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          DragFloatRange2(const char* label, float* v_current_min, float* v_current_max, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", const char* display_format_max = NULL, float power = 1.0f);
    YTE_Shared bool          DragInt(const char* label, int* v, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");                                       // If v_min >= v_max we have no bound
    YTE_Shared bool          DragInt2(const char* label, int v[2], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
    YTE_Shared bool          DragInt3(const char* label, int v[3], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
    YTE_Shared bool          DragInt4(const char* label, int v[4], float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f");
    YTE_Shared bool          DragIntRange2(const char* label, int* v_current_min, int* v_current_max, float v_speed = 1.0f, int v_min = 0, int v_max = 0, const char* display_format = "%.0f", const char* display_format_max = NULL);

    // Widgets: Input with Keyboard
    YTE_Shared bool          InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiTextEditCallback callback = NULL, void* user_data = NULL);
    YTE_Shared bool          InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiTextEditCallback callback = NULL, void* user_data = NULL);
    YTE_Shared bool          InputFloat(const char* label, float* v, float step = 0.0f, float step_fast = 0.0f, int decimal_precision = -1, ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputFloat2(const char* label, float v[2], int decimal_precision = -1, ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputFloat3(const char* label, float v[3], int decimal_precision = -1, ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputFloat4(const char* label, float v[4], int decimal_precision = -1, ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputInt2(const char* label, int v[2], ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputInt3(const char* label, int v[3], ImGuiInputTextFlags extra_flags = 0);
    YTE_Shared bool          InputInt4(const char* label, int v[4], ImGuiInputTextFlags extra_flags = 0);

    // Widgets: Sliders (tip: ctrl+click on a slider to input with keyboard. manually input values aren't clamped, can go off-bounds)
    YTE_Shared bool          SliderFloat(const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);     // adjust display_format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for logarithmic sliders
    YTE_Shared bool          SliderFloat2(const char* label, float v[2], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          SliderFloat3(const char* label, float v[3], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          SliderFloat4(const char* label, float v[4], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          SliderAngle(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f);
    YTE_Shared bool          SliderInt(const char* label, int* v, int v_min, int v_max, const char* display_format = "%.0f");
    YTE_Shared bool          SliderInt2(const char* label, int v[2], int v_min, int v_max, const char* display_format = "%.0f");
    YTE_Shared bool          SliderInt3(const char* label, int v[3], int v_min, int v_max, const char* display_format = "%.0f");
    YTE_Shared bool          SliderInt4(const char* label, int v[4], int v_min, int v_max, const char* display_format = "%.0f");
    YTE_Shared bool          VSliderFloat(const char* label, const ImVec2& size, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
    YTE_Shared bool          VSliderInt(const char* label, const ImVec2& size, int* v, int v_min, int v_max, const char* display_format = "%.0f");

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    // Note that a 'float v[X]' function argument is the same as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible. You can the pass the address of a first float element out of a contiguous structure, e.g. &myvector.x
    YTE_Shared bool          ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    YTE_Shared bool          ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
    YTE_Shared bool          ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    YTE_Shared bool          ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
    YTE_Shared bool          ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0, 0));  // display a colored square/button, hover for details, return true when pressed.
    YTE_Shared void          SetColorEditOptions(ImGuiColorEditFlags flags);                         // initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls.

    // Widgets: Trees
    YTE_Shared bool          TreeNode(const char* label);                                            // if returning 'true' the node is open and the tree id is pushed into the id stack. user is responsible for calling TreePop().
    YTE_Shared bool          TreeNode(const char* str_id, const char* fmt, ...);       // read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet().
    YTE_Shared bool          TreeNode(const void* ptr_id, const char* fmt, ...);       // "
    YTE_Shared bool          TreeNodeV(const char* str_id, const char* fmt, va_list args);
    YTE_Shared bool          TreeNodeV(const void* ptr_id, const char* fmt, va_list args);
    YTE_Shared bool          TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);
    YTE_Shared bool          TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, ...);
    YTE_Shared bool          TreeNodeEx(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, ...);
    YTE_Shared bool          TreeNodeExV(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);
    YTE_Shared bool          TreeNodeExV(const void* ptr_id, ImGuiTreeNodeFlags flags, const char* fmt, va_list args);
    YTE_Shared void          TreePush(const char* str_id);                                           // ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call Push/Pop yourself for layout purpose
    YTE_Shared void          TreePush(const void* ptr_id = NULL);                                    // "
    YTE_Shared void          TreePop();                                                              // ~ Unindent()+PopId()
    YTE_Shared void          TreeAdvanceToLabelPos();                                                // advance cursor x position by GetTreeNodeToLabelSpacing()
    YTE_Shared float         GetTreeNodeToLabelSpacing();                                            // horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode
    YTE_Shared void          SetNextTreeNodeOpen(bool is_open, ImGuiCond cond = 0);                  // set next TreeNode/CollapsingHeader open state.
    YTE_Shared bool          CollapsingHeader(const char* label, ImGuiTreeNodeFlags flags = 0);      // if returning 'true' the header is open. doesn't indent nor push on ID stack. user doesn't have to call TreePop().
    YTE_Shared bool          CollapsingHeader(const char* label, bool* p_open, ImGuiTreeNodeFlags flags = 0); // when 'p_open' isn't NULL, display an additional small close button on upper right of the header

                                                                                                            // Widgets: Selectable / Lists
    YTE_Shared bool          Selectable(const char* label, bool selected = false, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));  // "bool selected" carry the selection state (read-only). Selectable() is clicked is returns true so you can modify your selection state. size.x==0.0: use remaining width, size.x>0.0: specify width. size.y==0.0: use label height, size.y>0.0: specify height
    YTE_Shared bool          Selectable(const char* label, bool* p_selected, ImGuiSelectableFlags flags = 0, const ImVec2& size = ImVec2(0, 0));       // "bool* p_selected" point to the selection state (read-write), as a convenient helper.
    YTE_Shared bool          ListBox(const char* label, int* current_item, const char* const items[], int items_count, int height_in_items = -1);
    YTE_Shared bool          ListBox(const char* label, int* current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int height_in_items = -1);
    YTE_Shared bool          ListBoxHeader(const char* label, const ImVec2& size = ImVec2(0, 0));     // use if you want to reimplement ListBox() will custom data or interactions. make sure to call ListBoxFooter() afterwards.
    YTE_Shared bool          ListBoxHeader(const char* label, int items_count, int height_in_items = -1); // "
    YTE_Shared void          ListBoxFooter();                                                        // terminate the scrolling region

                                                                                                  // Widgets: Value() Helpers. Output single value in "name: value" format (tip: freely declare more in your code to handle your types. you can add functions to the ImGui namespace)
    YTE_Shared void          Value(const char* prefix, bool b);
    YTE_Shared void          Value(const char* prefix, int v);
    YTE_Shared void          Value(const char* prefix, unsigned int v);
    YTE_Shared void          Value(const char* prefix, float v, const char* float_format = NULL);

    // Tooltips
    YTE_Shared void          SetTooltip(const char* fmt, ...); // set text tooltip under mouse-cursor, typically use with ImGui::IsItemHovered(). overidde any previous call to SetTooltip().
    YTE_Shared void          SetTooltipV(const char* fmt, va_list args);
    YTE_Shared void          BeginTooltip();                                                     // begin/append a tooltip window. to create full-featured tooltip (with any kind of contents).
    YTE_Shared void          EndTooltip();

    // Menus
    YTE_Shared bool          BeginMainMenuBar();                                                 // create and append to a full screen menu-bar.
    YTE_Shared void          EndMainMenuBar();                                                   // only call EndMainMenuBar() if BeginMainMenuBar() returns true!
    YTE_Shared bool          BeginMenuBar();                                                     // append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar flag set on parent window).
    YTE_Shared void          EndMenuBar();                                                       // only call EndMenuBar() if BeginMenuBar() returns true!
    YTE_Shared bool          BeginMenu(const char* label, bool enabled = true);                  // create a sub-menu entry. only call EndMenu() if this returns true!
    YTE_Shared void          EndMenu();                                                          // only call EndBegin() if BeginMenu() returns true!
    YTE_Shared bool          MenuItem(const char* label, const char* shortcut = NULL, bool selected = false, bool enabled = true);  // return true when activated. shortcuts are displayed for convenience but not processed by ImGui at the moment
    YTE_Shared bool          MenuItem(const char* label, const char* shortcut, bool* p_selected, bool enabled = true);              // return true when activated + toggle (*p_selected) if p_selected != NULL

    // Popups
    YTE_Shared void          OpenPopup(const char* str_id);                                      // call to mark popup as open (don't call every frame!). popups are closed when user click outside, or if CloseCurrentPopup() is called within a BeginPopup()/EndPopup() block. By default, Selectable()/MenuItem() are calling CloseCurrentPopup(). Popup identifiers are relative to the current ID-stack (so OpenPopup and BeginPopup needs to be at the same level).
    YTE_Shared bool          BeginPopup(const char* str_id, ImGuiWindowFlags flags = 0);                                             // return true if the popup is open, and you can start outputting to it. only call EndPopup() if BeginPopup() returns true!
    YTE_Shared bool          BeginPopupContextItem(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked on last item. if you can pass a NULL str_id only if the previous item had an id. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp!
    YTE_Shared bool          BeginPopupContextWindow(const char* str_id = NULL, int mouse_button = 1, bool also_over_items = true);  // helper to open and begin popup when clicked on current window.
    YTE_Shared bool          BeginPopupContextVoid(const char* str_id = NULL, int mouse_button = 1);                                 // helper to open and begin popup when clicked in void (where there are no imgui windows).
    YTE_Shared bool          BeginPopupModal(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);                     // modal dialog (regular window with title bar, block interactions behind the modal window, can't close the modal window by clicking outside)
    YTE_Shared void          EndPopup();                                                                                             // only call EndPopup() if BeginPopupXXX() returns true!
    YTE_Shared bool          OpenPopupOnItemClick(const char* str_id = NULL, int mouse_button = 1);                                  // helper to open popup when clicked on last item. return true when just opened.
    YTE_Shared bool          IsPopupOpen(const char* str_id);                                    // return true if the popup is open
    YTE_Shared void          CloseCurrentPopup();                                                // close the popup we have begin-ed into. clicking on a MenuItem or Selectable automatically close the current popup.

    // Logging/Capture: all text output from interface is captured to tty/file/clipboard. By default, tree nodes are automatically opened during logging.
    YTE_Shared void          LogToTTY(int max_depth = -1);                                       // start logging to tty
    YTE_Shared void          LogToFile(int max_depth = -1, const char* filename = NULL);         // start logging to file
    YTE_Shared void          LogToClipboard(int max_depth = -1);                                 // start logging to OS clipboard
    YTE_Shared void          LogFinish();                                                        // stop logging (close file, etc.)
    YTE_Shared void          LogButtons();                                                       // helper to display buttons for logging to tty/file/clipboard
    YTE_Shared void          LogText(const char* fmt, ...) IM_FMTARGS(1);                        // pass text data straight to log (without being displayed)

    // Drag and Drop
    // [BETA API] Missing Demo code. API may evolve.
    YTE_Shared bool          BeginDragDropSource(ImGuiDragDropFlags flags = 0);                                      // call when the current item is active. If this return true, you can call SetDragDropPayload() + EndDragDropSource()
    YTE_Shared bool          SetDragDropPayload(const char* type, const void* data, size_t size, ImGuiCond cond = 0);// type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear imgui internal types. Data is copied and held by imgui.
    YTE_Shared void          EndDragDropSource();                                                                    // only call EndDragDropSource() if BeginDragDropSource() returns true!
    YTE_Shared bool          BeginDragDropTarget();                                                                  // call after submitting an item that may receive an item. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget()
    YTE_Shared const ImGuiPayload* AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);            // accept contents of a given type. If ImGuiDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
    YTE_Shared void          EndDragDropTarget();                                                                    // only call EndDragDropTarget() if BeginDragDropTarget() returns true!

                                                                                                                  // Clipping
    YTE_Shared void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    YTE_Shared void          PopClipRect();

    // Focus, Activation
    // (Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHere()" when applicable, to make your code more forward compatible when navigation branch is merged)
    YTE_Shared void          SetItemDefaultFocus();                                              // make last item the default focused item of a window. Please use instead of "if (IsWindowAppearing()) SetScrollHere()" to signify "default item".
    YTE_Shared void          SetKeyboardFocusHere(int offset = 0);                               // focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget.

    // Utilities
    YTE_Shared bool          IsItemHovered(ImGuiHoveredFlags flags = 0);                         // is the last item hovered? (and usable, aka not blocked by a popup, etc.). See ImGuiHoveredFlags for more options.
    YTE_Shared bool          IsItemActive();                                                     // is the last item active? (e.g. button being held, text field being edited- items that don't interact will always return false)
    YTE_Shared bool          IsItemFocused();                                                    // is the last item focused for keyboard/gamepad navigation?
    YTE_Shared bool          IsItemClicked(int mouse_button = 0);                                // is the last item clicked? (e.g. button/node just clicked on)
    YTE_Shared bool          IsItemVisible();                                                    // is the last item visible? (aka not out of sight due to clipping/scrolling.)
    YTE_Shared bool          IsAnyItemHovered();
    YTE_Shared bool          IsAnyItemActive();
    YTE_Shared bool          IsAnyItemFocused();
    YTE_Shared ImVec2        GetItemRectMin();                                                   // get bounding rectangle of last item, in screen space
    YTE_Shared ImVec2        GetItemRectMax();                                                   // "
    YTE_Shared ImVec2        GetItemRectSize();                                                  // get size of last item, in screen space
    YTE_Shared void          SetItemAllowOverlap();                                              // allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area.
    YTE_Shared bool          IsWindowFocused(ImGuiFocusedFlags flags = 0);                       // is current window focused? or its root/child, depending on flags. see flags for options.
    YTE_Shared bool          IsWindowHovered(ImGuiHoveredFlags flags = 0);                       // is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ!
    YTE_Shared bool          IsRectVisible(const ImVec2& size);                                  // test if rectangle (of given size, starting from cursor position) is visible / not clipped.
    YTE_Shared bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);      // test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side.
    YTE_Shared float         GetTime();
    YTE_Shared int           GetFrameCount();
    YTE_Shared ImDrawList*   GetOverlayDrawList();                                               // this draw list will be the last rendered one, useful to quickly draw overlays shapes/text
    YTE_Shared ImDrawListSharedData* GetDrawListSharedData();
    YTE_Shared const char*   GetStyleColorName(ImGuiCol idx);
    YTE_Shared ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);
    YTE_Shared void          CalcListClipping(int items_count, float items_height, int* out_items_display_start, int* out_items_display_end);    // calculate coarse clipping for large list of evenly sized items. Prefer using the ImGuiListClipper higher-level helper if you can.

    YTE_Shared bool          BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags = 0); // helper to create a child window / scrolling region that looks like a normal widget frame
    YTE_Shared void          EndChildFrame();                                                    // always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window)

    YTE_Shared ImVec4        ColorConvertU32ToFloat4(ImU32 in);
    YTE_Shared ImU32         ColorConvertFloat4ToU32(const ImVec4& in);
    YTE_Shared void          ColorConvertRGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v);
    YTE_Shared void          ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b);

    // Inputs
    YTE_Shared int           GetKeyIndex(ImGuiKey imgui_key);                                    // map ImGuiKey_* values into user's key index. == io.KeyMap[key]
    YTE_Shared bool          IsKeyDown(int user_key_index);                                      // is key being held. == io.KeysDown[user_key_index]. note that imgui doesn't know the semantic of each entry of io.KeyDown[]. Use your own indices/enums according to how your backend/engine stored them into KeyDown[]!
    YTE_Shared bool          IsKeyPressed(int user_key_index, bool repeat = true);               // was key pressed (went from !Down to Down). if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate
    YTE_Shared bool          IsKeyReleased(int user_key_index);                                  // was key released (went from Down to !Down)..
    YTE_Shared int           GetKeyPressedAmount(int key_index, float repeat_delay, float rate); // uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate
    YTE_Shared bool          IsMouseDown(int button);                                            // is mouse button held
    YTE_Shared bool          IsAnyMouseDown();                                                   // is any mouse button held
    YTE_Shared bool          IsMouseClicked(int button, bool repeat = false);                    // did mouse button clicked (went from !Down to Down)
    YTE_Shared bool          IsMouseDoubleClicked(int button);                                   // did mouse button double-clicked. a double-click returns false in IsMouseClicked(). uses io.MouseDoubleClickTime.
    YTE_Shared bool          IsMouseReleased(int button);                                        // did mouse button released (went from Down to !Down)
    YTE_Shared bool          IsMouseDragging(int button = 0, float lock_threshold = -1.0f);      // is mouse dragging. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
    YTE_Shared bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);  // is mouse hovering given bounding rect (in screen space). clipped by current clipping settings. disregarding of consideration of focus/window ordering/blocked by a popup.
    YTE_Shared bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);                    //
    YTE_Shared ImVec2        GetMousePos();                                                      // shortcut to ImGui::GetIO().MousePos provided by user, to be consistent with other calls
    YTE_Shared ImVec2        GetMousePosOnOpeningCurrentPopup();                                 // retrieve backup of mouse positioning at the time of opening popup we have BeginPopup() into
    YTE_Shared ImVec2        GetMouseDragDelta(int button = 0, float lock_threshold = -1.0f);    // dragging amount since clicking. if lock_threshold < -1.0f uses io.MouseDraggingThreshold
    YTE_Shared void          ResetMouseDragDelta(int button = 0);                                //
    YTE_Shared ImGuiMouseCursor GetMouseCursor();                                                // get desired cursor type, reset in ImGui::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor ImGui will render those for you
    YTE_Shared void          SetMouseCursor(ImGuiMouseCursor type);                              // set desired cursor type
    YTE_Shared void          CaptureKeyboardFromApp(bool capture = true);                        // manually override io.WantCaptureKeyboard flag next frame (said flag is entirely left for your application handle). e.g. force capture keyboard when your widget is being hovered.
    YTE_Shared void          CaptureMouseFromApp(bool capture = true);                           // manually override io.WantCaptureMouse flag next frame (said flag is entirely left for your application handle).

                                                                                              // Clipboard Utilities (also see the LogToClipboard() function to capture or output text data to the clipboard)
    YTE_Shared const char*   GetClipboardText();
    YTE_Shared void          SetClipboardText(const char* text);

    void SetCurrentContext()
    {
      ImGui::SetCurrentContext(mContext);
    }

    SubmeshData& GetSubmeshData()
    {
      return mSubmesh;
    }

    std::unique_ptr<InstantiatedModel>& GetInstantiatedModel()
    {
      return mInstantiatedModel;
    }

  private:
    static const char* GetClipboardTextImplementation(void *aSelf);
    static void SetClipboardTextImplementation(void *aSelf, const char* text);

    SubmeshData mSubmesh;
    std::unique_ptr<InstantiatedModel> mInstantiatedModel;
    ImGuiContext *mContext;
    GraphicsView *mView;
    std::string mClipboard;
  };
}
