#include <discord/discord_register.hpp>
#include <discord/discord_rpc.hpp>
#include <stdio.h>
#include <string>
#include <time.h>

extern std::string discord_uid;
extern std::string discord_name;

void handle_discord_ready(const DiscordUser* u) {
    discord_uid = u->userId;
    discord_name = u->username;
}


void init_discord_rpc() {
    DiscordEventHandlers handlers;
    memset(&handlers, 0, sizeof(handlers));
    handlers.ready = handle_discord_ready;
    Discord_Initialize("1199262153413242952", &handlers, 1, NULL);
}