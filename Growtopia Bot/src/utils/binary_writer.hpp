#pragma once
#include <cstdint>
#include <string>

class BinaryWriter {
public:
	BinaryWriter(uint8_t* data, const size_t& offset = 0) : m_data(data), m_offset(offset) {}
	BinaryWriter(const size_t& size) : m_offset(0), m_size(size) {
		m_data = (uint8_t*)std::malloc(size);
		std::memset(m_data, 0, size);
	}
	~BinaryWriter() {
		std::free(m_data);
	}

	template<typename T>
	void write(const T& value) {
		std::memcpy(m_data + m_offset, &value, sizeof(T));
		m_offset += sizeof(T);
	}

	void write(const std::string& value) {
		std::memcpy(m_data + m_offset, value.c_str(), value.length());
		m_offset += value.length();
	}

	void write(const uint8_t* data, const size_t& size) {
		std::memcpy(m_data + m_offset, data, size);
		m_offset += size;
	}

	void skip(const size_t& length) {
		m_offset += length;
	}

	uint8_t* get() {
		return m_data;
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