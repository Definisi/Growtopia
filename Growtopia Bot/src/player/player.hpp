#pragma once
#include <cstdint>

#include <proton/vector.hpp>

namespace CharacterState {
	enum : int32_t {
		NO_CLIP = 1 << 0,
		DOUBLE_JUMP = 1 << 1
	};
}

struct Player {
	Vector2i m_pos;

	int32_t m_net_id;
	int32_t m_character_state;

	uint32_t m_user_id;
};