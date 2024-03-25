#include <events/registered/game_packet/item_change_object.hpp>

void events::item_change_object(EventContext& ctx)
{
    std::lock_guard<std::mutex> lock(ctx.m_client->m_mutex);
    GameUpdatePacket* game_packet_in = ctx.m_game_packet;
    if (game_packet_in->m_net_id == -1) {
        FloatingItem m_floating_item;
        m_floating_item.item_id = game_packet_in->m_int_data;
        m_floating_item.pos.m_x = game_packet_in->m_pos_x;
        m_floating_item.pos.m_y = game_packet_in->m_pos_y;
        m_floating_item.amount = static_cast<uint8_t>(game_packet_in->m_float_var);
        m_floating_item.flags = 0;
        m_floating_item.drop_id_offset = ++ctx.m_client->m_world.m_last_floating_item_offset;
        ctx.m_client->m_world.m_floating_items.push_back(m_floating_item);
    }
    else if (game_packet_in->m_net_id == -3) {
        for (auto& floating_item : ctx.m_client->m_world.m_floating_items) {
            if (floating_item.item_id == game_packet_in->m_int_data &&
                floating_item.pos.m_x == game_packet_in->m_pos_x &&
                floating_item.pos.m_y == game_packet_in->m_pos_y) {
                floating_item.amount = static_cast<uint8_t>(game_packet_in->m_float_var);
                break;
            }
        }
    }
    else if (game_packet_in->m_net_id > 0) {
        for (auto it = ctx.m_client->m_world.m_floating_items.begin(); it != ctx.m_client->m_world.m_floating_items.end(); ++it) {
            if (it->drop_id_offset == game_packet_in->m_int_data) {
                if (game_packet_in->m_net_id == ctx.m_client->m_player.m_net_id) {
                    if (it->item_id == 112) {
                        ctx.m_client->m_gem_count += it->amount;
                    }
                    else {
                        auto& inventory_items = ctx.m_client->m_inventory.m_items;
                        auto item_iter = inventory_items.find(it->item_id);
                        if (item_iter != inventory_items.end()) {
                            item_iter->second += it->amount;
                            if (item_iter->second > 200)
                                item_iter->second = 200;
                        }
                        else {
                            inventory_items[it->item_id] = it->amount;
                        }
                    }
                }
                ctx.m_client->m_world.m_floating_items.erase(it);
                break;
            }
        }
    }
}
