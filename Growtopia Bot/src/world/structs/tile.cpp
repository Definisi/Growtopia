#include <format>
#include <iostream>

#include <item/item_core.hpp>
#include <utils/get_current_time.hpp>
#include <world/structs/tile.hpp>

void Tile::read_tile_extra(BinaryReader& reader, uint16_t world_version) {
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
		reader.skip(8);
		if (m_foreground == 5814) {// Guild Lock
			reader.skip(16);
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
		if (m_foreground != 5318 && (m_foreground != 10656) || world_version < 17) {
			break;
		}
		reader.skip(4);
		break;
	}

	case TileExtraType::ACHIEVEMENT_BLOCK: {
		reader.skip(4);
		reader.skip(1);
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
	case TileExtraType::XENONITE_CRYSTAL: {
		reader.skip(1);
		reader.skip(4);
		break;
	}
	case TileExtraType::PHONE_BOOTH: {
		reader.skip(18);
		break;
	}
	case TileExtraType::CRYSTAL: {
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);
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
		m_price_item = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::FISHTANK: {
		reader.skip(1);
		uint32_t length = reader.read<uint32_t>();
		reader.skip(length * 4);
		break;
	}

	case TileExtraType::SOLAR: {
		reader.skip(1);
		reader.skip(4);
		break;
	}

	case TileExtraType::DECO: {
		reader.skip(1);
		m_ready_time = reader.read<uint32_t>();
		reader.skip(1);
		break;
	}
	case TileExtraType::STEAM_ORGAN: {
		reader.skip(1);
		reader.skip(4);
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
	case TileExtraType::SEWING_MACHINE: {
		uint32_t length = reader.read<uint32_t>();
		reader.skip(length * 4);
		break;
	}
	case TileExtraType::COUNTRY_FLAG: {
		if (m_foreground != 3394) {
			break;
		}
		uint16_t length = reader.read<uint16_t>();
		reader.read(length); // ex : us | id | my ....
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
	case TileExtraType::BATTLE_PET_CAGE: {
		uint16_t length = reader.read<uint16_t>();
		m_label = reader.read(length);
		reader.skip(12);

		break;
	}
	case TileExtraType::PET_TRAINER: {
		uint16_t length = reader.read<uint16_t>();
		reader.skip(length);
		uint32_t length2 = reader.read<uint32_t>();
		reader.skip(length2 * 4);
		break;
	}
	case TileExtraType::STEAM_ENGINE: {
		reader.skip(4);
		break;
	}
	case TileExtraType::LOCK_BOT: {
		reader.skip(4);
		break;
	}
	case TileExtraType::WEATHER_SPECIAL: {
		m_display_item = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::SPIRIT_STORAGE: {
		reader.skip(4);
		break;
	}
	case TileExtraType::DATA_BEDROCK: {
		reader.skip(21);
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
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		uint16_t face_clothing = reader.read<uint16_t>();
		uint16_t hat_clothing = reader.read<uint16_t>();
		uint16_t back_clothing = reader.read<uint16_t>();
		if (world_version > 3) {
			if (face_clothing == 5712 || face_clothing == 10044 || // will of the wild or golems gift
				hat_clothing == 5712 || hat_clothing == 10044 ||
				back_clothing == 5712 || back_clothing == 10044) {
				reader.skip(4);
				reader.skip(4);
			}
		}
		if (world_version > 8) {
			reader.skip(4);
		}
		if (world_version > 22 && hat_clothing == 12958) { // infinity crown
			uint16_t length = reader.read<uint16_t>();
			reader.skip(length);
		}
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

	case TileExtraType::CHEMSYNTH_TANK: {
		reader.skip(4);
		reader.skip(4);
		break;
	}
	case TileExtraType::STORAGE_BOX: {
		uint16_t length = reader.read<uint16_t>();
		reader.skip(length);
		break;
	}
	case TileExtraType::COOKING_OVEN: {
		reader.skip(4);
		uint32_t ingredient_count = reader.read<uint32_t>();
		for (uint32_t index = 1; index < ingredient_count; ++index) {
			reader.skip(8);
		}
		reader.skip(4);
		break;
	}
	case TileExtraType::AUDIO_BLOCK: {
		uint16_t length = reader.read<uint16_t>();
		reader.skip(length);
		reader.skip(4);
		break;
	}
	case TileExtraType::GEIGER_CHARGER: {
		m_ready_time = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::FACTION: {
		reader.skip(1);
		reader.skip(4);
		break;
	}
	case TileExtraType::TRAINING_PORT: {
		reader.skip(35);
		break;
	}
	case TileExtraType::MAGPLANT: {
		m_display_item = reader.read<uint32_t>();
		reader.skip(10);
		break;
	}
	case TileExtraType::ROBOT: {
		uint32_t length = reader.read<uint32_t>();
		for (int i = 0; i < length; i++) {
			reader.skip(4);
			reader.skip(1);
			reader.skip(4);
			reader.skip(4);
			uint16_t length2 = reader.read<uint16_t>();
			reader.skip(length2);
		}
		reader.skip(4);
		reader.skip(4);
		break;
	}
	case TileExtraType::GUILD_ITEM: {
		reader.skip(1);
		reader.skip(16);
		break;
	}
	case TileExtraType::GROWSCAN9000: {
		reader.skip(1);
		break;
	}
	case TileExtraType::FIELD_NODE: {
		reader.skip(4);
		uint32_t length = reader.read<uint32_t>();
		reader.skip(length * 4);
		break;
	}
	case TileExtraType::SPIRIT_BOARD: {
		reader.skip(4);

		uint16_t length = reader.read<uint16_t>();
		reader.skip(length);

		length = reader.read<uint16_t>();
		reader.skip(length);


		uint32_t length2 = reader.read<uint32_t>();
		reader.skip(length2 * 4);
		break;
	}
	case TileExtraType::SUCKER2: {
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		if (world_version > 7)
			reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(1);
		reader.skip(1);
		reader.skip(4);
		reader.skip(4);

		break;
	}
	case TileExtraType::LIGHTNIG_IF_ON: {
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		break;
	}
	case TileExtraType::PHASED_BLOCK: {
		reader.skip(4);
		break;
	}	
	case TileExtraType::PHASED_BLOCK2: {
		reader.skip(4);
		uint16_t length = reader.read<uint16_t>();
		reader.skip(length);
		break;
	}
	case TileExtraType::INFINITY_WEATHER_MACHINE: {
		reader.skip(4);
		uint32_t length2 = reader.read<uint32_t>();
		reader.skip(length2 * 4);
		break;
	}
	case TileExtraType::FEEDING_BLOCK: {
		reader.skip(4);
		break;
	}
	case TileExtraType::KRANKEN_S_GALACTIC_BLOCK: {
		reader.skip(4);
		reader.skip(4);
		break;	
	}
	case TileExtraType::FRIENDS_ENTRANCE: {
		reader.skip(4);
		reader.skip(4);
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