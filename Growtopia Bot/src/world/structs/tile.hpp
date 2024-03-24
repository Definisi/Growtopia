#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <proton/vector.hpp>

#include <utils/binary_reader.hpp>

namespace TileCollisionType {
	enum : uint8_t {
		NO_COLLISION = 0,
		FULL_COLLISION = 1,
		COLLISION_ON_TOP = 2,
		NO_COLLISION_IF_ACCESSED = 3,
		ONE_SIDE_COLLISION_HORIZONTAL = 5,
		NO_COLLISION_IF_VIP = 6
	};
}

namespace TileExtraType {
	enum : uint8_t {
		NONE = 0,
		DOOR = 1,
		SIGN = 2,
		LOCK = 3,
		SEED = 4,
		FOSSIL = 5,
		RANDOM_BLOCK = 8,
		PROVIDER = 9,
		ACHIEVEMENT_BLOCK = 10,
		HEART_MONITOR = 11,
		MANNEQUIN = 14,
		MAGIC_EGG = 15,
		GAME_BLOCK = 16,
		XENONITE_CRYSTAL = 18,
		PHONE_BOOTH = 19,
		CRYSTAL = 20,
		CRIME_VILLAIN = 21,
		SPOTLIGHT = 22,
		DISPLAY_BLOCK = 23,
		VENDING_MACHINE = 24,
		FISH_TANK_PORT = 25,
		SOLAR_COLLECTOR = 26,
		FORGE = 27,
		GIVING_TREE = 28,
		SILKWORM = 31,
		SEWING_MACHINE = 32,
		COUNTRY_FLAG = 33,
		LOBSTER_TRAP = 34,
		PAINTING_EASEL = 35,
		BATTLE_PET_CAGE = 36,
		PET_TRAINER = 37,
		WEATHER_SPECIAL = 40,
		DISPLAY_SHELF = 43,
		VIP_ENTRANCE = 44,
		FISH_MOUNT = 47,
		PORTRAIT = 48,
		WEATHER_SPECIAL2 = 49,
		FOSSIL_PREP_STATION = 50,
		DNA_PROCESSOR = 51,
		CHEMSYNTH_TANK = 53,
		STORAGE_BOX = 54,
		COOKING_OVEN = 55,
		AUDIO_BLOCK = 56,
		GEIGER_CHARGER = 57,
		ADVENTURE_BEGIN = 58,
		TOMB_ROBBER = 59,
		TRAINING_PORT = 61,
		MAGPLANT = 62,
		DISAPPEAR_WHEN_STEPPED_ON = 73,
		SAFE_VAULT = 74,
		INFINITY_WEATHER_MACHINE = 77,
		KRANKEN_S_GALACTIC_BLOCK = 80
	};
}

namespace TileFlag {
	enum : uint16_t {
		NONE = 0,
		TILE_EXTRA = 1 << 0,
		FLIPPED = 1 << 5,
		OPEN = 1 << 6,
		PUBLIC = 1 << 7
	};
}

struct Tile {
	Vector2i m_pos;
	uint16_t m_foreground;
	uint16_t m_background;
	uint16_t m_lock_parent;
	uint16_t m_flags;
	uint8_t m_extra_type;

	std::string m_label;

	uint8_t m_lock_flag;
	uint16_t m_owner_uid;
	std::vector<uint32_t> m_access_list;

	uint64_t m_ready_time;
	uint8_t m_fruit_count;

	uint32_t m_display_item;

	void read_tile_extra(BinaryReader& reader);
	bool is_ready() const;
};