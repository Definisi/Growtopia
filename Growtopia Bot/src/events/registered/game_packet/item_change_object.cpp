#include <events/registered/game_packet/item_change_object.hpp>

void events::item_change_object(EventContext& ctx)
{
    std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);
    GameUpdatePacket* game_packet_in = ctx.m_game_packet;
    int net_id = game_packet_in->m_net_id;

    if (net_id == -1) {
        FloatingItem m_floating_item;
        m_floating_item.m_item_id = game_packet_in->m_int_data;
        m_floating_item.m_pos.m_x = game_packet_in->m_pos_x;
        m_floating_item.m_pos.m_y = game_packet_in->m_pos_y;
        m_floating_item.m_amount = static_cast<uint8_t>(game_packet_in->m_float_var);
        m_floating_item.m_flags = 0;
        m_floating_item.m_drop_id_offset = ++ctx.m_client->m_world.m_last_floating_item_offset;

        ctx.m_client->m_world.m_floating_items.push_back(m_floating_item);
    }
    else if (net_id == -3) {
        for (auto& floating_item : ctx.m_client->m_world.m_floating_items) {
            if (floating_item.m_item_id == game_packet_in->m_int_data &&
                floating_item.m_pos.m_x == game_packet_in->m_pos_x &&
                floating_item.m_pos.m_y == game_packet_in->m_pos_y) {
                floating_item.m_amount = static_cast<uint8_t>(game_packet_in->m_float_var);
                break;
            }
        }
    }
    else if (net_id > 0) {
        auto& floating_items = ctx.m_client->m_world.m_floating_items;
        for (auto it = floating_items.begin(); it != floating_items.end(); ++it) {
            if (it->m_drop_id_offset == game_packet_in->m_int_data) {
                FloatingItem& floating_item = *it;
                if (game_packet_in->m_net_id == ctx.m_client->m_player.m_net_id) {
                    if (floating_item.m_item_id == 112) {
                        ctx.m_client->m_gem_count += floating_item.m_amount;
                    }
                    else {
                        auto& inventory_items = ctx.m_client->m_inventory.m_items;
                        auto item_iter = inventory_items.find(floating_item.m_item_id);
                        if (item_iter != inventory_items.end()) {
                            item_iter->second += floating_item.m_amount;
                            if (item_iter->second > 200)
                                item_iter->second = 200;
                        }
                        else {
                            inventory_items[floating_item.m_item_id] = floating_item.m_amount;
                        }
                    }
                }
                floating_items.erase(it);
                break;
            }
        }
    }
}
