#include <gui/gui.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wide_parameter,
	LPARAM long_parameter
);

LRESULT CALLBACK window_process(
	HWND window,
	UINT message,
	WPARAM wide_parameter,
	LPARAM long_parameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wide_parameter, long_parameter))
		return true;

	switch (message) {
	case WM_SIZE: {
		if (gui::device && wide_parameter != SIZE_MINIMIZED) {
			gui::present_parameters.BackBufferWidth = LOWORD(long_parameter);
			gui::present_parameters.BackBufferHeight = HIWORD(long_parameter);
			gui::reset_device();
		}
		return 0;
	}

	case WM_SYSCOMMAND: {
		if ((wide_parameter & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	}

	case WM_DESTROY: {
		PostQuitMessage(0);
		return 0;
	}

	case WM_LBUTTONDOWN: {
		gui::position = MAKEPOINTS(long_parameter); // set click points
		return 0;
	}

	case WM_MOUSEMOVE: {
		if (wide_parameter == MK_LBUTTON) {
			const auto points = MAKEPOINTS(long_parameter);
			auto rect = ::RECT{ };

			GetWindowRect(gui::window, &rect);

			rect.left += points.x - gui::position.x;
			rect.top += points.y - gui::position.y;

			if (gui::position.x >= 0 &&
				gui::position.x <= gui::width &&
				gui::position.y >= 0 && gui::position.y <= 19)
				SetWindowPos(
					gui::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}
		return 0;
	}
	}

	return DefWindowProc(window, message, wide_parameter, long_parameter);
}
/*
if (etype == LOGIN_TYPE_WINDOWS or etype == LOGIN_TYPE_UBISOFT) {
		constexpr std::array salts = {
			"e9fc40ec08f9ea6393f59c65e37f750aacddf68490c4f92d0d2523a5bc02ea63",
			"c85df9056ee603b849a93e1ebab5dd5f66e1fb8b2f4a8caef8d13b9f9e013fa4",
			"3ca373dffbf463bb337e0fd768a2f395b8e417475438916506c721551f32038d",
			"73eff5914c61a20a71ada81a6fc7780700fb1c0285659b4899bc172a24c14fc1"
		};

		static std::array constant_values = {
			SHA256(MD5(SHA256(std::to_string(protocol)))),
			SHA256(SHA256(version)),
			SHA256(SHA256(std::to_string(protocol)) + salts[3])
		};

		return SHA256(constant_values[0]
			+ salts[0]
			+ constant_values[1]
			+ salts[1]
			+ SHA256(MD5(SHA256(rid)))
			+ salts[2]
			+ constant_values[2]
		);
	}
	else {
		assert(false);
	}
*/ // bentar gw lupa file gw

void gui::render_tab_item(const char* label, tab_callback callback) {
	if (ImGui::BeginTabBar("##LabelTabList")) {
		if (ImGui::BeginTabItem(label)) {
			if (callback) {
				callback();
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

void gui::tab_main()
{
	ImGui::Text("Oke");
}

void gui::render_tabs_from_vector() {
	if (tab_list.size() != tab_callbacks.size())
		return;

	for (size_t i = 0; i < tab_list.size(); ++i) {
		render_tab_item(tab_list[i], tab_callbacks[i]);
	}
}



void gui::create_hwindow(const char* window_name, const char* class_name) noexcept {
	window_class.cbSize = sizeof(WNDCLASSEX);
	window_class.style = CS_CLASSDC;
	window_class.lpfnWndProc = window_process;
	window_class.cbClsExtra = 0;
	window_class.cbWndExtra = 0;
	window_class.hInstance = GetModuleHandleA(0);
	window_class.hIcon = 0;
	window_class.hCursor = 0;
	window_class.hbrBackground = 0;
	window_class.lpszMenuName = 0;
	window_class.lpszClassName = class_name;
	window_class.hIconSm = 0;

	RegisterClassEx(&window_class);

	window = CreateWindowEx(
		0,
		class_name,
		window_name,
		WS_POPUP,
		100,
		100,
		width,
		height,
		0,
		0,
		window_class.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void gui::destroy_hwindow() noexcept {
	DestroyWindow(window);
	UnregisterClass(window_class.lpszClassName, window_class.hInstance);
}

bool gui::create_device() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&present_parameters, sizeof(present_parameters));

	present_parameters.Windowed = TRUE;
	present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present_parameters.BackBufferFormat = D3DFMT_UNKNOWN;
	present_parameters.EnableAutoDepthStencil = TRUE;
	present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
	present_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&present_parameters,
		&device) < 0)
		return false;

	return true;
}

void gui::reset_device() noexcept {
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&present_parameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::destroy_device() noexcept {
	if (device) {
		device->Release();
		device = nullptr;
	}
	if (d3d) {
		d3d->Release();
		d3d = nullptr;
	}
}

void gui::create_imgui() noexcept {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();

	io.IniFilename = NULL;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void gui::destroy_imgui() noexcept {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void gui::begin_render() noexcept {
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
		if (message.message == WM_QUIT) {
			is_running = !is_running;
			return;
		}
	}

	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void gui::end_render() noexcept {
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	// Handle loss of D3D9 device
	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		reset_device();
}

void gui::create(const char* window_name, const char* class_name) noexcept {
	create_hwindow(window_name, class_name);
	create_device();
	create_imgui();
}

void gui::destroy() noexcept {
	destroy_imgui();
	destroy_device();
	destroy_hwindow();
}
