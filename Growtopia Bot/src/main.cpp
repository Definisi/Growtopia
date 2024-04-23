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


std::string discord_uid = "";
std::string discord_name = "User";
bool success_get_avatar = false;

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

                if (gui.tab((const char*)ICON_FA_GEAR, "SETTINGS", gui.m_tab == 1) && gui.m_tab != 1)
                    gui.m_tab = 3, gui.m_anim = 0.f;

                if (gui.tab((const char*)ICON_FA_CODE, "EXECUTOR", gui.m_tab == 1) && gui.m_tab != 1)
                    gui.m_tab = 4, gui.m_anim = 0.f;


                EndChild();
                ImGui::SetCursorPos(ImVec2(190, 20));
                //ImGui::Button((const char*)ICON_FA_SAVE " Save", ImVec2(100, 25));



                
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

                ImGui::SetCursorPos(ImVec2(300, 20));
                ImGui::BeginChild("##subtabs", ImVec2(240, 25));

                ImGui::GetWindowDrawList()->AddRectFilled(GetWindowPos(), GetWindowPos() + GetWindowSize(), gui.button.to_im_color(), 4);
                ImGui::GetWindowDrawList()->AddRect(GetWindowPos(), GetWindowPos() + GetWindowSize(), gui.border.to_im_color(), 4);

                for (int i = 0; i < gui.automation_subtabs.size(); ++i) {
                    if (gui.subtab(gui.automation_subtabs.at(i), gui.m_automation_subtabs == i, gui.automation_subtabs.size(), i == 0 ? ImDrawFlags_RoundCornersLeft : i == gui.automation_subtabs.size() ? ImDrawFlags_RoundCornersRight : 0) && gui.m_automation_subtabs != i)
                        gui.m_automation_subtabs = i, gui.m_anim = 0.f;

                    if (i != gui.automation_subtabs.size() - 1)
                        ImGui::SameLine();

                }

                ImGui::EndChild();

                ImGui::PopStyleVar();

                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, gui.m_anim);
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));

                ImGui::SetCursorPos(ImVec2(185, 81 - (5 * gui.m_anim)));
                ImGui::BeginChild("##childs", ImVec2(size.x - 200, size.y - 96));

                switch (gui.m_tab) {

                case 0:

                    switch (gui.m_automation_subtabs) {

                    case 0:

                        gui.group_box((const char*)ICON_FA_SQUARE "", ImVec2(GetWindowWidth() / (3 / 2) - GetStyle().ItemSpacing.x / (3/2), 400)); {
                            
                            
                            auto clients = client_pool->get_clients();
                            if (clients.empty()) {
                                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Please select bot first");
                            }
                            else {
                                static const int grid_size = 5;
                                static const ImVec4 color_selected(1.0f, 0.0f, 0.0f, 1.0f);
                                static const ImVec4 color_unselected(0.5f, 0.5f, 0.5f, 1.0f);
                                static const ImVec2 button_size(50, 50);
                                static const float spacing = 10.0f; // Jarak antara tombol-tombol

                                // Memastikan bahwa auto_farm_tile dari semua bot yang dipilih adalah sama
                                for (auto& bot : clients) {
                                    bot->m_macro.auto_farm_tile = clients[0]->m_macro.auto_farm_tile;
                                }

                                for (int y = 0; y < grid_size; ++y) {
                                    for (int x = -2; x <= 2; ++x) {
                                        auto& auto_farm_tile = clients[0]->m_macro.auto_farm_tile; // Menggunakan auto_farm_tile dari bot pertama
                                        std::pair<int, int> coordinate = std::make_pair(x, -y);
                                        bool& tile_selected = auto_farm_tile[coordinate];
                                        ImVec4 color = tile_selected ? color_selected : color_unselected;

                                        ImGui::PushID(y * grid_size + (x + 2));
                                        ImGui::PushStyleColor(ImGuiCol_Button, color);
                                        if (ImGui::Button(std::format("[{}, {}]", x, -y).c_str(), button_size)) {
                                            for (auto& bot : clients) {
                                                bot->m_macro.auto_farm_tile[coordinate] = !tile_selected;
                                            }
                                        }
                                        ImGui::PopStyleColor();
                                        ImGui::PopID();

                                        if (x < 2) {
                                            ImGui::SameLine(0.0f, spacing);
                                        }
                                    }
                                }
                            }

                        
                        
                        } gui.end_group_box();

                        break;

                    case 1:
                        ImGui::Text("Oke2");
                        break;
                    }

                    break;

                case 1:

                    gui.group_box((const char*)ICON_FA_BABY " Baby", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight() / 2 - GetStyle().ItemSpacing.y / 2)); {

                    } gui.end_group_box();

                    gui.group_box((const char*)ICON_FA_AD " Ad", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight() / 2 - GetStyle().ItemSpacing.y / 2)); {

                    } gui.end_group_box();

                    SameLine(), SetCursorPosY(0);

                    gui.group_box("Non icon name", ImVec2(GetWindowWidth() / 2 - GetStyle().ItemSpacing.x / 2, GetWindowHeight())); {

                    } gui.end_group_box();

                    break;

                case 2:

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