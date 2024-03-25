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
	case TileExtraType::MAILBOX:
	case TileExtraType::BULLETIN:
	case TileExtraType::DONATION_BOX:
	case TileExtraType::TOYBOX: {
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);

		length = reader.read<uint16_t>();
		reader.read(length);

		length = reader.read<uint16_t>();
		reader.read(length);

		reader.skip(1);

		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}

	case TileExtraType::RANDOM_BLOCK: {
		m_display_item = reader.read<uint8_t>();
		break;
	}
	case TileExtraType::PROVIDER: {
		m_ready_time = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::ACHIEVEMENT_BLOCK: {
		reader.skip(5);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
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
	case TileExtraType::EXTRA18: {
		reader.skip(5);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
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
		reader.skip(4);
		break;
	}
	case TileExtraType::SOLAR_COLLECTOR: {
		reader.skip(1);
		uint32_t size = reader.read<uint32_t>();
		reader.skip(4 * size);
		break;
	}
	case TileExtraType::EXTRA26: {
		reader.skip(5);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::FORGE: {
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::GIVING_TREE: {
		reader.skip(1);
		m_ready_time = reader.read<uint32_t>();
		reader.skip(1);
		break;
	}
	case TileExtraType::EXTRA29:{
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA30: {
		reader.skip(5);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
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
		reader.read<uint32_t>();
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::COUNTRY_FLAG: {
		if (m_foreground != 3394) {
			if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
				if (m_foreground == 5814) {
					reader.skip(16);
				}
				else {
					reader.skip(17);
				}
			}
		}
		else {
			uint16_t length = reader.read<uint16_t>();
			reader.read(length);
		}
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
		reader.read(length);
		reader.skip(12);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::PET_TRAINER: {
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);
		reader.skip(32);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::WEATHER_SPECIAL: {
		m_display_item = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::EXTRA42: {
		reader.skip(21);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
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
	case TileExtraType::EXTRA45: {
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
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
		reader.read<uint32_t>();
		reader.read<uint32_t>();
		reader.read<uint32_t>();
		reader.read<uint32_t>();
		uint16_t face_clothing = reader.read<uint16_t>();
		uint16_t hat_clothing = reader.read<uint16_t>();
		uint16_t back_clothing = reader.read<uint16_t>();
		if (world_version > 3) {
			if (face_clothing == 5712 || face_clothing == 10044 ||
				hat_clothing == 5172 || hat_clothing == 10044 ||
				back_clothing == 5712 || back_clothing == 10044) {
				reader.read<uint32_t>();
				reader.read<uint32_t>();
			}
		}
		if (world_version > 8) {
			reader.read<uint32_t>();
		}
		if (world_version > 22 && hat_clothing == 12958) {
			uint16_t length = reader.read<uint16_t>();
			reader.read(length);
		}
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
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
	case TileExtraType::DNA_PROCESSOR: {
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::HOWLER: {
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::CHEMSYNTH_TANK: {
		reader.skip(8);
		break;
	}
	case TileExtraType::STORAGE_BOX: {
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
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
		reader.read(length);
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::GEIGER_CHARGER: {
		m_ready_time = reader.read<uint32_t>();
		break;
	}
	case TileExtraType::ADVENTURE_BEGIN: {
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::TOMB_ROBBER: {
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA60: {
		reader.skip(5);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::TRAINING_PORT: {
		reader.skip(4);
		reader.skip(1);
		reader.skip(1);
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		reader.skip(1);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::MAGPLANT: {
		m_display_item = reader.read<uint32_t>();
		reader.skip(10);
		break;
	}
	case TileExtraType::ROBOT: {
		reader.skip(reader.read<uint32_t>() * 15);
		reader.skip(8);
		break;
	}
	case TileExtraType::EXTRA64: {
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::GROWSCAN9000: {
		reader.skip(1);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA67: {
		reader.skip(4);
		reader.skip(reader.read<uint32_t>() * 4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA68: {
		reader.skip(4);
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);
		length = reader.read<uint16_t>();
		reader.read(length);
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA69:
	case TileExtraType::EXTRA70:
		reader.skip(4);
		reader.skip(4);
		if (world_version < 8u) {
			if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
				if (m_foreground == 5814) {
					reader.skip(16);
				}
				else {
					reader.skip(17);
				}
			}
			break;
		}
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	case TileExtraType::SUCKER2: {
		reader.skip(4);
		reader.skip(4);
		if (world_version >= 8u) {
			reader.skip(4);
		}
		reader.skip(4);
		reader.skip(4);
		reader.skip(1);
		reader.skip(1);
		reader.skip(4);
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA72: {
		reader.skip(4);
		reader.skip(4);
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::DISAPPEAR_WHEN_STEPPED_ON: {
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::SAFE_VAULT: {
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA75: {
		reader.skip(4);
		uint16_t length = reader.read<uint16_t>();
		reader.read(length);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA76: {
		reader.skip(3 * 24);
		reader.skip(4);

		uint32_t size = reader.read<uint32_t>();
		reader.skip(8 * size);
		break;
	}
	case TileExtraType::INFINITY_WEATHER_MACHINE: {
		reader.skip(4);
		uint32_t size = reader.read<uint32_t>();
		reader.skip(size * 4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::KRANKEN_S_GALACTIC_BLOCK: {
		reader.skip(4);
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	case TileExtraType::EXTRA81: {
		reader.skip(4);
		reader.skip(4);
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
	}
	default:
		if (item_database->get_item(m_foreground).bytes_80[9] & 2) {
			if (m_foreground == 5814) {
				reader.skip(16);
			}
			else {
				reader.skip(17);
			}
		}
		break;
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