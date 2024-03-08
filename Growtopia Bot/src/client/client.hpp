#pragma once
#include <cstdint>
#include <mutex>
#include <memory>

#include <client/login_info.hpp>

class Client : public std::enable_shared_from_this<Client> {
private:
    ENetHost* m_host;
    ENetPeer* m_peer;

    std::mutex m_mutex;

    uint64_t m_last_connected;

    LoginInfo m_login_info;
public:
    Client();
    ~Client();
    
    void set_socks5_info(const std::string& ip, const uint16_t port);
    void set_socks5_info(const std::string& ip, const uint16_t port, const std::string& username, const std::string& password);

    bool connect();
};