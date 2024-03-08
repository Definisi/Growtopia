#include <client/login_info.hpp>

#include <algorithm>

#include <proton/hash.hpp>

#include <utils/generate_random_uuid.hpp>
#include <utils/generate_random_hex.hpp>

LoginInfo::LoginInfo() {
    m_gid = utils::generate_random_uuid();
    m_rid = utils::generate_random_hex(32);
    std::transform(m_rid.begin(), m_rid.end(), m_rid.begin(), ::toupper);
    m_hash = hash::proton(m_gid.c_str(), m_gid.length());
}

void LoginInfo::reset() {
	m_address = "";
	m_port = 0;
	m_meta = "";

	m_gid = utils::generate_random_uuid();
	m_rid = utils::generate_random_hex(32);
	std::transform(m_rid.begin(), m_rid.end(), m_rid.begin(), ::toupper);
	m_hash = hash::proton(m_gid.c_str(), m_gid.length());
}