#pragma once
#include <cstdint>
#include <format>
#include <string>
#include <variant>
#include <vector>

#include <proton/vector.hpp>

#include <utils/binary_reader.hpp>
#include <utils/binary_writer.hpp>

#define C_MAX_VARIANT_LIST_PARMS 7

class Variant {
public:
	enum VariantType : uint8_t {
		NONE,
		FLOAT,
		STRING,
		VECTOR2,
		VECTOR3,
		UINT,
		INT = 9
	};
public:
	using variant_t = std::variant<std::monostate, float, std::string, Vector2, Vector3, uint32_t, int32_t>;
	Variant() = default;
	Variant(const float& value) : m_object({ value }) {}
	Variant(const std::string& value) : m_object({ value }) {}
	Variant(const Vector2& value) : m_object({ value }) {}
	Variant(const Vector3& value) : m_object({ value }) {}
	Variant(const uint32_t& value) : m_object({ value }) {}
	Variant(const int32_t& value) : m_object({ value }) {}
	Variant(const variant_t& value) : m_object({ value }) {}

	void operator=(float value) {
		m_object = value;
	}
	void operator=(std::string value) {
		m_object = value;
	}
	void operator=(Vector2 value) {
		m_object = value;
	}
	void operator=(Vector3 value) {
		m_object = value;
	}
	void operator=(uint32_t value) {
		m_object = value;
	}
	void operator=(int32_t value) {
		m_object = value;
	}
	void operator=(variant_t value) {
		m_object = value;
	}

	template<typename T>
	T get() const {
		return std::get<T>(m_object);
	}

	VariantType get_type() const {
		switch (m_object.index()) {
		case 0:
			return VariantType::NONE;
		case 1:
			return VariantType::FLOAT;
		case 2:
			return VariantType::STRING;
		case 3:
			return VariantType::VECTOR2;
		case 4:
			return VariantType::VECTOR3;
		case 5:
			return VariantType::UINT;
		case 6:
			return VariantType::INT;
		default:
			return VariantType::NONE;
		}
	}

	size_t get_memory_usage() const {
		size_t size = 0;
		switch (this->get_type()) {
		case VariantType::FLOAT:
		case VariantType::UINT:
		case VariantType::INT: {
			size = 5;
			break;
		}
		case VariantType::STRING: {
			size = 5 + this->get<std::string>().size();
			break;
		}
		case VariantType::VECTOR2: {
			size = 1 + (2 * sizeof(float));
			break;
		}
		case VariantType::VECTOR3: {
			size = 1 + (3 * sizeof(float));
			break;
		}
		default:
			break;
		}
		return size;
	}

	void pack(BinaryWriter& writer) {
		writer.write<uint8_t>(static_cast<uint8_t>(this->get_type()));

		switch (this->get_type()) {
		case VariantType::FLOAT: {
			writer.write<float>(this->get<float>());
			break;
		}
		case VariantType::STRING: {
			writer.write<uint32_t>(this->get<std::string>().size());
			writer.write(this->get<std::string>());
			break;
		}
		case VariantType::UINT: {
			writer.write<uint32_t>(this->get<uint32_t>());
			break;
		}
		case VariantType::VECTOR2: {
			const auto& value = this->get<Vector2>();
			writer.write<float>(value.m_x);
			writer.write<float>(value.m_y);
			break;
		}
		case VariantType::VECTOR3: {
			const auto& value = this->get<Vector3>();
			writer.write<float>(value.m_x);
			writer.write<float>(value.m_y);
			writer.write<float>(value.m_z);
			break;
		}
		case VariantType::INT: {
			writer.write<int32_t>(this->get<int32_t>());
			break;
		}
		default:
			break;
		}
	}

	void unpack(BinaryReader& reader) {
		uint8_t type = reader.read<uint8_t>();

		switch (static_cast<VariantType>(type)) {
		case VariantType::FLOAT: {
			m_object = reader.read<float>();
			break;
		}
		case VariantType::STRING: {
			uint32_t length = reader.read<uint32_t>();
			m_object = reader.read(length);
			break;
		}
		case VariantType::VECTOR2: {
			float x = reader.read<float>();
			float y = reader.read<float>();
			m_object = Vector2(x, y);
			break;
		}
		case VariantType::VECTOR3: {
			float x = reader.read<float>();
			float y = reader.read<float>();
			float z = reader.read<float>();
			m_object = Vector3(x, y, z);
			break;
		}
		case VariantType::UINT: {
			m_object = reader.read<uint32_t>();
			break;
		}
		case VariantType::INT: {
			m_object = reader.read<int32_t>();
			break;
		}
		default:
			break;
		}
	}
private:
	variant_t m_object;
};

class VariantList {
private:
	Variant m_objects[C_MAX_VARIANT_LIST_PARMS];
public:
	VariantList() = default;
	VariantList(const Variant::variant_t& v0) {
		m_objects[0] = v0;
	}
	VariantList(const Variant::variant_t& v0, const Variant::variant_t& v1) {
		m_objects[0] = v0;
		m_objects[1] = v1;
	}
	VariantList(const Variant::variant_t& v0, const Variant::variant_t& v1, const Variant::variant_t& v2) {
		m_objects[0] = v0;
		m_objects[1] = v1;
		m_objects[2] = v2;
	}
	VariantList(const Variant::variant_t& v0, const Variant::variant_t& v1, const Variant::variant_t& v2, const Variant::variant_t& v3) {
		m_objects[0] = v0;
		m_objects[1] = v1;
		m_objects[2] = v2;
		m_objects[3] = v3;
	}
	VariantList(const Variant::variant_t& v0, const Variant::variant_t& v1, const Variant::variant_t& v2, const Variant::variant_t& v3, const Variant::variant_t& v4) {
		m_objects[0] = v0;
		m_objects[1] = v1;
		m_objects[2] = v2;
		m_objects[3] = v3;
		m_objects[4] = v4;
	}
	VariantList(const Variant::variant_t& v0, const Variant::variant_t& v1, const Variant::variant_t& v2, const Variant::variant_t& v3, const Variant::variant_t& v4, const Variant::variant_t& v5) {
		m_objects[0] = v0;
		m_objects[1] = v1;
		m_objects[2] = v2;
		m_objects[3] = v3;
		m_objects[4] = v4;
		m_objects[5] = v5;
	}
	VariantList(const Variant::variant_t& v0, const Variant::variant_t& v1, const Variant::variant_t& v2, const Variant::variant_t& v3, const Variant::variant_t& v4, const Variant::variant_t& v5, const Variant::variant_t& v6) {
		m_objects[0] = v0;
		m_objects[1] = v1;
		m_objects[2] = v2;
		m_objects[3] = v3;
		m_objects[4] = v4;
		m_objects[5] = v5;
		m_objects[6] = v6;
	}

	Variant& operator[](uint8_t index) {
		return m_objects[index];
	}

	size_t get_memory_usage() {
		size_t size = 1;
		for (auto& object : m_objects) {
			++size;
			size += object.get_memory_usage();
		}
		return size;
	}

	void deserialize(uint8_t* data) {
		uint8_t count = *(data++);

		for (uint8_t i = 0; i < count; ++i) {
			uint8_t index = *(data++);
			uint8_t type = *(data++);
			switch (static_cast<Variant::VariantType>(type)) {
			case Variant::VariantType::FLOAT: {
				float object;
				std::memcpy(&object, data, sizeof(float));
				data += sizeof(float);

				m_objects[index] = object;
				break;
			}
			case Variant::VariantType::STRING: {
				uint32_t length;
				std::memcpy(&length, data, sizeof(uint32_t));
				data += sizeof(uint32_t);

				std::string object(reinterpret_cast<char*>(data), length);
				data += length;

				m_objects[index] = object;
				break;
			}
			case Variant::VariantType::VECTOR2: {
				Vector2 object = Vector2();

				std::memcpy(&object.m_x, data, sizeof(float));
				data += sizeof(float);

				std::memcpy(&object.m_y, data, sizeof(float));
				data += sizeof(float);

				m_objects[index] = object;
				break;
			}
			case Variant::VariantType::VECTOR3: {
				Vector3 object = Vector3();

				std::memcpy(&object.m_x, data, sizeof(float));
				data += sizeof(float);

				std::memcpy(&object.m_y, data, sizeof(float));
				data += sizeof(float);

				std::memcpy(&object.m_z, data, sizeof(float));
				data += sizeof(float);

				m_objects[index] = object;
				break;
			}
			case Variant::VariantType::UINT: {
				uint32_t object;
				std::memcpy(&object, data, sizeof(uint32_t));
				data += sizeof(uint32_t);

				m_objects[index] = object;
				break;
			}
			case Variant::VariantType::INT: {
				int32_t object;
				std::memcpy(&object, data, sizeof(int32_t));
				data += sizeof(int32_t);

				m_objects[index] = object;
				break;
			}
			default:
				break;
			}
		}
	}

	void deserialize(uint8_t* data, uint32_t size) {
		BinaryReader reader(data, size);
		uint8_t count = reader.read<uint8_t>();

		for (uint8_t i = 0; i < count; ++i) {
			uint8_t index = reader.read<uint8_t>();
			m_objects[index].unpack(reader);
		}
	}

	uint8_t* serialize() {
		BinaryWriter writer(this->get_memory_usage());
		uint8_t count = 0;
		for (uint8_t index = 0; index < C_MAX_VARIANT_LIST_PARMS; ++index) {
			if (m_objects[index].get_type() == Variant::VariantType::NONE)
				continue;
			++count;
		}
		writer.write<uint8_t>(count);

		for (size_t index = 0; index < 8; ++index) {
			if (m_objects[index].get_type() == Variant::VariantType::NONE)
				continue;
			writer.write<uint8_t>(static_cast<uint8_t>(index));
			m_objects[index].pack(writer);
		}

		uint8_t* data = new uint8_t[this->get_memory_usage()];
		std::memcpy(data, writer.get(), writer.get_size());
		return data;
	}

	std::string to_string() {
		std::string str;
		for (uint8_t index = 0; index < C_MAX_VARIANT_LIST_PARMS; ++index) {
			if (m_objects[index].get_type() == Variant::VariantType::NONE)
				continue;
			switch (m_objects[index].get_type()) {
			case Variant::VariantType::FLOAT: {
				str.append(std::format("[{}]: {:.6f}\n", index, m_objects[index].get<float>()));
				break;
			}
			case Variant::VariantType::STRING: {
				str.append(std::format("[{}]: {}\n", index, m_objects[index].get<std::string>()));
				break;
			}
			case Variant::VariantType::VECTOR2: {
				str.append(std::format(
					"[{}]: x: {:.6f} y: {:.6f}\n",
					index,
					m_objects[index].get<Vector2>().m_x,
					m_objects[index].get<Vector2>().m_y
				));
				break;
			}
			case Variant::VariantType::VECTOR3: {
				str.append(std::format(
					"[{}]: x: {:.6f} y: {:.6f} z: {:.6f}\n",
					index,
					m_objects[index].get<Vector3>().m_x,
					m_objects[index].get<Vector3>().m_y,
					m_objects[index].get<Vector3>().m_z
				));
				break;
			}
			case Variant::VariantType::UINT: {
				str.append(std::format("[{}]: {}\n", index, m_objects[index].get<uint32_t>()));
				break;
			}
			case Variant::VariantType::INT: {
				str.append(std::format("[{}]: {}\n", index, m_objects[index].get<int32_t>()));
				break;
			}
			default:
				break;
			}
		}
		return str;
	}
};