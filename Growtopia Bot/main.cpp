#include <gui/gui.h>

#include <thread>

int main()
{
	// create gui
	gui::create("Growtopia Bot", "GrowtopiaBotClass001");

	while (gui::is_running)
	{
		gui::begin_render();

		ImGui::SetNextWindowPos({ 0, 0 });
		ImGui::SetNextWindowSize({ gui::width, gui::height });
		ImGui::Begin("Growtopia", &gui::is_running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

		ImGui::Text(" Hai dari imgui ");

		ImGui::End();

		gui::end_render();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	// destroy gui
	gui::destroy();

	return EXIT_SUCCESS;
}
