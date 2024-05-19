#include <chrono>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <lapi/api.hpp>
#include <item/item_core.hpp>
#include <gui/gui.hpp>

#include <utils/string_split.hpp>
#include <d3dx9.h>
 
#include <utils/imgui/bytes.hpp>
#include <utils/imgui/blur.hpp>
#include <utils/imgui/gui.hpp>
#include <utils/imgui/hashes.hpp>

#include <utils/discord.hpp>

#include <utils/http_get.hpp>
#include <utils/containsignorecase.hpp>



std::string discord_uid = "";
std::string discord_name = "User";
bool success_get_avatar = false;

static char bot_search_keyboard[50];
static char world_input[50];
static char growid_username_input[50];
static char growid_password_input[50];
static char socks5_input[50];


#define ALPHA    ( ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )
#define NO_ALPHA ( ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoBorder )

IDirect3DTexture9* avatar{ };


int main() {
    client_pool->add("asik", "asil");
    init_discord_rpc();
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
        //ImGui::StyleColorsEnemymouse();
        ImGui::StyleColorsDark();
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

        io.Fonts->AddFontFromMemoryTTF(museo500_binary, sizeof museo500_binary, 14);
        static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;
        io.Fonts->AddFontFromMemoryTTF(&font_awesome_binary, sizeof font_awesome_binary, 13, &icons_config, icon_ranges);

        io.Fonts->AddFontFromMemoryTTF(museo900_binary, sizeof museo900_binary, 28);

        // Our state
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


        download_image_from_url("https://static.wikia.nocookie.net/growtopia/images/8/8f/ItemSprites.png/revision/latest/window-crop/width/32/x-offset/3456/y-offset/160/window-width/32/window-height/32?format=png&fill=cb-20240407123120", "avatar.png");
        D3DXCreateTextureFromFileExA(Gui::device, "avatar.png", 30, 30, D3DX_DEFAULT, 0,
            D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &avatar);

        while (Gui::instance) {

            for (auto client : client_pool->get_clients()) {
                client->service_poll();
            }


            if (!success_get_avatar && !discord_uid.empty()) {
                std::string result = http_get("https://pfpfinder.com/api/discord/user/" + discord_uid);
                if (!result.empty() && result.find("avatar") != std::string::npos) {
                    nlohmann::json result_json = nlohmann::json::parse(result.c_str());
                    try {
                        std::string uri_avatar = result_json["avatar"];
                        download_image_from_url(uri_avatar, "avatar.png");
                        D3DXCreateTextureFromFileExA(Gui::device, "avatar.png", 30, 30, D3DX_DEFAULT, 0,
                           D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &avatar);
                    }
                    catch (std::exception& e) {}
                }
                success_get_avatar = true;
            }
            Discord_RunCallbacks();
            if (Gui::begin_render())
                continue;
            
            //ImGui::ShowDemoWindow();

            blur::device = Gui::device;

            static bool bools[50]{};
            static int ints[50]{}, combo = 0;
            std::vector < const char* > items = { "Option", "Option 1", "Option 2", "Option 3", "Option 4", "Option 5", " Option 6", "Option 7", "Option 8", "Option 9" };
            static char buf[64];

            static float color[4] = { 1.f, 1.f, 1.f, 1.f };

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));



            ImGui::Begin("Speedy", &Gui::instance, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize); {

                auto window = ImGui::GetCurrentWindow();
                auto draw = window->DrawList;
                auto pos = window->Pos;
                auto size = window->Size;
                auto style = ImGui::GetStyle();

                gui.m_anim = ImLerp(gui.m_anim, 1.f, 0.045f);

                ImGui::SetWindowSize(ImVec2(690, 500));


                draw->AddText(io.Fonts->Fonts[1], io.Fonts->Fonts[1]->FontSize, pos + ImVec2(170 / 2 - io.Fonts->Fonts[1]->CalcTextSizeA(io.Fonts->Fonts[1]->FontSize, FLT_MAX, 0, "SPEEDY").x / 2 + 1, 20), gui.accent_color.to_im_color(), "SPEEDY");
                draw->AddText(io.Fonts->Fonts[1], io.Fonts->Fonts[1]->FontSize, pos + ImVec2(170 / 2 - io.Fonts->Fonts[1]->CalcTextSizeA(io.Fonts->Fonts[1]->FontSize, FLT_MAX, 0, "SPEEDY").x / 2, 20), GetColorU32(ImGuiCol_Text), "SPEEDY");

                draw->AddLine(pos + ImVec2(0, size.y - 50), pos + ImVec2(170, size.y - 50), GetColorU32(ImGuiCol_WindowBg, 0.5f));
                draw->AddImageRounded(avatar, pos + ImVec2(15, size.y - 40), pos + ImVec2(45, size.y - 10), ImVec2(0, 0), ImVec2(1, 1), ImColor(1.f, 1.f, 1.f, 1.f), 100);
                draw->AddText(pos + ImVec2(50, size.y - 40), gui.text.to_im_color(), discord_name.c_str());
                draw->AddText(pos + ImVec2(50, size.y - 25), gui.text_disabled.to_im_color(), "Till:");
                draw->AddText(pos + ImVec2(50 + CalcTextSize("Till: ").x, size.y - 25), gui.accent_color.to_im_color(), "Lifetime");

                ImGui::SetCursorPos(ImVec2(10, 70));
                ImGui::BeginChild("##tabs", ImVec2(150, size.y - 120));

                if (gui.tab((const char*)ICON_FA_ROTATE_RIGHT, "AUTOMATION", gui.m_tab == 0) && gui.m_tab != 0)
                    gui.m_tab = 0, gui.m_anim = 0.f;

                if (gui.tab((const char*)ICON_FA_ROBOT, "BOT LIST", gui.m_tab == 1) && gui.m_tab != 1)
                    gui.m_tab = 1, gui.m_anim = 0.f;

                if (gui.tab((const char*)ICON_FA_GEAR, "SETTINGS", gui.m_tab == 2) && gui.m_tab != 2)
                    gui.m_tab = 2, gui.m_anim = 0.f;

                if (gui.tab((const char*)ICON_FA_CODE, "EXECUTOR", gui.m_tab == 3) && gui.m_tab != 3)
                    gui.m_tab = 3, gui.m_anim = 0.f;


                EndChild();
                ImGui::SetCursorPos(ImVec2(190, 20));
                //ImGui::Button((const char*)ICON_FA_SAVE " Save", ImVec2(100, 25));



                
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                ImGui::SetCursorPos(ImVec2(190, 20));
                ImGui::BeginChild("##subtabs", ImVec2(350, 25));

                ImGui::GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + GetWindowSize(), gui.button.to_im_color(), 4);
                ImGui::GetWindowDrawList()->AddRect(GetWindowPos(), GetWindowPos() + GetWindowSize(), gui.border.to_im_color(), 4);

                if (gui.m_tab == 0) {
                    for (int i = 0; i < gui.automation_subtabs.size(); ++i) {
                        if (gui.subtab(gui.automation_subtabs.at(i), gui.m_automation_subtabs == i, gui.automation_subtabs.size(), i == 0 ? ImDrawFlags_RoundCornersLeft : i == gui.automation_subtabs.size() ? ImDrawFlags_RoundCornersRight : 0) && gui.m_automation_subtabs != i)
                            gui.m_automation_subtabs = i, gui.m_anim = 0.f;

                        if (i != gui.automation_subtabs.size() - 1)
                            ImGui::SameLine();

                    }
                }

                if (gui.m_tab == 1) {
                    if (gui.account_tab) {
                        if (gui.subtab("Go to status", true, 1, ImDrawFlags_RoundCornersLeft |ImDrawFlags_RoundCornersRight ))
                            gui.account_tab = false, gui.m_anim = 0.f;
                    }
                    else {
                        if (gui.subtab("Remove all bots", true, 3, ImDrawFlags_RoundCornersLeft)) {
                            for (auto bot : client_pool->get_clients()) {
                                client_pool->remove(bot->m_login_info.m_tank_id_name);
                            }
                        }
                        ImGui::SameLine();

                        if (gui.subtab("Remove inactive bots", true, 3, 0)) {
                            for (auto bot : client_pool->get_clients()){
                                if (bot->get_peer()->state == ENET_PEER_STATE_DISCONNECTED) {
                                    client_pool->remove(bot->m_login_info.m_tank_id_name);
                                }
                            }
                        }
                        ImGui::SameLine();

                        if (gui.subtab("Go to account", true, 3, ImDrawFlags_RoundCornersRight))
                            gui.account_tab = true, gui.m_anim = 0.f;
                    }
                }

                ImGui::EndChild();

                ImGui::PopStyleVar();

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui.m_anim);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

                ImGui::SetCursorPos(ImVec2(185, 81 - (5 * gui.m_anim)));
                ImGui::BeginChild("##childs", ImVec2(size.x - 200, size.y - 96));

                switch (gui.m_tab) {

                    case 0:
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Coming Soon!");
                        /*
                        switch (gui.m_automation_subtabs) {

                        case 0:

                            gui.group_box((const char*)ICON_FA_SQUARE " Auto Farm", ImVec2(360, 400)); {
                            
                                float posisi_y_awal = ImGui::GetCursorPosY();
                            
                                auto clients = client_pool->get_clients();
                                bool selected = std::any_of(clients.begin(), clients.end(), [](const auto& client) { return client->selected; });
                                std::shared_ptr<Client> selected_client = std::find_if(clients.begin(), clients.end(), [](const std::shared_ptr<Client>& _client) { return _client->selected; }) != clients.end() ? *std::find_if(clients.begin(), clients.end(), [](const std::shared_ptr<Client>& _client) { return _client->selected; }) : nullptr;
                                if (!selected) {
                                    ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select bot first");
                                }
                                else {
                                    static const int grid_size = 5;
                                    static const ImVec4 color_selected(1.0f, 0.0f, 0.0f, 1.0f);
                                    static const ImVec4 color_unselected = gui.button.to_vec4();
                                    static const ImVec2 button_size(40, 40);
                                    static const float spacing = 5.0f;

                                    for (auto& bot : clients) {
                                        bot->m_macro.auto_farm_tile = selected_client->m_macro.auto_farm_tile;
                                    }
                                    for (int y = 2; y >= -2; --y) {
                                        for (int x = -2; x <= 2; ++x) {
                                            auto& auto_farm_tile = selected_client->m_macro.auto_farm_tile;
                                            std::pair<int, int> coordinate = std::make_pair(x, -y);
                                            bool& tile_selected = auto_farm_tile[coordinate];
                                            ImVec4 color = tile_selected ? color_selected : color_unselected;

                                            ImGui::PushID(y * grid_size + (x + 2));
                                            color_t color_backup = gui.button;
                                            color_t color_backup2 = gui.button_hovered;
                                            gui.button = {color.x, color.y, color.z, color.w};
                                            gui.button_hovered = { color.x, color.y, color.z, color.w };
                                            if (x == 0 && -y == 0) {
                                                if (ImGui::Button((const char*)ICON_FA_SQUARE_ROOT, button_size)) {
                                                    for (auto& bot : clients) {
                                                        bot->m_macro.auto_farm_tile[coordinate] = !tile_selected;
                                                    }
                                                }
                                            }else{
                                                if (ImGui::Button(std::format("[{}, {}]", x, -y).c_str(), button_size)) {
                                                    for (auto& bot : clients) {
                                                        bot->m_macro.auto_farm_tile[coordinate] = !tile_selected;
                                                    }
                                                }
                                            }
                                        

                                            ImGui::PopID();

                                            gui.button = color_backup;
                                            gui.button_hovered = color_backup2;

                                            if (x < 2) {
                                                ImGui::SameLine(0.0f, spacing);
                                            }
                                        }
                                    }


                                    static bool show_punch = false;
                                    static bool place = false;
                                    static bool punch = false;
                                    static int place_delay = 110;
                                    static int punch_delay = 120;
                                    static int block_id = 0;
                                    ImGui::SetCursorPosY(270);
                                    ImGui::InputInt("Punch Delay", &punch_delay);
                                    ImGui::InputInt("Place Delay", &place_delay);
                                    ImGui::InputInt("Block ID", &block_id);

                                    ImGui::SetCursorPos({ 230,posisi_y_awal + 20 });
                                    ImGui::BeginChild("##button_section_autofarm", ImVec2(105,150));
                                    ImGui::Checkbox("Show Punch", &show_punch);
                                    ImGui::Checkbox("Punch", &punch);
                                    ImGui::Checkbox("Place", &place);

                                    ImGui::Text("X: %d", (int)selected_client->m_player.m_pos.m_x / 32);
                                    ImGui::Text("Y: %d", (int)selected_client->m_player.m_pos.m_y / 32);
                                    ImGui::EndChild();
                                }

                        
                        
                            } gui.end_group_box();

                            ImGui::SameLine();

                            gui.group_box((const char*)ICON_FA_LIST " Bot", ImVec2(120, 400)); {
                                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                ImGui::InputTextWithHint("##bot_search_keyword", "Search Bot", bot_search_keyboard, 50);

                                if (ImGui::BeginListBox("##list_bot", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 50)))
                                {
                                    for (int i = 0; i < client_pool->get_clients().size(); i++)
                                    {
                                        auto client = client_pool->get_clients()[i];
                                        if (!contains_ignore_case(client->m_login_info.m_tank_id_name, std::string(bot_search_keyboard)))
                                            continue;
                                        int maxIndexLength = 0;
                                        int currentIndexLength = std::to_string(i).length();
                                        if (currentIndexLength > maxIndexLength)
                                        {
                                            maxIndexLength = currentIndexLength;
                                        }

                                        if (ImGui::Selectable(client->m_login_info.m_tank_id_name.c_str(), client->selected))
                                        {
                                            if (client->selected)
                                                client->selected = false;
                                            else
                                                client->selected = true;
                                        }
                                    }

                                    ImGui::EndListBox();
                                }

                            }
                            gui.end_group_box();
                        
                            break;

                        case 1:
                            ImGui::Text("Oke2");
                            break;
                        }
                        */
                        break;

                    case 1:


                        if (gui.account_tab){


                            gui.group_box((const char*)ICON_FA_LIST " Management", ImVec2(320, 400)); {
                                auto clients = client_pool->get_clients();
                                bool selected = std::any_of(clients.begin(), clients.end(), [](const auto& client) { return client->selected; });
                                std::shared_ptr<Client> selected_client = std::find_if(clients.begin(), clients.end(), [](const std::shared_ptr<Client>& _client) { return _client->selected; }) != clients.end() ? *std::find_if(clients.begin(), clients.end(), [](const std::shared_ptr<Client>& _client) { return _client->selected; }) : nullptr;

                                ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                                ImGui::InputTextWithHint("##bot_search_keyword", "Search Bot", bot_search_keyboard, 50);

                                if (ImGui::BeginListBox("##list_bot", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 150)))
                                {
                                    for (int i = 0; i < client_pool->get_clients().size(); i++)
                                    {
                                        auto client = client_pool->get_clients()[i];
                                        if (!contains_ignore_case(client->m_login_info.m_tank_id_name, std::string(bot_search_keyboard)))
                                            continue;
                                        int maxIndexLength = 0;
                                        int currentIndexLength = std::to_string(i).length();
                                        if (currentIndexLength > maxIndexLength)
                                        {
                                            maxIndexLength = currentIndexLength;
                                        }

                                        if (ImGui::Selectable(client->m_login_info.m_tank_id_name.c_str(), client->selected))
                                        {
                                            if (client->selected)
                                                client->selected = false;
                                            else
                                                client->selected = true;
                                        }
                                    }

                                    ImGui::EndListBox();
                                }

                                ImGui::Text("GrowID");

                                ImGui::SameLine();

                                ImGui::InputTextWithHint("##GrowIDUsername", "SpeedyBot", growid_username_input, 50);

                                ImGui::Text("Password");

                                ImGui::SameLine();

                                ImGui::InputTextWithHint("##GrowIDPassword", "SpeedyBot", growid_password_input, 50);

                                ImGui::Text("Socks5");

                                ImGui::SameLine();

                                ImGui::InputTextWithHint("##Socks5", "ip:port ip:port:username:password", socks5_input, 50);

                                if (ImGui::Button("Add", ImVec2(ImGui::GetContentRegionAvail().x / 2, 50))) {
                                    auto bot = client_pool->add(growid_username_input, growid_password_input);
                                    if (!std::string(socks5_input).empty()) {
                                        try {
                                            auto row = split(socks5_input, ':');
                                            bot->set_socks5_info(row[0], std::stoi(row[1]));
                                            if (row.size() >= 4) {
                                                bot->set_socks5_info(row[0], std::stoi(row[1]), row[2], row[3]);
                                            }
                                        }
                                        catch (std::exception& e) {
                                            // Nothing
                                        }
                                    }
                                }
                                ImGui::SameLine();

                                if (ImGui::Button("Remove", ImVec2(ImGui::GetContentRegionAvail().x, 50))) {
                                    if (selected) {
                                        client_pool->remove(selected_client->m_login_info.m_tank_id_name);
                                    }
                                }

                            } gui.end_group_box();

                            ImGui::SameLine();

                            gui.group_box((const char*)ICON_FA_INFO " Information", ImVec2(160, 400)); {
                                auto clients = client_pool->get_clients();
                                bool selected = std::any_of(clients.begin(), clients.end(), [](const auto& client) { return client->selected; });
                                std::shared_ptr<Client> selected_client = std::find_if(clients.begin(), clients.end(), [](const std::shared_ptr<Client>& _client) { return _client->selected; }) != clients.end() ? *std::find_if(clients.begin(), clients.end(), [](const std::shared_ptr<Client>& _client) { return _client->selected; }) : nullptr;
                                gui.group_box("#infony", ImVec2(135, 160)); {
                                    if (!selected) {
                                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select bot first");
                                    }
                                    else {
                                        ImGui::Text("World Name: %s", selected_client->m_world.m_name.c_str());
                                        ImGui::Text("Status: %s", selected_client->get_status_string().c_str());
                                        ImGui::Text("Position: %d, %d", (int)selected_client->m_player.m_pos.m_x / 32, (int)selected_client->m_player.m_pos.m_y / 32);
                                        ImGui::Text("Level: %d", (int)selected_client->m_level);
                                        ImGui::Text("Gems: %d", (int)selected_client->m_gem_count);
                                        ImGui::Text("Backpack Size: %d", (int)selected_client->m_inventory.m_size);
                                    }
                                } gui.end_group_box();

                                ImGui::InputTextWithHint("##World Name", "World Name", world_input, 50);
                                ImGui::SameLine();
                                if (ImGui::Button("Warp")) {
                                    if (selected) {
                                        selected_client->warp(world_input);
                                    }
                                }


                                if (ImGui::Button("Connect", ImVec2(ImGui::GetContentRegionAvail().x / 2, 25))) {
                                    if (selected) {
                                        selected_client->connect();
                                    }
                                }
                                ImGui::SameLine();
                                if (ImGui::Button("Disconnect", ImVec2(ImGui::GetContentRegionAvail().x, 25))) {
                                    if (selected) {
                                        selected_client->disconnect();
                                    }
                                }

                                if (selected) {
                                    ImGui::Checkbox("Auto Reconnect", &selected_client->m_macro.auto_reconnect);
                                }

                            } gui.end_group_box();


                        }
                       

                        else {
                            gui.group_box("##BotList", ImVec2(ImGui::GetContentRegionAvail().x - 10, 400)); {

                                if (ImGui::BeginTable("Bot List", 6))
                                {
                                    ImGui::TableSetupColumn("BOT NAME");
                                    ImGui::TableSetupColumn("IP");
                                    ImGui::TableSetupColumn("PING");
                                    ImGui::TableSetupColumn("LEVEL");
                                    ImGui::TableSetupColumn("STATUS");
                                    ImGui::TableSetupColumn("WORLD");
                                    ImGui::TableHeadersRow();

                                    for (auto bot : client_pool->get_clients())
                                    {
                                        ImGui::TableNextRow();
                                        ImGui::TableSetColumnIndex(0);
                                        ImGui::Text("%s", bot->m_login_info.m_tank_id_name.c_str());
                                        ImGui::TableSetColumnIndex(1);
                                        ImGui::Text("%s", bot->socks5_ip.c_str());
                                        ImGui::TableSetColumnIndex(2);
                                        ImGui::Text("%d", (int)bot->get_ping());
                                        ImGui::TableSetColumnIndex(3);
                                        ImGui::Text("%d", (int)bot->m_level);
                                        ImGui::TableSetColumnIndex(4);
                                        ImGui::Text("%s", bot->get_status_string().c_str());
                                        ImGui::TableSetColumnIndex(5);
                                        ImGui::Text("%s", bot->m_world.m_name.c_str());
                                    }

                                    ImGui::EndTable();
                                }

                            }gui.end_group_box();
                        }
                        break;

                    case 2:

                        break;

                    case 3: 

                        Gui::editor.Render("A Tittle");

                        break;
                }

                ImGui::EndChild();

                ImGui::PopStyleVar(2);

            } ImGui::End();

            ImGui::PopStyleVar();

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