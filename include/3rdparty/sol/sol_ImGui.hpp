// Initial update to imgui[docking] v1.88 WIP.

#pragma once

#include <imgui.h>
#include <imgui_internal.h>
#include <sol/sol.hpp>
#include <string>

namespace sol_ImGui {
// Windows
inline bool Begin(const std::string &name) {
  return Begin(name.c_str());
}
inline std::tuple<bool, bool> Begin(const std::string &name, bool open) {
  if (!open)
    return std::make_tuple(false, false);

  auto [shouldDraw, _] = Begin(name.c_str(), &open);

  if (!open) {
    End();
    return std::make_tuple(false, false);
  }

  return std::make_tuple(open, shouldDraw);
}
inline std::tuple<bool, bool> Begin(const std::string &name, bool open,
                                    int flags) {
  if (!open)
    return std::make_tuple(false, false);
  auto [shouldDraw, _] =
      Begin(name.c_str(), &open, static_cast<ImGuiWindowFlags_>(flags));

  if (!open) {
    End();
    return std::make_tuple(false, false);
  }

  return std::make_tuple(open, shouldDraw);
}
inline void End() { End(); }

// Child Windows
inline bool BeginChild(const std::string &name) {
  return BeginChild(name.c_str());
}
inline bool BeginChild(const std::string &name, float sizeX) {
  return BeginChild(name.c_str(), {sizeX, 0});
}
inline bool BeginChild(const std::string &name, float sizeX, float sizeY) {
  return BeginChild(name.c_str(), {sizeX, sizeY});
}
inline bool BeginChild(const std::string &name, float sizeX, float sizeY,
                       bool border) {
  return BeginChild(name.c_str(), {sizeX, sizeY}, border);
}
inline bool BeginChild(const std::string &name, float sizeX, float sizeY,
                       bool border, int flags) {
  return BeginChild(name.c_str(), {sizeX, sizeY}, border,
                           static_cast<ImGuiWindowFlags>(flags));
}
inline void EndChild() { EndChild(); }

// Windows Utilities
inline bool IsWindowAppearing() { return IsWindowAppearing(); }
inline bool IsWindowCollapsed() { return IsWindowCollapsed(); }
inline bool IsWindowFocused() { return IsWindowFocused(); }
inline bool IsWindowFocused(int flags) {
  return IsWindowFocused(static_cast<ImGuiFocusedFlags>(flags));
}
inline bool IsWindowHovered() { return IsWindowHovered(); }
inline bool IsWindowHovered(int flags) {
  return IsWindowHovered(static_cast<ImGuiHoveredFlags>(flags));
}
inline ImDrawList *GetWindowDrawList() {
  return nullptr; /* TODO: GetWindowDrawList() ==> UNSUPPORTED */
}
inline float GetWindowDpiScale() { return GetWindowDpiScale(); }
inline ImGuiViewport *GetWindowViewport() {
  return nullptr; /* TODO: GetWindowViewport() ==> UNSUPPORTED */
}
inline std::tuple<float, float> GetWindowPos() {
  const auto vec2{GetWindowPos()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetWindowSize() {
  const auto vec2{GetWindowSize()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline float GetWindowWidth() { return GetWindowWidth(); }
inline float GetWindowHeight() { return GetWindowHeight(); }

// Prefer using SetNext...
inline void SetNextWindowPos(float posX, float posY) {
  SetNextWindowPos({posX, posY});
}
inline void SetNextWindowPos(float posX, float posY, int cond) {
  SetNextWindowPos({posX, posY}, static_cast<ImGuiCond>(cond));
}
inline void SetNextWindowPos(float posX, float posY, int cond, float pivotX,
                             float pivotY) {
  SetNextWindowPos({posX, posY}, static_cast<ImGuiCond>(cond),
                          {pivotX, pivotY});
}
inline void SetNextWindowSize(float sizeX, float sizeY) {
  SetNextWindowSize({sizeX, sizeY});
}
inline void SetNextWindowSize(float sizeX, float sizeY, int cond) {
  SetNextWindowSize({sizeX, sizeY}, static_cast<ImGuiCond>(cond));
}
inline void SetNextWindowSizeConstraints(float minX, float minY, float maxX,
                                         float maxY) {
  SetNextWindowSizeConstraints({minX, minY}, {maxX, maxY});
}
inline void SetNextWindowContentSize(float sizeX, float sizeY) {
  SetNextWindowContentSize({sizeX, sizeY});
}
inline void SetNextWindowCollapsed(bool collapsed) {
  SetNextWindowCollapsed(collapsed);
}
inline void SetNextWindowCollapsed(bool collapsed, int cond) {
  SetNextWindowCollapsed(collapsed, static_cast<ImGuiCond>(cond));
}
inline void SetNextWindowFocus() { SetNextWindowFocus(); }
inline void SetNextWindowBgAlpha(float alpha) {
  SetNextWindowBgAlpha(alpha);
}
inline void SetWindowPos(float posX, float posY) {
  SetWindowPos({posX, posY});
}
inline void SetWindowPos(float posX, float posY, int cond) {
  SetWindowPos({posX, posY}, static_cast<ImGuiCond>(cond));
}
inline void SetWindowSize(float sizeX, float sizeY) {
  SetWindowSize({sizeX, sizeY});
}
inline void SetWindowSize(float sizeX, float sizeY, int cond) {
  SetWindowSize({sizeX, sizeY}, static_cast<ImGuiCond>(cond));
}
inline void SetWindowCollapsed(bool collapsed) {
  SetWindowCollapsed(collapsed);
}
inline void SetWindowCollapsed(bool collapsed, int cond) {
  SetWindowCollapsed(collapsed, static_cast<ImGuiCond>(cond));
}
inline void SetWindowFocus() { SetWindowFocus(); }
inline void SetWindowFontScale(float scale) {
  SetWindowFontScale(scale);
}
inline void SetWindowPos(const std::string &name, float posX, float posY) {
  SetWindowPos(name.c_str(), {posX, posY});
}
inline void SetWindowPos(const std::string &name, float posX, float posY,
                         int cond) {
  SetWindowPos(name.c_str(), {posX, posY}, static_cast<ImGuiCond>(cond));
}
inline void SetWindowSize(const std::string &name, float sizeX, float sizeY) {
  SetWindowSize(name.c_str(), {sizeX, sizeY});
}
inline void SetWindowSize(const std::string &name, float sizeX, float sizeY,
                          int cond) {
  SetWindowSize(name.c_str(), {sizeX, sizeY},
                       static_cast<ImGuiCond>(cond));
}
inline void SetWindowCollapsed(const std::string &name, bool collapsed) {
  SetWindowCollapsed(name.c_str(), collapsed);
}
inline void SetWindowCollapsed(const std::string &name, bool collapsed,
                               int cond) {
  SetWindowCollapsed(name.c_str(), collapsed,
                            static_cast<ImGuiCond>(cond));
}
inline void SetWindowFocus(const std::string &name) {
  SetWindowFocus(name.c_str());
}

// Content Region
inline std::tuple<float, float> GetContentRegionMax() {
  const auto vec2{GetContentRegionMax()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetContentRegionAvail() {
  const auto vec2{GetContentRegionAvail()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetWindowContentRegionMin() {
  const auto vec2{GetWindowContentRegionMin()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetWindowContentRegionMax() {
  const auto vec2{GetWindowContentRegionMax()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline float GetWindowContentRegionWidth() {
  return GetWindowContentRegionWidth();
}

// Windows Scrolling
inline float GetScrollX() { return GetScrollX(); }
inline float GetScrollY() { return GetScrollY(); }
inline float GetScrollMaxX() { return GetScrollMaxX(); }
inline float GetScrollMaxY() { return GetScrollMaxY(); }
inline void SetScrollX(float scrollX) { SetScrollX(scrollX); }
inline void SetScrollY(float scrollY) { SetScrollY(scrollY); }
inline void SetScrollHereX() { SetScrollHereX(); }
inline void SetScrollHereX(float centerXRatio) {
  SetScrollHereX(centerXRatio);
}
inline void SetScrollHereY() { SetScrollHereY(); }
inline void SetScrollHereY(float centerYRatio) {
  SetScrollHereY(centerYRatio);
}
inline void SetScrollFromPosX(float localX) {
  SetScrollFromPosX(localX);
}
inline void SetScrollFromPosX(float localX, float centerXRatio) {
  SetScrollFromPosX(localX, centerXRatio);
}
inline void SetScrollFromPosY(float localY) {
  SetScrollFromPosY(localY);
}
inline void SetScrollFromPosY(float localY, float centerYRatio) {
  SetScrollFromPosY(localY, centerYRatio);
}

// Parameters stacks (shared)
inline void PushFont(ImFont *pFont) { PushFont(pFont); }
inline void PopFont() { PopFont(); }
#ifdef SOL_IMGUI_USE_COLOR_U32
inline void PushStyleColor(int idx, int col) {
  PushStyleColor(static_cast<ImGuiCol>(idx), ImU32(col));
}
#endif
inline void PushStyleColor(int idx, float colR, float colG, float colB,
                           float colA) {
  PushStyleColor(static_cast<ImGuiCol>(idx), {colR, colG, colB, colA});
}
inline void PopStyleColor() { PopStyleColor(); }
inline void PopStyleColor(int count) { PopStyleColor(count); }
inline void PushStyleVar(int idx, float val) {
  PushStyleVar(static_cast<ImGuiStyleVar>(idx), val);
}
inline void PushStyleVar(int idx, float valX, float valY) {
  PushStyleVar(static_cast<ImGuiStyleVar>(idx), {valX, valY});
}
inline void PopStyleVar() { PopStyleVar(); }
inline void PopStyleVar(int count) { PopStyleVar(count); }
inline std::tuple<float, float, float, float> GetStyleColorVec4(int idx) {
  const auto &col{GetStyleColorVec4(static_cast<ImGuiCol>(idx))};
  return std::make_tuple(col.x, col.y, col.z, col.w);
}
inline ImFont *GetFont() { return GetFont(); }
inline float GetFontSize() { return GetFontSize(); }
inline std::tuple<float, float> GetFontTexUvWhitePixel() {
  const auto vec2{GetFontTexUvWhitePixel()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
#ifdef SOL_IMGUI_USE_COLOR_U32
inline int GetColorU32(int idx, float alphaMul) {
  return GetColorU32(static_cast<ImGuiCol>(idx), alphaMul);
}
inline int GetColorU32(float colR, float colG, float colB, float colA) {
  return GetColorU32({colR, colG, colB, colA});
}
inline int GetColorU32(int col) { return GetColorU32(ImU32(col)); }
#endif

// Parameters stacks (current window)
inline void PushItemWidth(float itemWidth) { PushItemWidth(itemWidth); }
inline void PopItemWidth() { PopItemWidth(); }
inline void SetNextItemWidth(float itemWidth) {
  SetNextItemWidth(itemWidth);
}
inline float CalcItemWidth() { return CalcItemWidth(); }
inline void PushTextWrapPos() { PushTextWrapPos(); }
inline void PushTextWrapPos(float wrapLocalPosX) {
  PushTextWrapPos(wrapLocalPosX);
}
inline void PopTextWrapPos() { PopTextWrapPos(); }
inline void PushAllowKeyboardFocus(bool allowKeyboardFocus) {
  PushAllowKeyboardFocus(allowKeyboardFocus);
}
inline void PopAllowKeyboardFocus() { PopAllowKeyboardFocus(); }
inline void PushButtonRepeat(bool repeat) { PushButtonRepeat(repeat); }
inline void PopButtonRepeat() { PopButtonRepeat(); }

// Cursor / Layout
inline void Separator() { Separator(); }
inline void SameLine() { SameLine(); }
inline void SameLine(float offsetFromStartX) {
  SameLine(offsetFromStartX);
}
inline void SameLine(float offsetFromStartX, float spacing) {
  SameLine(offsetFromStartX, spacing);
}
inline void NewLine() { NewLine(); }
inline void Spacing() { Spacing(); }
inline void Dummy(float sizeX, float sizeY) { Dummy({sizeX, sizeY}); }
inline void Indent() { Indent(); }
inline void Indent(float indentW) { Indent(indentW); }
inline void Unindent() { Unindent(); }
inline void Unindent(float indentW) { Unindent(indentW); }
inline void BeginGroup() { BeginGroup(); }
inline void EndGroup() { EndGroup(); }
inline std::tuple<float, float> GetCursorPos() {
  const auto vec2{GetCursorPos()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline float GetCursorPosX() { return GetCursorPosX(); }
inline float GetCursorPosY() { return GetCursorPosY(); }
inline void SetCursorPos(float localX, float localY) {
  SetCursorPos({localX, localY});
}
inline void SetCursorPosX(float localX) { SetCursorPosX(localX); }
inline void SetCursorPosY(float localY) { SetCursorPosY(localY); }
inline std::tuple<float, float> GetCursorStartPos() {
  const auto vec2{GetCursorStartPos()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetCursorScreenPos() {
  const auto vec2{GetCursorScreenPos()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline void SetCursorScreenPos(float posX, float posY) {
  SetCursorScreenPos({posX, posY});
}
inline void AlignTextToFramePadding() { AlignTextToFramePadding(); }
inline float GetTextLineHeight() { return GetTextLineHeight(); }
inline float GetTextLineHeightWithSpacing() {
  return GetTextLineHeightWithSpacing();
}
inline float GetFrameHeight() { return GetFrameHeight(); }
inline float GetFrameHeightWithSpacing() {
  return GetFrameHeightWithSpacing();
}

// ID stack / scopes
inline void PushID(const std::string &stringID) {
  PushID(stringID.c_str());
}
inline void PushID(const std::string &stringIDBegin,
                   const std::string &stringIDEnd) {
  PushID(stringIDBegin.c_str(), stringIDEnd.c_str());
}
inline void PushID(const void *) { /* TODO: PushID(void*) ==> UNSUPPORTED */
}
inline void PushID(int intID) { PushID(intID); }
inline void PopID() { PopID(); }
inline int GetID(const std::string &stringID) {
  return GetID(stringID.c_str());
}
inline int GetID(const std::string &stringIDBegin,
                 const std::string &stringIDEnd) {
  return GetID(stringIDBegin.c_str(), stringIDEnd.c_str());
}
inline int GetID(const void *) {
  return 0; /* TODO: GetID(void*) ==> UNSUPPORTED */
}

// Widgets: Text
inline void TextUnformatted(const std::string &text) {
  TextUnformatted(text.c_str());
}
inline void TextUnformatted(const std::string &text,
                            const std::string &textEnd) {
  TextUnformatted(text.c_str(), textEnd.c_str());
}
inline void Text(const std::string &text) { Text(text.c_str()); }
inline void TextColored(float colR, float colG, float colB, float colA,
                        const std::string &text) {
  TextColored({colR, colG, colB, colA}, text.c_str());
}
inline void TextDisabled(const std::string &text) {
  TextDisabled(text.c_str());
}
inline void TextWrapped(const std::string text) {
  TextWrapped(text.c_str());
}
inline void LabelText(const std::string &label, const std::string &text) {
  LabelText(label.c_str(), text.c_str());
}
inline void BulletText(const std::string &text) {
  BulletText(text.c_str());
}

// Widgets: Main
inline bool Button(const std::string &label) {
  return Button(label.c_str());
}
inline bool Button(const std::string &label, float sizeX, float sizeY) {
  return Button(label.c_str(), {sizeX, sizeY});
}
inline bool SmallButton(const std::string &label) {
  return SmallButton(label.c_str());
}
inline bool InvisibleButton(const std::string &stringID, float sizeX,
                            float sizeY) {
  return InvisibleButton(stringID.c_str(), {sizeX, sizeY});
}
inline bool ArrowButton(const std::string &stringID, int dir) {
  return ArrowButton(stringID.c_str(), static_cast<ImGuiDir>(dir));
}
inline void Image() { /* TODO: Image(...) ==> UNSUPPORTED */
}
inline void ImageButton() { /* TODO: ImageButton(...) ==> UNSUPPORTED */
}
inline std::tuple<bool, bool> Checkbox(const std::string &label, bool v) {
  bool value{v};
  bool pressed = Checkbox(label.c_str(), &value);

  return std::make_tuple(value, pressed);
}
inline bool CheckboxFlags() {
  return false; /* TODO: CheckboxFlags(...) ==> UNSUPPORTED */
}
inline bool RadioButton(const std::string &label, bool active) {
  return RadioButton(label.c_str(), active);
}
inline std::tuple<int, bool> RadioButton(const std::string &label, int v,
                                         int vButton) {
  bool ret{RadioButton(label.c_str(), &v, vButton)};
  return std::make_tuple(v, ret);
}
inline void ProgressBar(float fraction) { ProgressBar(fraction); }
inline void ProgressBar(float fraction, float sizeX, float sizeY) {
  ProgressBar(fraction, {sizeX, sizeY});
}
inline void ProgressBar(float fraction, float sizeX, float sizeY,
                        const std::string &overlay) {
  ProgressBar(fraction, {sizeX, sizeY}, overlay.c_str());
}
inline void Bullet() { Bullet(); }

// Widgets: Combo Box
inline bool BeginCombo(const std::string &label,
                       const std::string &previewValue) {
  return BeginCombo(label.c_str(), previewValue.c_str());
}
inline bool BeginCombo(const std::string &label,
                       const std::string &previewValue, int flags) {
  return BeginCombo(label.c_str(), previewValue.c_str(),
                           static_cast<ImGuiComboFlags>(flags));
}
inline void EndCombo() { EndCombo(); }
inline std::tuple<int, bool> Combo(const std::string &label, int currentItem,
                                   const sol::table &items, int itemsCount) {
  std::vector<std::string> strings;
  for (int i{1}; i <= itemsCount; i++) {
    const auto &stringItem = items.get<sol::optional<std::string>>(i);
    strings.push_back(stringItem.value_or("Missing"));
  }

  std::vector<const char *> cstrings;
  for (auto &string : strings)
    cstrings.push_back(string.c_str());

  bool clicked =
      Combo(label.c_str(), &currentItem, cstrings.data(), itemsCount);
  return std::make_tuple(currentItem, clicked);
}
inline std::tuple<int, bool> Combo(const std::string &label, int currentItem,
                                   const sol::table &items, int itemsCount,
                                   int popupMaxHeightInItems) {
  std::vector<std::string> strings;
  for (int i{1}; i <= itemsCount; i++) {
    const auto &stringItem = items.get<sol::optional<std::string>>(i);
    strings.push_back(stringItem.value_or("Missing"));
  }

  std::vector<const char *> cstrings;
  for (auto &string : strings)
    cstrings.push_back(string.c_str());

  bool clicked = Combo(label.c_str(), &currentItem, cstrings.data(),
                              itemsCount, popupMaxHeightInItems);
  return std::make_tuple(currentItem, clicked);
}
inline std::tuple<int, bool> Combo(const std::string &label, int currentItem,
                                   const std::string &itemsSeparatedByZeros) {
  bool clicked =
      Combo(label.c_str(), &currentItem, itemsSeparatedByZeros.c_str());
  return std::make_tuple(currentItem, clicked);
}
inline std::tuple<int, bool> Combo(const std::string &label, int currentItem,
                                   const std::string &itemsSeparatedByZeros,
                                   int popupMaxHeightInItems) {
  bool clicked =
      Combo(label.c_str(), &currentItem, itemsSeparatedByZeros.c_str(),
                   popupMaxHeightInItems);
  return std::make_tuple(currentItem, clicked);
}
// TODO: 3rd Combo from ImGui not Supported

// Widgets: Drags
inline std::tuple<float, bool> DragFloat(const std::string &label, float v) {
  bool used = DragFloat(label.c_str(), &v);
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> DragFloat(const std::string &label, float v,
                                         float v_speed) {
  bool used = DragFloat(label.c_str(), &v, v_speed);
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> DragFloat(const std::string &label, float v,
                                         float v_speed, float v_min) {
  bool used = DragFloat(label.c_str(), &v, v_speed, v_min);
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> DragFloat(const std::string &label, float v,
                                         float v_speed, float v_min,
                                         float v_max) {
  bool used = DragFloat(label.c_str(), &v, v_speed, v_min, v_max);
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> DragFloat(const std::string &label, float v,
                                         float v_speed, float v_min,
                                         float v_max,
                                         const std::string &format) {
  bool used = DragFloat(label.c_str(), &v, v_speed, v_min, v_max,
                               format.c_str());
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> DragFloat(const std::string &label, float v,
                                         float v_speed, float v_min,
                                         float v_max, const std::string &format,
                                         float power) {
  bool used = DragFloat(label.c_str(), &v, v_speed, v_min, v_max,
                               format.c_str(), power);
  return std::make_tuple(v, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat2(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = DragFloat2(label.c_str(), value);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat2(const std::string &label, const sol::table &v, float v_speed) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = DragFloat2(label.c_str(), value, v_speed);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat2(const std::string &label, const sol::table &v, float v_speed,
           float v_min) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = DragFloat2(label.c_str(), value, v_speed, v_min);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat2(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = DragFloat2(label.c_str(), value, v_speed, v_min, v_max);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat2(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = DragFloat2(label.c_str(), value, v_speed, v_min, v_max,
                                format.c_str());

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat2(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max, const std::string &format, float power) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = DragFloat2(label.c_str(), value, v_speed, v_min, v_max,
                                format.c_str(), power);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat3(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = DragFloat3(label.c_str(), value);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat3(const std::string &label, const sol::table &v, float v_speed) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = DragFloat3(label.c_str(), value, v_speed);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat3(const std::string &label, const sol::table &v, float v_speed,
           float v_min) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = DragFloat3(label.c_str(), value, v_speed, v_min);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat3(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = DragFloat3(label.c_str(), value, v_speed, v_min, v_max);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat3(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = DragFloat3(label.c_str(), value, v_speed, v_min, v_max,
                                format.c_str());

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat3(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max, const std::string &format, float power) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = DragFloat3(label.c_str(), value, v_speed, v_min, v_max,
                                format.c_str(), power);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat4(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = DragFloat4(label.c_str(), value);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat4(const std::string &label, const sol::table &v, float v_speed) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = DragFloat4(label.c_str(), value, v_speed);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat4(const std::string &label, const sol::table &v, float v_speed,
           float v_min) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = DragFloat4(label.c_str(), value, v_speed, v_min);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat4(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = DragFloat4(label.c_str(), value, v_speed, v_min, v_max);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat4(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = DragFloat4(label.c_str(), value, v_speed, v_min, v_max,
                                format.c_str());

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
DragFloat4(const std::string &label, const sol::table &v, float v_speed,
           float v_min, float v_max, const std::string &format, float power) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = DragFloat4(label.c_str(), value, v_speed, v_min, v_max,
                                format.c_str(), power);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline void DragFloatRange2() { /* TODO: DragFloatRange2(...) ==> UNSUPPORTED */
}
inline std::tuple<int, bool> DragInt(const std::string &label, int v) {
  bool used = DragInt(label.c_str(), &v);
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> DragInt(const std::string &label, int v,
                                     float v_speed) {
  bool used = DragInt(label.c_str(), &v, v_speed);
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> DragInt(const std::string &label, int v,
                                     float v_speed, int v_min) {
  bool used = DragInt(label.c_str(), &v, v_speed, v_min);
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> DragInt(const std::string &label, int v,
                                     float v_speed, int v_min, int v_max) {
  bool used = DragInt(label.c_str(), &v, v_speed, v_min, v_max);
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> DragInt(const std::string &label, int v,
                                     float v_speed, int v_min, int v_max,
                                     const std::string &format) {
  bool used =
      DragInt(label.c_str(), &v, v_speed, v_min, v_max, format.c_str());
  return std::make_tuple(v, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt2(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = DragInt2(label.c_str(), value);

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt2(const std::string &label, const sol::table &v, float v_speed) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = DragInt2(label.c_str(), value, v_speed);

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt2(const std::string &label, const sol::table &v, float v_speed,
         int v_min) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = DragInt2(label.c_str(), value, v_speed, v_min);

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt2(const std::string &label, const sol::table &v, float v_speed,
         int v_min, int v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = DragInt2(label.c_str(), value, v_speed, v_min, v_max);

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt2(const std::string &label, const sol::table &v, float v_speed,
         int v_min, int v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = DragInt2(label.c_str(), value, v_speed, v_min, v_max,
                              format.c_str());

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt3(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = DragInt3(label.c_str(), value);

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt3(const std::string &label, const sol::table &v, float v_speed) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = DragInt3(label.c_str(), value, v_speed);

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt3(const std::string &label, const sol::table &v, float v_speed,
         int v_min) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = DragInt3(label.c_str(), value, v_speed, v_min);

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt3(const std::string &label, const sol::table &v, float v_speed,
         int v_min, int v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = DragInt3(label.c_str(), value, v_speed, v_min, v_max);

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt3(const std::string &label, const sol::table &v, float v_speed,
         int v_min, int v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = DragInt3(label.c_str(), value, v_speed, v_min, v_max,
                              format.c_str());

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt4(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = DragInt4(label.c_str(), value);

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt4(const std::string &label, const sol::table &v, float v_speed) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = DragInt4(label.c_str(), value, v_speed);

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt4(const std::string &label, const sol::table &v, float v_speed,
         int v_min) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = DragInt4(label.c_str(), value, v_speed, v_min);

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt4(const std::string &label, const sol::table &v, float v_speed,
         int v_min, int v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = DragInt4(label.c_str(), value, v_speed, v_min, v_max);

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
DragInt4(const std::string &label, const sol::table &v, float v_speed,
         int v_min, int v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = DragInt4(label.c_str(), value, v_speed, v_min, v_max,
                              format.c_str());

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline void DragIntRange2() { /* TODO: DragIntRange2(...) ==> UNSUPPORTED */
}
inline void DragScalar() { /* TODO: DragScalar(...) ==> UNSUPPORTED */
}
inline void DragScalarN() { /* TODO: DragScalarN(...) ==> UNSUPPORTED */
}

// Widgets: Sliders
inline std::tuple<float, bool> SliderFloat(const std::string &label, float v,
                                           float v_min, float v_max) {
  bool used = SliderFloat(label.c_str(), &v, v_min, v_max);
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> SliderFloat(const std::string &label, float v,
                                           float v_min, float v_max,
                                           const std::string &format) {
  bool used =
      SliderFloat(label.c_str(), &v, v_min, v_max, format.c_str());
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> SliderFloat(const std::string &label, float v,
                                           float v_min, float v_max,
                                           const std::string &format,
                                           float power) {
  bool used = SliderFloat(label.c_str(), &v, v_min, v_max,
                                 format.c_str(), power);
  return std::make_tuple(v, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat2(const std::string &label, const sol::table &v, float v_min,
             float v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = SliderFloat2(label.c_str(), value, v_min, v_max);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat2(const std::string &label, const sol::table &v, float v_min,
             float v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used =
      SliderFloat2(label.c_str(), value, v_min, v_max, format.c_str());

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat2(const std::string &label, const sol::table &v, float v_min,
             float v_max, const std::string &format, float power) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = SliderFloat2(label.c_str(), value, v_min, v_max,
                                  format.c_str(), power);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat3(const std::string &label, const sol::table &v, float v_min,
             float v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = SliderFloat3(label.c_str(), value, v_min, v_max);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[3]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat3(const std::string &label, const sol::table &v, float v_min,
             float v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used =
      SliderFloat3(label.c_str(), value, v_min, v_max, format.c_str());

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[3]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat3(const std::string &label, const sol::table &v, float v_min,
             float v_max, const std::string &format, float power) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = SliderFloat3(label.c_str(), value, v_min, v_max,
                                  format.c_str(), power);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[3]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat4(const std::string &label, const sol::table &v, float v_min,
             float v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = SliderFloat4(label.c_str(), value, v_min, v_max);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat4(const std::string &label, const sol::table &v, float v_min,
             float v_max, const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used =
      SliderFloat4(label.c_str(), value, v_min, v_max, format.c_str());

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
SliderFloat4(const std::string &label, const sol::table &v, float v_min,
             float v_max, const std::string &format, float power) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = SliderFloat4(label.c_str(), value, v_min, v_max,
                                  format.c_str(), power);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<float, bool> SliderAngle(const std::string &label,
                                           float v_rad) {
  bool used = SliderAngle(label.c_str(), &v_rad);
  return std::make_tuple(v_rad, used);
}
inline std::tuple<float, bool> SliderAngle(const std::string &label,
                                           float v_rad, float v_degrees_min) {
  bool used = SliderAngle(label.c_str(), &v_rad, v_degrees_min);
  return std::make_tuple(v_rad, used);
}
inline std::tuple<float, bool> SliderAngle(const std::string &label,
                                           float v_rad, float v_degrees_min,
                                           float v_degrees_max) {
  bool used =
      SliderAngle(label.c_str(), &v_rad, v_degrees_min, v_degrees_max);
  return std::make_tuple(v_rad, used);
}
inline std::tuple<float, bool> SliderAngle(const std::string &label,
                                           float v_rad, float v_degrees_min,
                                           float v_degrees_max,
                                           const std::string &format) {
  bool used = SliderAngle(label.c_str(), &v_rad, v_degrees_min,
                                 v_degrees_max, format.c_str());
  return std::make_tuple(v_rad, used);
}
inline std::tuple<int, bool> SliderInt(const std::string &label, int v,
                                       int v_min, int v_max) {
  bool used = SliderInt(label.c_str(), &v, v_min, v_max);
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> SliderInt(const std::string &label, int v,
                                       int v_min, int v_max,
                                       const std::string &format) {
  bool used = SliderInt(label.c_str(), &v, v_min, v_max, format.c_str());
  return std::make_tuple(v, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
SliderInt2(const std::string &label, const sol::table &v, int v_min,
           int v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = SliderInt2(label.c_str(), value, v_min, v_max);

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
SliderInt2(const std::string &label, const sol::table &v, int v_min, int v_max,
           const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used =
      SliderInt2(label.c_str(), value, v_min, v_max, format.c_str());

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
SliderInt3(const std::string &label, const sol::table &v, int v_min,
           int v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = SliderInt3(label.c_str(), value, v_min, v_max);

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
SliderInt3(const std::string &label, const sol::table &v, int v_min, int v_max,
           const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used =
      SliderInt3(label.c_str(), value, v_min, v_max, format.c_str());

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
SliderInt4(const std::string &label, const sol::table &v, int v_min,
           int v_max) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = SliderInt4(label.c_str(), value, v_min, v_max);

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
SliderInt4(const std::string &label, const sol::table &v, int v_min, int v_max,
           const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used =
      SliderInt4(label.c_str(), value, v_min, v_max, format.c_str());

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline void SliderScalar() { /* TODO: SliderScalar(...) ==> UNSUPPORTED */
}
inline void SliderScalarN() { /* TODO: SliderScalarN(...) ==> UNSUPPORTED */
}
inline std::tuple<float, bool> VSliderFloat(const std::string &label,
                                            float sizeX, float sizeY, float v,
                                            float v_min, float v_max) {
  bool used =
      VSliderFloat(label.c_str(), {sizeX, sizeY}, &v, v_min, v_max);
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool> VSliderFloat(const std::string &label,
                                            float sizeX, float sizeY, float v,
                                            float v_min, float v_max,
                                            const std::string &format) {
  bool used = VSliderFloat(label.c_str(), {sizeX, sizeY}, &v, v_min,
                                  v_max, format.c_str());
  return std::make_tuple(v, used);
}
inline std::tuple<float, bool>
VSliderFloat(const std::string &label, float sizeX, float sizeY, float v,
             float v_min, float v_max, const std::string &format, int flags) {
  bool used =
      VSliderFloat(label.c_str(), {sizeX, sizeY}, &v, v_min, v_max,
                          format.c_str(), static_cast<ImGuiSliderFlags>(flags));
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> VSliderInt(const std::string &label, float sizeX,
                                        float sizeY, int v, int v_min,
                                        int v_max) {
  bool used =
      VSliderInt(label.c_str(), {sizeX, sizeY}, &v, v_min, v_max);
  return std::make_tuple(v, used);
}
inline std::tuple<int, bool> VSliderInt(const std::string &label, float sizeX,
                                        float sizeY, int v, int v_min,
                                        int v_max, const std::string &format) {
  bool used = VSliderInt(label.c_str(), {sizeX, sizeY}, &v, v_min, v_max,
                                format.c_str());
  return std::make_tuple(v, used);
}
inline void VSliderScalar() { /* TODO: VSliderScalar(...) ==> UNSUPPORTED */
}

// Widgets: Input with Keyboard
inline std::tuple<std::string, bool>
InputText(const std::string &label, std::string text, unsigned int buf_size) {
  bool selected = InputText(label.c_str(), &text[0], buf_size);
  return std::make_tuple(text, selected);
}
inline std::tuple<std::string, bool> InputText(const std::string &label,
                                               std::string text,
                                               unsigned int buf_size,
                                               int flags) {
  bool selected = InputText(label.c_str(), &text[0], buf_size,
                                   static_cast<ImGuiInputTextFlags>(flags));
  return std::make_tuple(text, selected);
}
inline std::tuple<std::string, bool>
InputTextMultiline(const std::string &label, std::string text,
                   unsigned int buf_size) {
  bool selected = InputTextMultiline(label.c_str(), &text[0], buf_size);
  return std::make_tuple(text, selected);
}
inline std::tuple<std::string, bool>
InputTextMultiline(const std::string &label, std::string text,
                   unsigned int buf_size, float sizeX, float sizeY) {
  bool selected = InputTextMultiline(label.c_str(), &text[0], buf_size,
                                            {sizeX, sizeY});
  return std::make_tuple(text, selected);
}
inline std::tuple<std::string, bool>
InputTextMultiline(const std::string &label, std::string text,
                   unsigned int buf_size, float sizeX, float sizeY, int flags) {
  bool selected = InputTextMultiline(
      label.c_str(), &text[0], buf_size, {sizeX, sizeY},
      static_cast<ImGuiInputTextFlags>(flags));
  return std::make_tuple(text, selected);
}
inline std::tuple<std::string, bool> InputTextWithHint(const std::string &label,
                                                       const std::string &hint,
                                                       std::string text,
                                                       unsigned int buf_size) {
  bool selected =
      InputTextWithHint(label.c_str(), hint.c_str(), &text[0], buf_size);
  return std::make_tuple(text, selected);
}
inline std::tuple<std::string, bool>
InputTextWithHint(const std::string &label, const std::string &hint,
                  std::string text, unsigned int buf_size, int flags) {
  bool selected =
      InputTextWithHint(label.c_str(), hint.c_str(), &text[0], buf_size,
                               static_cast<ImGuiInputTextFlags>(flags));
  return std::make_tuple(text, selected);
}
inline std::tuple<float, bool> InputFloat(const std::string &label, float v) {
  bool selected = InputFloat(label.c_str(), &v);
  return std::make_tuple(v, selected);
}
inline std::tuple<float, bool> InputFloat(const std::string &label, float v,
                                          float step) {
  bool selected = InputFloat(label.c_str(), &v, step);
  return std::make_tuple(v, selected);
}
inline std::tuple<float, bool> InputFloat(const std::string &label, float v,
                                          float step, float step_fast) {
  bool selected = InputFloat(label.c_str(), &v, step, step_fast);
  return std::make_tuple(v, selected);
}
inline std::tuple<float, bool> InputFloat(const std::string &label, float v,
                                          float step, float step_fast,
                                          const std::string &format) {
  bool selected =
      InputFloat(label.c_str(), &v, step, step_fast, format.c_str());
  return std::make_tuple(v, selected);
}
inline std::tuple<float, bool> InputFloat(const std::string &label, float v,
                                          float step, float step_fast,
                                          const std::string &format,
                                          int flags) {
  bool selected =
      InputFloat(label.c_str(), &v, step, step_fast, format.c_str(),
                        static_cast<ImGuiInputTextFlags>(flags));
  return std::make_tuple(v, selected);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat2(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = InputFloat2(label.c_str(), value);

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat2(const std::string &label, const sol::table &v,
            const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = InputFloat2(label.c_str(), value, format.c_str());

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat2(const std::string &label, const sol::table &v,
            const std::string &format, int flags) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[2] = {float(v1), float(v2)};
  bool used = InputFloat2(label.c_str(), value, format.c_str(),
                                 static_cast<ImGuiInputTextFlags>(flags));

  sol::as_table_t float2 =
      sol::as_table(std::vector<float>{value[0], value[1]});

  return std::make_tuple(float2, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat3(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = InputFloat3(label.c_str(), value);

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat3(const std::string &label, const sol::table &v,
            const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = InputFloat3(label.c_str(), value, format.c_str());

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat3(const std::string &label, const sol::table &v,
            const std::string &format, int flags) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[3] = {float(v1), float(v2), float(v3)};
  bool used = InputFloat3(label.c_str(), value, format.c_str(),
                                 static_cast<ImGuiInputTextFlags>(flags));

  sol::as_table_t float3 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2]});

  return std::make_tuple(float3, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat4(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = InputFloat4(label.c_str(), value);

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat4(const std::string &label, const sol::table &v,
            const std::string &format) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = InputFloat4(label.c_str(), value, format.c_str());

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
InputFloat4(const std::string &label, const sol::table &v,
            const std::string &format, int flags) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float value[4] = {float(v1), float(v2), float(v3), float(v4)};
  bool used = InputFloat4(label.c_str(), value, format.c_str(),
                                 static_cast<ImGuiInputTextFlags>(flags));

  sol::as_table_t float4 =
      sol::as_table(std::vector<float>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(float4, used);
}
inline std::tuple<int, bool> InputInt(const std::string &label, int v) {
  bool selected = InputInt(label.c_str(), &v);
  return std::make_tuple(v, selected);
}
inline std::tuple<int, bool> InputInt(const std::string &label, int v,
                                      int step) {
  bool selected = InputInt(label.c_str(), &v, step);
  return std::make_tuple(v, selected);
}
inline std::tuple<int, bool> InputInt(const std::string &label, int v, int step,
                                      int step_fast) {
  bool selected = InputInt(label.c_str(), &v, step, step_fast);
  return std::make_tuple(v, selected);
}
inline std::tuple<int, bool> InputInt(const std::string &label, int v, int step,
                                      int step_fast, int flags) {
  bool selected = InputInt(label.c_str(), &v, step, step_fast,
                                  static_cast<ImGuiInputTextFlags>(flags));
  return std::make_tuple(v, selected);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
InputInt2(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = InputInt2(label.c_str(), value);

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
InputInt2(const std::string &label, const sol::table &v, int flags) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[2] = {int(v1), int(v2)};
  bool used = InputInt2(label.c_str(), value,
                               static_cast<ImGuiInputTextFlags>(flags));

  sol::as_table_t int2 = sol::as_table(std::vector<int>{value[0], value[1]});

  return std::make_tuple(int2, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
InputInt3(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = InputInt3(label.c_str(), value);

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
InputInt3(const std::string &label, const sol::table &v, int flags) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[3] = {int(v1), int(v2), int(v3)};
  bool used = InputInt3(label.c_str(), value,
                               static_cast<ImGuiInputTextFlags>(flags));

  sol::as_table_t int3 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2]});

  return std::make_tuple(int3, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
InputInt4(const std::string &label, const sol::table &v) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = InputInt4(label.c_str(), value);

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<sol::as_table_t<std::vector<int>>, bool>
InputInt4(const std::string &label, const sol::table &v, int flags) {
  const lua_Number v1{v[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      v2{v[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v3{v[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      v4{v[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  int value[4] = {int(v1), int(v2), int(v3), int(v4)};
  bool used = InputInt4(label.c_str(), value,
                               static_cast<ImGuiInputTextFlags>(flags));

  sol::as_table_t int4 =
      sol::as_table(std::vector<int>{value[0], value[1], value[2], value[3]});

  return std::make_tuple(int4, used);
}
inline std::tuple<double, bool> InputDouble(const std::string &label,
                                            double v) {
  bool selected = InputDouble(label.c_str(), &v);
  return std::make_tuple(v, selected);
}
inline std::tuple<double, bool> InputDouble(const std::string &label, double v,
                                            double step) {
  bool selected = InputDouble(label.c_str(), &v, step);
  return std::make_tuple(v, selected);
}
inline std::tuple<double, bool> InputDouble(const std::string &label, double v,
                                            double step, double step_fast) {
  bool selected = InputDouble(label.c_str(), &v, step, step_fast);
  return std::make_tuple(v, selected);
}
inline std::tuple<double, bool> InputDouble(const std::string &label, double v,
                                            double step, double step_fast,
                                            const std::string &format) {
  bool selected =
      InputDouble(label.c_str(), &v, step, step_fast, format.c_str());
  return std::make_tuple(v, selected);
}
inline std::tuple<double, bool> InputDouble(const std::string &label, double v,
                                            double step, double step_fast,
                                            const std::string &format,
                                            int flags) {
  bool selected =
      InputDouble(label.c_str(), &v, step, step_fast, format.c_str(),
                         static_cast<ImGuiInputTextFlags>(flags));
  return std::make_tuple(v, selected);
}
inline void InputScalar() { /* TODO: InputScalar(...) ==> UNSUPPORTED */
}
inline void InputScalarN() { /* TODO: InputScalarN(...) ==> UNSUPPORTED */
}

// Widgets: Color Editor / Picker
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorEdit3(const std::string &label, const sol::table &col) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[3] = {float(r), float(g), float(b)};
  bool used = ColorEdit3(label.c_str(), color);

  sol::as_table_t rgb =
      sol::as_table(std::vector<float>{color[0], color[1], color[2]});

  return std::make_tuple(rgb, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorEdit3(const std::string &label, const sol::table &col, int flags) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[3] = {float(r), float(g), float(b)};
  bool used = ColorEdit3(label.c_str(), color,
                                static_cast<ImGuiColorEditFlags>(flags));

  sol::as_table_t rgb =
      sol::as_table(std::vector<float>{color[0], color[1], color[2]});

  return std::make_tuple(rgb, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorEdit4(const std::string &label, const sol::table &col) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[4] = {float(r), float(g), float(b), float(a)};
  bool used = ColorEdit4(label.c_str(), color);

  sol::as_table_t rgba =
      sol::as_table(std::vector<float>{color[0], color[1], color[2], color[3]});

  return std::make_tuple(rgba, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorEdit4(const std::string &label, const sol::table &col, int flags) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[4] = {float(r), float(g), float(b), float(a)};
  bool used = ColorEdit4(label.c_str(), color,
                                static_cast<ImGuiColorEditFlags>(flags));

  sol::as_table_t rgba =
      sol::as_table(std::vector<float>{color[0], color[1], color[2], color[3]});

  return std::make_tuple(rgba, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorPicker3(const std::string &label, const sol::table &col) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[3] = {float(r), float(g), float(b)};
  bool used = ColorPicker3(label.c_str(), color);

  sol::as_table_t rgb =
      sol::as_table(std::vector<float>{color[0], color[1], color[2]});

  return std::make_tuple(rgb, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorPicker3(const std::string &label, const sol::table &col, int flags) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[3] = {float(r), float(g), float(b)};
  bool used = ColorPicker3(label.c_str(), color,
                                  static_cast<ImGuiColorEditFlags>(flags));

  sol::as_table_t rgb =
      sol::as_table(std::vector<float>{color[0], color[1], color[2]});

  return std::make_tuple(rgb, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorPicker4(const std::string &label, const sol::table &col) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[4] = {float(r), float(g), float(b), float(a)};
  bool used = ColorPicker4(label.c_str(), color);

  sol::as_table_t rgba =
      sol::as_table(std::vector<float>{color[0], color[1], color[2], color[3]});

  return std::make_tuple(rgba, used);
}
inline std::tuple<sol::as_table_t<std::vector<float>>, bool>
ColorPicker4(const std::string &label, const sol::table &col, int flags) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  float color[4] = {float(r), float(g), float(b), float(a)};
  bool used = ColorPicker4(label.c_str(), color,
                                  static_cast<ImGuiColorEditFlags>(flags));

  sol::as_table_t rgba =
      sol::as_table(std::vector<float>{color[0], color[1], color[2], color[3]});

  return std::make_tuple(rgba, used);
}
inline bool ColorButton(const std::string &desc_id, const sol::table &col) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  const ImVec4 color{float(r), float(g), float(b), float(a)};
  return ColorButton(desc_id.c_str(), color);
}
inline bool ColorButton(const std::string &desc_id, const sol::table &col,
                        int flags) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  const ImVec4 color{float(r), float(g), float(b), float(a)};
  return ColorButton(desc_id.c_str(), color,
                            static_cast<ImGuiColorEditFlags>(flags));
}
inline bool ColorButton(const std::string &desc_id, const sol::table &col,
                        int flags, float sizeX, float sizeY) {
  const lua_Number r{col[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{col[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{col[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{col[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};
  const ImVec4 color{float(r), float(g), float(b), float(a)};
  return ColorButton(desc_id.c_str(), color,
                            static_cast<ImGuiColorEditFlags>(flags),
                            {sizeX, sizeY});
}
inline void SetColorEditOptions(int flags) {
  SetColorEditOptions(static_cast<ImGuiColorEditFlags>(flags));
}

// Widgets: Trees
inline bool TreeNode(const std::string &label) {
  return TreeNode(label.c_str());
}
inline bool TreeNode(const std::string &label, const std::string &fmt) {
  return TreeNode(label.c_str(), fmt.c_str());
}
/* TODO: TreeNodeV(...) (2) ==> UNSUPPORTED */
inline bool TreeNodeEx(const std::string &label) {
  return TreeNodeEx(label.c_str());
}
inline bool TreeNodeEx(const std::string &label, int flags) {
  return TreeNodeEx(label.c_str(),
                           static_cast<ImGuiTreeNodeFlags>(flags));
}
inline bool TreeNodeEx(const std::string &label, int flags,
                       const std::string &fmt) {
  return TreeNodeEx(label.c_str(),
                           static_cast<ImGuiTreeNodeFlags>(flags), fmt.c_str());
}
/* TODO: TreeNodeExV(...) (2) ==> UNSUPPORTED */
inline void TreePush(const std::string &str_id) {
  TreePush(str_id.c_str());
}
/* TODO: TreePush(const void*) ==> UNSUPPORTED */
inline void TreePop() { TreePop(); }
inline float GetTreeNodeToLabelSpacing() {
  return GetTreeNodeToLabelSpacing();
}
inline bool CollapsingHeader(const std::string &label) {
  return CollapsingHeader(label.c_str());
}
inline bool CollapsingHeader(const std::string &label, int flags) {
  return CollapsingHeader(label.c_str(),
                                 static_cast<ImGuiTreeNodeFlags>(flags));
}
inline std::tuple<bool, bool> CollapsingHeader(const std::string &label,
                                               bool open) {
  bool notCollapsed = CollapsingHeader(label.c_str(), &open);
  return std::make_tuple(open, notCollapsed);
}
inline std::tuple<bool, bool> CollapsingHeader(const std::string &label,
                                               bool open, int flags) {
  bool notCollapsed = CollapsingHeader(
      label.c_str(), &open, static_cast<ImGuiTreeNodeFlags>(flags));
  return std::make_tuple(open, notCollapsed);
}
inline void SetNextItemOpen(bool is_open) { SetNextItemOpen(is_open); }
inline void SetNextItemOpen(bool is_open, int cond) {
  SetNextItemOpen(is_open, static_cast<ImGuiCond>(cond));
}

// Widgets: Selectables
// TODO: Only one of Selectable variations is possible due to same parameters
// for Lua
inline bool Selectable(const std::string &label) {
  return Selectable(label.c_str());
}
inline bool Selectable(const std::string &label, bool selected) {
  Selectable(label.c_str(), &selected);
  return selected;
}
inline bool Selectable(const std::string &label, bool selected, int flags) {
  Selectable(label.c_str(), &selected,
                    static_cast<ImGuiSelectableFlags>(flags));
  return selected;
}
inline bool Selectable(const std::string &label, bool selected, int flags,
                       float sizeX, float sizeY) {
  Selectable(label.c_str(), &selected,
                    static_cast<ImGuiSelectableFlags>(flags), {sizeX, sizeY});
  return selected;
}

// Widgets: List Boxes
inline std::tuple<int, bool> ListBox(const std::string &label, int current_item,
                                     const sol::table &items, int items_count) {
  std::vector<std::string> strings;
  for (int i{1}; i <= items_count; i++) {
    const auto &stringItem = items.get<sol::optional<std::string>>(i);
    strings.push_back(stringItem.value_or("Missing"));
  }

  std::vector<const char *> cstrings;
  for (auto &string : strings)
    cstrings.push_back(string.c_str());

  bool clicked = ListBox(label.c_str(), &current_item, cstrings.data(),
                                items_count);
  return std::make_tuple(current_item, clicked);
}
inline std::tuple<int, bool> ListBox(const std::string &label, int current_item,
                                     const sol::table &items, int items_count,
                                     int height_in_items) {
  std::vector<std::string> strings;
  for (int i{1}; i <= items_count; i++) {
    const auto &stringItem = items.get<sol::optional<std::string>>(i);
    strings.push_back(stringItem.value_or("Missing"));
  }

  std::vector<const char *> cstrings;
  for (auto &string : strings)
    cstrings.push_back(string.c_str());

  bool clicked = ListBox(label.c_str(), &current_item, cstrings.data(),
                                items_count, height_in_items);
  return std::make_tuple(current_item, clicked);
}
inline bool ListBoxHeader(const std::string &label, float sizeX, float sizeY) {
  return ListBoxHeader(label.c_str(), {sizeX, sizeY});
}
inline bool ListBoxHeader(const std::string &label, int items_count) {
  return ListBoxHeader(label.c_str(), items_count);
}
inline bool ListBoxHeader(const std::string &label, int items_count,
                          int height_in_items) {
  return ListBoxHeader(label.c_str(), items_count, height_in_items);
}
inline void ListBoxFooter() { ListBoxFooter(); }

// Widgets: Data Plotting
/* TODO: Widgets Data Plotting ==> UNSUPPORTED (barely used and quite long
 * functions) */

// Widgets: Value() helpers
inline void Value(const std::string &prefix, bool b) {
  Value(prefix.c_str(), b);
}
inline void Value(const std::string &prefix, int v) {
  Value(prefix.c_str(), v);
}
inline void Value(const std::string &prefix, unsigned int v) {
  Value(prefix.c_str(), v);
}
inline void Value(const std::string &prefix, float v) {
  Value(prefix.c_str(), v);
}
inline void Value(const std::string &prefix, float v,
                  const std::string &float_format) {
  Value(prefix.c_str(), v, float_format.c_str());
}

// Widgets: Menus
inline bool BeginMenuBar() { return BeginMenuBar(); }
inline void EndMenuBar() { EndMenuBar(); }
inline bool BeginMainMenuBar() { return BeginMainMenuBar(); }
inline void EndMainMenuBar() { EndMainMenuBar(); }
inline bool BeginMenu(const std::string &label) {
  return BeginMenu(label.c_str());
}
inline bool BeginMenu(const std::string &label, bool enabled) {
  return BeginMenu(label.c_str(), enabled);
}
inline void EndMenu() { EndMenu(); }
inline bool MenuItem(const std::string &label) {
  return MenuItem(label.c_str());
}
inline bool MenuItem(const std::string &label, const std::string &shortcut) {
  return MenuItem(label.c_str(), shortcut.c_str());
}
inline std::tuple<bool, bool>
MenuItem(const std::string &label, const std::string &shortcut, bool selected) {
  bool activated = MenuItem(label.c_str(), shortcut.c_str(), &selected);
  return std::make_tuple(selected, activated);
}
inline std::tuple<bool, bool> MenuItem(const std::string &label,
                                       const std::string &shortcut,
                                       bool selected, bool enabled) {
  bool activated =
      MenuItem(label.c_str(), shortcut.c_str(), &selected, enabled);
  return std::make_tuple(selected, activated);
}

// Tooltips
inline void BeginTooltip() { BeginTooltip(); }
inline void EndTooltip() { EndTooltip(); }
inline void SetTooltip(const std::string &fmt) {
  SetTooltip(fmt.c_str());
}
inline void SetTooltipV() { /* TODO: SetTooltipV(...) ==> UNSUPPORTED */
}

// Popups, Modals
inline bool BeginPopup(const std::string &str_id) {
  return BeginPopup(str_id.c_str());
}
inline bool BeginPopup(const std::string &str_id, int flags) {
  return BeginPopup(str_id.c_str(),
                           static_cast<ImGuiWindowFlags>(flags));
}
inline bool BeginPopupModal(const std::string &name) {
  return BeginPopupModal(name.c_str());
}
inline bool BeginPopupModal(const std::string &name, bool open) {
  return BeginPopupModal(name.c_str(), &open);
}
inline bool BeginPopupModal(const std::string &name, bool open, int flags) {
  return BeginPopupModal(name.c_str(), &open,
                                static_cast<ImGuiWindowFlags>(flags));
}
inline void EndPopup() { EndPopup(); }
inline void OpenPopup(const std::string &str_id) {
  OpenPopup(str_id.c_str());
}
inline void OpenPopup(const std::string &str_id, int popup_flags) {
  OpenPopup(str_id.c_str(), static_cast<ImGuiPopupFlags>(popup_flags));
}
[[deprecated("Use OpenPopupOnItemClick instead.")]] inline void
OpenPopupContextItem() {
  return OpenPopupContextItem();
}
[[deprecated("Use OpenPopupOnItemClick instead.")]] inline void
OpenPopupContextItem(const std::string &str_id) {
  return OpenPopupContextItem(str_id.c_str());
}
[[deprecated("Use OpenPopupOnItemClick instead.")]] inline void
OpenPopupContextItem(const std::string &str_id, int popup_flags) {
  return OpenPopupContextItem(str_id.c_str(),
                                     static_cast<ImGuiPopupFlags>(popup_flags));
}
inline void OpenPopupOnItemClick() { return OpenPopupOnItemClick(); }
inline void OpenPopupOnItemClick(const std::string &str_id) {
  return OpenPopupOnItemClick(str_id.c_str());
}
inline void OpenPopupOnItemClick(const std::string &str_id, int popup_flags) {
  return OpenPopupOnItemClick(str_id.c_str(),
                                     static_cast<ImGuiPopupFlags>(popup_flags));
}
inline void CloseCurrentPopup() { CloseCurrentPopup(); }
inline bool BeginPopupContextItem() { return BeginPopupContextItem(); }
inline bool BeginPopupContextItem(const std::string &str_id) {
  return BeginPopupContextItem(str_id.c_str());
}
inline bool BeginPopupContextItem(const std::string &str_id, int popup_flags) {
  return BeginPopupContextItem(
      str_id.c_str(), static_cast<ImGuiPopupFlags>(popup_flags));
}
inline bool BeginPopupContextWindow() {
  return BeginPopupContextWindow();
}
inline bool BeginPopupContextWindow(const std::string &str_id) {
  return BeginPopupContextWindow(str_id.c_str());
}
inline bool BeginPopupContextWindow(const std::string &str_id,
                                    int popup_flags) {
  return BeginPopupContextWindow(
      str_id.c_str(), static_cast<ImGuiPopupFlags>(popup_flags));
}
inline bool BeginPopupContextVoid() { return BeginPopupContextVoid(); }
inline bool BeginPopupContextVoid(const std::string &str_id) {
  return BeginPopupContextVoid(str_id.c_str());
}
inline bool BeginPopupContextVoid(const std::string &str_id, int popup_flags) {
  return BeginPopupContextVoid(
      str_id.c_str(), static_cast<ImGuiPopupFlags>(popup_flags));
}
inline bool IsPopupOpen(const std::string &str_id) {
  return IsPopupOpen(str_id.c_str());
}
inline bool IsPopupOpen(const std::string &str_id, int popup_flags) {
  return IsPopupOpen(str_id.c_str(), popup_flags);
}

// Columns
inline void Columns() { Columns(); }
inline void Columns(int count) { Columns(count); }
inline void Columns(int count, const std::string &id) {
  Columns(count, id.c_str());
}
inline void Columns(int count, const std::string &id, bool border) {
  Columns(count, id.c_str(), border);
}
inline void NextColumn() { NextColumn(); }
inline int GetColumnIndex() { return GetColumnIndex(); }
inline float GetColumnWidth() { return GetColumnWidth(); }
inline float GetColumnWidth(int column_index) {
  return GetColumnWidth(column_index);
}
inline void SetColumnWidth(int column_index, float width) {
  SetColumnWidth(column_index, width);
}
inline float GetColumnOffset() { return GetColumnOffset(); }
inline float GetColumnOffset(int column_index) {
  return GetColumnOffset(column_index);
}
inline void SetColumnOffset(int column_index, float offset_x) {
  SetColumnOffset(column_index, offset_x);
}
inline int GetColumnsCount() { return GetColumnsCount(); }

// Tab Bars, Tabs
inline bool BeginTabBar(const std::string &str_id) {
  return BeginTabBar(str_id.c_str());
}
inline bool BeginTabBar(const std::string &str_id, int flags) {
  return BeginTabBar(str_id.c_str(),
                            static_cast<ImGuiTabBarFlags>(flags));
}
inline void EndTabBar() { EndTabBar(); }
inline bool BeginTabItem(const std::string &label) {
  return BeginTabItem(label.c_str());
}
inline std::tuple<bool, bool> BeginTabItem(const std::string &label,
                                           bool open) {
  bool selected = BeginTabItem(label.c_str(), &open);
  return std::make_tuple(open, selected);
}
inline std::tuple<bool, bool> BeginTabItem(const std::string &label, bool open,
                                           int flags) {
  bool selected = BeginTabItem(label.c_str(), &open,
                                      static_cast<ImGuiTabItemFlags>(flags));
  return std::make_tuple(open, selected);
}
inline void EndTabItem() { EndTabItem(); }
inline void SetTabItemClosed(const std::string &tab_or_docked_window_label) {
  SetTabItemClosed(tab_or_docked_window_label.c_str());
}

// Docking
inline void DockSpace(unsigned int id) { DockSpace(id); }
inline void DockSpace(unsigned int id, float sizeX, float sizeY) {
  DockSpace(id, {sizeX, sizeY});
}
inline void DockSpace(unsigned int id, float sizeX, float sizeY, int flags) {
  DockSpace(id, {sizeX, sizeY}, static_cast<ImGuiDockNodeFlags>(flags));
}
inline unsigned int DockSpaceOverViewport() {
  return 0; /* TODO: DockSpaceOverViwport(...) ==> UNSUPPORTED */
}
inline void SetNextWindowDockID(unsigned int dock_id) {
  SetNextWindowDockID(dock_id);
}
inline void SetNextWindowDockID(unsigned int dock_id, int cond) {
  SetNextWindowDockID(dock_id, static_cast<ImGuiCond>(cond));
}
inline void
SetNextWindowClass() { /* TODO: SetNextWindowClass(...) ==> UNSUPPORTED */
}
inline unsigned int GetWindowDockID() { return GetWindowDockID(); }
inline bool IsWindowDocked() { return IsWindowDocked(); }

// Logging
inline void LogToTTY() { LogToTTY(); }
inline void LogToTTY(int auto_open_depth) { LogToTTY(auto_open_depth); }
inline void LogToFile() { LogToFile(); }
inline void LogToFile(int auto_open_depth) {
  LogToFile(auto_open_depth);
}
inline void LogToFile(int auto_open_depth, const std::string &filename) {
  LogToFile(auto_open_depth, filename.c_str());
}
inline void LogToClipboard() { LogToClipboard(); }
inline void LogToClipboard(int auto_open_depth) {
  LogToClipboard(auto_open_depth);
}
inline void LogFinish() { LogFinish(); }
inline void LogButtons() { LogButtons(); }
inline void LogText(const std::string &fmt) { LogText(fmt.c_str()); }

// Drag and Drop
// TODO: Drag and Drop ==> UNSUPPORTED

// Clipping
inline void PushClipRect(float min_x, float min_y, float max_x, float max_y,
                         bool intersect_current) {
  PushClipRect({min_x, min_y}, {max_x, max_y}, intersect_current);
}
inline void PopClipRect() { PopClipRect(); }

// Focus, Activation
inline void SetItemDefaultFocus() { SetItemDefaultFocus(); }
inline void SetKeyboardFocusHere() { SetKeyboardFocusHere(); }
inline void SetKeyboardFocusHere(int offset) {
  SetKeyboardFocusHere(offset);
}

// Item/Widgets Utilities
inline bool IsItemHovered() { return IsItemHovered(); }
inline bool IsItemHovered(int flags) {
  return IsItemHovered(static_cast<ImGuiHoveredFlags>(flags));
}
inline bool IsItemActive() { return IsItemActive(); }
inline bool IsItemFocused() { return IsItemFocused(); }
inline bool IsItemClicked() { return IsItemClicked(); }
inline bool IsItemClicked(int mouse_button) {
  return IsItemClicked(static_cast<ImGuiMouseButton>(mouse_button));
}
inline bool IsItemVisible() { return IsItemVisible(); }
inline bool IsItemEdited() { return IsItemEdited(); }
inline bool IsItemActivated() { return IsItemActivated(); }
inline bool IsItemDeactivated() { return IsItemDeactivated(); }
inline bool IsItemDeactivatedAfterEdit() {
  return IsItemDeactivatedAfterEdit();
}
inline bool IsItemToggledOpen() { return IsItemToggledOpen(); }
inline bool IsAnyItemHovered() { return IsAnyItemHovered(); }
inline bool IsAnyItemActive() { return IsAnyItemActive(); }
inline bool IsAnyItemFocused() { return IsAnyItemFocused(); }
inline std::tuple<float, float> GetItemRectMin() {
  const auto vec2{GetItemRectMin()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetItemRectMax() {
  const auto vec2{GetItemRectMax()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetItemRectSize() {
  const auto vec2{GetItemRectSize()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline void SetItemAllowOverlap() { SetItemAllowOverlap(); }

// Miscellaneous Utilities
inline bool IsRectVisible(float sizeX, float sizeY) {
  return IsRectVisible({sizeX, sizeY});
}
inline bool IsRectVisible(float minX, float minY, float maxX, float maxY) {
  return IsRectVisible({minX, minY}, {maxX, maxY});
}
inline double GetTime() { return GetTime(); }
inline int GetFrameCount() { return GetFrameCount(); }
/* TODO: GetBackgroundDrawList(), GetForeGroundDrawList(),
 * GetDrawListSharedData() ==> UNSUPPORTED */
inline std::string GetStyleColorName(int idx) {
  return std::string(GetStyleColorName(static_cast<ImGuiCol>(idx)));
}
/* TODO: SetStateStorage(), GetStateStorage(), CalcListClipping() ==>
 * UNSUPPORTED */
inline bool BeginChildFrame(unsigned int id, float sizeX, float sizeY) {
  return BeginChildFrame(id, {sizeX, sizeY});
}
inline bool BeginChildFrame(unsigned int id, float sizeX, float sizeY,
                            int flags) {
  return BeginChildFrame(id, {sizeX, sizeY},
                                static_cast<ImGuiWindowFlags>(flags));
}
inline void EndChildFrame() { return EndChildFrame(); }

// Text Utilities
inline std::tuple<float, float> CalcTextSize(const std::string &text) {
  const auto vec2{CalcTextSize(text.c_str())};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> CalcTextSize(const std::string &text,
                                             const std::string &text_end) {
  const auto vec2{CalcTextSize(text.c_str(), text_end.c_str())};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> CalcTextSize(const std::string &text,
                                             const std::string &text_end,
                                             bool hide_text_after_double_hash) {
  const auto vec2{CalcTextSize(text.c_str(), text_end.c_str(),
                                      hide_text_after_double_hash)};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> CalcTextSize(const std::string &text,
                                             const std::string &text_end,
                                             bool hide_text_after_double_hash,
                                             float wrap_width) {
  const auto vec2{CalcTextSize(text.c_str(), text_end.c_str(),
                                      hide_text_after_double_hash, wrap_width)};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}

// Color Utilities
#ifdef SOL_IMGUI_USE_COLOR_U32
inline sol::as_table_t<std::vector<float>>
ColorConvertU32ToFloat4(unsigned int in) {
  const auto vec4 = ColorConvertU32ToFloat4(in);
  sol::as_table_t rgba =
      sol::as_table(std::vector<float>{vec4.x, vec4.y, vec4.z, vec4.w});

  return rgba;
}
inline unsigned int ColorConvertFloat4ToU32(const sol::table &rgba) {
  const lua_Number r{rgba[1].get<std::optional<lua_Number>>().value_or(
      static_cast<lua_Number>(0))},
      g{rgba[2].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      b{rgba[3].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))},
      a{rgba[4].get<std::optional<lua_Number>>().value_or(
          static_cast<lua_Number>(0))};

  return ColorConvertFloat4ToU32(
      {float(r), float(g), float(b), float(a)});
}
#endif
inline std::tuple<float, float, float> ColorConvertRGBtoHSV(float r, float g,
                                                            float b) {
  float h{}, s{}, v{};
  ColorConvertRGBtoHSV(r, g, b, h, s, v);
  return std::make_tuple(h, s, v);
}
inline std::tuple<float, float, float> ColorConvertHSVtoRGB(float h, float s,
                                                            float v) {
  float r{}, g{}, b{};
  ColorConvertHSVtoRGB(h, s, v, r, g, b);
  return std::make_tuple(r, g, b);
}

// Inputs Utilities: Keyboard
inline int GetKeyIndex(int imgui_key) {
  return GetKeyIndex(static_cast<ImGuiKey>(imgui_key));
}
inline bool IsKeyDown(int user_key_index) {
  return IsKeyDown(user_key_index);
}
inline bool IsKeyPressed(int user_key_index) {
  return IsKeyPressed(user_key_index);
}
inline bool IsKeyPressed(int user_key_index, bool repeat) {
  return IsKeyPressed(user_key_index, repeat);
}
inline bool IsKeyReleased(int user_key_index) {
  return IsKeyReleased(user_key_index);
}
inline int GetKeyPressedAmount(int key_index, float repeat_delay, float rate) {
  return GetKeyPressedAmount(key_index, repeat_delay, rate);
}
inline void CaptureKeyboardFromApp() { CaptureKeyboardFromApp(); }
inline void CaptureKeyboardFromApp(bool want_capture_keyboard_value) {
  CaptureKeyboardFromApp(want_capture_keyboard_value);
}

// Inputs Utilities: Mouse
inline bool IsMouseDown(int button) {
  return IsMouseDown(static_cast<ImGuiMouseButton>(button));
}
inline bool IsMouseClicked(int button) {
  return IsMouseClicked(static_cast<ImGuiMouseButton>(button));
}
inline bool IsMouseClicked(int button, bool repeat) {
  return IsMouseClicked(static_cast<ImGuiMouseButton>(button), repeat);
}
inline bool IsMouseReleased(int button) {
  return IsMouseReleased(static_cast<ImGuiMouseButton>(button));
}
inline bool IsMouseDoubleClicked(int button) {
  return IsMouseDoubleClicked(static_cast<ImGuiMouseButton>(button));
}
inline bool IsMouseHoveringRect(float min_x, float min_y, float max_x,
                                float max_y) {
  return IsMouseHoveringRect({min_x, min_y}, {max_x, max_y});
}
inline bool IsMouseHoveringRect(float min_x, float min_y, float max_x,
                                float max_y, bool clip) {
  return IsMouseHoveringRect({min_x, min_y}, {max_x, max_y}, clip);
}
inline bool IsMousePosValid() {
  return false; /* TODO: IsMousePosValid() ==> UNSUPPORTED */
}
inline bool IsAnyMouseDown() { return IsAnyMouseDown(); }
inline std::tuple<float, float> GetMousePos() {
  const auto vec2{GetMousePos()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetMousePosOnOpeningCurrentPopup() {
  const auto vec2{GetMousePosOnOpeningCurrentPopup()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline bool IsMouseDragging(int button) {
  return IsMouseDragging(static_cast<ImGuiMouseButton>(button));
}
inline bool IsMouseDragging(int button, float lock_threshold) {
  return IsMouseDragging(static_cast<ImGuiMouseButton>(button),
                                lock_threshold);
}
inline std::tuple<float, float> GetMouseDragDelta() {
  const auto vec2{GetMouseDragDelta()};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetMouseDragDelta(int button) {
  const auto vec2{
      GetMouseDragDelta(static_cast<ImGuiMouseButton>(button))};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline std::tuple<float, float> GetMouseDragDelta(int button,
                                                  float lock_threshold) {
  const auto vec2{GetMouseDragDelta(
      static_cast<ImGuiMouseButton>(button), lock_threshold)};
  return std::make_tuple(std::get<0>(vec2), std::get<1>(vec2));
}
inline void ResetMouseDragDelta() { ResetMouseDragDelta(); }
inline void ResetMouseDragDelta(int button) {
  ResetMouseDragDelta(static_cast<ImGuiMouseButton>(button));
}
inline int GetMouseCursor() { return GetMouseCursor(); }
inline void SetMouseCursor(int cursor_type) {
  SetMouseCursor(static_cast<ImGuiMouseCursor>(cursor_type));
}
inline void CaptureMouseFromApp() { CaptureMouseFromApp(); }
inline void CaptureMouseFromApp(bool want_capture_mouse_value) {
  CaptureMouseFromApp(want_capture_mouse_value);
}

// Clipboard Utilities
inline std::string GetClipboardText() {
  return std::string(GetClipboardText());
}
inline void SetClipboardText(const std::string &text) {
  SetClipboardText(text.c_str());
}

inline void InitEnums(sol::state &lua) {
#pragma region Window Flags
  lua.new_enum(
      "ImGuiWindowFlags", "None", ImGuiWindowFlags_None, "NoTitleBar",
      ImGuiWindowFlags_NoTitleBar, "NoResize", ImGuiWindowFlags_NoResize,
      "NoMove", ImGuiWindowFlags_NoMove, "NoScrollbar",
      ImGuiWindowFlags_NoScrollbar, "NoScrollWithMouse",
      ImGuiWindowFlags_NoScrollWithMouse, "NoCollapse",
      ImGuiWindowFlags_NoCollapse, "AlwaysAutoResize",
      ImGuiWindowFlags_AlwaysAutoResize, "NoBackground",
      ImGuiWindowFlags_NoBackground, "NoSavedSettings",
      ImGuiWindowFlags_NoSavedSettings, "NoMouseInputs",
      ImGuiWindowFlags_NoMouseInputs, "MenuBar", ImGuiWindowFlags_MenuBar,
      "HorizontalScrollbar", ImGuiWindowFlags_HorizontalScrollbar,
      "NoFocusOnAppearing", ImGuiWindowFlags_NoFocusOnAppearing,
      "NoBringToFrontOnFocus", ImGuiWindowFlags_NoBringToFrontOnFocus,
      "AlwaysVerticalScrollbar", ImGuiWindowFlags_AlwaysVerticalScrollbar,
      "AlwaysHorizontalScrollbar", ImGuiWindowFlags_AlwaysHorizontalScrollbar,
      "AlwaysUseWindowPadding", ImGuiWindowFlags_AlwaysUseWindowPadding,
      "NoNavInputs", ImGuiWindowFlags_NoNavInputs, "NoNavFocus",
      ImGuiWindowFlags_NoNavFocus, "UnsavedDocument",
      ImGuiWindowFlags_UnsavedDocument, "NoDocking", ImGuiWindowFlags_NoDocking,

      "NoNav", ImGuiWindowFlags_NoNav, "NoDecoration",
      ImGuiWindowFlags_NoDecoration, "NoInputs", ImGuiWindowFlags_NoInputs,

      "NavFlattened", ImGuiWindowFlags_NavFlattened, "ChildWindow",
      ImGuiWindowFlags_ChildWindow, "Tooltip", ImGuiWindowFlags_Tooltip,
      "Popup", ImGuiWindowFlags_Popup, "Modal", ImGuiWindowFlags_Modal,
      "ChildMenu", ImGuiWindowFlags_ChildMenu,

#ifdef CUSTOM_IMGUI
      "DockNodeHost", ImGuiWindowFlags_DockNodeHost, "NoClose",
      ImGuiWindowFlags_NoClose, "NoDockTab", ImGuiWindowFlags_NoDockTab
#else
      "DockNodeHost", ImGuiWindowFlags_DockNodeHost
#endif
  );
#pragma endregion Window Flags

#pragma region Focused Flags
  lua.new_enum("ImGuiFocusedFlags", "None", ImGuiFocusedFlags_None,
               "ChildWindows", ImGuiFocusedFlags_ChildWindows, "RootWindow",
               ImGuiFocusedFlags_RootWindow, "AnyWindow",
               ImGuiFocusedFlags_AnyWindow, "RootAndChildWindows",
               ImGuiFocusedFlags_RootAndChildWindows);
#pragma endregion Focused Flags

#pragma region Hovered Flags
  lua.new_enum("ImGuiHoveredFlags", "None", ImGuiHoveredFlags_None,
               "ChildWindows", ImGuiHoveredFlags_ChildWindows, "RootWindow",
               ImGuiHoveredFlags_RootWindow, "AnyWindow",
               ImGuiHoveredFlags_AnyWindow, "AllowWhenBlockedByPopup",
               ImGuiHoveredFlags_AllowWhenBlockedByPopup,
               "AllowWhenBlockedByActiveItem",
               ImGuiHoveredFlags_AllowWhenBlockedByActiveItem,
               "AllowWhenOverlapped", ImGuiHoveredFlags_AllowWhenOverlapped,
               "AllowWhenDisabled", ImGuiHoveredFlags_AllowWhenDisabled,
               "RectOnly", ImGuiHoveredFlags_RectOnly, "RootAndChildWindows",
               ImGuiHoveredFlags_RootAndChildWindows);
#pragma endregion Hovered Flags

#pragma region Cond
  lua.new_enum("ImGuiCond", "None", ImGuiCond_None, "Always", ImGuiCond_Always,
               "Once", ImGuiCond_Once, "FirstUseEver", ImGuiCond_FirstUseEver,
               "Appearing", ImGuiCond_Appearing);
#pragma endregion Cond

#pragma region Col
  lua.new_enum(
      "ImGuiCol", "Text", ImGuiCol_Text, "TextDisabled", ImGuiCol_TextDisabled,
      "WindowBg", ImGuiCol_WindowBg, "ChildBg", ImGuiCol_ChildBg, "PopupBg",
      ImGuiCol_PopupBg, "Border", ImGuiCol_Border, "BorderShadow",
      ImGuiCol_BorderShadow, "FrameBg", ImGuiCol_FrameBg, "FrameBgHovered",
      ImGuiCol_FrameBgHovered, "FrameBgActive", ImGuiCol_FrameBgActive,
      "TitleBg", ImGuiCol_TitleBg, "TitleBgActive", ImGuiCol_TitleBgActive,
      "TitleBgCollapsed", ImGuiCol_TitleBgCollapsed, "MenuBarBg",
      ImGuiCol_MenuBarBg, "ScrollbarBg", ImGuiCol_ScrollbarBg, "ScrollbarGrab",
      ImGuiCol_ScrollbarGrab, "ScrollbarGrabHovered",
      ImGuiCol_ScrollbarGrabHovered, "ScrollbarGrabActive",
      ImGuiCol_ScrollbarGrabActive, "CheckMark", ImGuiCol_CheckMark,
      "SliderGrab", ImGuiCol_SliderGrab, "SliderGrabActive",
      ImGuiCol_SliderGrabActive, "Button", ImGuiCol_Button, "ButtonHovered",
      ImGuiCol_ButtonHovered, "ButtonActive", ImGuiCol_ButtonActive, "Header",
      ImGuiCol_Header, "HeaderHovered", ImGuiCol_HeaderHovered, "HeaderActive",
      ImGuiCol_HeaderActive, "Separator", ImGuiCol_Separator,
      "SeparatorHovered", ImGuiCol_SeparatorHovered, "SeparatorActive",
      ImGuiCol_SeparatorActive, "ResizeGrip", ImGuiCol_ResizeGrip,
      "ResizeGripHovered", ImGuiCol_ResizeGripHovered, "ResizeGripActive",
      ImGuiCol_ResizeGripActive, "Tab", ImGuiCol_Tab, "TabHovered",
      ImGuiCol_TabHovered, "TabActive", ImGuiCol_TabActive, "TabUnfocused",
      ImGuiCol_TabUnfocused, "TabUnfocusedActive", ImGuiCol_TabUnfocusedActive,
      "DockingPreview", ImGuiCol_DockingPreview, "DockingEmptyBg",
      ImGuiCol_DockingEmptyBg, "PlotLines", ImGuiCol_PlotLines,
      "PlotLinesHovered", ImGuiCol_PlotLinesHovered, "PlotHistogram",
      ImGuiCol_PlotHistogram, "PlotHistogramHovered",
      ImGuiCol_PlotHistogramHovered, "TextSelectedBg", ImGuiCol_TextSelectedBg,
      "DragDropTarget", ImGuiCol_DragDropTarget, "NavHighlight",
      ImGuiCol_NavHighlight, "NavWindowingHighlight",
      ImGuiCol_NavWindowingHighlight, "NavWindowingDimBg",
      ImGuiCol_NavWindowingDimBg, "ModalWindowDimBg", ImGuiCol_ModalWindowDimBg,
      "ModalWindowDarkening", ImGuiCol_ModalWindowDimBg, "COUNT",
      ImGuiCol_COUNT);
#pragma endregion Col

#pragma region Style
  lua.new_enum(
      "ImGuiStyleVar", "Alpha", ImGuiStyleVar_Alpha,  // float     Alpha
      "DisabledAlpha", ImGuiStyleVar_DisabledAlpha,   // float     DisabledAlpha
      "WindowPadding", ImGuiStyleVar_WindowPadding,   // ImVec2    WindowPadding
      "WindowRounding", ImGuiStyleVar_WindowRounding, // float WindowRounding
      "WindowBorderSize",
      ImGuiStyleVar_WindowBorderSize, // float     WindowBorderSize
      "WindowMinSize", ImGuiStyleVar_WindowMinSize, // ImVec2    WindowMinSize
      "WindowTitleAlign",
      ImGuiStyleVar_WindowTitleAlign, // ImVec2    WindowTitleAlign
      "ChildRounding", ImGuiStyleVar_ChildRounding, // float     ChildRounding
      "ChildBorderSize", ImGuiStyleVar_ChildBorderSize, // float ChildBorderSize
      "PopupRounding", ImGuiStyleVar_PopupRounding, // float     PopupRounding
      "PopupBorderSize", ImGuiStyleVar_PopupBorderSize, // float PopupBorderSize
      "FramePadding", ImGuiStyleVar_FramePadding,   // ImVec2    FramePadding
      "FrameRounding", ImGuiStyleVar_FrameRounding, // float     FrameRounding
      "FrameBorderSize", ImGuiStyleVar_FrameBorderSize, // float FrameBorderSize
      "ItemSpacing", ImGuiStyleVar_ItemSpacing,         // ImVec2    ItemSpacing
      "ItemInnerSpace",
      ImGuiStyleVar_ItemInnerSpacing, // ImVec2    ItemInnerSpacing
      "IndentSpacing", ImGuiStyleVar_IndentSpacing, // float     IndentSpacing
      "CellPadding", ImGuiStyleVar_CellPadding,     // ImVec2    CellPadding
      "ScrollbarSize", ImGuiStyleVar_ScrollbarSize, // float     ScrollbarSize
      "ScrollbarRounding",
      ImGuiStyleVar_ScrollbarRounding,            // float     ScrollbarRounding
      "GrabMinSize", ImGuiStyleVar_GrabMinSize,   // float     GrabMinSize
      "GrabRounding", ImGuiStyleVar_GrabRounding, // float     GrabRounding
      "TabRounding", ImGuiStyleVar_TabRounding,   // float     TabRounding
      "ButtonTextAlign",
      ImGuiStyleVar_ButtonTextAlign, // ImVec2    ButtonTextAlign
      "SelectableTextAlign",
      ImGuiStyleVar_SelectableTextAlign, // ImVec2    SelectableTextAlign
      "COUNT", ImGuiStyleVar_COUNT

  );
#pragma endregion Style

#pragma region Dir
  lua.new_enum("ImGuiDir", "None", ImGuiDir_None, "Left", ImGuiDir_Left,
               "Right", ImGuiDir_Right, "Up", ImGuiDir_Up, "Down",
               ImGuiDir_Down, "COUNT", ImGuiDir_COUNT);
#pragma endregion Dir

#pragma region Combo Flags
  lua.new_enum(
      "ImGuiComboFlags", "None", ImGuiComboFlags_None, "PopupAlignLeft",
      ImGuiComboFlags_PopupAlignLeft, "HeightSmall",
      ImGuiComboFlags_HeightSmall, "HeightRegular",
      ImGuiComboFlags_HeightRegular, "HeightLarge", ImGuiComboFlags_HeightLarge,
      "HeightLargest", ImGuiComboFlags_HeightLargest, "NoArrowButton",
      ImGuiComboFlags_NoArrowButton, "NoPreview", ImGuiComboFlags_NoPreview,
      "HeightMask", ImGuiComboFlags_HeightMask_);
#pragma endregion Combo Flags

#pragma region InputText Flags
  lua.new_enum("ImGuiInputTextFlags", "None", ImGuiInputTextFlags_None,
               "CharsDecimal", ImGuiInputTextFlags_CharsDecimal,
               "CharsHexadecimal", ImGuiInputTextFlags_CharsHexadecimal,
               "CharsUppercase", ImGuiInputTextFlags_CharsUppercase,
               "CharsNoBlank", ImGuiInputTextFlags_CharsNoBlank,
               "AutoSelectAll", ImGuiInputTextFlags_AutoSelectAll,
               "EnterReturnsTrue", ImGuiInputTextFlags_EnterReturnsTrue,
               "CallbackCompletion", ImGuiInputTextFlags_CallbackCompletion,
               "CallbackHistory", ImGuiInputTextFlags_CallbackHistory,
               "CallbackAlways", ImGuiInputTextFlags_CallbackAlways,
               "CallbackCharFilter", ImGuiInputTextFlags_CallbackCharFilter,
               "AllowTabInput", ImGuiInputTextFlags_AllowTabInput,
               "CtrlEnterForNewLine", ImGuiInputTextFlags_CtrlEnterForNewLine,
               "NoHorizontalScroll", ImGuiInputTextFlags_NoHorizontalScroll,
               "AlwaysOverwrite", ImGuiInputTextFlags_AlwaysOverwrite,
               "ReadOnly", ImGuiInputTextFlags_ReadOnly, "Password",
               ImGuiInputTextFlags_Password, "NoUndoRedo",
               ImGuiInputTextFlags_NoUndoRedo, "CharsScientific",
               ImGuiInputTextFlags_CharsScientific, "CallbackResize",
               ImGuiInputTextFlags_CallbackResize, "CallbackEdit",
               ImGuiInputTextFlags_CallbackEdit, "Multiline",
               ImGuiInputTextFlags_Multiline, "NoMarkEdited",
               ImGuiInputTextFlags_NoMarkEdited, "MergedItem",
               ImGuiInputTextFlags_MergedItem);
#pragma endregion InputText Flags

#pragma region ColorEdit Flags
  lua.new_enum(
      "ImGuiColorEditFlags", "None", ImGuiColorEditFlags_None, "NoAlpha",
      ImGuiColorEditFlags_NoAlpha, "NoPicker", ImGuiColorEditFlags_NoPicker,
      "NoOptions", ImGuiColorEditFlags_NoOptions, "NoSmallPreview",
      ImGuiColorEditFlags_NoSmallPreview, "NoInputs",
      ImGuiColorEditFlags_NoInputs, "NoTooltip", ImGuiColorEditFlags_NoTooltip,
      "NoLabel", ImGuiColorEditFlags_NoLabel, "NoSidePreview",
      ImGuiColorEditFlags_NoSidePreview, "NoDragDrop",
      ImGuiColorEditFlags_NoDragDrop, "NoBorder", ImGuiColorEditFlags_NoBorder,

      "AlphaBar", ImGuiColorEditFlags_AlphaBar, "AlphaPreview",
      ImGuiColorEditFlags_AlphaPreview, "AlphaPreviewHalf",
      ImGuiColorEditFlags_AlphaPreviewHalf, "HDR", ImGuiColorEditFlags_HDR,
      "DisplayRGB", ImGuiColorEditFlags_DisplayRGB, "DisplayHSV",
      ImGuiColorEditFlags_DisplayHSV, "DisplayHex",
      ImGuiColorEditFlags_DisplayHex, "Uint8", ImGuiColorEditFlags_Uint8,
      "Float", ImGuiColorEditFlags_Float, "PickerHueBar",
      ImGuiColorEditFlags_PickerHueBar, "PickerHueWheel",
      ImGuiColorEditFlags_PickerHueWheel, "InputRGB",
      ImGuiColorEditFlags_InputRGB, "InputHSV", ImGuiColorEditFlags_InputHSV,

      "DefaultOptions_", ImGuiColorEditFlags_DefaultOptions_,

      "DisplayMask_", ImGuiColorEditFlags_DisplayMask_, "DataTypeMask_",
      ImGuiColorEditFlags_DataTypeMask_, "PickerMask_",
      ImGuiColorEditFlags_PickerMask_, "InputMask_",
      ImGuiColorEditFlags_InputMask_

      // Obsolete names (will be removed)
      // ImGuiColorEditFlags_RGB = ImGuiColorEditFlags_DisplayRGB,
      // ImGuiColorEditFlags_HSV = ImGuiColorEditFlags_DisplayHSV,
      // ImGuiColorEditFlags_HEX = ImGuiColorEditFlags_DisplayHex  // [renamed
      // in 1.69]
      //"RGB"					, ImGuiColorEditFlags_RGB
  );
#pragma endregion ColorEdit Flags

#pragma region TreeNode Flags
  lua.new_enum("ImGuiTreeNodeFlags", "None", ImGuiTreeNodeFlags_None,
               "Selected", ImGuiTreeNodeFlags_Selected, "Framed",
               ImGuiTreeNodeFlags_Framed, "AllowItemOverlap",
               ImGuiTreeNodeFlags_AllowItemOverlap, "NoTreePushOnOpen",
               ImGuiTreeNodeFlags_NoTreePushOnOpen, "NoAutoOpenOnLog",
               ImGuiTreeNodeFlags_NoAutoOpenOnLog, "DefaultOpen",
               ImGuiTreeNodeFlags_DefaultOpen, "OpenOnDoubleClick",
               ImGuiTreeNodeFlags_OpenOnDoubleClick, "OpenOnArrow",
               ImGuiTreeNodeFlags_OpenOnArrow, "Leaf", ImGuiTreeNodeFlags_Leaf,
               "Bullet", ImGuiTreeNodeFlags_Bullet, "FramePadding",
               ImGuiTreeNodeFlags_FramePadding, "SpanAvailWidth",
               ImGuiTreeNodeFlags_SpanAvailWidth, "SpanFullWidth",
               ImGuiTreeNodeFlags_SpanFullWidth, "NavLeftJumpsBackHere",
               ImGuiTreeNodeFlags_NavLeftJumpsBackHere, "CollapsingHeader",
               ImGuiTreeNodeFlags_CollapsingHeader);
#pragma endregion TreeNode Flags

#pragma region Selectable Flags
  lua.new_enum("ImGuiSelectableFlags", "None", ImGuiSelectableFlags_None,
               "DontClosePopups", ImGuiSelectableFlags_DontClosePopups,
               "SpanAllColumns", ImGuiSelectableFlags_SpanAllColumns,
               "AllowDoubleClick", ImGuiSelectableFlags_AllowDoubleClick,
               "Disabled", ImGuiSelectableFlags_Disabled, "AllowItemOverlap",
               ImGuiSelectableFlags_AllowItemOverlap);
#pragma endregion Selectable Flags

#pragma region Popup Flags
  lua.new_enum(
      "ImGuiPopupFlags", "None", ImGuiPopupFlags_None, "MouseButtonLeft",
      ImGuiPopupFlags_MouseButtonLeft, "MouseButtonRight",
      ImGuiPopupFlags_MouseButtonRight, "MouseButtonMiddle",
      ImGuiPopupFlags_MouseButtonMiddle, "MouseButtonMask_",
      ImGuiPopupFlags_MouseButtonMask_, "MouseButtonDefault_",
      ImGuiPopupFlags_MouseButtonDefault_, "NoOpenOverExistingPopup",
      ImGuiPopupFlags_NoOpenOverExistingPopup, "NoOpenOverItems",
      ImGuiPopupFlags_NoOpenOverItems, "AnyPopupId", ImGuiPopupFlags_AnyPopupId,
      "AnyPopupLevel", ImGuiPopupFlags_AnyPopupLevel, "AnyPopup",
      ImGuiPopupFlags_AnyPopup);
#pragma endregion Popup Flags

#pragma region TabBar Flags
  lua.new_enum(
      "ImGuiTabBarFlags", "None", ImGuiTabBarFlags_None, "Reorderable",
      ImGuiTabBarFlags_Reorderable, "AutoSelectNewTabs",
      ImGuiTabBarFlags_AutoSelectNewTabs, "TabListPopupButton",
      ImGuiTabBarFlags_TabListPopupButton, "NoCloseWithMiddleMouseButton",
      ImGuiTabBarFlags_NoCloseWithMiddleMouseButton,
      "NoTabListScrollingButtons", ImGuiTabBarFlags_NoTabListScrollingButtons,
      "NoTooltip", ImGuiTabBarFlags_NoTooltip, "FittingPolicyResizeDown",
      ImGuiTabBarFlags_FittingPolicyResizeDown, "FittingPolicyScroll",
      ImGuiTabBarFlags_FittingPolicyScroll, "FittingPolicyMask_",
      ImGuiTabBarFlags_FittingPolicyMask_, "FittingPolicyDefault_",
      ImGuiTabBarFlags_FittingPolicyDefault_);
#pragma endregion TabBar Flags

#pragma region TabItem Flags
  lua.new_enum(
      "ImGuiTabItemFlags", "None", ImGuiTabItemFlags_None, "UnsavedDocument",
      ImGuiTabItemFlags_UnsavedDocument, "SetSelected",
      ImGuiTabItemFlags_SetSelected, "NoCloseWithMiddleMouseButton",
      ImGuiTabItemFlags_NoCloseWithMiddleMouseButton, "NoPushId",
      ImGuiTabItemFlags_NoPushId, "NoTooltip", ImGuiTabItemFlags_NoTooltip);
#pragma endregion TabItem Flags

#pragma region DockNode Flags
  lua.new_enum(
      "ImGuiDockNodeFlags", "None", ImGuiDockNodeFlags_None, "KeepAliveOnly",
      ImGuiDockNodeFlags_KeepAliveOnly, "NoDockingInCentralNode",
      ImGuiDockNodeFlags_NoDockingInCentralNode, "PassthruCentralNode",
      ImGuiDockNodeFlags_PassthruCentralNode, "NoSplit",
      ImGuiDockNodeFlags_NoSplit, "NoResize", ImGuiDockNodeFlags_NoResize,
      "AutoHideTabBar", ImGuiDockNodeFlags_AutoHideTabBar);
#pragma endregion DockNode Flags

#pragma region MouseButton
  lua.new_enum("ImGuiMouseButton", "ImGuiMouseButton_Left",
               ImGuiMouseButton_Left, "ImGuiMouseButton_Right",
               ImGuiMouseButton_Right, "ImGuiMouseButton_Middle",
               ImGuiMouseButton_Middle, "ImGuiMouseButton_COUNT",
               ImGuiMouseButton_COUNT);
#pragma endregion MouseButton

#pragma region Key
  lua.new_enum("ImGuiKey", "Tab", ImGuiKey_Tab, "LeftArrow", ImGuiKey_LeftArrow,
               "RightArrow", ImGuiKey_RightArrow, "UpArrow", ImGuiKey_UpArrow,
               "DownArrow", ImGuiKey_DownArrow, "PageUp", ImGuiKey_PageUp,
               "PageDown", ImGuiKey_PageDown, "Home", ImGuiKey_Home, "End",
               ImGuiKey_End, "Insert", ImGuiKey_Insert, "Delete",
               ImGuiKey_Delete, "Backspace", ImGuiKey_Backspace, "Space",
               ImGuiKey_Space, "Enter", ImGuiKey_Enter, "Escape",
               ImGuiKey_Escape, "KeyPadEnter", ImGuiKey_KeyPadEnter, "A",
               ImGuiKey_A, "C", ImGuiKey_C, "V", ImGuiKey_V, "X", ImGuiKey_X,
               "Y", ImGuiKey_Y, "Z", ImGuiKey_Z, "COUNT", ImGuiKey_COUNT);
#pragma endregion Key

#pragma region MouseCursor
  lua.new_enum("ImGuiMouseCursor", "None", ImGuiMouseCursor_None, "Arrow",
               ImGuiMouseCursor_Arrow, "TextInput", ImGuiMouseCursor_TextInput,
               "ResizeAll", ImGuiMouseCursor_ResizeAll, "ResizeNS",
               ImGuiMouseCursor_ResizeNS, "ResizeEW", ImGuiMouseCursor_ResizeEW,
               "ResizeNESW", ImGuiMouseCursor_ResizeNESW, "ResizeNWSE",
               ImGuiMouseCursor_ResizeNWSE, "Hand", ImGuiMouseCursor_Hand,
               "NotAllowed", ImGuiMouseCursor_NotAllowed, "COUNT",
               ImGuiMouseCursor_COUNT);
#pragma endregion MouseCursor
}

inline void Init(sol::state &lua) {
  InitEnums(lua);

  sol::table ImGui = lua.create_named_table("ImGui");

#pragma region Windows
  ImGui.set_function(
      "Begin",
      sol::overload(
          sol::resolve<bool(const std::string &)>(Begin),
          sol::resolve<std::tuple<bool, bool>(const std::string &, bool)>(
              Begin),
          sol::resolve<std::tuple<bool, bool>(const std::string &, bool, int)>(
              Begin)));
  ImGui.set_function("End", End);
#pragma endregion Windows

#pragma region Child Windows
  ImGui.set_function(
      "BeginChild",
      sol::overload(
          sol::resolve<bool(const std::string &)>(BeginChild),
          sol::resolve<bool(const std::string &, float)>(BeginChild),
          sol::resolve<bool(const std::string &, float, float)>(BeginChild),
          sol::resolve<bool(const std::string &, float, float, bool)>(
              BeginChild),
          sol::resolve<bool(const std::string &, float, float, bool, int)>(
              BeginChild)));
  ImGui.set_function("EndChild", EndChild);
#pragma endregion Child Windows

#pragma region Window Utilities
  ImGui.set_function("IsWindowAppearing", IsWindowAppearing);
  ImGui.set_function("IsWindowCollapsed", IsWindowCollapsed);
  ImGui.set_function("IsWindowFocused",
                     sol::overload(sol::resolve<bool()>(IsWindowFocused),
                                   sol::resolve<bool(int)>(IsWindowFocused)));
  ImGui.set_function("IsWindowHovered",
                     sol::overload(sol::resolve<bool()>(IsWindowHovered),
                                   sol::resolve<bool(int)>(IsWindowHovered)));
  ImGui.set_function("GetWindowDpiScale", GetWindowDpiScale);
  ImGui.set_function("GetWindowPos", GetWindowPos);
  ImGui.set_function("GetWindowSize", GetWindowSize);
  ImGui.set_function("GetWindowWidth", GetWindowWidth);
  ImGui.set_function("GetWindowHeight", GetWindowHeight);

  // Prefer  SetNext...
  ImGui.set_function(
      "SetNextWindowPos",
      sol::overload(sol::resolve<void(float, float)>(SetNextWindowPos),
                    sol::resolve<void(float, float, int)>(SetNextWindowPos),
                    sol::resolve<void(float, float, int, float, float)>(
                        SetNextWindowPos)));
  ImGui.set_function(
      "SetNextWindowSize",
      sol::overload(sol::resolve<void(float, float)>(SetNextWindowSize),
                    sol::resolve<void(float, float, int)>(SetNextWindowSize)));
  ImGui.set_function("SetNextWindowSizeConstraints",
                     SetNextWindowSizeConstraints);
  ImGui.set_function("SetNextWindowContentSize", SetNextWindowContentSize);
  ImGui.set_function(
      "SetNextWindowCollapsed",
      sol::overload(sol::resolve<void(bool)>(SetNextWindowCollapsed),
                    sol::resolve<void(bool, int)>(SetNextWindowCollapsed)));
  ImGui.set_function("SetNextWindowFocus", SetNextWindowFocus);
  ImGui.set_function("SetNextWindowBgAlpha", SetNextWindowBgAlpha);
  ImGui.set_function(
      "SetWindowPos",
      sol::overload(
          sol::resolve<void(float, float)>(SetWindowPos),
          sol::resolve<void(float, float, int)>(SetWindowPos),
          sol::resolve<void(const std::string &, float, float)>(SetWindowPos),
          sol::resolve<void(const std::string &, float, float, int)>(
              SetWindowPos)));
  ImGui.set_function(
      "SetWindowSize",
      sol::overload(
          sol::resolve<void(float, float)>(SetWindowSize),
          sol::resolve<void(float, float, int)>(SetWindowSize),
          sol::resolve<void(const std::string &, float, float)>(SetWindowSize),
          sol::resolve<void(const std::string &, float, float, int)>(
              SetWindowSize)));
  ImGui.set_function(
      "SetWindowCollapsed",
      sol::overload(
          sol::resolve<void(bool)>(SetWindowCollapsed),
          sol::resolve<void(bool, int)>(SetWindowCollapsed),
          sol::resolve<void(const std::string &, bool)>(SetWindowCollapsed),
          sol::resolve<void(const std::string &, bool, int)>(
              SetWindowCollapsed)));
  ImGui.set_function(
      "SetWindowFocus",
      sol::overload(sol::resolve<void()>(SetWindowFocus),
                    sol::resolve<void(const std::string &)>(SetWindowFocus)));
  ImGui.set_function("SetWindowFontScale", SetWindowFontScale);
#pragma endregion Window Utilities

#pragma region Content Region
  ImGui.set_function("GetContentRegionMax", GetContentRegionMax);
  ImGui.set_function("GetContentRegionAvail", GetContentRegionAvail);
  ImGui.set_function("GetWindowContentRegionMin", GetWindowContentRegionMin);
  ImGui.set_function("GetWindowContentRegionMax", GetWindowContentRegionMax);
  ImGui.set_function("GetWindowContentRegionWidth",
                     GetWindowContentRegionWidth);
#pragma endregion Content Region

#pragma region Windows Scrolling
  ImGui.set_function("GetScrollX", GetScrollX);
  ImGui.set_function("GetScrollY", GetScrollY);
  ImGui.set_function("GetScrollMaxX", GetScrollMaxX);
  ImGui.set_function("GetScrollMaxY", GetScrollMaxY);
  ImGui.set_function("SetScrollX", SetScrollX);
  ImGui.set_function("SetScrollY", SetScrollY);
  ImGui.set_function("SetScrollHereX",
                     sol::overload(sol::resolve<void()>(SetScrollHereX),
                                   sol::resolve<void(float)>(SetScrollHereX)));
  ImGui.set_function("SetScrollHereY",
                     sol::overload(sol::resolve<void()>(SetScrollHereY),
                                   sol::resolve<void(float)>(SetScrollHereY)));
  ImGui.set_function(
      "SetScrollFromPosX",
      sol::overload(sol::resolve<void(float)>(SetScrollFromPosX),
                    sol::resolve<void(float, float)>(SetScrollFromPosX)));
  ImGui.set_function(
      "SetScrollFromPosY",
      sol::overload(sol::resolve<void(float)>(SetScrollFromPosY),
                    sol::resolve<void(float, float)>(SetScrollFromPosY)));
#pragma endregion Windows Scrolling

#pragma region Parameters stacks(shared)
  ImGui.set_function("PushFont", PushFont);
  ImGui.set_function("PopFont", PopFont);
#ifdef SOL_IMGUI_USE_COLOR_U32
  ImGui.set_function(
      "PushStyleColor",
      sol::overload(
          sol::resolve<void(int, int)>(PushStyleColor),
          sol::resolve<void(int, float, float, float, float)>(PushStyleColor)));
#else
  ImGui.set_function("PushStyleColor", PushStyleColor);
#endif
  ImGui.set_function("PopStyleColor",
                     sol::overload(sol::resolve<void()>(PopStyleColor),
                                   sol::resolve<void(int)>(PopStyleColor)));
  ImGui.set_function("GetStyleColorVec4", GetStyleColorVec4);
  ImGui.set_function("GetFont", GetFont);
  ImGui.set_function("GetFontSize", GetFontSize);
  ImGui.set_function("GetFontTexUvWhitePixel", GetFontTexUvWhitePixel);
#ifdef SOL_IMGUI_USE_COLOR_U32
  ImGui.set_function(
      "GetColorU32",
      sol::overload(sol::resolve<int(int, float)>(GetColorU32),
                    sol::resolve<int(float, float, float, float)>(GetColorU32),
                    sol::resolve<int(int)>(GetColorU32)));
#endif
#pragma endregion Parameters stacks(shared)

#pragma region Parameters stacks(current window)
  ImGui.set_function("PushItemWidth", PushItemWidth);
  ImGui.set_function("PopItemWidth", PopItemWidth);
  ImGui.set_function("SetNextItemWidth", SetNextItemWidth);
  ImGui.set_function("CalcItemWidth", CalcItemWidth);
  ImGui.set_function("PushTextWrapPos",
                     sol::overload(sol::resolve<void()>(PushTextWrapPos),
                                   sol::resolve<void(float)>(PushTextWrapPos)));
  ImGui.set_function("PopTextWrapPos", PopTextWrapPos);
  ImGui.set_function("PushAllowKeyboardFocus", PushAllowKeyboardFocus);
  ImGui.set_function("PopAllowKeyboardFocus", PopAllowKeyboardFocus);
  ImGui.set_function("PushButtonRepeat", PushButtonRepeat);
  ImGui.set_function("PopButtonRepeat", PopButtonRepeat);
#pragma endregion Parameters stacks(current window)

#pragma region Cursor / Layout
  ImGui.set_function("Separator", Separator);
  ImGui.set_function("SameLine",
                     sol::overload(sol::resolve<void()>(SameLine),
                                   sol::resolve<void(float)>(SameLine)));
  ImGui.set_function("NewLine", NewLine);
  ImGui.set_function("Spacing", Spacing);
  ImGui.set_function("Dummy", Dummy);
  ImGui.set_function("Indent",
                     sol::overload(sol::resolve<void()>(Indent),
                                   sol::resolve<void(float)>(Indent)));
  ImGui.set_function("Unindent",
                     sol::overload(sol::resolve<void()>(Unindent),
                                   sol::resolve<void(float)>(Unindent)));
  ImGui.set_function("BeginGroup", BeginGroup);
  ImGui.set_function("EndGroup", EndGroup);
  ImGui.set_function("GetCursorPos", GetCursorPos);
  ImGui.set_function("GetCursorPosX", GetCursorPosX);
  ImGui.set_function("GetCursorPosY", GetCursorPosY);
  ImGui.set_function("SetCursorPos", SetCursorPos);
  ImGui.set_function("SetCursorPosX", SetCursorPosX);
  ImGui.set_function("SetCursorPosY", SetCursorPosY);
  ImGui.set_function("GetCursorStartPos", GetCursorStartPos);
  ImGui.set_function("GetCursorScreenPos", GetCursorScreenPos);
  ImGui.set_function("SetCursorScreenPos", SetCursorScreenPos);
  ImGui.set_function("AlignTextToFramePadding", AlignTextToFramePadding);
  ImGui.set_function("GetTextLineHeight", GetTextLineHeight);
  ImGui.set_function("GetTextLineHeightWithSpacing",
                     GetTextLineHeightWithSpacing);
  ImGui.set_function("GetFrameHeight", GetFrameHeight);
  ImGui.set_function("GetFrameHeightWithSpacing", GetFrameHeightWithSpacing);
#pragma endregion Cursor / Layout

#pragma region ID stack / scopes
  ImGui.set_function(
      "PushID",
      sol::overload(
          sol::resolve<void(const std::string &)>(PushID),
          sol::resolve<void(const std::string &, const std::string &)>(PushID),
          sol::resolve<void(int)>(PushID)));
  ImGui.set_function("PopID", PopID);
  ImGui.set_function(
      "GetID",
      sol::overload(
          sol::resolve<int(const std::string &)>(GetID),
          sol::resolve<int(const std::string &, const std::string &)>(GetID)));
#pragma endregion ID stack / scopes

#pragma region Widgets : Text
  ImGui.set_function(
      "TextUnformatted",
      sol::overload(
          sol::resolve<void(const std::string &)>(TextUnformatted),
          sol::resolve<void(const std::string &, const std::string &)>(
              TextUnformatted)));
  ImGui.set_function("Text", Text);
  ImGui.set_function("TextColored", TextColored);
  ImGui.set_function("TextDisabled", TextDisabled);
  ImGui.set_function("TextWrapped", TextWrapped);
  ImGui.set_function("LabelText", LabelText);
  ImGui.set_function("BulletText", BulletText);
#pragma endregion Widgets : Text

#pragma region Widgets : Main
  ImGui.set_function(
      "Button",
      sol::overload(
          sol::resolve<bool(const std::string &)>(Button),
          sol::resolve<bool(const std::string &, float, float)>(Button)));
  ImGui.set_function("SmallButton", SmallButton);
  ImGui.set_function("InvisibleButton", InvisibleButton);
  ImGui.set_function("ArrowButton", ArrowButton);
  ImGui.set_function("Checkbox", Checkbox);
  ImGui.set_function(
      "RadioButton",
      sol::overload(
          sol::resolve<bool(const std::string &, bool)>(RadioButton),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int)>(
              RadioButton)));
  ImGui.set_function(
      "ProgressBar",
      sol::overload(
          sol::resolve<void(float)>(ProgressBar),
          sol::resolve<void(float, float, float)>(ProgressBar),
          sol::resolve<void(float, float, float, const std::string &)>(
              ProgressBar)));
  ImGui.set_function("Bullet", Bullet);
#pragma endregion Widgets : Main

#pragma region Widgets : Combo Box
  ImGui.set_function(
      "BeginCombo",
      sol::overload(
          sol::resolve<bool(const std::string &, const std::string &)>(
              BeginCombo),
          sol::resolve<bool(const std::string &, const std::string &, int)>(
              BeginCombo)));
  ImGui.set_function("EndCombo", EndCombo);
  ImGui.set_function(
      "Combo",
      sol::overload(
          sol::resolve<std::tuple<int, bool>(const std::string &, int,
                                             const sol::table &, int)>(Combo),
          sol::resolve<std::tuple<int, bool>(
              const std::string &, int, const sol::table &, int, int)>(Combo),
          sol::resolve<std::tuple<int, bool>(const std::string &, int,
                                             const std::string &)>(Combo),
          sol::resolve<std::tuple<int, bool>(
              const std::string &, int, const std::string &, int)>(Combo)));
#pragma endregion Widgets : Combo Box

#pragma region Widgets : Drags
  ImGui.set_function(
      "DragFloat",
      sol::overload(
          sol::resolve<std::tuple<float, bool>(const std::string &, float)>(
              DragFloat),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float)>(DragFloat),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float)>(DragFloat),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float, float)>(DragFloat),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float, float,
                                               const std::string &)>(DragFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, float,
              const std::string &, float)>(DragFloat)));
  ImGui.set_function(
      "DragFloat2",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(DragFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float)>(DragFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float)>(
              DragFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float)>(
              DragFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float,
              const std::string &)>(DragFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float,
              const std::string &, float)>(DragFloat2)));
  ImGui.set_function(
      "DragFloat3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(DragFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float)>(DragFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float)>(
              DragFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float)>(
              DragFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float,
              const std::string &)>(DragFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float,
              const std::string &, float)>(DragFloat3)));
  ImGui.set_function(
      "DragFloat4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(DragFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float)>(DragFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float)>(
              DragFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float)>(
              DragFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float,
              const std::string &)>(DragFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float, float,
              const std::string &, float)>(DragFloat4)));
  ImGui.set_function(
      "DragInt",
      sol::overload(
          sol::resolve<std::tuple<int, bool>(const std::string &, int)>(
              DragInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, float)>(
              DragInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, float,
                                             int)>(DragInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, float,
                                             int, int)>(DragInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, float,
                                             int, int, const std::string &)>(
              DragInt)));
  ImGui.set_function(
      "DragInt2",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &)>(DragInt2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float)>(DragInt2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int)>(DragInt2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int, int)>(
              DragInt2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int, int,
              const std::string &)>(DragInt2)));
  ImGui.set_function(
      "DragInt3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &)>(DragInt3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float)>(DragInt3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int)>(DragInt3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int, int)>(
              DragInt3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int, int,
              const std::string &)>(DragInt3)));
  ImGui.set_function(
      "DragInt4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &)>(DragInt4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float)>(DragInt4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int)>(DragInt4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int, int)>(
              DragInt4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, float, int, int,
              const std::string &)>(DragInt4)));
#pragma endregion Widgets : Drags

#pragma region Widgets : Sliders
  ImGui.set_function(
      "SliderFloat",
      sol::overload(
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float)>(SliderFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, const std::string &)>(
              SliderFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, const std::string &,
              float)>(SliderFloat)));
  ImGui.set_function(
      "SliderFloat2",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float)>(
              SliderFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float,
              const std::string &)>(SliderFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float,
              const std::string &, float)>(SliderFloat2)));
  ImGui.set_function(
      "SliderFloat3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float)>(
              SliderFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float,
              const std::string &)>(SliderFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float,
              const std::string &, float)>(SliderFloat3)));
  ImGui.set_function(
      "SliderFloat4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float)>(
              SliderFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float,
              const std::string &)>(SliderFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, float, float,
              const std::string &, float)>(SliderFloat4)));
  ImGui.set_function(
      "SliderAngle",
      sol::overload(
          sol::resolve<std::tuple<float, bool>(const std::string &, float)>(
              SliderAngle),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float)>(SliderAngle),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float)>(SliderAngle),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, const std::string &)>(
              SliderAngle)));
  ImGui.set_function(
      "SliderInt",
      sol::overload(
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int,
                                             int)>(SliderInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int, int,
                                             const std::string &)>(SliderInt)));
  ImGui.set_function(
      "SliderInt2",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int, int)>(SliderInt2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int, int,
              const std::string &)>(SliderInt2)));
  ImGui.set_function(
      "SliderInt3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int, int)>(SliderInt3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int, int,
              const std::string &)>(SliderInt3)));
  ImGui.set_function(
      "SliderInt4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int, int)>(SliderInt4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int, int,
              const std::string &)>(SliderInt4)));
  ImGui.set_function(
      "VSliderFloat",
      sol::overload(
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float, float, float)>(
              VSliderFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, float, float,
              const std::string &)>(VSliderFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, float, float,
              const std::string &, int)>(VSliderFloat)));
  ImGui.set_function(
      "VSliderInt",
      sol::overload(
          sol::resolve<std::tuple<int, bool>(const std::string &, float, float,
                                             int, int, int)>(VSliderInt),
          sol::resolve<std::tuple<int, bool>(
              const std::string &, float, float, int, int, int,
              const std::string &)>(VSliderInt)));
#pragma endregion Widgets : Sliders

#pragma region Widgets : Inputs using Keyboard
  ImGui.set_function(
      "InputText",
      sol::overload(
          sol::resolve<std::tuple<std::string, bool>(
              const std::string &, std::string, unsigned int)>(InputText),
          sol::resolve<std::tuple<std::string, bool>(
              const std::string &, std::string, unsigned int, int)>(
              InputText)));
  ImGui.set_function(
      "InputTextMultiline",
      sol::overload(
          sol::resolve<std::tuple<std::string, bool>(
              const std::string &, std::string, unsigned int)>(
              InputTextMultiline),
          sol::resolve<std::tuple<std::string, bool>(
              const std::string &, std::string, unsigned int, float, float)>(
              InputTextMultiline),
          sol::resolve<std::tuple<std::string, bool>(
              const std::string &, std::string, unsigned int, float, float,
              int)>(InputTextMultiline)));
  ImGui.set_function(
      "InputTextWithHint",
      sol::overload(sol::resolve<std::tuple<std::string, bool>(
                        const std::string &, const std::string &, std::string,
                        unsigned int)>(InputTextWithHint),
                    sol::resolve<std::tuple<std::string, bool>(
                        const std::string &, const std::string &, std::string,
                        unsigned int, int)>(InputTextWithHint)));
  ImGui.set_function(
      "InputFloat",
      sol::overload(
          sol::resolve<std::tuple<float, bool>(const std::string &, float)>(
              InputFloat),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float)>(InputFloat),
          sol::resolve<std::tuple<float, bool>(const std::string &, float,
                                               float, float)>(InputFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, const std::string &)>(
              InputFloat),
          sol::resolve<std::tuple<float, bool>(
              const std::string &, float, float, float, const std::string &,
              int)>(InputFloat)));
  ImGui.set_function(
      "InputFloat2",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(InputFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, const std::string &)>(
              InputFloat2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, const std::string &,
              int)>(InputFloat2)));
  ImGui.set_function(
      "InputFloat3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(InputFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, const std::string &)>(
              InputFloat3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, const std::string &,
              int)>(InputFloat3)));
  ImGui.set_function(
      "InputFloat4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(InputFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, const std::string &)>(
              InputFloat4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, const std::string &,
              int)>(InputFloat4)));
  ImGui.set_function(
      "InputInt",
      sol::overload(
          sol::resolve<std::tuple<int, bool>(const std::string &, int)>(
              InputInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int)>(
              InputInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int,
                                             int)>(InputInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int,
                                             int)>(InputInt),
          sol::resolve<std::tuple<int, bool>(const std::string &, int, int, int,
                                             int)>(InputInt)));
  ImGui.set_function(
      "InputInt2",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &)>(InputInt2),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int)>(InputInt2)));
  ImGui.set_function(
      "InputInt3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &)>(InputInt3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int)>(InputInt3)));
  ImGui.set_function(
      "InputInt4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &)>(InputInt4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<int>>, bool>(
              const std::string &, const sol::table &, int)>(InputInt4)));
  ImGui.set_function(
      "InputDouble",
      sol::overload(
          sol::resolve<std::tuple<double, bool>(const std::string &, double)>(
              InputDouble),
          sol::resolve<std::tuple<double, bool>(const std::string &, double,
                                                double)>(InputDouble),
          sol::resolve<std::tuple<double, bool>(const std::string &, double,
                                                double, double)>(InputDouble),
          sol::resolve<std::tuple<double, bool>(
              const std::string &, double, double, double,
              const std::string &)>(InputDouble),
          sol::resolve<std::tuple<double, bool>(
              const std::string &, double, double, double, const std::string &,
              int)>(InputDouble)));
#pragma endregion Widgets : Inputs using Keyboard

#pragma region Widgets : Color Editor / Picker
  ImGui.set_function(
      "ColorEdit3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(ColorEdit3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, int)>(ColorEdit3)));
  ImGui.set_function(
      "ColorEdit4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(ColorEdit4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, int)>(ColorEdit4)));
  ImGui.set_function(
      "ColorPicker3",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(ColorPicker3),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, int)>(ColorPicker3)));
  ImGui.set_function(
      "ColorPicker4",
      sol::overload(
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &)>(ColorPicker4),
          sol::resolve<std::tuple<sol::as_table_t<std::vector<float>>, bool>(
              const std::string &, const sol::table &, int)>(ColorPicker4)));
#pragma endregion Widgets : Color Editor / Picker

#pragma region Widgets : Trees
  ImGui.set_function(
      "TreeNode",
      sol::overload(
          sol::resolve<bool(const std::string &)>(TreeNode),
          sol::resolve<bool(const std::string &, const std::string &)>(
              TreeNode)));
  ImGui.set_function(
      "TreeNodeEx",
      sol::overload(
          sol::resolve<bool(const std::string &)>(TreeNodeEx),
          sol::resolve<bool(const std::string &, int)>(TreeNodeEx),
          sol::resolve<bool(const std::string &, int, const std::string &)>(
              TreeNodeEx)));
  ImGui.set_function("TreePush", TreePush);
  ImGui.set_function("GetTreeNodeToLabelSpacing", GetTreeNodeToLabelSpacing);
  ImGui.set_function(
      "CollapsingHeader",
      sol::overload(
          sol::resolve<bool(const std::string &)>(CollapsingHeader),
          sol::resolve<bool(const std::string &, int)>(CollapsingHeader),
          sol::resolve<std::tuple<bool, bool>(const std::string &, bool)>(
              CollapsingHeader),
          sol::resolve<std::tuple<bool, bool>(const std::string &, bool, int)>(
              CollapsingHeader)));
  ImGui.set_function(
      "SetNextItemOpen",
      sol::overload(sol::resolve<void(bool)>(SetNextItemOpen),
                    sol::resolve<void(bool, int)>(SetNextItemOpen)));
#pragma endregion Widgets : Trees

#pragma region Widgets : Selectables
  ImGui.set_function(
      "Selectable",
      sol::overload(
          sol::resolve<bool(const std::string &)>(Selectable),
          sol::resolve<bool(const std::string &, bool)>(Selectable),
          sol::resolve<bool(const std::string &, bool, int)>(Selectable),
          sol::resolve<bool(const std::string &, bool, int, float, float)>(
              Selectable)));
#pragma endregion Widgets : Selectables

#pragma region Widgets : List Boxes
  ImGui.set_function(
      "ListBox",
      sol::overload(
          sol::resolve<std::tuple<int, bool>(const std::string &, int,
                                             const sol::table &, int)>(ListBox),
          sol::resolve<std::tuple<int, bool>(const std::string &, int,
                                             const sol::table &, int, int)>(
              ListBox)));
  ImGui.set_function(
      "ListBoxHeader",
      sol::overload(
          sol::resolve<bool(const std::string &, float, float)>(ListBoxHeader),
          sol::resolve<bool(const std::string &, int)>(ListBoxHeader),
          sol::resolve<bool(const std::string &, int, int)>(ListBoxHeader)));
  ImGui.set_function("ListBoxFooter", ListBoxFooter);
#pragma endregion Widgets : List Boxes

#pragma region Widgets : Value() Helpers
  ImGui.set_function(
      "Value",
      sol::overload(
          sol::resolve<void(const std::string &, bool)>(Value),
          sol::resolve<void(const std::string &, int)>(Value),
          sol::resolve<void(const std::string &, unsigned int)>(Value),
          sol::resolve<void(const std::string &, float)>(Value),
          sol::resolve<void(const std::string &, float, const std::string &)>(
              Value)));
#pragma endregion Widgets : Value() Helpers

#pragma region Widgets : Menu
  ImGui.set_function("BeginMenuBar", BeginMenuBar);
  ImGui.set_function("EndMenuBar", EndMenuBar);
  ImGui.set_function("BeginMainMenuBar", BeginMainMenuBar);
  ImGui.set_function("EndMainMenuBar", EndMainMenuBar);
  ImGui.set_function(
      "BeginMenu",
      sol::overload(sol::resolve<bool(const std::string &)>(BeginMenu),
                    sol::resolve<bool(const std::string &, bool)>(BeginMenu)));
  ImGui.set_function("EndMenu", EndMenu);
  ImGui.set_function(
      "MenuItem",
      sol::overload(
          sol::resolve<bool(const std::string &)>(MenuItem),
          sol::resolve<bool(const std::string &, const std::string &)>(
              MenuItem),
          sol::resolve<std::tuple<bool, bool>(
              const std::string &, const std::string &, bool)>(MenuItem),
          sol::resolve<std::tuple<bool, bool>(const std::string &,
                                              const std::string &, bool, bool)>(
              MenuItem)));
#pragma endregion Widgets : Menu

#pragma region Tooltips
  ImGui.set_function("BeginTooltip", BeginTooltip);
  ImGui.set_function("EndTooltip", EndTooltip);
  ImGui.set_function("SetTooltip", SetTooltip);
#pragma endregion Tooltips

#pragma region Popups, Modals
  ImGui.set_function(
      "BeginPopup",
      sol::overload(sol::resolve<bool(const std::string &)>(BeginPopup),
                    sol::resolve<bool(const std::string &, int)>(BeginPopup)));
  ImGui.set_function(
      "BeginPopupModal",
      sol::overload(
          sol::resolve<bool(const std::string &)>(BeginPopupModal),
          sol::resolve<bool(const std::string &, bool)>(BeginPopupModal),
          sol::resolve<bool(const std::string &, bool, int)>(BeginPopupModal)));
  ImGui.set_function("EndPopup", EndPopup);
  ImGui.set_function(
      "OpenPopup",
      sol::overload(sol::resolve<void(const std::string &)>(OpenPopup),
                    sol::resolve<void(const std::string &, int)>(OpenPopup)));
  // Deprecated, use OpenPopupOnItemClick instead.
  // ImGui.set_function("OpenPopupContextItem"			, sol::overload(
  //														sol::resolve<void()>(OpenPopupContextItem),
  //														sol::resolve<void(const
  //std::string&)>(OpenPopupContextItem), 														sol::resolve<void(const std::string&,
  //int)>(OpenPopupContextItem)
  //													));
  ImGui.set_function(
      "OpenPopupOnItemClick",
      sol::overload(
          sol::resolve<void()>(OpenPopupOnItemClick),
          sol::resolve<void(const std::string &)>(OpenPopupOnItemClick),
          sol::resolve<void(const std::string &, int)>(OpenPopupOnItemClick)));
  ImGui.set_function("CloseCurrentPopup", CloseCurrentPopup);
  ImGui.set_function(
      "BeginPopupContextItem",
      sol::overload(
          sol::resolve<bool()>(BeginPopupContextItem),
          sol::resolve<bool(const std::string &)>(BeginPopupContextItem),
          sol::resolve<bool(const std::string &, int)>(BeginPopupContextItem)));
  ImGui.set_function(
      "BeginPopupContextWindow",
      sol::overload(
          sol::resolve<bool()>(BeginPopupContextWindow),
          sol::resolve<bool(const std::string &)>(BeginPopupContextWindow),
          sol::resolve<bool(const std::string &, int)>(
              BeginPopupContextWindow)));
  ImGui.set_function(
      "BeginPopupContextVoid",
      sol::overload(
          sol::resolve<bool()>(BeginPopupContextVoid),
          sol::resolve<bool(const std::string &)>(BeginPopupContextVoid),
          sol::resolve<bool(const std::string &, int)>(BeginPopupContextVoid)));
  ImGui.set_function(
      "IsPopupOpen",
      sol::overload(sol::resolve<bool(const std::string &)>(IsPopupOpen),
                    sol::resolve<bool(const std::string &, int)>(IsPopupOpen)));
#pragma endregion Popups, Modals

#pragma region Columns
  ImGui.set_function(
      "Columns",
      sol::overload(
          sol::resolve<void()>(Columns), sol::resolve<void(int)>(Columns),
          sol::resolve<void(int, const std::string &)>(Columns),
          sol::resolve<void(int, const std::string &, bool)>(Columns)));
  ImGui.set_function("NextColumn", NextColumn);
  ImGui.set_function("GetColumnIndex", GetColumnIndex);
  ImGui.set_function("GetColumnWidth",
                     sol::overload(sol::resolve<float()>(GetColumnWidth),
                                   sol::resolve<float(int)>(GetColumnWidth)));
  ImGui.set_function("SetColumnWidth", SetColumnWidth);
  ImGui.set_function("GetColumnOffset",
                     sol::overload(sol::resolve<float()>(GetColumnOffset),
                                   sol::resolve<float(int)>(GetColumnOffset)));
  ImGui.set_function("SetColumnOffset", SetColumnOffset);
  ImGui.set_function("GetColumnsCount", GetColumnsCount);
#pragma endregion Columns

#pragma region Tab Bars, Tabs
  ImGui.set_function(
      "BeginTabBar",
      sol::overload(sol::resolve<bool(const std::string &)>(BeginTabBar),
                    sol::resolve<bool(const std::string &, int)>(BeginTabBar)));
  ImGui.set_function("EndTabBar", EndTabBar);
  ImGui.set_function(
      "BeginTabItem",
      sol::overload(
          sol::resolve<bool(const std::string &)>(BeginTabItem),
          sol::resolve<std::tuple<bool, bool>(const std::string &, bool)>(
              BeginTabItem),
          sol::resolve<std::tuple<bool, bool>(const std::string &, bool, int)>(
              BeginTabItem)));
  ImGui.set_function("EndTabItem", EndTabItem);
  ImGui.set_function("SetTabItemClosed", SetTabItemClosed);
#pragma endregion Tab Bars, Tabs

#pragma region Docking
  ImGui.set_function(
      "DockSpace",
      sol::overload(
          sol::resolve<void(unsigned int)>(DockSpace),
          sol::resolve<void(unsigned int, float, float)>(DockSpace),
          sol::resolve<void(unsigned int, float, float, int)>(DockSpace)));
  ImGui.set_function(
      "SetNextWindowDockID",
      sol::overload(
          sol::resolve<void(unsigned int)>(SetNextWindowDockID),
          sol::resolve<void(unsigned int, int)>(SetNextWindowDockID)));
  ImGui.set_function("GetWindowDockID", GetWindowDockID);
  ImGui.set_function("IsWindowDocked", IsWindowDocked);
#pragma endregion Docking

#pragma region Logging / Capture
  ImGui.set_function("LogToTTY",
                     sol::overload(sol::resolve<void()>(LogToTTY),
                                   sol::resolve<void(int)>(LogToTTY)));
  ImGui.set_function(
      "LogToFile",
      sol::overload(sol::resolve<void(int)>(LogToFile),
                    sol::resolve<void(int, const std::string &)>(LogToFile)));
  ImGui.set_function("LogToClipboard",
                     sol::overload(sol::resolve<void()>(LogToClipboard),
                                   sol::resolve<void(int)>(LogToClipboard)));
  ImGui.set_function("LogFinish", LogFinish);
  ImGui.set_function("LogButtons", LogButtons);
  ImGui.set_function("LogText", LogText);
#pragma endregion Logging / Capture

#pragma region Clipping
  ImGui.set_function("PushClipRect", PushClipRect);
  ImGui.set_function("PopClipRect", PopClipRect);
#pragma endregion Clipping

#pragma region Focus, Activation
  ImGui.set_function("SetItemDefaultFocus", SetItemDefaultFocus);
  ImGui.set_function(
      "SetKeyboardFocusHere",
      sol::overload(sol::resolve<void()>(SetKeyboardFocusHere),
                    sol::resolve<void(int)>(SetKeyboardFocusHere)));
#pragma endregion Focus, Activation

#pragma region Item / Widgets Utilities
  ImGui.set_function("IsItemHovered",
                     sol::overload(sol::resolve<bool()>(IsItemHovered),
                                   sol::resolve<bool(int)>(IsItemHovered)));
  ImGui.set_function("IsItemActive", IsItemActive);
  ImGui.set_function("IsItemFocused", IsItemFocused);
  ImGui.set_function("IsItemClicked",
                     sol::overload(sol::resolve<bool()>(IsItemClicked),
                                   sol::resolve<bool(int)>(IsItemClicked)));
  ImGui.set_function("IsItemVisible", IsItemVisible);
  ImGui.set_function("IsItemEdited", IsItemEdited);
  ImGui.set_function("IsItemActivated", IsItemActivated);
  ImGui.set_function("IsItemDeactivated", IsItemDeactivated);
  ImGui.set_function("IsItemDeactivatedAfterEdit", IsItemDeactivatedAfterEdit);
  ImGui.set_function("IsItemToggledOpen", IsItemToggledOpen);
  ImGui.set_function("IsAnyItemHovered", IsAnyItemHovered);
  ImGui.set_function("IsAnyItemActive", IsAnyItemActive);
  ImGui.set_function("IsAnyItemFocused", IsAnyItemFocused);
  ImGui.set_function("GetItemRectMin", GetItemRectMin);
  ImGui.set_function("GetItemRectMax", GetItemRectMax);
  ImGui.set_function("GetItemRectSize", GetItemRectSize);
  ImGui.set_function("SetItemAllowOverlap", SetItemAllowOverlap);
#pragma endregion Item / Widgets Utilities

#pragma region Miscellaneous Utilities
  ImGui.set_function(
      "IsRectVisible",
      sol::overload(
          sol::resolve<bool(float, float)>(IsRectVisible),
          sol::resolve<bool(float, float, float, float)>(IsRectVisible)));
  ImGui.set_function("GetTime", GetTime);
  ImGui.set_function("GetFrameCount", GetFrameCount);
  ImGui.set_function("GetStyleColorName", GetStyleColorName);
  ImGui.set_function(
      "BeginChildFrame",
      sol::overload(
          sol::resolve<bool(unsigned int, float, float)>(BeginChildFrame),
          sol::resolve<bool(unsigned int, float, float, int)>(
              BeginChildFrame)));
  ImGui.set_function("EndChildFrame", EndChildFrame);
#pragma endregion Miscellaneous Utilities

#pragma region Text Utilities
  ImGui.set_function(
      "CalcTextSize",
      sol::overload(
          sol::resolve<std::tuple<float, float>(const std::string &)>(
              CalcTextSize),
          sol::resolve<std::tuple<float, float>(
              const std::string &, const std::string &)>(CalcTextSize),
          sol::resolve<std::tuple<float, float>(
              const std::string &, const std::string &, bool)>(CalcTextSize),
          sol::resolve<std::tuple<float, float>(
              const std::string &, const std::string &, bool, float)>(
              CalcTextSize)));
#pragma endregion Text Utilities

#pragma region Color Utilities
#ifdef SOL_IMGUI_USE_COLOR_U32
  ImGui.set_function("ColorConvertU32ToFloat4", ColorConvertU32ToFloat4);
  ImGui.set_function("ColorConvertFloat4ToU32", ColorConvertFloat4ToU32);
#endif
  ImGui.set_function("ColorConvertRGBtoHSV", ColorConvertRGBtoHSV);
  ImGui.set_function("ColorConvertHSVtoRGB", ColorConvertHSVtoRGB);
#pragma endregion Color Utilities

#pragma region Inputs Utilities : Keyboard
  ImGui.set_function("GetKeyIndex", GetKeyIndex);
  ImGui.set_function("IsKeyDown", IsKeyDown);
  ImGui.set_function(
      "IsKeyPressed",
      sol::overload(sol::resolve<bool(int)>(IsKeyPressed),
                    sol::resolve<bool(int, bool)>(IsKeyPressed)));
  ImGui.set_function("IsKeyReleased", IsKeyReleased);
  ImGui.set_function(
      "CaptureKeyboardFromApp",
      sol::overload(sol::resolve<void()>(CaptureKeyboardFromApp),
                    sol::resolve<void(bool)>(CaptureKeyboardFromApp)));
#pragma endregion Inputs Utilities : Keyboard

#pragma region Inputs Utilities : Mouse
  ImGui.set_function("IsMouseDown", IsMouseDown);
  ImGui.set_function(
      "IsMouseClicked",
      sol::overload(sol::resolve<bool(int)>(IsMouseClicked),
                    sol::resolve<bool(int, bool)>(IsMouseClicked)));
  ImGui.set_function("IsMouseReleased", IsMouseReleased);
  ImGui.set_function("IsMouseDoubleClicked", IsMouseDoubleClicked);
  ImGui.set_function(
      "IsMouseHoveringRect",
      sol::overload(
          sol::resolve<bool(float, float, float, float)>(IsMouseHoveringRect),
          sol::resolve<bool(float, float, float, float, bool)>(
              IsMouseHoveringRect)));
  ImGui.set_function("IsAnyMouseDown", IsAnyMouseDown);
  ImGui.set_function("GetMousePos", GetMousePos);
  ImGui.set_function("GetMousePosOnOpeningCurrentPopup",
                     GetMousePosOnOpeningCurrentPopup);
  ImGui.set_function(
      "IsMouseDragging",
      sol::overload(sol::resolve<bool(int)>(IsMouseDragging),
                    sol::resolve<bool(int, float)>(IsMouseDragging)));
  ImGui.set_function(
      "GetMouseDragDelta",
      sol::overload(
          sol::resolve<std::tuple<float, float>()>(GetMouseDragDelta),
          sol::resolve<std::tuple<float, float>(int)>(GetMouseDragDelta),
          sol::resolve<std::tuple<float, float>(int, float)>(
              GetMouseDragDelta)));
  ImGui.set_function(
      "ResetMouseDragDelta",
      sol::overload(sol::resolve<void()>(ResetMouseDragDelta),
                    sol::resolve<void(int)>(ResetMouseDragDelta)));
  ImGui.set_function("GetMouseCursor", GetMouseCursor);
  ImGui.set_function("SetMouseCursor", SetMouseCursor);
  ImGui.set_function(
      "CaptureMouseFromApp",
      sol::overload(sol::resolve<void()>(CaptureMouseFromApp),
                    sol::resolve<void(bool)>(CaptureMouseFromApp)));
#pragma endregion Inputs Utilities : Mouse

#pragma region Clipboard Utilities
  ImGui.set_function("GetClipboardText", GetClipboardText);
  ImGui.set_function("SetClipboardText", SetClipboardText);
#pragma endregion Clipboard Utilities
}
} // namespace sol_ImGui
