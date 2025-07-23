#include"Vector.h"
#include<math.h>
#include"Struct.h"

Vector3 Add(const Vector3& vector1, const Vector3& vector2) {
	Vector3 result = {};
	result.x = vector1.x + vector2.x;
	result.y = vector1.y + vector2.y;
	result.z = vector1.z + vector2.z;
	return result;
}

Vector3 Subtract(const Vector3& vector1, const Vector3& vector2) {
	Vector3 result = {};
	result.x = vector1.x - vector2.x;
	result.y = vector1.y - vector2.y;
	result.z = vector1.z - vector2.z;
	return result;
}

Vector3 Multiply(float scalar, const Vector3& vector) {
	Vector3 result = {};
	result.x = scalar * vector.x;
	result.y = scalar * vector.y;
	result.z = scalar * vector.z;
	return result;
}

float Dot(const Vector3& vector1, const Vector3& vector2) {
	float result = {};
	result = vector1.x * vector2.x +
		vector1.y * vector2.y +
		vector1.z * vector2.z;
	return result;
}

float Length(const Vector3& vector) {
	float result = {};
	result = vector.x * vector.x +
		vector.y * vector.y +
		vector.z * vector.z;
	result = sqrtf(result);
	return result;
}

Vector3 Normalize(const Vector3& vector) {
	Vector3 result = {};
	float length = {};
	length = vector.x * vector.x +
		vector.y * vector.y +
		vector.z * vector.z;
	length = 1.0f / sqrtf(length);
	result.x = length * vector.x;
	result.y = length * vector.y;
	result.z = length * vector.z;
	return result;
}

Vector3 Cross(const Vector3& vector1, const Vector3& vector2) {
	Vector3 result = {};
	result.x = vector1.y * vector2.z - vector1.z * vector2.y;
	result.y = vector1.z * vector2.x - vector1.x * vector2.z;
	result.z = vector1.x * vector2.y - vector1.y * vector2.x;
	return result;
}

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t) {
	Vector3 result = {};
	result.x = (v1.x * (1.0f - t)) + (v2.x * t);
	result.y = (v1.y * (1.0f - t)) + (v2.y * t);
	result.z = (v1.z * (1.0f - t)) + (v2.z * t);
	return result;
}

Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {
	t = std::fmax(0.0f, std::fmin(1.0f, t));

	float dot = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

	dot = std::fmax(-1.0f, std::fmin(1.0f, dot));

	float theta = std::acos(dot);

	if (std::fabs(theta) < 1e-6f) {
		Vector3 result = {};
		result.x = (v1.x * (1.0f - t)) + (v2.x * t);
		result.y = (v1.y * (1.0f - t)) + (v2.y * t);
		result.z = (v1.z * (1.0f - t)) + (v2.z * t);
		return result;
	}

	float sinTheta = std::sin(theta);

	float weight1 = std::sin((1.0f - t) * theta) / sinTheta;
	float weight2 = std::sin(t * theta) / sinTheta;

	Vector3 result = {};
	result.x = (v1.x * weight1) + (v2.x * weight2);
	result.y = (v1.y * weight1) + (v2.y * weight2);
	result.z = (v1.z * weight1) + (v2.z * weight2);

	return result;
}