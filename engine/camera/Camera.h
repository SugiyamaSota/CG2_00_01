#pragma once
#include <cmath>
#include <algorithm>

#include"../math/Struct.h"


class Camera {
private:
	///// --- 変数 ---
	// 累積回転行列
	Matrix4x4 matRot_;

	// ローカル座標
	Vector3 translation_;

	// ワールド行列
	Matrix4x4 worldMatrix_;

	// ビュー行列
	Matrix4x4 viewMatrix_;

	// 射影行列
	Matrix4x4 projectionMatrix_;

	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

	// カメラの速度(ズーム、移動、回転)
	const float multiSpeed_ = 0.01f; 

	// ターゲット関連
	Vector3 targetPosition_; // 注視する対象座標 (球面座標の中心)

	// 球面座標系に使う変数
	float radius_;
	float theta_;
	float phi_;

	///// --- 関数 ---
	// 移動処理
	void Move();

	// 回転処理
	void Rotate();

	// 球面座標からカメラのワールド座標と回転行列を計算
	void CalculateCameraPositionAndRotation();

public:

	// カメラタイプ
	enum class CameraType {
		kNormal,
		kDebug,
	};

	///// --- 関数 --- /////
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(uint32_t clientWidth, uint32_t clientHeight);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="type">カメラタイプ</param>
	void Update(CameraType type);


	// 対象を見つめる処理
	Matrix4x4 MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);

	///// --- ゲッター関数 ---
	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }
	Matrix4x4 GetViewProjectionMatrix()const { return viewProjectionMatrix_; }

	///// --- セッター関数 ---
	void ResetPosition();
	void ResetRotation();
	void SetViewMatrix(Matrix4x4 matrix) { viewMatrix_ = matrix; }

};