//    Copyright The asap Project Authors 2018.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

#include <array>
#include <cstring>
#include <fstream>
#include <map>
#include <mutex> // for call_once()

#include <imgui.h>

// #include <common/logging.h>
// #include <config.h>
#include <app/fonts/fonts.h>
#include <app/fonts/material_design_icons.h>
#include <app/style/theme.h>

// namespace asap {
// namespace ui {

std::string const Font::FAMILY_MONOSPACE{"Inconsolata"};
std::string const Font::FAMILY_PROPORTIONAL{"Roboto"};

std::map<std::string, ImFont *> Theme::fonts_;
ImFont *Theme::icons_font_normal_{nullptr};

namespace {

std::string BuildFontName(std::string const &family, Font::Weight weight,
                          Font::Style style, Font::Size size) {
  std::string name(family);
  name.append(" ").append(Font::WeightString(weight));
  if (style == Font::Style::ITALIC)
    name.append(" ").append(Font::StyleString(style));
  name.append(" ").append(Font::SizeString(size));
  return name;
}

/// Merge in icons from Font Material Design Icons font.
ImFont *MergeIcons(float size) {
  ImGuiIO &io = ImGui::GetIO();
  // The ranges array is not copied by the AddFont* functions and is used lazily
  // so ensure it is available for duration of font usage
  static const ImWchar icons_ranges[] = {ICON_MIN_MDI, ICON_MAX_MDI, 0};

  ImFontConfig fontConfig;
  // Set Oversamping parameters to 1 on both axis, the texture will be 6 times
  // smaller. See https://github.com/ocornut/imgui/issues/1527
  fontConfig.OversampleH = 1;
  fontConfig.OversampleV = 1;
  fontConfig.MergeMode = true;
  fontConfig.PixelSnapH = true;
  auto font = io.Fonts->AddFontFromMemoryCompressedTTF(
      Fonts::MATERIAL_DESIGN_ICONS_COMPRESSED_DATA,
      Fonts::MATERIAL_DESIGN_ICONS_COMPRESSED_SIZE, size, &fontConfig,
      icons_ranges);
  // use FONT_ICON_FILE_NAME_FAR if you want regular instead of solid

  return font;
}

ImFont *LoadRobotoFont(std::string const &name, Font::Weight weight,
                       Font::Style style, Font::Size size) {
  ImGuiIO &io = ImGui::GetIO();
  ImFontConfig fontConfig;
  // Set Oversamping parameters to 1 on both axis, the texture will be 6 times
  // smaller. See https://github.com/ocornut/imgui/issues/1527
  fontConfig.OversampleH = 1;
  fontConfig.OversampleV = 1;
  fontConfig.MergeMode = false;
  std::strncpy(fontConfig.Name, name.c_str(), sizeof(fontConfig.Name) - 1);
  fontConfig.Name[sizeof(fontConfig.Name) - 1] = 0;
  ImFont *font = nullptr;
  switch (weight) {
  case Font::Weight::LIGHT:
    switch (style) {
    case Font::Style::ITALIC:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::ROBOTO_LIGHTITALIC_COMPRESSED_DATA,
          Fonts::ROBOTO_LIGHTITALIC_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig, io.Fonts->GetGlyphRangesDefault());
      font = MergeIcons(Font::SizeFloat(size));
      break;
    case Font::Style::NORMAL:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::ROBOTO_LIGHT_COMPRESSED_DATA,
          Fonts::ROBOTO_LIGHT_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig, io.Fonts->GetGlyphRangesDefault());
      font = MergeIcons(Font::SizeFloat(size));
      break;
    }
    break;
  case Font::Weight::REGULAR:
    switch (style) {
    case Font::Style::ITALIC:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::ROBOTO_ITALIC_COMPRESSED_DATA,
          Fonts::ROBOTO_ITALIC_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig, io.Fonts->GetGlyphRangesDefault());
      font = MergeIcons(Font::SizeFloat(size));
      break;
    case Font::Style::NORMAL:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::ROBOTO_REGULAR_COMPRESSED_DATA,
          Fonts::ROBOTO_REGULAR_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig, io.Fonts->GetGlyphRangesDefault());
      font = MergeIcons(Font::SizeFloat(size));
      break;
    }
    break;
  case Font::Weight::BOLD:
    switch (style) {
    case Font::Style::ITALIC:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::ROBOTO_BOLDITALIC_COMPRESSED_DATA,
          Fonts::ROBOTO_BOLDITALIC_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig, io.Fonts->GetGlyphRangesDefault());
      font = MergeIcons(Font::SizeFloat(size));
      break;
    case Font::Style::NORMAL:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::ROBOTO_BOLD_COMPRESSED_DATA,
          Fonts::ROBOTO_BOLD_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig);
      font = MergeIcons(Font::SizeFloat(size));
      break;
    }
    break;
  }
  return font;
}

ImFont *LoadInconsolataFont(std::string const &name, Font::Weight weight,
                            Font::Style style, Font::Size size) {
  ImGuiIO &io = ImGui::GetIO();
  ImFontConfig fontConfig;
  // Set Oversamping parameters to 1 on both axis, the texture will be 6 times
  // smaller. See https://github.com/ocornut/imgui/issues/1527
  fontConfig.OversampleH = 1;
  fontConfig.OversampleV = 1;
  fontConfig.MergeMode = false;
  std::strncpy(fontConfig.Name, name.c_str(), sizeof(fontConfig.Name) - 1);
  fontConfig.Name[sizeof(fontConfig.Name) - 1] = 0;
  ImFont *font = nullptr;
  switch (weight) {
  case Font::Weight::LIGHT:
    break;
  case Font::Weight::REGULAR:
    switch (style) {
    case Font::Style::ITALIC:
    case Font::Style::NORMAL:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::INCONSOLATA_REGULAR_COMPRESSED_DATA,
          Fonts::INCONSOLATA_REGULAR_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig);
      font = MergeIcons(Font::SizeFloat(size));
      break;
    }
    break;
  case Font::Weight::BOLD:
    switch (style) {
    case Font::Style::ITALIC:
    case Font::Style::NORMAL:
      io.Fonts->AddFontFromMemoryCompressedTTF(
          Fonts::INCONSOLATA_BOLD_COMPRESSED_DATA,
          Fonts::INCONSOLATA_BOLD_COMPRESSED_SIZE, Font::SizeFloat(size),
          &fontConfig);
      font = MergeIcons(Font::SizeFloat(size));
      break;
    }
    break;
  }
  return font;
}

ImFont *LoadIconsFont(float size) {
  ImGuiIO &io = ImGui::GetIO();
  ImFontConfig fontConfig;
  // Set Oversamping parameters to 1 on both axis, the texture will be 6 times
  // smaller. See https://github.com/ocornut/imgui/issues/1527
  fontConfig.OversampleH = 1;
  fontConfig.OversampleV = 1;
  fontConfig.MergeMode = false;
  std::strncpy(fontConfig.Name, "Material Design Icons",
               sizeof(fontConfig.Name) - 1);
  fontConfig.Name[sizeof(fontConfig.Name) - 1] = 0;
  ImFont *font = nullptr;
  font = io.Fonts->AddFontFromMemoryCompressedTTF(
      Fonts::MATERIAL_DESIGN_ICONS_COMPRESSED_DATA,
      Fonts::MATERIAL_DESIGN_ICONS_COMPRESSED_SIZE, size, &fontConfig);
  return font;
}

} // namespace

Font::Font(std::string family) : family_(std::move(family)) { InitFont(); }

void Font::InitFont() {
  BuildName();
  font_ = Theme::GetFont(name_);
  name_.assign(font_->GetDebugName());
}

Font::Font(Font const &other)
    : font_(other.font_), size_(other.size_), style_(other.style_),
      weight_(other.weight_), name_(other.name_) {}

Font &Font::operator=(Font const &rhs) {
  font_ = rhs.font_;
  size_ = rhs.size_;
  style_ = rhs.style_;
  weight_ = rhs.weight_;
  name_ = rhs.name_;
  return *this;
}
Font::Font(Font &&moved) noexcept
    : font_(moved.font_), size_(moved.size_), style_(moved.style_),
      weight_(moved.weight_), name_(std::move(moved.name_)) {
  moved.font_ = nullptr;
}
Font &Font::operator=(Font &&moved) noexcept {
  auto tmp = std::move(moved);
  swap(tmp);
  return *this;
}
void Font::swap(Font &other) {
  std::swap(font_, other.font_);
  std::swap(size_, other.size_);
  std::swap(style_, other.style_);
  std::swap(weight_, other.weight_);
  name_.swap(other.name_);
}

void Font::BuildName() {
  name_ = BuildFontName(family_, weight_, style_, size_);
}

Font &Font::SmallSize() {
  size_ = Size::SMALL;
  InitFont();
  return *this;
}
Font &Font::MediumSize() {
  size_ = Size::MEDIUM;
  InitFont();
  return *this;
}
Font &Font::LargeSize() {
  size_ = Size::LARGE;
  InitFont();
  return *this;
}
Font &Font::LargerSize() {
  size_ = Size::LARGER;
  InitFont();
  return *this;
}

Font &Font::Normal() {
  style_ = Style::NORMAL;
  InitFont();
  return *this;
}

Font &Font::Italic() {
  style_ = Style::ITALIC;
  InitFont();
  return *this;
}

Font &Font::Light() {
  weight_ = Weight::LIGHT;
  InitFont();
  return *this;
}
Font &Font::Regular() {
  weight_ = Weight::REGULAR;
  InitFont();
  return *this;
}
Font &Font::Bold() {
  weight_ = Weight::BOLD;
  InitFont();
  return *this;
}

float Font::SizeFloat(Font::Size size) {
  auto val = static_cast<typename std::underlying_type<Font::Size>::type>(size);
  return static_cast<float>(val);
}
char const *Font::SizeString(Font::Size size) {
  switch (size) {
  case Size::SMALL:
    return "11px";
  case Size::MEDIUM:
    return "14px";
  case Size::LARGE:
    return "16px";
  case Size::LARGER:
    return "24px";
  }
  // Only needed for compilers that complain about not all control paths
  // return a value.
  return "__NEVER__";
}

char const *Font::StyleString(Font::Style style) {
  switch (style) {
  case Style::NORMAL:
    return "Normal";
  case Style::ITALIC:
    return "Italic";
  }
  // Only needed for compilers that complain about not all control paths
  // return a value.
  return "__NEVER__";
}

char const *Font::WeightString(Font::Weight weight) {
  switch (weight) {
  case Weight::LIGHT:
    return "Light";
  case Weight::REGULAR:
    return "Regular";
  case Weight::BOLD:
    return "Bold";
  }
  // Only needed for compilers that complain about not all control paths
  // return a value.
  return "__NEVER__";
}

void Theme::Init() {
  // LoadStyle();
  LoadDefaultStyle();

  //
  // Fonts
  //
  static std::once_flag init_flag;
  std::call_once(init_flag, []() {
    ImGui::GetIO().Fonts->AddFontDefault();
    LoadDefaultFonts();
    // TODO: Temporary default font - can be configured
    Font default_font(Font::FAMILY_PROPORTIONAL);
    default_font.Regular().MediumSize();
    ImGui::GetIO().FontDefault = default_font.ImGuiFont();
  });
}

void Theme::LoadDefaultFonts() {
  // There is currently an issue with too many fonts being loaded by ImGui that
  // cause a problems on certain platforms.
  // To decrease the chances of people running into this problem (usually a
  // blank screen for ImGui), we are only loading a minimal number of
  // variations. On systems with pretty decent 3D graphics, all variations can
  // be loaded with no issues.

  std::array<Font::Weight, 2> font_weights{
      {Font::Weight::REGULAR, Font::Weight::BOLD}};
  // std::array<Font::Weight, 3> font_weights{
  //     {Font::Weight::LIGHT, Font::Weight::REGULAR, Font::Weight::BOLD}};
  std::array<Font::Style, 2> font_styles{
      {Font::Style::NORMAL, Font::Style::ITALIC}};
  std::array<Font::Size, 2> font_sizes{{Font::Size::SMALL, Font::Size::MEDIUM}};
  // std::array<Font::Size, 4> font_sizes{{Font::Size::SMALL,
  // Font::Size::MEDIUM,
  //                                       Font::Size::LARGE,
  //                                       Font::Size::LARGER}};
  for (auto size : font_sizes) {
    for (auto weight : font_weights) {
      for (auto style : font_styles) {
        auto name =
            BuildFontName(Font::FAMILY_PROPORTIONAL, weight, style, size);
        auto font = LoadRobotoFont(name, weight, style, size);
        if (font)
          AddFont(name, font);
      }
    }

    // Monospaced
    auto name = BuildFontName(Font::FAMILY_MONOSPACE, Font::Weight::REGULAR,
                              Font::Style::NORMAL, size);
    auto font = LoadInconsolataFont(name, Font::Weight::REGULAR,
                                    Font::Style::NORMAL, size);
    if (font) {
      AddFont(name, font);
      // No Italic
      AddFont(BuildFontName(Font::FAMILY_MONOSPACE, Font::Weight::REGULAR,
                            Font::Style::ITALIC, size),
              font);
      // Treat LIGHT same as REGULAR
      AddFont(BuildFontName(Font::FAMILY_MONOSPACE, Font::Weight::LIGHT,
                            Font::Style::NORMAL, size),
              font);
      AddFont(BuildFontName(Font::FAMILY_MONOSPACE, Font::Weight::LIGHT,
                            Font::Style::ITALIC, size),
              font);
    }

    name = BuildFontName(Font::FAMILY_MONOSPACE, Font::Weight::BOLD,
                         Font::Style::NORMAL, size);
    font = LoadInconsolataFont(name, Font::Weight::BOLD, Font::Style::NORMAL,
                               size);
    if (font) {
      AddFont(name, font);
      // No Italic
      AddFont(BuildFontName(Font::FAMILY_MONOSPACE, Font::Weight::BOLD,
                            Font::Style::ITALIC, size),
              font);
    }
  }

  // The Icons font
  icons_font_normal_ = LoadIconsFont(32.0f);
}

void Theme::LoadDefaultStyle() {
  auto &style = ImGui::GetStyle();

  style.WindowPadding = ImVec2(5, 5);
  style.WindowRounding = 0.0f;
  style.FramePadding = ImVec2(5, 5);
  style.FrameRounding = 3.0f;
  style.ItemSpacing = ImVec2(12, 5);
  style.ItemInnerSpacing = ImVec2(5, 5);
  style.IndentSpacing = 25.0f;
  style.ScrollbarSize = 15.0f;
  style.ScrollbarRounding = 9.0f;
  style.GrabMinSize = 5.0f;
  style.GrabRounding = 3.0f;

  // clang-format off
  style.Colors[ImGuiCol_Text] = ImVec4(0.91f, 0.91f, 0.91f, 1.00f);
  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
  style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.40f);
  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.75f, 0.42f, 0.02f, 0.67f);
  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 0.80f);
  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
  style.Colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
  style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.75f, 0.42f, 0.02f, 0.78f);
  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
  style.Colors[ImGuiCol_Button] = ImVec4(0.75f, 0.42f, 0.02f, 0.40f);
  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.94f, 0.47f, 0.02f, 1.00f);
  style.Colors[ImGuiCol_Header] = ImVec4(0.75f, 0.42f, 0.02f, 0.31f);
  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.80f);
  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
  style.Colors[ImGuiCol_Separator] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.78f);
  style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.75f, 0.42f, 0.02f, 1.00f);
  style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
  style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.75f, 0.42f, 0.02f, 0.67f);
  style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.75f, 0.42f, 0.02f, 0.95f);
  style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
  style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 0.57f, 0.65f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.10f, 0.30f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 0.40f, 1.00f, 1.00f);
  style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.75f, 0.42f, 0.02f, 0.35f);
  style.Colors[ImGuiCol_PopupBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.94f);
  style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.35f);
  // clang-format on
}

ImFont *Theme::GetFont(std::string const &name) {
  auto font = fonts_.find(name);
  if (font == fonts_.end()) {
    return ImGui::GetFont();
  }
  return font->second;
}

void Theme::AddFont(std::string const &name, ImFont *font) {
  fonts_.insert({name, font});
}

// }  // namespace ui
// }  // namespace asap
