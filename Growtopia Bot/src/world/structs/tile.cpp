#include <format>
#include <iostream>

#include <item/item_core.hpp>
#include <utils/get_current_time.hpp>
#include <world/structs/tile.hpp>

void Tile::read_tile_extra(BinaryReader& reader) {
	m_extra_type = reader.read<uint8_t>();
	switch (m_extra_type) {
	case TileExtraType::NONE:
		break;
	case TileExtraType::DOOR: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		reader.skip(1);
		break;
	}
	case TileExtraType::SIGN: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		reader.skip(4);
		break;
	}
	case TileExtraType::LOCK: {
		m_lock_flag = reader.read<uint8_t>();
		m_owner_uid = reader.read<uint32_t>();
		uint32_t access_count = reader.read<uint32_t>();
		for (int index = 0; index < access_count; ++index) {
			uint32_t uid = reader.read<uint32_t>();
			m_access_list.push_back(uid);
		}
		break;
	}
	case TileExtraType::SEED: {
		const Item& item = item_database->get_item(m_foreground);

		m_ready_time = get_current_time<std::chrono::seconds>() + item.m_grow_time - reader.read<uint32_t>();
		m_fruit_count = reader.read<uint8_t>();
		break;
	}
	case TileExtraType::FOSSIL:
		break;
	case TileExtraType::RANDOM_BLOCK: {
		m_display_item = reader.read<uint8_t>();
		break;
	}
	case TileExtraType::PROVIDER: {
		m_ready_time = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::HEART_MONITOR: {
		m_display_item = reader.read<uint32_t>();
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		break;
	}
	case TileExtraType::MANNEQUIN: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		reader.skip(23);
		break;
	}
	case TileExtraType::MAGIC_EGG: {
		m_display_item = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::GAME_BLOCK: {
		m_display_item = reader.read<uint8_t>();
		break;
	}
	case TileExtraType::PHONE_BOOTH: {
		reader.skip(18);
		break;
	}
	case TileExtraType::CRIME_VILLAIN: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		m_ready_time = reader.read<uint32_t>();
		reader.skip(1);
		break;
	}
	case TileExtraType::SPOTLIGHT:
		break;
	case TileExtraType::DISPLAY_BLOCK: {
		m_display_item = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::VENDING_MACHINE: {
		m_display_item = reader.read<uint32_t>();
		reader.skip(4);
		break;
	}
	case TileExtraType::GIVING_TREE: {
		reader.skip(1);
		m_ready_time = reader.read<uint32_t>();
		reader.skip(1);
		break;
	}
	case TileExtraType::SILKWORM: {
		reader.skip(1);
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		reader.skip(4);
		m_ready_time = reader.read<uint32_t>();
		reader.skip(17);
		break;
	}
	case TileExtraType::LOBSTER_TRAP:
		break;
	case TileExtraType::PAINTING_EASEL: {
		m_display_item = reader.read<uint32_t>();
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		break;
	}
	case TileExtraType::WEATHER_SPECIAL: {
		m_display_item = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::DISPLAY_SHELF: {
		reader.skip(16);
		break;
	}
	case TileExtraType::VIP_ENTRANCE: {
		m_access_list.clear();
		reader.skip(1);
		m_owner_uid = reader.read<uint32_t>();
		uint32_t access_count = reader.read<uint32_t>();
		for (int index = 0; index < access_count; ++index) {
			uint32_t uid = reader.read<uint32_t>();
			m_access_list.push_back(uid);
		}
		break;
	}
	case TileExtraType::FISH_MOUNT: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		m_display_item = reader.read<uint32_t>();
		reader.skip(1);
		break;
	}
	case TileExtraType::PORTRAIT: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		reader.skip(22);
		break;
	}
	case TileExtraType::WEATHER_SPECIAL2: {
		m_display_item = reader.read<uint32_t>();
		reader.skip(5);
		break;
	}
	case TileExtraType::FOSSIL_PREP_STATION: {
		m_ready_time = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::DNA_PROCESSOR:
		break;
	case TileExtraType::COOKING_OVEN: {
		reader.skip(4);
		uint32_t ingredient_count = reader.read<uint32_t>();
		for (uint32_t index = 1; index < ingredient_count; ++index) {
			reader.skip(8);
		}
		reader.skip(4);
		break;
	}
	case TileExtraType::GEIGER_CHARGER: {
		m_ready_time = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::MAGPLANT: {
		m_display_item = reader.read<uint32_t>();
		reader.skip(10);
		break;
	}
	default:
		std::cout << std::format("Foreground -> {}", m_foreground) << std::endl;
		std::cout << std::format("X -> {}", m_pos.m_x) << std::endl;
		std::cout << std::format("Y -> {}", m_pos.m_y) << std::endl;
		std::cout << std::format("Unknown Tile Extra Type -> {}", m_extra_type) << std::endl;
		break;
	}
}

bool Tile::is_ready() const {
	if (m_extra_type == TileExtraType::SEED) {
		return get_current_time<std::chrono::seconds>() >= m_ready_time;
	}
	return false;
}