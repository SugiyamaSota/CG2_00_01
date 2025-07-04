#pragma once
#include<stdint.h>
#include<string>
#include<vector>
#include<array>
#include<cmath>

struct Matrix4x4 {
    // 列優先(column-major)または行優先(row-major)かは慣習によって異なりますが、
    // ここでは一般的な行優先(row-major)で実装します。
    // m[row][column]
    std::array<std::array<float, 4>, 4> m;

    // デフォルトコンストラクタ (単位行列で初期化)
    Matrix4x4() {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                m[i][j] = (i == j) ? 1.0f : 0.0f; // 対角成分を1、その他を0
            }
        }
    }

    // 値を指定して初期化するコンストラクタ
    Matrix4x4(float _m00, float _m01, float _m02, float _m03,
        float _m10, float _m11, float _m12, float _m13,
        float _m20, float _m21, float _m22, float _m23,
        float _m30, float _m31, float _m32, float _m33) {
        m[0][0] = _m00; m[0][1] = _m01; m[0][2] = _m02; m[0][3] = _m03;
        m[1][0] = _m10; m[1][1] = _m11; m[1][2] = _m12; m[1][3] = _m13;
        m[2][0] = _m20; m[2][1] = _m21; m[2][2] = _m22; m[2][3] = _m23;
        m[3][0] = _m30; m[3][1] = _m31; m[3][2] = _m32; m[3][3] = _m33;
    }

    // 行列の加算
    Matrix4x4 operator+(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] + other.m[i][j];
            }
        }
        return result;
    }

    // 行列の減算
    Matrix4x4 operator-(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] - other.m[i][j];
            }
        }
        return result;
    }

    // 行列の乗算 (最も重要)
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result; // デフォルトで単位行列で初期化されるが、上書きされる
        for (int i = 0; i < 4; ++i) { // resultの行
            for (int j = 0; j < 4; ++j) { // resultの列
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 4; ++k) { // 積和演算
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    // スカラー乗算 (Matrix4x4 * float)
    Matrix4x4 operator*(float scalar) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                result.m[i][j] = m[i][j] * scalar;
            }
        }
        return result;
    }

    // 等価比較
    bool operator==(const Matrix4x4& other) const {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (m[i][j] != other.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    // 非等価比較
    bool operator!=(const Matrix4x4& other) const {
        return !(*this == other);
    }
};

struct Vector2 {
    float x;
    float y;

    // デフォルトコンストラクタ
    Vector2() : x(0.0f), y(0.0f) {}

    // コンストラクタ
    Vector2(float _x, float _y) : x(_x), y(_y) {}

    // 加算演算子オーバーロード (メンバ関数として)
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // 減算演算子オーバーロード (メンバ関数として)
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // スカラー乗算演算子オーバーロード (メンバ関数として)
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    // 加算代入演算子オーバーロード (メンバ関数として)
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // 等価比較演算子オーバーロード
    bool operator==(const Vector2& other) const {
        return (x == other.x && y == other.y);
    }

    // 非等価比較演算子オーバーロード
    bool operator!=(const Vector2& other) const {
        return !(*this == other); // == 演算子を利用
    }
};

//三次元ベクトル
struct Vector3 {
    float x;
    float y;
    float z;

    // デフォルトコンストラクタ
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

    // コンストラクタ
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    // 加算演算子オーバーロード (メンバ関数として)
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    // 減算演算子オーバーロード (メンバ関数として)
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    // スカラー乗算演算子オーバーロード (メンバ関数として)
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    // スカラー除算演算子オーバーロード (メンバ関数として)
    Vector3 operator/(float scalar) const {
        if (scalar == 0.0f) {
            return Vector3();
        }
        return Vector3(x / scalar, y / scalar, z / scalar);
    }

    // ドット積 (内積)
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // クロス積 (外積)
    Vector3 cross(const Vector3& other) const {
        return Vector3(y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    // ベクトルの長さ (マグニチュード)
    float magnitude() const {
        return std::sqrtf(x * x + y * y + z * z);
    }

    // ベクトルの正規化
    Vector3 normalized() const {
        float mag = magnitude();
        if (mag == 0.0f) {
            return Vector3();
        }
        return *this / mag;
    }

    // 加算代入演算子オーバーロード (メンバ関数として)
    Vector3& operator+=(const Vector3& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    // 減算代入演算子オーバーロード (メンバ関数として)
    Vector3& operator-=(const Vector3& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    // スカラー乗算代入演算子オーバーロード (メンバ関数として)
    Vector3& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    // スカラー除算代入演算子オーバーロード (メンバ関数として)
    Vector3& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // 単項マイナス演算子オーバーロード
    Vector3 operator-() const {
        return Vector3(-x, -y, -z);
    }

    // 等価比較演算子オーバーロード
    bool operator==(const Vector3& other) const {
        return (x == other.x && y == other.y && z == other.z);
    }

    // 非等価比較演算子オーバーロード
    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }
};

struct Vector4 {
    float x, y, z, w;
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

struct MaterialData {
	std::string textureFilepath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct WorldTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

