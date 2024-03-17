#pragma once
#include <memory>
#include <vector>

#include <d3d9.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx9.h>
#include <imgui/imgui_impl_win32.h>

namespace gui { 
	using tab_callback = void (*)();
	inline std::vector<const char*> tab_list = { "Main" };
	
	void render_tabs_from_vector();
	void render_tab_item(const char* label, tab_callback callback);
	void tab_main();
	inline std::vector<tab_callback> tab_callbacks = { tab_main };

	// constant window size
	constexpr int width = 500;
	constexpr int height = 300;

	// when this changes, exit threads
	// and close menu :)
	inline bool is_running = true;

	// winapi window vars
	inline HWND window = nullptr;
	inline WNDCLASSEX window_class = { };

	// points for window movement
	inline POINTS position = { };

	// direct x state vars
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS present_parameters = { };

	// handle window creation & destruction
	void create_hwindow(const char* window_name, const char* class_name) noexcept;
	void destroy_hwindow() noexcept;

	// handle device creation & destruction
	bool create_device() noexcept;
	void reset_device() noexcept;
	void destroy_device() noexcept;

	// handle ImGui creation & destruction
	void create_imgui() noexcept;
	void destroy_imgui() noexcept;

	void begin_render() noexcept;
	void end_render() noexcept;

	void create(const char* window_name, const char* class_name) noexcept;
	void destroy() noexcept;
}
