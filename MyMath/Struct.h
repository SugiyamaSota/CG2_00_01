#pragma once
#include<cstdint>
#include<vector>
#include<format>

struct Vector2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;
};

struct Vector4 {
	float x, y, z, w;
};

struct Matrix4x4 {
	float m[4][4];
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};


struct Material {
	Vector4 color;
	int32_t enableLighting;
	float pedding[3];
	Matrix4x4 uvTransform;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intentity;
};

struct MaterialData {
	std::string textureFilepath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

