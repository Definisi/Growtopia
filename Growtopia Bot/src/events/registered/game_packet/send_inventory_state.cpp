#include <events/registered/game_packet/send_inventory_state.hpp>

#include <format>
#include <iostream>

namespace events {
	void send_inventory_state(EventContext& ctx) {
		std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);

		ctx.m_client->m_inventory.m_items.clear();

		BinaryReader reader(ctx.m_extended_data, ctx.m_game_packet->m_data_size);

		reader.skip(1);
		ctx.m_client->m_inventory.m_size = reader.read<uint32_t>();
		uint8_t item_count = reader.read<uint8_t>();

		for (uint32_t index = 0; index < item_count; ++index) {
			uint16_t id = reader.read<uint16_t>();
			uint8_t amount = reader.read<uint8_t>();
			ctx.m_client->m_inventory.m_items[id] = amount;
			reader.skip(1);
		}
	}
}