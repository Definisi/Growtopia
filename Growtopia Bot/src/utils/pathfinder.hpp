#pragma once
#include <algorithm>
#include <cmath>
#include <vector>
#include <queue>

#include <client/client.hpp>
#include <item/item_core.hpp>
#include <world/structs/tile.hpp>

class Client;

struct Node {
	int x, y;
	float f, g, h;
	Node* parent;

	Node(int x, int y, float g, float h, Node* parent)
		: x(x), y(y), g(g), h(h), f(g + h), parent(parent) {}
};

class Pathfinder {
public:
	Pathfinder(std::shared_ptr<Client> client, std::vector<Tile>& m_tiles, int m_width, int m_height)
		: m_client(client), m_tiles(m_tiles), m_width(m_width), m_height(m_height) {}

	std::vector<Tile*> find(int start_x, int start_y, int end_x, int end_y) {
		std::priority_queue<Node*, std::vector<Node*>, NodeCompare> open_set;
		std::vector<bool> closed_set(m_width * m_height, false);

		open_set.push(new Node(start_x, start_y, 0.0f, heuristic(start_x, start_y, end_x, end_y), nullptr));

		while (!open_set.empty()) {
			Node* current = open_set.top();
			open_set.pop();

			if (current->x == end_x && current->y == end_y) {
				std::vector<Tile*> path;
				while (current) {
					path.push_back(&m_tiles[current->x + current->y * m_width]);
					current = current->parent;
				}
				cleanup(open_set);
				return path;
			}

			closed_set[current->x + current->y * m_width] = true;

			int dx[4] = { -1, 1, 0, 0 };
			int dy[4] = { 0, 0, -1, 1 };

			for (int i = 0; i < 4; ++i) {
				int new_x = current->x + dx[i];
				int new_y = current->y + dy[i];

				if (!is_valid(new_x, new_y))
					continue;

				const Tile& tile = m_tiles[new_x + new_y * m_width];
				const Item& item = item_database->get_item(tile.m_foreground);

				if (item.m_collision_type == TileCollisionType::FULL_COLLISION)
					continue;

				if (item.m_collision_type == TileCollisionType::COLLISION_ON_TOP && dy[i] > 0)
					continue;

				if (item.m_collision_type == TileCollisionType::NO_COLLISION_IF_ACCESSED && !(tile.m_flags & TileFlag::PUBLIC) && m_client->m_world.m_owner_uid != m_client->m_player.m_user_id && std::find(m_client->m_world.m_access_list.begin(), m_client->m_world.m_access_list.end(), m_client->m_player.m_user_id) == m_client->m_world.m_access_list.end())
					continue;

				if (item.m_collision_type == TileCollisionType::ONE_SIDE_COLLISION_HORIZONTAL && !(tile.m_flags & TileFlag::FLIPPED) && dx[i] < 0)
					continue;
				if (item.m_collision_type == TileCollisionType::ONE_SIDE_COLLISION_HORIZONTAL && (tile.m_flags & TileFlag::FLIPPED) && dx[i] > 0)
					continue;
				if (item_database->get_item(m_tiles[current->x + current->y * m_width].m_foreground).m_collision_type == TileCollisionType::ONE_SIDE_COLLISION_HORIZONTAL && !(m_tiles[current->x + current->y * m_width].m_flags & TileFlag::FLIPPED) && dx[i] < 0)
					continue;
				if (item_database->get_item(m_tiles[current->x + current->y * m_width].m_foreground).m_collision_type == TileCollisionType::ONE_SIDE_COLLISION_HORIZONTAL && (m_tiles[current->x + current->y * m_width].m_flags & TileFlag::FLIPPED) && dx[i] > 0)
					continue;

				if (item.m_collision_type == TileCollisionType::NO_COLLISION_IF_VIP && m_client->m_world.m_owner_uid != m_client->m_player.m_user_id && std::find(tile.m_access_list.begin(), tile.m_access_list.end(), m_client->m_player.m_user_id) == tile.m_access_list.end())
					continue;

				if (!closed_set[new_x + new_y * m_width]) {
					float new_g = current->g + 1.0f;
					float new_h = heuristic(new_x, new_y, end_x, end_y);
					Node* neighbor = new Node(new_x, new_y, new_g, new_h, current);
					open_set.push(neighbor);
				}
			}
		}

		cleanup(open_set);
		return std::vector<Tile*>();
	}
private:
	struct NodeCompare {
		bool operator()(const Node* lhs, const Node* rhs) {
			return lhs->f > rhs->f;
		}
	};

	float heuristic(int x1, int y1, int x2, int y2) {
		return std::abs(x1 - x2) + std::abs(y1 - y2);
	}

	bool is_valid(int x, int y) {
		return x >= 0 && x < m_width && y >= 0 && y < m_height;
	}

	void cleanup(std::priority_queue<Node*, std::vector<Node*>, NodeCompare>& pq) {
		while (!pq.empty()) {
			delete pq.top();
			pq.pop();
		}
	}

	std::shared_ptr<Client> m_client;
	std::vector<Tile>& m_tiles;
	int m_width, m_height;
};