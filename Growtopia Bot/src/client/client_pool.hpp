#pragma once
#include <client/client.hpp>
#include <vector>

class ClientPool {
private:
    std::mutex m_mutex;
    std::vector<std::shared_ptr<Client>> m_clients;
public:
    ClientPool() = default;
    ~ClientPool() = default;

    int selected_bot = -1;

    void initialize();

    std::shared_ptr<Client> add(const std::string& tank_id_name, const std::string& tank_id_pass);
    std::shared_ptr<Client> get(const std::string& tank_id_name);
    void remove(const std::string& tank_id_name);

    std::vector<std::shared_ptr<Client>> get_clients();
};

extern ClientPool* client_pool;