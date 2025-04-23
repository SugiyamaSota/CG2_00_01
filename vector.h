#pragma once

struct Vector3 {
	float x, y, z;
};

struct Vector4 {
	float x, y, z, w;
};

//===計算関数===//
//加算
Vector3 Add(const Vector3& vector1, const Vector3& vector2);

//減算
Vector3 Subtract(const Vector3& vector1, const Vector3& vector2);

//スカラー倍
Vector3 Multiply(float scalar, const Vector3& vector);

//内積
float Dot(const Vector3& vector1, const Vector3& vector2);

//長さ(ノルム)
float Length(const Vector3& vector);

//正規化
Vector3 Normalize(const Vector3& vector);

//クロス積
Vector3 Cross(const Vector3& vector1, const Vector3& vector2);
