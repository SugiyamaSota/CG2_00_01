#pragma once

// 数学関連
#include"../math/Struct.h"
#include "../math/Vector.h"
#include "../math/Matrix.h"
#include "../math/Convert.h"

// 球面座標系の計算に必要なもの
#include <cmath>      // std::sin, std::cos, std::atan2, std::fmod など
#include <algorithm>  // std::clamp のため

class Camera {
private:
	///// --- 変数 --- /////
	// 累積回転行列 (球面座標を使う場合、直接操作することは減るかもしれません)
	Matrix4x4 matRot_; // これは引き続きカメラの向きを表す行列として利用します

	// ローカル座標 (球面座標から計算されるため、直接設定は減ります)
	Vector3 translation_;

	//MakeAffineで使うため(基本変えないためconst)
	const Vector3 scale_{ 1,1,1 };

	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 projectionMatrix_;
	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

	// カメラの速度(ズーム、移動、回転)
	const float rotationSpeed_ = 0.01f; 

	// ターゲット関連
	bool isTargeting_;       // フラグ
	Vector3 targetPosition_; // 注視する対象座標 (球面座標の中心)

	// 球面座標系に使う変数
	float radius_;
	float theta_;
	float phi_;

	///// --- 関数 --- /////
	// 移動処理
	void Move();

	// 回転処理
	void Rotate();

	// 球面座標からカメラのワールド座標と回転行列を計算
	void CalculateCameraPositionAndRotation();

	// 対象を見つめる処理
	Matrix4x4 MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up);

	// ターゲットするかどうかのチェック関数
	void Check() { isTargeting_ = false; }
public:
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
	/// 更新処理
	/// </summary>
	/// <param name="isDebug">デバッグカメラフラグ</param>
	void Update(CameraType type);

	/// <summary>
	/// 注視する対象の設定
	/// </summary>
	/// <param name="targetPosition">注視する座標</param>
	void SetTarget(Vector3 targetPosition);

	///// --- ゲッター関数 --- /////
	Matrix4x4 GetViewMatrix()const { return viewMatrix_; }
	Matrix4x4 GetViewProjectionMatrix()const { return viewProjectionMatrix_; }

	///// --- セッター関数 --- /////
	void ResetPosition();
	void ResetRotation();
};