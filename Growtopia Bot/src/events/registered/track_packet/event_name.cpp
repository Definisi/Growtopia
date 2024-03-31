#include <events/registered/track_packet/event_name.hpp>
#include <iostream>

enum class login_error {
    none = 0,
    unk_1 = 1,
    unk_2 = 2,
    invalid_mac = 3,
    unk_4 = 4,
    bad_logon = 5,  // i forgot when this happens
    unk_6 = 6,
    unk_7 = 7,
    initializing_subservers = 9,
    server_requested_relogon = 10,
    unk_11 = 11,
    login_failed = 12, // i forgot when this happens
    unk_13 = 13,
    account_suspended = 14,
    account_suspended_temporarily = 15,
    permanently_banned = 16,
    server_overloaded = 17, // and too many people logging in
    unk_18 = 18,
    unk_19 = 19,
    unk_20 = 20,
    missing_login_params = 21,
    unk_22 = 22,
    undergoing_maintenance = 23,
    unk_24 = 24,
    ip_banned = 25,
    cant_make_accounts_with_this_ip = 26,
    max_logon_attempts = 27,
    update_required = 28,
    invalid_name = 29,
    unk_30 = 30,
    unk_31 = 31,
    unk_32 = 32,
    unk_33 = 33,
    unk_34 = 34,
    wrong_password = 35,
    too_many_accounts_created = 36,
    unk_37 = 37,
    unk_38 = 38,
    unk_39 = 39,
    unk_40 = 40,
    unk_41 = 41,
    unk_42 = 42,
    unk_43 = 43,
    unk_44 = 44,
    unk_45 = 45,
    unk_46 = 46,
    unk_47 = 47,
    unk_48 = 48,
    unk_49 = 49,
    unk_50 = 50,

    connection_timed_out = 0xbad, // not a login error but uhhh
};

void events::event_name(EventContext& ctx)
{
	//std::cout << ctx.m_scanner.get_all() << std::endl;

    const std::string& error = ctx.m_scanner.get("Authentication_error", 0);
    if (!error.empty()) {
        login_error type = (login_error)std::stoi(error);
        switch (type)
        {
        case login_error::none:
            break;
        case login_error::invalid_mac:
            break;
        case login_error::bad_logon:
            break;
        case login_error::initializing_subservers:
            break;
        case login_error::server_requested_relogon:
            break;
        case login_error::login_failed:
            break;
        case login_error::account_suspended:
            ctx.m_client->status = BotStatus::SUSPENDED;
            break;
        case login_error::account_suspended_temporarily:
            ctx.m_client->status = BotStatus::TEMPBAN;
            break;
        case login_error::permanently_banned:
            ctx.m_client->status = BotStatus::SUSPENDED;
            break;
        case login_error::server_overloaded:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::missing_login_params:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::undergoing_maintenance:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::ip_banned:
            ctx.m_client->status = BotStatus::IPBAN;
            ctx.m_client->m_login_info.reset();
            break;
        case login_error::cant_make_accounts_with_this_ip:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::max_logon_attempts:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::update_required:
            ctx.m_client->status = BotStatus::UPDATE_REQUIRED;
            break;
        case login_error::invalid_name:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::wrong_password:
            ctx.m_client->status = BotStatus::WRONGPASS;
            break;
        case login_error::too_many_accounts_created:
            ctx.m_client->status = BotStatus::LOGINFAILED;
            break;
        case login_error::connection_timed_out:
            break;
        default:
            break;
        }
    }
}
