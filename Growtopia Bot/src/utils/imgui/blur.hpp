#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

#ifdef _WIN32
#include <d3d9.h>
// #include <d3dx9.h> // DirectX 9 SDK header - commented out if not available
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

namespace blur {
#ifdef _WIN32
    inline IDirect3DDevice9* device;
#endif
}

#ifdef _WIN32
extern void draw_blur( ImDrawList* drawList );
#else
inline void draw_blur( ImDrawList* drawList ) { /* DirectX 9 not available */ }
#endif
