#pragma once
#include <functional>
#include <list>
#include <unordered_map>
#include <string>
#include <vector>

#include <proton/packet.hpp>

struct EventContext;

class EventPool {
private:
	std::unordered_map<uint32_t, std::list<std::pair<std::string, std::function<void(EventContext&)>>>> m_events;
public:
	void register_text(const std::string& name, std::function<void(EventContext&)> callback) {
		m_events[NET_MESSAGE_GENERIC_TEXT].push_back({ name, callback });
	}

	void register_message(const std::string& name, std::function<void(EventContext&)> callback) {
		m_events[NET_MESSAGE_GAME_MESSAGE].push_back({ name, callback });
	}

	void register_packet(const uint8_t& type, std::function<void(EventContext&)> callback) {
		m_events[NET_MESSAGE_GAME_PACKET].push_back({ "gp_" + std::to_string(type), callback });
	}

	bool execute(const uint32_t& type, const std::string& name, EventContext& ctx) {
		auto it = m_events.find(type);

		if (it == m_events.end())
			return false;

		for (const auto& event : it->second) {
			if (name != event.first)
				continue;

			event.second(ctx);
			return true;
		}

		return false;
	}
};