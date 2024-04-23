#pragma once

#include <d3d9.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_texteditor.hpp>

#include <chrono>
#include <thread>
#include <Windows.h>

namespace Gui {

	bool instance = true;
	bool show_gui = true;

	HWND window = nullptr;
	WNDCLASSEX window_class{ 0 };

	PDIRECT3D9 d3d;
	LPDIRECT3DDEVICE9 device;
	D3DPRESENT_PARAMETERS present_parameters{ 0 };

	MSG message{ 0 };

	TextEditor editor;
	static const char* file_to_edit = "main.lua";
	TextEditor::Coordinates cpos = {};


	std::string username;
	std::string password;
	std::string proxy;
	std::string world_name;

	void create_hwindow(LPCWSTR windowName);
	void destroy_hwindow();

	bool create_device();
	void reset_device();
	void destroy_device();

	void save_file();

	void create_imgui();
	void destroy_imgui();

	bool begin_render();
	void style();
	void end_render();
};

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND window, UINT message, WPARAM wideParameter, LPARAM longParameter);
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg) {
	case WM_SIZE:
		if (Gui::device != 0 && wParam != SIZE_MINIMIZED) {
			Gui::present_parameters.BackBufferWidth = LOWORD(lParam);
			Gui::present_parameters.BackBufferHeight = HIWORD(lParam);
			Gui::reset_device();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



void Gui::create_hwindow(LPCWSTR windowName) {

	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_CLASSDC;
	window_class.lpfnWndProc = WndProc;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = GetModuleHandleA(0);
	window_class.hIcon = 0;
	window_class.hCursor = 0;
	window_class.hbrBackground = 0;
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = windowName;
	window_class.hIconSm = 0;
	RegisterClassEx(&window_class);
	window = CreateWindow(window_class.lpszClassName, window_class.lpszClassName, WS_POPUP, 0, 0, 5, 5, 0, 0, window_class.hInstance, 0);
}

void Gui::destroy_hwindow() {
	DestroyWindow(window);
	UnregisterClass(window_class.lpszClassName, window_class.hInstance);
}




bool Gui::create_device() {
	if (!(d3d = Direct3DCreate9(D3D_SDK_VERSION)))
		return false;

	present_parameters.Windowed = TRUE;
	present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
	present_parameters.EnableAutoDepthStencil = TRUE;
	present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
	present_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &present_parameters, &device) < 0)
		return false;

	ShowWindow(window, SW_HIDE);
	UpdateWindow(window);
	return true;
}

void Gui::reset_device() {
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT result = device->Reset(&present_parameters);
	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Gui::destroy_device() {
	if (device) {
		device->Release();
		device = nullptr;
	}

	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}

void Gui::save_file()
{
	std::ofstream file(file_to_edit);
	if (file.is_open()) {
		file << editor.GetText();
		file.close();
	}
}



void Gui::create_imgui() {
	ImGui::CreateContext();
	ImGui::StyleColorsDark();


	//ImFontConfig font_cfg;
	//font_cfg.FontDataOwnedByAtlas = false;
	//font_cfg.MergeMode = true;
	//font_cfg.PixelSnapH = true;

	ImGuiIO& io = ::ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.IniFilename = nullptr;
	//io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 13.f);

	
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
	auto lang = TextEditor::LanguageDefinition::Lua();
	static const char* const general_identifiers[] = {
			"register_event", "listen_events", "run_thread", "sleep", "unlisten_events"
	};
	for (auto& k : general_identifiers)
	{
		TextEditor::Identifier id;
		id.mDeclaration = "Speedy general function";
		lang.mIdentifiers.insert(std::make_pair(std::string(k), id));
	}
	
	static const char* const bot_identifiers[] = {
			"connect", "get_peer", "get_inventory", "get_player", "get_world", "find_path",
			"place", "send_packet", "move", "punch", "warp", "wear", "wrench", "get_items",
			"get_item", "get_tiles", "get_tile", "get_floating_items", "send_packet_raw",
			"collect", "auto_collect"
	};
	
	for (auto& k : bot_identifiers)
	{
		TextEditor::Identifier id;
		id.mDeclaration = "Speedy bot function";
		lang.mIdentifiers.insert(std::make_pair(std::string(k), id));
	}
	editor.SetLanguageDefinition(lang);
	{
		std::ifstream t(file_to_edit);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			editor.SetText(str);
		}
	}
}

void Gui::destroy_imgui() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}




bool Gui::begin_render() {
	cpos = editor.GetCursorPosition();
	if (PeekMessage(&message, 0, 0U, 0U, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
		return true;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	return false;
}

void Gui::style() {
	ImVec4* colors = ImGui::GetStyle().Colors;

	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);

	ImGui::GetStyle().FrameRounding = 5.0f;
	ImGui::GetStyle().GrabRounding = 4.0f;
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
}

struct Color {
	int r, g, b, a;
};

struct MessagePart {
	std::string text;
	Color color;
};


void Gui::end_render() {
	ImGui::EndFrame();

	//device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	// Using multi viewports
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();

	if (device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		reset_device();
}
