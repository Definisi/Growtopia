#pragma once
#include <string>
#include <cstdint>

class BinaryReader {
public:
	BinaryReader(uint8_t* data, const size_t& size) : m_offset(0), m_size(size) {
		m_data = new uint8_t[size];
		std::memcpy(m_data, data, size);
	}
	~BinaryReader() {
		delete[] m_data;
	}

	template<typename T>
	T read() {
		T value;
		std::memcpy(&value, m_data + m_offset, sizeof(T));
		m_offset += sizeof(T);
		return value;
	}

	std::string read(const size_t& length) {
		std::string value(reinterpret_cast<const char*>(m_data + m_offset), length);
		m_offset += length;
		return value;
	}

	void skip(const size_t& length) {
		m_offset += length;
	}

	size_t get_offset() {
		return m_offset;
	}

	size_t get_size() {
		return m_size;
	}
private:
	uint8_t* m_data;
	size_t m_offset;
	size_t m_size;
};