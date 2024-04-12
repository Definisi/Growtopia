#include <chrono>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <lapi/api.hpp>
#include <item/item_core.hpp>
#include <gui/gui.hpp>

#include <utils/string_split.hpp>

#include <utils/imgui/byte_array.hpp>
#include <utils/imgui/etc_elements.hpp>
#include <utils/imgui/nav_elements.hpp>
#include <utils/imgui/font_awesome.hpp>
 

enum heads {
    management, farmandspam, automation, inventory, world, settings
};


ImFont* medium;
ImFont* bold;
ImFont* tab_icons;
ImFont* logo;
ImFont* tab_title;
ImFont* tab_title_icon;
ImFont* subtab_title;
ImFont* combo_arrow;


ImFont* font_awesome;

int main() {
	if (!item_database->initialize("items.dat"))
		return EXIT_FAILURE;
	system("cls");

	if (enet_initialize() != 0) {
		//std::cout << "An error occured while initializing ENet" << std::endl;
		return EXIT_FAILURE;
	}

	Gui::create_hwindow(L"main window");
	if (Gui::create_device()) {
		Gui::create_imgui();
		ImGui::StyleColorsEnemymouse();

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigViewportsNoDecoration = false;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Load Fonts
        ImFontConfig font_config;
        font_config.PixelSnapH = false;
        font_config.OversampleH = 5;
        font_config.OversampleV = 5;
        font_config.RasterizerMultiply = 1.2f;

        static const ImWchar ranges[] =
        {
            0x0020, 0x00FF, // Basic Latin + Latin Supplement
            0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
            0x2DE0, 0x2DFF, // Cyrillic Extended-A
            0xA640, 0xA69F, // Cyrillic Extended-B
            0xE000, 0xE226, // icons
            0,
        };

        font_config.GlyphRanges = ranges;

        medium = io.Fonts->AddFontFromMemoryTTF(PTRootUIMedium, sizeof(PTRootUIMedium), 15.0f, &font_config, ranges);
        bold = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 15.0f, &font_config, ranges);

        tab_icons = io.Fonts->AddFontFromMemoryTTF(clarityfont, sizeof(clarityfont), 15.0f, &font_config, ranges);
        logo = io.Fonts->AddFontFromMemoryTTF(clarityfont, sizeof(clarityfont), 21.0f, &font_config, ranges);

        tab_title = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 19.0f, &font_config, ranges);
        tab_title_icon = io.Fonts->AddFontFromMemoryTTF(clarityfont, sizeof(clarityfont), 18.0f, &font_config, ranges);

        subtab_title = io.Fonts->AddFontFromMemoryTTF(PTRootUIBold, sizeof(PTRootUIBold), 15.0f, &font_config, ranges);

        combo_arrow = io.Fonts->AddFontFromMemoryTTF(combo, sizeof(combo), 9.0f, &font_config, ranges);


        // Our state
        ImVec4 clear_color = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);

		while (Gui::instance) {
			if (Gui::begin_render())
				continue;
            static heads tab{ management };

            ImGui::SetNextWindowSize({ 730, 460 });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });

            ImGui::Begin("Speedy Multibot", nullptr, ImGuiWindowFlags_NoDecoration); {
                auto draw = ImGui::GetWindowDrawList();

                auto pos = ImGui::GetWindowPos();
                auto size = ImGui::GetWindowSize();

                ImGuiStyle style = ImGui::GetStyle();

                draw->AddRectFilled(pos, ImVec2(pos.x + 165, pos.y + size.y), ImColor(24, 24, 26), style.WindowRounding, ImDrawFlags_RoundCornersLeft);
                draw->AddLine(ImVec2(pos.x + 165, pos.y + 2), ImVec2(pos.x + 165, pos.y + size.y - 2), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
                //draw->AddLine(ImVec2(pos.x + 47, pos.y + 2), ImVec2(pos.x + 47, pos.y + size.y - 2), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
                draw->AddLine(ImVec2(pos.x + 2, pos.y + 47), ImVec2(pos.x + 47, pos.y + 47), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
                draw->AddLine(ImVec2(pos.x + 63, pos.y + 47), ImVec2(pos.x + 195, pos.y + 47), ImColor(1.0f, 1.0f, 1.0f, 0.03f));
                draw->AddText(bold, 25.0f, ImVec2(pos.x + 25, pos.y + 12), ImColor(0.9098039269447327f, 0.6392157077789307f, 0.1294117718935013f), "Speedy");

                draw->AddRect(ImVec2(pos.x + 1.0f, pos.y + 1.0f), ImVec2(pos.x + size.x - 1.0f, pos.y + size.y - 1.0f), ImColor(1.0f, 1.0f, 1.0f, 0.03f), style.WindowRounding);

                ImGui::SetCursorPos({ 8, 56 });
                ImGui::BeginGroup(); {
                    if (elements::subtab("Management", tab == management)) { tab = management; }
                    if (elements::subtab("Farm and Spam", tab == farmandspam)) { tab = farmandspam; }
                    if (elements::subtab("Automation", tab == automation)) { tab = automation; }
                    if (elements::subtab("Inventory", tab == inventory)) { tab = inventory; }
                    if (elements::subtab("World", tab == world)) { tab = world; }
                    if (elements::subtab("Setting", tab == settings)) { tab = settings; }
                } ImGui::EndGroup();

                ImGui::SetCursorPos({ 165, 0 });
                ImGui::BeginChild("##SubTab", ImVec2(0, 0), true);
                ImGui::SetCursorPos({ 25, 25 });
                switch (tab) {

                    case management: {
                        ImGui::BeginTabBar("##Managements");
                        ImGuiStyle& style = ImGui::GetStyle();
                        auto FramePadding = style.FramePadding;
                        style.FramePadding = ImVec2(20, 10);
                        if (ImGui::BeginTabItem("Accounts")) {
                            style.FramePadding = FramePadding;
                            ImGui::SetCursorPosX(25);
                            ImGui::BeginChild("List", ImVec2(250, 200), true);
                           
                            
                            if (ImGui::BeginListBox("##client_list", ImVec2(250, 200))) {
                                for (int i = 0; i < client_pool->get_clients().size(); ++i) {
                                    bool is_selected = client_pool->selected_bot == i;
                                    if (ImGui::Selectable(client_pool->get_clients()[i]->m_login_info.m_tank_id_name.c_str(), is_selected))
                                        client_pool->selected_bot = i;
                                }
                                ImGui::EndListBox();
                            }

                            ImGui::EndChild();

                            ImGui::SameLine();

                            ImGui::BeginChild("Form", ImVec2(250, 200), true);

                            ImGui::InputText("Username", &Gui::username);
                            ImGui::InputText("Password", &Gui::password);
                            ImGui::InputText("Socks5", &Gui::proxy);
                            ImGui::SetCursorPosY(160);
                            ImGui::SetCursorPosX(5);
                            if (ImGui::Button("Add", ImVec2(100, 0))) {
                                std::shared_ptr<Client> new_client = client_pool->add(Gui::username, Gui::password);
                                if (!Gui::proxy.empty()) {
                                    auto nrow = split(Gui::proxy, ':');
                                    if (nrow.size() >= 2) {
                                        if (nrow.size() >= 4) {
                                            new_client->set_socks5_info(nrow[0], std::stoi(nrow[1]), nrow[2], nrow[3]);
                                        }
                                        else {
                                            new_client->set_socks5_info(nrow[0], std::stoi(nrow[1]));
                                        }
                                    }
                                }
                                std::thread([&new_client]() {
                                    new_client->connect();
                                    bool found_client = true;
                                    while (found_client) {
                                        bool found_client2 = false;
                                        for (auto& client : client_pool->get_clients()) {
                                            if (client == new_client) {
                                                new_client->service_poll();
                                                found_client2 = true;
                                                break;
                                            }
                                        }
                                        found_client = found_client2;
                                    }
                                    }).detach();
                            }
                            ImGui::SameLine();
                            ImGui::InvisibleButton("##AddRemove", ImVec2(15, 0));
                            ImGui::SameLine();
                            if (ImGui::Button("Remove", ImVec2(100, 0))) {
                                if (client_pool->get_clients().size() >= client_pool->selected_bot) {
                                    client_pool->remove(client_pool->get_clients()[client_pool->selected_bot]->m_login_info.m_tank_id_name);
                                    client_pool->selected_bot = -1;
                                }
                            }

                            ImGui::EndChild();



                            ImGui::SetCursorPosX(25);
                            ImGui::BeginChild("Information", ImVec2(510, 180), true);

                            if (client_pool->selected_bot >= 0){
                                ImGui::SetCursorPosX(25);
                                ImGui::SetCursorPosY(25);
                                ImGui::Text("World Name: %s", client_pool->get_clients()[client_pool->selected_bot]->m_world.m_name.c_str());
                                ImGui::SetCursorPosX(25); 
                                ImGui::Text("World Size: %d, %d (%d tiles)", client_pool->get_clients()[client_pool->selected_bot]->m_world.m_width, client_pool->get_clients()[client_pool->selected_bot]->m_world.m_height, client_pool->get_clients()[client_pool->selected_bot]->m_world.m_tile_count);
                                ImGui::SetCursorPosX(25);
                                ImGui::Text("Position: %d, %d", floor(client_pool->get_clients()[client_pool->selected_bot]->m_player.m_pos.m_x / 32), floor(client_pool->get_clients()[client_pool->selected_bot]->m_player.m_pos.m_y / 32));
                                ImGui::SetCursorPosX(25);
                                ImGui::Text("Level: %d", client_pool->get_clients()[client_pool->selected_bot]->m_level.load(std::memory_order_relaxed));
                                ImGui::SetCursorPosX(25);
                                ImGui::Text("Gems: %d", client_pool->get_clients()[client_pool->selected_bot]->m_gem_count.load(std::memory_order_relaxed));
                                ImGui::SetCursorPosX(25);
                                ImGui::InputText("World name", &Gui::world_name);
                                ImGui::SameLine();
                                if (ImGui::Button("Warp")) {
                                    client_pool->get_clients()[client_pool->selected_bot]->warp(Gui::world_name);
                                }

                                ImGui::SetCursorPosX(25);
                                if (ImGui::Button("Connect")) {
                                    std::thread([&]() {
                                        while (!client_pool->get_clients()[client_pool->selected_bot]->connect() && client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_reconnect) {
                                            client_pool->get_clients()[client_pool->selected_bot]->reset();
                                            client_pool->get_clients()[client_pool->selected_bot]->m_login_info.reset();
                                            std::this_thread::sleep_for(std::chrono::seconds(30));
                                        }
                                        }).detach();

                                }
                            }

                            ImGui::EndChild();

                            ImGui::EndTabItem();
                        }
                        style.FramePadding = ImVec2(20, 10);
                        if (ImGui::BeginTabItem("Status")) {
                            style.FramePadding = FramePadding;

                            ImGui::EndTabItem();
                        }
                        break;
                        ImGui::EndTabBar();
                    }



                    case farmandspam: {
                        ImGui::SetCursorPos({ 226, 16 });
                        e_elements::begin_child("Automatic tiles", ImVec2(240, 300)); {
                        }
                        e_elements::end_child();
                        break;
                    }

                }


                ImGui::EndChild();
            }

            ImGui::End();

			Gui::end_render();
		}
		Gui::destroy_imgui();
	}
	Gui::destroy_device();
	Gui::destroy_hwindow();
	for (auto client : client_pool->get_clients()) {
		//lua::api::deinitialize(client->m_lua_state);
	}
	return EXIT_SUCCESS;
}
