
#include <gui/gui.hpp>

#include <gui/gui.hpp>
#include <curl/curl.h>

#include <auth/auth.hpp>

#include <utils/encryption/SkCrypt_custom.hpp>

#include <client/client.hpp>

#include <thread>
#include <iostream>


std::vector<std::shared_ptr<Client>> clients;



int main()
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}

	std::cout << skCrypt("hai").decrypt() << std::endl;
	auth::init();
	
	// create gui
	gui::create("Growtopia Bot", "GrowtopiaBotClass001");

	std::shared_ptr<Client> client1 = std::make_shared<Client>();
	clients.push_back(client1);

	//gui::tab_callbacks.push_back(gui::tab_main);
	
	bool sekali_aja = true;

	while (gui::is_running)
	{
		clients.back()->service_poll();

		if (sekali_aja) {
			clients.back()->connect();

			sekali_aja = false;
		}

		gui::begin_render();
		
		ImGui::SetNextWindowPos(ImVec2(0,0));
		ImGui::SetNextWindowSize(ImVec2(gui::width, gui::height));
		ImGui::Begin("Growtopia", &gui::is_running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

		gui::render_tabs_from_vector();

		ImGui::End();

		gui::end_render();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	// destroy gui
	gui::destroy();

	return EXIT_SUCCESS;
}
