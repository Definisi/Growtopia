#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <item/item.hpp>

class ItemDatabase {
private:
	std::vector<Item> m_items;

	uint8_t* m_data;

	uint16_t m_version;
public:
	ItemDatabase() = default;
	~ItemDatabase();

	void deinitialize();
	const Item& get_item(const uint32_t& id) const;
	bool initialize(const std::string& path);
	bool is_background(const uint32_t& id);
};

extern ItemDatabase* item_database;