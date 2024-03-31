#include <chrono>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <lapi/api.hpp>
#include <item/item_core.hpp>
#include <gui/gui.hpp>

#include <utils/string_split.hpp>
 
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
			if (kbdStruct->vkCode == VK_F1) {
				Gui::show_gui = !Gui::show_gui;
			}
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


int main() {
	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
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
		ImGui::StyleColorsDark();
		while (Gui::instance) {
			if (Gui::begin_render())
				continue;
			if (Gui::show_gui) {
				ImGui::SetNextWindowSize({ 685, 445 });
				ImGui::Begin("Speedy in Water - https://Growpai.site", 0, ImGuiWindowFlags_NoResize);

				ImGui::BeginChild("##GUI", ImGui::GetContentRegionAvail(), false);

				ImGui::BeginTabBar("##MainMenu");

				if (ImGui::BeginTabItem("Main")) {
					ImGui::BeginChild("##ListBot", ImVec2(167.25, -1));
					if (ImGui::BeginListBox("##client_list", ImVec2(167, 250))) {
						for (int i = 0; i < client_pool->get_clients().size(); ++i) {
							bool is_selected = client_pool->selected_bot == i;
							if (ImGui::Selectable(client_pool->get_clients()[i]->m_login_info.m_tank_id_name.c_str(), is_selected))
								client_pool->selected_bot = i;
						}
						ImGui::EndListBox();
					}
					ImGui::InputText("Username", &Gui::username);
					ImGui::InputText("Password", &Gui::password);
					ImGui::InputText("Socks5", &Gui::proxy);
					if (ImGui::Button("Add", ImVec2(-1, 0))) {
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
					if (ImGui::Button("Remove", ImVec2(-1, 0))) {
						if (client_pool->get_clients().size() >= client_pool->selected_bot) {
							client_pool->remove(client_pool->get_clients()[client_pool->selected_bot]->m_login_info.m_tank_id_name);
							client_pool->selected_bot = -1;
						}
					}
					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::BeginChild("##MenuBot", ImVec2(-1, -1));

					ImGui::BeginTabBar("##BotBar");

					if (ImGui::BeginTabItem("information")) {
						if (client_pool->get_clients().size() >= client_pool->selected_bot) {
							static std::string world_name;
							ImGui::Text("name: %s", client_pool->get_clients()[client_pool->selected_bot]->m_login_info.m_tank_id_name.c_str());
							ImGui::Text("status: %s", client_pool->get_clients()[client_pool->selected_bot]->get_status_string().c_str());
							ImGui::Text("ping: %d", client_pool->get_clients()[client_pool->selected_bot]->get_ping());
							if (ImGui::TreeNode("World")) {
								ImGui::Text("world name: %s", client_pool->get_clients()[client_pool->selected_bot]->m_world.m_name.c_str());
								ImGui::Text("world tile count: %d", client_pool->get_clients()[client_pool->selected_bot]->m_world.m_tile_count);
								ImGui::InputText("World name", &world_name);
								if (ImGui::Button("warp")) {
									client_pool->get_clients()[client_pool->selected_bot]->warp(world_name);
								}
								ImGui::TreePop();
							}
							if (ImGui::TreeNode("Local")) {
								ImGui::Text("gems: %d", client_pool->get_clients()[client_pool->selected_bot]->m_gem_count.load(std::memory_order_relaxed));
								ImGui::Text("userid: %d", client_pool->get_clients()[client_pool->selected_bot]->m_player.m_user_id);
								ImGui::Text("pos: %d : %d", client_pool->get_clients()[client_pool->selected_bot]->m_player.m_pos.m_x / 32, client_pool->get_clients()[client_pool->selected_bot]->m_player.m_pos.m_y / 32);
								ImGui::TreePop();
							}
							if (ImGui::TreeNode("Macro-v1")) {
								ImGui::Checkbox("auto reconnect", &client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_reconnect);
								ImGui::Separator();
								ImGui::Checkbox("auto collect", &client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_collect);
								ImGui::Checkbox("auto collect force", &client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_collect_force);
								ImGui::InputInt("auto collect range", &client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_collect_range);
								ImGui::InputInt("auto collect interval", &client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_collect_interval);
								ImGui::TreePop();
							}
							if (ImGui::Button("connect_bot")) {
								std::thread([&]() {
									while (!client_pool->get_clients()[client_pool->selected_bot]->connect() && client_pool->get_clients()[client_pool->selected_bot]->m_macro.auto_reconnect) {
										client_pool->get_clients()[client_pool->selected_bot]->reset();
										client_pool->get_clients()[client_pool->selected_bot]->m_login_info.reset();
										std::this_thread::sleep_for(std::chrono::seconds(30));
									}
									}).detach();
								
							}

						}
						ImGui::EndTabItem();
					}

					if (ImGui::BeginTabItem("Executor")) {
						if (client_pool->get_clients().size() >= client_pool->selected_bot) {

							ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", Gui::cpos.mLine + 1, Gui::cpos.mColumn + 1, Gui::editor.GetTotalLines(),
								Gui::editor.IsOverwrite() ? "Ovr" : "Ins",
								Gui::editor.CanUndo() ? "*" : " ",
								Gui::editor.GetLanguageDefinition().mName.c_str(), Gui::file_to_edit);
							ImGui::BeginChild("##TextEditor", ImVec2(0, -40));
							Gui::editor.Render("TextEditor");
							ImGui::EndChild();
							if (ImGui::Button("execute")) {
								Gui::save_file();
								lua::api::initialize(client_pool->get_clients()[client_pool->selected_bot]->m_lua_state, client_pool->get_clients()[client_pool->selected_bot]);
								std::thread(&lua::api::run, client_pool->get_clients()[client_pool->selected_bot]->m_lua_state, "main.lua").detach();
							}
							ImGui::SameLine();
							if (ImGui::Button("terminate [ todo ]")) {
								//lua::api::deinitialize(client_pool->get_clients()[client_pool->selected_bot]->m_lua_state);
								// Todo
							}
						}
					}

					ImGui::EndTabBar();

					ImGui::EndChild();
				}

				ImGui::EndTabBar();

				ImGui::EndChild();

				ImGui::End();
			}

			Gui::end_render();
		}
		Gui::destroy_imgui();
	}
	Gui::destroy_device();
	Gui::destroy_hwindow();
	for (auto client : client_pool->get_clients()) {
		lua::api::deinitialize(client->m_lua_state);
	}
	UnhookWindowsHookEx(hook);
	return EXIT_SUCCESS;
}
