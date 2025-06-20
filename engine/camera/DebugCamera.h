#pragma once

// 数学関連
#include"../math/Struct.h"
#include "../math/Vector.h"
#include "../math/Matrix.h"
#include "../math/Convert.h"

// 入力
#include"../input/InputKey.h"

class DebugCamera {
private:
	///// 変数 /////
	// 累積回転行列
	Matrix4x4 matRot_;

	Vector3 translation_; // ローカル座標

	const Vector3 scale_{ 1,1,1 }; //MakeAffineで使うため(基本変えないためconst)

	Matrix4x4 viewMatrix_; // ビュー行列

	Matrix4x4 projectionMatrix_;

	// キー入力に使う変数
	InputKey* inputKey_;

	const float rotationSpeed_ = 0.01f; //回転速度

	bool isTargeting_;
	Vector3 targetPosition_;

	///// 関数 /////
	void Move(); // 移動処理
	void Rotate();// 回転処理
	Matrix4x4 MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);
public:
	void Initialize(InputKey* inputKey); // 初期化

	void Check() { isTargeting_ = false; }

	void Update(bool isDebug); // 更新

	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }
	void SetCameraTranslate(Vector3 position) {
		translation_.x = position.x; translation_.y = position.y;
	}

	void SetTarget(Vector3 targetPosition);

	void ResetPosition() { translation_ = { 0,0,-50 }; }
	void ResetRotation();
};

