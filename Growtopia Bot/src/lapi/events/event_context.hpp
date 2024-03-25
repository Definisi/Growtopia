#pragma once
#include <proton/variant.hpp>

#include <client/client.hpp>

class Client;
namespace lua::events {
	struct EventContext {
		std::string m_name;
		std::shared_ptr<Client> m_client;
		VariantList m_varlist;
	};
}