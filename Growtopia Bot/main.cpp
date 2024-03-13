#define CURL_STATICLIB


#include <gui/gui.hpp>
#include <curl/curl.h>

#include <thread>
#include <iostream>


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}


int main()
{


	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "https://api.ipify.org/?format=text");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		std::cout << readBuffer << std::endl;
	}

	// create gui
	gui::create("Growtopia Bot", "GrowtopiaBotClass001");

	TyoGui m_gui;

	while (gui::is_running)
	{
		gui::begin_render();
		m_gui.create_window("Ini Judul", ImVec2(100, 100), ImVec2(0, 0), &gui::is_running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);

		ImGui::Text("");

		m_gui.end_window();

		gui::end_render();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	// destroy gui
	gui::destroy();

	return EXIT_SUCCESS;
}
