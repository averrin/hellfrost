#ifndef __IMGUISTL_H_
#define __IMGUISTL_H_
#include <imgui.h>
#include <string>
#include <vector>

namespace ImGui
{

bool Combo(const char* label, int* currIndex, std::vector<std::string>& values);
bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values);
bool InputText(const char* label, std::string& str, size_t maxInputSize = 255,
    ImGuiInputTextFlags flags = 0, ImGuiTextEditCallback callback = 0, void* user_data = 0);
} // end of namespace ImGui

#endif // __IMGUI-STL_H_
