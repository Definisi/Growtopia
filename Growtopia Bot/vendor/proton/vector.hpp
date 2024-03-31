#pragma once

struct Vector2 {
	float m_x;
	float m_y;
	
	Vector2()
		: m_x(0), m_y(0) {}
	Vector2(float x, float y)
		: m_x(x), m_y(y) {}
};

struct Vector2i {
	int m_x;
	int m_y;
	
	Vector2i()
		: m_x(0), m_y(0) {}
	Vector2i(int x, int y)
		: m_x(x), m_y(y) {}
	float distance(float x, float y) {
		float value_x = this->m_x - x;
		float value_y = this->m_y - y;
		return sqrt(value_x * value_x + value_y * value_y);
	}
};

struct Vector3 {
	float m_x;
	float m_y;
	float m_z;
	
	Vector3()
		: m_x(0), m_y(0), m_z(0) {}
	Vector3(float x, float y, float z)
		: m_x(x), m_y(y), m_z(z) {}
};