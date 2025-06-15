#pragma once
#include "MyMath/Struct.h"
#include"InputKey.h"
#include<Windows.h>

class DebugCamera {
private:
	///// 変数 /////
	// 累積回転行列
	Matrix4x4 matRot_;

	Vector3 translation_; // ローカル座標

	const Vector3 scale_{ 1,1,1 }; //MakeAffineで使うため(基本変えないためconst)

	Matrix4x4 viewMatrix_; // ビュー行列

	Matrix4x4 projactionMatirx_; // 射影行列

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

	void Check(){ isTargeting_ = false; }

	void Update(); // 更新

	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }

	void SetTarget(Vector3 targetPosition);

	void ResetPosition() { translation_ = { 0,0,-50 }; }
	void ResetRotation();
};

