#include <fstream>
#include <iostream>
#include <item/item_core.hpp>
#include <item/types.hpp>
#include <utils/binary_reader.hpp>

ItemDatabase* item_database = new ItemDatabase();

ItemDatabase::~ItemDatabase() {
	delete[] m_data;
}

void ItemDatabase::deinitialize() {
	delete[] m_data;
	m_items.clear();
}

const Item& ItemDatabase::get_item(const uint32_t& id) const {
	if (id > m_items.size() || id < 0)
		return m_items.at(0);
	return m_items.at(id);
}

bool ItemDatabase::initialize(const std::string& path) {
	std::ifstream file(path, std::ios::binary);

	if (!file) {
		std::cerr << "items.dat: No such file or directory" << std::endl;
		return false;
	}

	file.seekg(0, std::ios::end);
	std::streampos size = file.tellg();
	file.seekg(0, std::ios::beg);

	m_data = new uint8_t[size];

	file.read(reinterpret_cast<char*>(m_data), size);
	file.close();

	std::string key = "PBG892FXX982ABC*";

	BinaryReader reader(m_data, size);
	m_version = reader.read<uint16_t>();
	uint32_t amount = reader.read<uint32_t>();

	for (size_t index = 0; index < amount; ++index) {
		Item item;
		item.m_id = reader.read<uint32_t>();

		item.m_editable_type = reader.read<uint8_t>();
		item.m_item_category = reader.read<uint8_t>();
		item.m_action_type = reader.read<uint8_t>();
		item.m_hit_sound_type = reader.read<uint8_t>();

		{
			uint16_t length = reader.read<uint16_t>();
			std::string str = reader.read(length);
			std::string name;

			for (size_t pos = 0; pos < length; ++pos) {
				name += str[pos] ^ (key[(pos + item.m_id) % key.length()]);
			}
		}

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_texture = reader.read(length);
		}

		item.m_texture_hash = reader.read<int32_t>();
		item.m_item_kind = reader.read<uint8_t>();
		item.m_val1 = reader.read<int32_t>();
		item.m_texture_x = reader.read<uint8_t>();
		item.m_texture_y = reader.read<uint8_t>();
		item.m_spread_type = reader.read<uint8_t>();
		item.m_is_stripey_wallpaper = reader.read<uint8_t>();
		item.m_collision_type = reader.read<uint8_t>();
		item.m_break_hits = reader.read<uint8_t>() / 6;
		item.m_reset_time = reader.read<int32_t>();
		item.m_clothing_type = reader.read<uint8_t>();
		item.m_rarity = reader.read<uint16_t>();
		item.m_max_amount = reader.read<uint8_t>();

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_extra_file = reader.read(length);
		}

		item.m_extra_file_hash = reader.read<int32_t>();
		item.m_audio_volume = reader.read<int32_t>();

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_pet_name = reader.read(length);
		}

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_pet_prefix = reader.read(length);
		}

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_pet_suffix = reader.read(length);
		}

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_pet_ability = reader.read(length);
		}

		item.m_seed_base = reader.read<uint8_t>();
		item.m_seed_overlay = reader.read<uint8_t>();
		item.m_tree_base = reader.read<uint8_t>();
		item.m_tree_leaves = reader.read<uint8_t>();
		item.m_seed_color = reader.read<uint32_t>();
		item.m_seed_overlay_color = reader.read<uint32_t>();
		item.m_ingredient = reader.read<int32_t>();
		item.m_grow_time = reader.read<uint32_t>();
		item.m_val2 = reader.read<uint16_t>();
		item.m_rayman = reader.read<uint16_t>();

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_extra_options = reader.read(length);
		}

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_texture2 = reader.read(length);
		}

		{
			uint16_t length = reader.read<uint16_t>();
			item.m_extra_options2 = reader.read(length);
		}

		item.bytes_80 = std::vector<uint8_t>((uint8_t*)reader.get_offset(), (uint8_t*)reader.get_offset() + 80);
		reader.skip(80);

		if (m_version >= 11) {
			uint16_t length = reader.read<uint16_t>();
			item.m_punch_options = reader.read(length);
		}

		if (m_version >= 12) {
			item.m_val3 = reader.read<uint32_t>();

			for (size_t part = 0; part < 9; ++part)
				item.m_body_parts[part] = reader.read<uint8_t>();
		}
		
		if (m_version >= 13) {
			item.m_val4 = reader.read<uint32_t>();
		}
		
		if (m_version >= 14) {
			item.m_val5 = reader.read<uint32_t>();
		}

		if (m_version >= 15) {
			reader.skip(25);

			{
				uint16_t length = reader.read<uint16_t>();
				reader.skip(length);
			}
		}

		if (m_version >= 16) {
			uint16_t length = reader.read<uint16_t>();
			reader.skip(length);
		}

		if (index != item.m_id) {
			std::cerr << "Items are unordered " << index << "/" << item.m_id << std::endl;
			return false;
		}

		switch (item.m_action_type) {
		case ITEM_TYPE_SEED:
		case ITEM_TYPE_HEART_MONITOR:
		case ITEM_TYPE_VENDING_MACHINE: {
			item.m_has_extra = true;
			break;
		}
		}

		m_items.push_back(item);
	}
	return true;
}

bool ItemDatabase::is_background(const uint32_t& id) {
	const Item& item = this->get_item(id);
	return item.m_action_type == ITEM_TYPE_BACKGROUND ||
		item.m_action_type == ITEM_TYPE_BACKGD_SFX_EXTRA_FRAME ||
		item.m_action_type == ITEM_TYPE_MUSIC_NOTE;
}