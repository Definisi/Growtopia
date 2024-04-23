#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <math.h>

#include <d3dx9.h>
#include <d3d9.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

namespace blur {

    inline IDirect3DDevice9* device;
}

extern void draw_blur( ImDrawList* drawList );
