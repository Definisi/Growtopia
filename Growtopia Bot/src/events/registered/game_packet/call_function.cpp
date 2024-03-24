#include <events/registered/game_packet/call_function.hpp>

#include <cstdint>
#include <iostream>
#include <regex>
#include <string>

#include <proton/packet.hpp>
#include <proton/variant.hpp>

#include <item/item_core.hpp>

namespace events {
	void call_function(EventContext& ctx) {
		VariantList varlist;
		varlist.deserialize(ctx.m_extended_data);

		if (ctx.m_client->m_is_listening_events.load(std::memory_order_relaxed)) {
			lua::events::EventContext context{
				.m_name = "NET_GAME_PACKET_CALL_FUNCTION",
				.m_client = ctx.m_client,
				.m_varlist = varlist
			};
			ctx.m_client->m_event_queue.push(context);
		}

		if (varlist[0].get<std::string>() == "OnSetBux") {
			ctx.m_client->m_gem_count.store(varlist[1].get<int32_t>(), std::memory_order_relaxed);
		}
		else if (varlist[0].get<std::string>().find("OnSuperMain") != std::string::npos) {
			GameUpdatePacket game_packet;
			game_packet.m_type = NET_GAME_PACKET_APP_CHECK_RESPONSE;
			game_packet.m_net_id = -1;
			game_packet.m_int_data = 443347149;
			ctx.m_client->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));;

			ctx.m_client->send_packet(NET_MESSAGE_GENERIC_TEXT, "action|enter_game\n");
		}
		else if (varlist[0].get<std::string>() == "OnSendToServer") {
			{
				std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

				uint16_t port = varlist[1].get<int32_t>();
				ctx.m_client->m_login_info.m_port = port;

				int32_t token = varlist[2].get<int32_t>();
				ctx.m_client->m_login_info.m_token = token;

				int32_t user = varlist[3].get<int32_t>();
				ctx.m_client->m_login_info.m_user = user;

				std::string data = varlist[4].get<std::string>();
				size_t pos = data.find("|");

				int32_t lmode = varlist[5].get<int32_t>();
				ctx.m_client->m_login_info.m_lmode = lmode;

				std::string address = data.substr(0, pos);
				ctx.m_client->m_login_info.m_address = address;

				std::string door_id = data.substr(pos + 1, data.find("|", pos + 1) - pos - 1);
				ctx.m_client->m_login_info.m_door_id = door_id;

				std::string uuid_token = data.substr(data.find("|", pos + 1) + 1);
				ctx.m_client->m_login_info.m_uuid_token = uuid_token;
			}

			GameUpdatePacket game_packet;
			game_packet.m_type = NET_GAME_PACKET_DISCONNECT;
			ctx.m_client->send_packet(NET_MESSAGE_GAME_PACKET, &game_packet, sizeof(GameUpdatePacket));;

			ctx.m_client->connect();
		}
		else if (varlist[0].get<std::string>() == "OnConsoleMessage") {
			std::string message = std::regex_replace(varlist[1].get<std::string>(), std::regex("`."), "");
			std::cout << std::format("{}", message) << std::endl;
		}
		else if (varlist[0].get<std::string>() == "OnDialogRequest") {
			/*TextScanner scanner(varlist[1].get<std::string>());
			const std::string& end_dialog = scanner.get("end_dialog", 0);

			if (end_dialog == "magplant_edit") {
				std::string x, y;

				scanner.for_each([&](const std::string& label, const std::vector<std::string>& values) {
					if (label == "embed_data") {
						if (values[0] == "x") {
							x = values[1];
						}
						else if (values[0] == "y") {
							y = values[1];
						}
					}
					});

				if (x.empty() || y.empty())
					return;

				TextScanner text;
				text.add("action", "dialog_return");
				text.add("dialog_name", "magplant_edit");
				text.add("x", x);
				text.add("y", y);
				text.add("buttonClicked", "getRemote");

				ctx.m_client->send_packet(NET_MESSAGE_GENERIC_TEXT, text.get_all());
			}*/
		}
		else if (varlist[0].get<std::string>() == "SetHasGrowID") {
			std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

			ctx.m_client->m_login_info.m_tank_id_name = varlist[2].get<std::string>();
			ctx.m_client->m_login_info.m_tank_id_pass = varlist[3].get<std::string>();
		}
		else if (varlist[0].get<std::string>() == "OnSpawn") {
			TextScanner scanner(varlist[1].get<std::string>());

			if (scanner.get("type", 0) == "local") {
				std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

				int net_id = std::stoi(scanner.get("netID", 0));
				int user_id = std::stoi(scanner.get("userID", 0));
				int x = std::stoi(scanner.get("posXY", 0));
				int y = std::stoi(scanner.get("posXY", 1));

				ctx.m_client->m_player.m_net_id = net_id;
				ctx.m_client->m_player.m_user_id = user_id;
				Vector2i pos(x, y);
				ctx.m_client->m_player.m_pos = pos;
			}
		}
		else if (varlist[0].get<std::string>() == "OnSetPos") {
			std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

			if (ctx.m_client->m_player.m_net_id == ctx.m_game_packet->m_net_id) {
				if (ctx.m_client->m_is_pathfinding.load(std::memory_order_relaxed))
					ctx.m_client->m_is_pathfinding.store(false, std::memory_order_relaxed);

				Vector2i pos(varlist[1].get<Vector2>().m_x, varlist[1].get<Vector2>().m_y);
				ctx.m_client->m_player.m_pos = pos;
			}
		}
		else if (varlist[0].get<std::string>() == "OnRequestWorldSelectMenu") {
			std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

			Player player;
			player.m_user_id = ctx.m_client->m_player.m_user_id;
			ctx.m_client->m_player = player;

			World world;
			world.m_name = "EXIT";
			ctx.m_client->m_world = world;
		}
	}
}