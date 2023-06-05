#pragma once
#include <algorithm>
#define ABS(x) ((x < 0) ? (-x) : (x));


class Vector3
{
public:
	float x, y, z;

	Vector3() { x = y = z = 0; };
	Vector3(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	Vector3 operator + (const Vector3& rhs) const { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3 operator - (const Vector3& rhs) const { return Vector3(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3 operator * (const float& rhs) const { return Vector3(x * rhs, y * rhs, z * rhs); }
	Vector3 operator / (const float& rhs) const { return Vector3(x / rhs, y / rhs, z / rhs); }
	Vector3& operator += (const Vector3& rhs) { return *this = *this + rhs; };
	Vector3& operator -= (const Vector3& rhs) { return *this = *this - rhs; };
	Vector3& operator *= (const float& rhs) { return *this = *this * rhs; };
	Vector3& operator /= (const float& rhs) { return *this = *this / rhs; };

	float length() const { return sqrtf(x * x + y * y + z * z); }
	Vector3 Normalize() const { return *this * (1 / length()); }
	float Distance(const Vector3& rhs) const { return (*this - rhs).length(); }
};

struct Vector2
{
	float x, y;
};

struct Vector4
{
	float x, y, z, w;
};

